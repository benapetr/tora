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

#ifndef __TOBROWSER_H
#define __TOBROWSER_H

#include <qvbox.h>
#include <map>

#include "totemplate.h"

class QComboBox;
class QTabWidget;
class toResult;
class toResultView;
class QListViewItem;
class toResultFilter;

class toBrowser : public QVBox {
  Q_OBJECT

  toConnection &Connection;

  QComboBox *Schema;
  QTabWidget *TopTab;

  QString SecondText;
  toResultView *FirstTab;
  toResult *SecondTab;
  toResultFilter *Filter;

  map<QString,toResultView *> Map;
  map<QString,toResult *> SecondMap;
  void setNewFilter(toResultFilter *filter);
public:
  toBrowser(QWidget *parent,toConnection &connection);
  virtual ~toBrowser();

public slots:
  void refresh(void);
  void changeSchema(int)
  { refresh(); }
  void changeTab(QWidget *tab);
  void changeSecondTab(QWidget *tab);
  void changeItem(QListViewItem *item);
  void clearFilter(void);
  void defineFilter(void);
};

class toBrowseTemplate : public QObject,public toTemplateProvider {
  Q_OBJECT

  list<toTemplateItem *> Parents;
  bool Registered;
public:
  toBrowseTemplate(void)
    : toTemplateProvider()
  { Registered=false; }
  void insertItems(QListView *parent);
  void removeItem(QListViewItem *item);
public slots:
  void addDatabase(const QString &);
  void removeDatabase(const QString &); 
};

#endif
