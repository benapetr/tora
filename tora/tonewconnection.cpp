//***************************************************************************
/* $Id$
**
** Copyright (C) 2000-2001 GlobeCom AB.  All rights reserved.
**
** This file is part of the Toolkit for Oracle.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.globecom.net/tora/ for more information.
**
** Contact tora@globecom.se if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifdef WIN32
#  include "windows/cregistry.h"
#endif

#include <qfile.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>
#include <qtooltip.h>
#include <qlistview.h>

#include "totool.h"
#include "toconf.h"
#include "tonewconnection.h"
#include "tomain.h"
#include "toconnection.h"

#include "tonewconnection.moc"
#include "tonewconnectionui.moc"

toNewConnection::toNewConnection(QWidget* parent, const char* name,bool modal,WFlags fl)
  : toNewConnectionUI(parent,name,modal,fl),toHelpContext("newconnection.html")
{
  toHelp::connectDialog(this);

  Database->insertItem(toTool::globalConfig(CONF_DATABASE,DEFAULT_DATABASE));

  std::list<QString> lst=toConnectionProvider::providers();
  int sel=0,cur=0;
  QString provider=toTool::globalConfig(CONF_PROVIDER,DEFAULT_PROVIDER);
  for(std::list<QString>::iterator i=lst.begin();i!=lst.end();i++) {
    Provider->insertItem(*i);
    if (*i==provider)
      sel=cur;
    cur++;
  }
  if (cur==0) {
    TOMessageBox::information(this,
			      "No connection provider",
			      "No available connection provider, plugins probably missing");
    throw QString("Coudln't make connection");
  }
  Provider->setCurrentItem(sel);
  changeProvider();

  SqlNet->setChecked(!toTool::globalConfig(CONF_HOST,DEFAULT_HOST).isEmpty());
  changeHost();

  Username->setText(toTool::globalConfig(CONF_USER,DEFAULT_USER));
  Username->setFocus();

  bool pass=toTool::globalConfig(CONF_PASSWORD,DEFAULT_PASSWORD).isEmpty();
  if (pass)
    Password->setText(DEFAULT_PASSWORD);
  else
    Password->setText(toTool::globalConfig(CONF_PASSWORD,DEFAULT_PASSWORD));

  QString defdb=toTool::globalConfig(CONF_DATABASE,DEFAULT_DATABASE);
  Database->setEditable(true);

  {
    int maxHist=toTool::globalConfig(CONF_CONNECT_CURRENT,0).toInt();
    Previous->setSorting(-1);
    QListViewItem *last=NULL;
    for (int i=0;i<maxHist;i++) {
      QString path=CONF_CONNECT_HISTORY;
      path+=":";
      path+=QString::number(i);
      QString tmp=path;
      tmp+=CONF_USER;
      QString user=toTool::globalConfig(tmp,QString::null);

      tmp=path;
      tmp+=CONF_PASSWORD;
      QString passstr=(pass?QString(DEFAULT_PASSWORD):
                      (toTool::globalConfig(tmp,DEFAULT_PASSWORD)));

      tmp=path;
      tmp+=CONF_HOST;
      QString host=toTool::globalConfig(tmp,DEFAULT_HOST);

      tmp=path;
      tmp+=CONF_DATABASE;
      QString database=toTool::globalConfig(tmp,DEFAULT_DATABASE);

      tmp=path;
      tmp+=CONF_PROVIDER;
      QString provider=toTool::globalConfig(tmp,DEFAULT_PROVIDER);

      last=new QListViewItem(Previous,last,provider,host,database,user,passstr);
    }
  }
}

void toNewConnection::changeProvider(void)
{
  std::list<QString> hosts=toConnectionProvider::hosts(Provider->currentText());
  QString current=Host->currentText();

  Host->clear();
  bool sqlNet=false;
  for(std::list<QString>::iterator i=hosts.begin();i!=hosts.end();i++) {
    if ((*i).isEmpty())
      sqlNet=true;
    Host->insertItem(*i);
  }
  if (sqlNet) {
    HostLabel->hide();
    Host->hide();
    SqlNet->show();
  } else {
    HostLabel->show();
    Host->show();
    SqlNet->hide();
  }
  Host->lineEdit()->setText(current);
}

void toNewConnection::changeHost(void)
{
  std::list<QString> databases=toConnectionProvider::databases(Provider->currentText(),
							       Host->currentText());
  QString current=Database->currentText();

  Database->clear();
  for(std::list<QString>::iterator i=databases.begin();i!=databases.end();i++)
    Database->insertItem(*i);
  Database->lineEdit()->setText(current);
}

toConnection *toNewConnection::makeConnection(void)
{
  try {
    toTool::globalSetConfig(CONF_PROVIDER,Provider->currentText());
    toTool::globalSetConfig(CONF_USER,Username->text());
    QString pass;
    if (toTool::globalConfig(CONF_SAVE_PWD,DEFAULT_SAVE_PWD)!=DEFAULT_SAVE_PWD)
      pass=Password->text();
    else
      pass=DEFAULT_SAVE_PWD;
    toTool::globalSetConfig(CONF_PASSWORD,pass);
    toTool::globalSetConfig(CONF_DATABASE,Database->currentText());
    QString host;
    if (SqlNet->isHidden())
      host=Host->currentText();
    else
      host=SqlNet->isChecked()?QString("SQL*Net"):QString::null;
    toTool::globalSetConfig(CONF_HOST,host);
    toConnection *retCon=new toConnection(Provider->currentText(),
					  Username->text(),
					  Password->text(),
					  host,
					  Database->currentText(),
					  Mode->currentText());
    {
      for(QListViewItem *item=Previous->firstChild();item;item=item->nextSibling()) {
	if (item->text(0)==Provider->currentText()&&
	    ((item->text(1)==host)||(item->text(1).isEmpty()==host.isEmpty()))&&
	    item->text(2)==Database->currentText()&&
	    item->text(3)==Username->text()) {
	  delete item;
	  break;
	}
      }
    }
    new QListViewItem(Previous,NULL,
		      Provider->currentText(),
		      host,
		      Database->currentText(),
		      Username->text(),
		      pass);
    {
      int siz=toTool::globalConfig(CONF_CONNECT_SIZE,DEFAULT_CONNECT_SIZE).toInt();
      int i=0;
      for(QListViewItem *item=Previous->firstChild();item&&i<siz;item=item->nextSibling()) {
	QString path=CONF_CONNECT_HISTORY;
	path+=":";
	path+=QString::number(i);

	QString tmp=path;
	tmp+=CONF_PROVIDER;
	toTool::globalSetConfig(tmp,item->text(0));

	tmp=path;
	tmp+=CONF_HOST;
	toTool::globalSetConfig(tmp,item->text(1));

	tmp=path;
	tmp+=CONF_DATABASE;
	toTool::globalSetConfig(tmp,item->text(2));

	tmp=path;
	tmp+=CONF_USER;
	toTool::globalSetConfig(tmp,item->text(3));

	tmp=path;
	tmp+=CONF_PASSWORD;
	toTool::globalSetConfig(tmp,item->text(4));

	i++;
      }
      toTool::globalSetConfig(CONF_CONNECT_CURRENT,QString::number(i));
    }

    return retCon;
  } catch (const QString &exc) {
    QString str("Unable to connect to the database.\n");
    str.append(exc);
    TOMessageBox::information(this,
			      "Unable to connect to the database",
			      str);
    return NULL;
  }
}

void toNewConnection::historySelection(void)
{
  QListViewItem *item=Previous->selectedItem();
  if (item) {
    for (int i=0;i<Provider->count();i++)
      if (Provider->text(i)==item->text(0)) {
	Provider->setCurrentItem(i);
	break;
      }
    if (SqlNet->isHidden())
      Host->lineEdit()->setText(item->text(1));
    else
      SqlNet->setChecked(!item->text(1).isEmpty());
    Database->lineEdit()->setText(item->text(2));
    Username->setText(item->text(3));
    if(item->text(4)!=DEFAULT_PASSWORD)
      Password->setText(item->text(4));
  }
}
