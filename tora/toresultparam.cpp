//***************************************************************************
/*
 * TOra - An Oracle Toolkit for DBA's and developers
 * Copyright (C) 2003 Quest Software, Inc
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *      As a special exception, you have permission to link this program
 *      with the Oracle Client libraries and distribute executables, as long
 *      as you follow the requirements of the GNU GPL in regard to all of the
 *      software in the executable aside from Oracle client libraries.
 *
 *      Specifically you are not permitted to link this program with the
 *      Qt/UNIX, Qt/Windows or Qt Non Commercial products of TrollTech.
 *      And you are not permitted to distribute binaries compiled against
 *      these libraries without written consent from Quest Software, Inc.
 *      Observe that this does not disallow linking to the Qt Free Edition.
 *
 * All trademarks belong to their respective owners.
 *
 ****************************************************************************/

#include "utils.h"

#include "toconf.h"
#include "toresultparam.h"
#include "tosql.h"
#include "toresultview.h"
#include "tomemoeditor.h"
#include "toconnection.h"
#include "toresultlong.h"

#include <qlabel.h>
#include <qlineedit.h>
#include <qregexp.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qtooltip.h>

#include "toresultparam.moc"

#include "icons/database.xpm"
#include "icons/filesave.xpm"
#include "icons/scansource.xpm"
#include "icons/tocurrent.xpm"
#include "icons/trash.xpm"
#include "icons/commit.xpm"

static toSQL SQLParamsMySQL("toResultParam:ListParam",
			    "TOAD 1,2 show variables",
			    "List parameters available in the session",
			    "4.0",
			    "MySQL");

static toSQL SQLParamsGlobal("toResultParam:ListGlobal",
			     "TOAD 1,2 show global variables",
			     "List parameters available in the database",
			     "4.0",
			     "MySQL");

static toSQL SQLParams("toResultParam:ListParam",
		       "select name \"Parameter\",value \"Value\",' ' \"Changed\",\n"
		       "       description \"Description\", num \" Number\",\n"
		       "       type \" Type\",isdefault \" Default\",\n"
		       "       isses_modifiable \" Sesmod\",issys_modifiable \" Sysmod\"\n"
		       "  from v$parameter order by name",
		       "");

static toSQL SQLHiddenParams("toResultParam:ListHidden",
			     "SELECT KSPPINM \"Parameter\",\n"
			     "       KSPFTCTXVL \"Value\",\n"
			     "       ' ' \"Changed\",\n"
			     "       KSPPDESC \"Description\",\n"
			     "       KSPFTCTXPN \" Num\",\n"
			     "       ksppity \" Type\",\n"
			     "       KSPFTCTXDF \" Default\",\n"
			     "       DECODE(MOD(TRUNC(KSPPIFLG/256),2),0,'FALSE','TRUE') \" Sesmod\",\n"
			     "       DECODE(MOD(TRUNC(KSPPIFLG/65536),8),0,'FALSE','TRUE') \" Sysmod\"\n"
			     "  FROM X$KSPPI x,\n"
			     "       X$KSPPCV2 y\n"
			     " WHERE x.INDX+1=y.KSPFTCTXPN ORDER BY KSPPINM",
			     "List parameters available in the database including hidden parameters");

bool toResultParam::canHandle(toConnection &conn)
{
  return toIsOracle(conn)||toIsMySQL(conn);
}

toResultParam::toResultParam(QWidget *parent,const char *name)
  : QVBox(parent,name)
{
  QToolBar *toolbar=toAllocBar(this,tr("Parameter editor"));
  Toggle=new QToolButton(toolbar);
  Toggle->setToggleButton(true);
  if (toIsOracle(connection())) {
    Toggle->setIconSet(QIconSet(QPixmap((const char **)scansource_xpm)));
    connect(Toggle,SIGNAL(toggled(bool)),this,SLOT(showHidden(bool)));
    QToolTip::add(Toggle,tr("Display hidden parameters. This will only word if you are logged in as the sys user."));
  } else {
    QIconSet iconset(QPixmap((const char **)tocurrent_xpm));
    iconset.setPixmap(QPixmap((const char **)database_xpm),QIconSet::Automatic,QIconSet::Normal,QIconSet::On);
    Toggle->setIconSet(iconset);
    connect(Toggle,SIGNAL(toggled(bool)),this,SLOT(showGlobal(bool)));
    QToolTip::add(Toggle,tr("Switch between global and session variables to show."));
  }
  toolbar->addSeparator();

  new QToolButton(QPixmap((const char **)filesave_xpm),
		  tr("Generate configuration file"),
		  tr("Generate configuration file"),
		  this,SLOT(generateFile()),toolbar);
  toolbar->addSeparator();
  if (toIsOracle(connection())) {
    new QToolButton(QPixmap((const char **)database_xpm),
		    tr("Apply changes to system"),
		    tr("Apply changes to system"),
		    this,SLOT(applySystem()),toolbar);
    new QToolButton(QPixmap((const char **)tocurrent_xpm),
		    tr("Apply changes to session"),
		    tr("Apply changes to session"),
		    this,SLOT(applySession()),toolbar);
  } else {
    new QToolButton(QPixmap((const char **)commit_xpm),
		    tr("Apply changes"),
		    tr("Apply changes"),
		    this,SLOT(applyChanges()),toolbar);
  }
  if (toIsOracle(connection())) {
    toolbar->addSeparator();
    new QToolButton(QPixmap((const char **)trash_xpm),
		    tr("Drop current changes"),
		    tr("Drop current changes"),
		    this,SLOT(dropChanges()),toolbar);
  }
  toolbar->setStretchableWidget(new QLabel(toolbar,TO_KDE_TOOLBAR_WIDGET));

  Params=new toResultLong(false,false,toQuery::Background,this);
  Params->setSQL(SQLParams);
  Params->setReadAll(true);
  Params->setSelectionMode(QListView::Single);
  connect(Params,SIGNAL(selectionChanged()),this,SLOT(changeItem()));
  connect(Params,SIGNAL(done()),this,SLOT(done()));
  Value=new QLineEdit(this);
  Value->setEnabled(false);
  refresh();
}

void toResultParam::showHidden(bool hid)
{
  if (hid)
    Params->setSQL(SQLHiddenParams);
  else
    Params->setSQL(SQLParams);
  refresh();
}

void toResultParam::showGlobal(bool glb)
{
  if (glb)
    Params->setSQL(SQLParamsGlobal);
  else
    Params->setSQL(SQLParams);
  refresh();
}

void toResultParam::query(const QString &,const toQList &)
{
  saveChange();
  LastItem=QString::null;

  Params->refresh();
}

void toResultParam::dropChanges(void)
{
  NewValues.clear();
  refresh();
}

void toResultParam::done()
{
  for(QListViewItem *item=Params->firstChild();item;item=item->nextSibling()) {
    std::map<QString,QString>::iterator i=NewValues.find(item->text(0));
    if (i!=NewValues.end()) {
      item->setText(1,(*i).second);
      item->setText(6,QString::fromLatin1("FALSE"));
      item->setText(2,tr("Changed"));
    }
  }
}

void toResultParam::saveChange()
{
  if(!LastItem.isEmpty()&&LastValue!=Value->text()) {
    NewValues[LastItem]=Value->text();
    LastValue=Value->text();
    for(QListViewItem *item=Params->firstChild();item;item=item->nextSibling()) {
      if (item->text(0)==LastItem) {
	item->setText(1,LastValue);
	item->setText(6,QString::fromLatin1("FALSE"));
	item->setText(2,tr("Changed"));
	break;
      }
    }
  }
}

void toResultParam::generateFile(void)
{
  saveChange();
  QString str=tr("# Generated by TOra version %1\n\n").arg(QString::fromLatin1(TOVERSION));
  QRegExp comma(QString::fromLatin1("\\s*,\\s+"));
  for(QListViewItem *item=Params->firstChild();item;item=item->nextSibling()) {
    if (toIsOracle(connection())) {
      if (item->text(6)==QString::fromLatin1("FALSE")) {
	str+=item->text(0);
	str+=QString::fromLatin1(" = ");
	if(item->text(5)==QString::fromLatin1("2")) {
	  QStringList lst=QStringList::split(comma,item->text(1));
	  if (lst.count()>1)
	    str+=QString::fromLatin1("( ");
	  for(unsigned int i=0;i<lst.count();i++) {
	    if (i>0)
	      str+=QString::fromLatin1(", ");
	    str+=QString::fromLatin1("\"")+lst[i]+QString::fromLatin1("\"");
	  }
	  if (lst.count()>1)
	    str+=QString::fromLatin1(" )");
	} else
	  str+=item->text(1);
	str+=QString::fromLatin1("\n");
      }
    } else {
      str+=item->text(0)+" = '"+item->text(1)+"'\n";
    }
  }
  connect(new toMemoEditor(this,str,0,0),SIGNAL(changeData(int,int,const QString &)),
	  this,SLOT(changedData(int,int,const QString &)));
}

void toResultParam::applySession(void)
{
  try {
    saveChange();
    toConnection &conn=connection();
    for(QListViewItem *item=Params->firstChild();item;item=item->nextSibling()) {
      if (item->text(2)==tr("Changed")) {
	try {
	  if (item->text(7)!=QString::fromLatin1("FALSE")) {
	    QString str=QString::fromLatin1("ALTER SESSION SET ");
	    str+=item->text(0);
	    str+=QString::fromLatin1(" = ");
	    if (item->text(5)==QString::fromLatin1("2")) {
	      str+=QString::fromLatin1("'");
	      str+=item->text(1);
	      str+=QString::fromLatin1("'");
	    } else
	      str+=item->text(1);
	    conn.allExecute(str);
	    std::map<QString,QString>::iterator i=NewValues.find(item->text(0));
	    if (i!=NewValues.end())
	      NewValues.erase(i);
	  }
	} TOCATCH
	}
    }
  } TOCATCH
  refresh();
}

void toResultParam::applySystem(void)
{
  try {
    saveChange();
    toConnection &conn=connection();
    for(QListViewItem *item=Params->firstChild();item;item=item->nextSibling()) {
      if (item->text(2)==tr("Changed")) {
	try {
	  if (item->text(8)!=QString::fromLatin1("FALSE")) {
	    QString str=QString::fromLatin1("ALTER SYSTEM SET ");
	    str+=item->text(0);
	    str+=QString::fromLatin1(" = ");
	    if (item->text(5)==QString::fromLatin1("2")) {
	      str+=QString::fromLatin1("'");
	      str+=item->text(1);
	      str+=QString::fromLatin1("'");
	    } else
	      str+=item->text(1);
	    conn.execute(str);
	    std::map<QString,QString>::iterator i=NewValues.find(item->text(0));
	    if (i!=NewValues.end())
	      NewValues.erase(i);
	  }
	} TOCATCH
      }
    }
  } TOCATCH
  refresh();
}

// Only used by MySQL

void toResultParam::applyChanges(void)
{
  try {
    saveChange();
    toConnection &conn=connection();
    for(QListViewItem *item=Params->firstChild();item;item=item->nextSibling()) {
      if (item->text(2)==tr("Changed")) {
	try {
	  QString str="SET ";
	  if (!Toggle->isOn())
	    str+="GLOBAL ";
	  else
	    str+="SESSION ";
	  str+=item->text(0);
	  str+=" = ";
	  str+=QString::fromLatin1("'");
	  str+=item->text(1);
	  str+=QString::fromLatin1("'");
	  conn.execute(str);
	  std::map<QString,QString>::iterator i=NewValues.find(item->text(0));
	  if (i!=NewValues.end())
	    NewValues.erase(i);
	} TOCATCH
      }
    }
  } TOCATCH
  refresh();
}

void toResultParam::changeItem(void)
{
  saveChange();

  QListViewItem *item=Params->selectedItem();
  if (item) {
    LastItem=item->text(0);
    LastValue=item->text(1);
    Value->setText(LastValue);
    Value->setEnabled(true);
  } else {
    LastItem=QString::null;
    Value->setEnabled(false);
  }
}

void toResultParam::changedData(int,int,const QString &data)
{
  QString file=toSaveFilename(QString::null,toIsOracle(connection())?"*.pfile":"*.conf",this);
  if (!file.isEmpty())
    toWriteFile(file,data);
}
