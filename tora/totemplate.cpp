/****************************************************************************
 *
 * TOra - An Oracle Toolkit for DBA's and developers
 * Copyright (C) 2000 GlobeCom AB
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
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
 *      Qt/UNIX or Qt/Windows products of TrollTech. And you are not
 *      permitted to distribute binaries compiled against these libraries
 *      without written consent from GlobeCom AB. Observe that this does not
 *      disallow linking to the Qt Free Edition.
 *
 * All trademarks belong to their respective owners.
 *
 ****************************************************************************/

TO_NAMESPACE;

#ifdef TO_KDE
#include <kfiledialog.h>
#endif

#include <qsplitter.h>
#include <qtextview.h>
#include <qfileinfo.h>
#include <qfiledialog.h>

#include "toresultview.h"
#include "totool.h"
#include "totemplate.h"
#include "tomain.h"

#include "totemplate.moc"
#include "totemplatesetupui.moc"
#include "totemplateaddfileui.moc"

#include "icons/totemplate.xpm"

class toTemplateAddFile : public toTemplateAddFileUI {
public:
  toTemplateAddFile(QWidget *parent,const char *name=0)
    : toTemplateAddFileUI(parent,name,true)
  { OkButton->setEnabled(false); }
  virtual void browse(void)
  {
    QFileInfo file(Filename->text());
    QString filename=TOFileDialog::getOpenFileName(file.dirPath(),"*.tpl",this);
    if (!filename.isEmpty())
      Filename->setText(filename);
  }
  virtual void valid(void)
  {
    if (Filename->text().isEmpty()||Root->text().isEmpty())
      OkButton->setEnabled(false);
    else
      OkButton->setEnabled(true);
  }
};

class toTemplatePrefs : public toTemplateSetupUI, public toSettingTab
{
  toTool *Tool;
public:
  toTemplatePrefs(toTool *tool,QWidget *parent,const char *name=0)
    : toTemplateSetupUI(parent,name),Tool(tool)
  {
    int tot=Tool->config("Number",QString::number(-1)).toInt();
    if(tot!=-1) {
      for(int i=0;i<tot;i++) {
	QString num=QString::number(i);
	QString root=Tool->config(num,"");
	num+="file";
	QString file=Tool->config(num,"");
	new QListViewItem(FileList,root,file);
      }
    } else {
      QString file=DEFAULT_PLUGIN_DIR;
      file+="/sqlfunctions.tpl";
      new QListViewItem(FileList,"PL/SQL Functions",file);
    }
  }
  virtual void saveSetting(void)
  {
    int i=0;
    for(QListViewItem *item=FileList->firstChild();item;item=item->nextSibling()) {
      QString nam=QString::number(i);
      Tool->setConfig(nam,item->text(0));
      nam+="file";
      Tool->setConfig(nam,item->text(1));
      i++;
    }
    Tool->setConfig("Number",QString::number(i));
  }
  virtual void addFile(void)
  {
    toTemplateAddFile file(this);
    if (file.exec())
      new QListViewItem(FileList,file.Root->text(),file.Filename->text());
  }
  virtual void delFile(void)
  {
    delete FileList->selectedItem();
  }
};

class toTemplateTool : public toTool {
  TODock *Dock;
protected:
  virtual char **pictureXPM(void)
  { return totemplate_xpm; }
public:
  toTemplateTool()
    : toTool(301,"SQL Template")
  { Dock=NULL; }
  virtual const char *menuItem()
  { return "SQL Template"; }
  virtual QWidget *toolWindow(QWidget *parent,toConnection &connection)
  {
    if (!Dock) {
      Dock=toAllocDock("Template",connection.connectString(),*toolbarImage());
      QWidget *window=new toTemplate(Dock);
      toAttachDock(Dock,window,QMainWindow::Left);
      return Dock;
    }
    Dock->show();
    Dock->setFocus();
    return NULL;
  }
  void closeWindow(toConnection &connection)
  { Dock=NULL; }
  virtual QWidget *configurationTab(QWidget *parent)
  { return new toTemplatePrefs(this,parent); }
};

static toTemplateTool TemplateTool;

QWidget *toTemplate::parentWidget(QListViewItem *item)
{
  return templateWidget(item)->frame();
}

toTemplate *toTemplate::templateWidget(QListView *obj)
{
  QObject *lst=obj;
  while(lst) {
    if (dynamic_cast<toTemplate *>(lst))
      return dynamic_cast<toTemplate *>(lst);
    lst=lst->parent();
  }
  throw("Not a toTemplate parent");
}

toTemplate *toTemplate::templateWidget(QListViewItem *item)
{
  return templateWidget(item->listView());
}

toTemplate::toTemplate(QWidget *parent)
  : QVBox(parent)
{
  Splitter=new QSplitter(Vertical,this);
  List=new toListView(Splitter);
  List->addColumn("Template");
  List->setRootIsDecorated(true);
  List->setSorting(0);
  List->setShowSortIndicator(false);
  List->setTreeStepSize(10);
  Frame=new QVBox(Splitter);

  connect(List,SIGNAL(expanded(QListViewItem *)),this,SLOT(expand(QListViewItem *)));
  connect(List,SIGNAL(collapsed(QListViewItem *)),this,SLOT(collapse(QListViewItem *)));

  if (toTemplateProvider::Providers)
    for (list<toTemplateProvider *>::iterator i=toTemplateProvider::Providers->begin();
	 i!=toTemplateProvider::Providers->end();
	 i++)
      (*i)->insertItems(List);

  WidgetExtra=NULL;
  setWidget(NULL);
}

toTemplate::~toTemplate()
{
}

void toTemplate::expand(QListViewItem *item)
{
  toTemplateItem *ti=dynamic_cast<toTemplateItem *>(item);
  if (ti)
    ti->expand();
}

void toTemplate::collapse(QListViewItem *item)
{
  toTemplateItem *ti=dynamic_cast<toTemplateItem *>(item);
  if (ti)
    ti->collapse();
}

void toTemplateItem::setSelected(bool sel)
{
  toTemplate *temp=toTemplate::templateWidget(this);
  if (sel&&temp)
    temp->setWidget(selectedWidget(toTemplate::parentWidget(this)));
  toResultViewItem::setSelected(sel);
}

QWidget *toTemplateText::selectedWidget(QWidget *parent)
{
  return new QTextView(Note,QString::null,parent);
}

void toTemplate::setWidget(QWidget *widget)
{
  if (!widget)
    widget=new QTextView(frame());

  widget->show();
  if (WidgetExtra)
    delete WidgetExtra;

  WidgetExtra=widget;
}

class toTextTemplate : toTemplateProvider {
public:
  toTextTemplate()
  { }
  void addFile(QListView *parent,const QString &root,const QString &file);
  virtual void insertItems(QListView *parent);
};

static bool CompareLists(QStringList &lst1,QStringList &lst2,unsigned int len)
{
  if (lst1.count()<len||lst2.count()<len)
    return false;
  for (unsigned int i=0;i<len;i++)
    if (lst1[i]!=lst2[i])
      return false;
  return true;
}

void toTextTemplate::insertItems(QListView *parent)
{
  int tot=TemplateTool.config("Number",QString::number(-1)).toInt();
  if(tot!=-1) {
    for(int i=0;i<tot;i++) {
      QString num=QString::number(i);
      QString root=TemplateTool.config(num,"");
      num+="file";
      QString file=TemplateTool.config(num,"");
      addFile(parent,root,file);
    }
  } else {
    QString file=DEFAULT_PLUGIN_DIR;
    file+="/sqlfunctions.tpl";
    addFile(parent,"PL/SQL Functions",file);
  }
}

void toTextTemplate::addFile(QListView *parent,const QString &root,const QString &file)
{
  map<QString,QString> pairs;
  toTool::loadMap(file,pairs);
  toTemplateItem *last=new toTemplateItem(*this,parent,root);
  int lastLevel=0;
  QStringList lstCtx;
  for(map<QString,QString>::iterator i=pairs.begin();i!=pairs.end();i++) {
    QStringList ctx=QStringList::split(":",(*i).first);
    if (last) {
      while(last&&lastLevel>=int(ctx.count())) {
	last=dynamic_cast<toTemplateItem *>(last->parent());
	lastLevel--;
      }
      while(last&&lastLevel>=0&&!CompareLists(lstCtx,ctx,(unsigned int)lastLevel)) {
	last=dynamic_cast<toTemplateItem *>(last->parent());
	lastLevel--;
      }
    }
    if (lastLevel<0)
      throw QString("Internal error, lastLevel < 0");
    while(lastLevel<int(ctx.count())-1) {
      last=new toTemplateItem(last,ctx[lastLevel]);
      lastLevel++;
    }
    last=new toTemplateText(last,ctx[lastLevel],(*i).second);
    lstCtx=ctx;
    lastLevel++;
  }
}

void toTemplateSQL::expand(void)
{
  while(firstChild())
    delete firstChild();
  try {
    otl_stream inf(1,
		   SQL,
		   Connection.connection());
    list<QString> par=parameters();
    list<QString> val=toReadQuery(inf,par);
    for (list<QString>::iterator i=val.begin();i!=val.end();i++)
      createChild(*i);
  } TOCATCH
}

static toTextTemplate TextTemplate;
