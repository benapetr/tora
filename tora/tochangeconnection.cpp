//***************************************************************************
/*
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
 *      software in the executable aside from Oracle client libraries. You
 *      are also allowed to link this program with the Qt Non Commercial for
 *      Windows.
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

#include <qpopupmenu.h>

#include "tomain.h"
#include "tochangeconnection.h"

#include "tochangeconnection.moc"
#include "icons/changeconnect.xpm"

static QPixmap *Pixmap;

toChangeConnection::toChangeConnection(QToolBar *parent,const char *name)
  : QToolButton(parent,name)
{
  if (!Pixmap)
    Pixmap=new QPixmap((const char **)changeconnect_xpm);

  setPopup(new QPopupMenu(this));
  connect(popup(),SIGNAL(aboutToShow()),this,SLOT(popupMenu()));
  connect(popup(),SIGNAL(activated(int)),this,SLOT(changeConnection(int)));
  setPopupDelay(0);
  setOffIconSet(*Pixmap);
  QToolTip::add(this,"Change the connection of the tool.");
}

void toChangeConnection::popupMenu(void)
{
  popup()->clear();
  toConnection &conn=toCurrentConnection(this);
  list<QString> cons=toMainWidget()->connections();
  int idx=0;
  for (list<QString>::iterator i=cons.begin();i!=cons.end();i++,idx++) {
    popup()->insertItem(*i,idx);
    if (conn.connectString()==*i)
      popup()->setItemChecked(idx,true);
  }
}

void toChangeConnection::changeConnection(int val)
{
  list<QString> cons=toMainWidget()->connections();
  list<QString>::iterator i=cons.begin();
  while (i!=cons.end()&&val>0) {
    i++;
    val--;
  }
  if (i==cons.end())
    throw QString("Couldn't find selected connection");
  QWidget *cur=parentWidget();
  while(cur) {
    try {
      toToolWidget *tool=dynamic_cast<toToolWidget *>(cur);
      if (tool) {
	tool->setConnection(toMainWidget()->connection(*i));
	return;
      }
    } catch(...) {
      // Catch problems with Visual C++ missing RTTI
    }
    cur=cur->parentWidget();
  }
  throw QString("Couldn't find parent connection. Internal error.");
}
