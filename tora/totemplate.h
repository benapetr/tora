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

#ifndef __TOTEMPLATE_H
#define __TOTEMPLATE_H

#include <list>
#include <map>

#include <qvbox.h>
#include "toresultview.h"

class QListView;
class QListViewItem;
class QSplitter;
class QTextView;
class toConnection;
class toListView;
class toTemplateItem;
class toTemplateProvider;

/** Not part of the API.
 */

class toTemplate : public QVBox {
  Q_OBJECT

  QSplitter *Splitter;
  toListView *List;
  QWidget *WidgetExtra;
  QVBox *Frame;
public:
  toTemplate(QWidget *parent);
  virtual ~toTemplate();
  void setWidget(QWidget *widget);
  QWidget *widget(void)
  { return WidgetExtra; }
  QWidget *frame(void)
  { return Frame; }

  static QWidget *parentWidget(QListViewItem *item);
  static toTemplate *templateWidget(QListViewItem *item);
  static toTemplate *templateWidget(QListView *obj);

public slots:
  void expand(QListViewItem *item);
  void collapse(QListViewItem *item);
};

/**
 * This is the base class of a provider of template items. Usually it is created with
 * a static object just like the tools.
 */

class toTemplateProvider {
  /** List of currently available template providers.
   */
  static list<toTemplateProvider *> *Providers;
public:
  toTemplateProvider();
  virtual ~toTemplateProvider()
  { }

  /** Insert a parent item for this template provider into a list view.
   * @param parent The list into which to insert the item.
   */
  virtual void insertItems(QListView *parent)=0;

  friend class toTemplate;
};

/** This is an item that is contained in a template. It is different in the expand and
 * collapse methods that are called when this item is expanded or collapsed from the
 * template widget.
 */
class toTemplateItem : public toResultViewItem {
  /** The provider responsible for this item.
   */
  toTemplateProvider &Provider;
public:
  /** Create an item.
   * @param prov Provider for this item.
   * @param parent Parent of this item.
   * @param name Contents of the first column of the item.
   */
  toTemplateItem(toTemplateProvider &prov,QListView *parent,const QString &name)
    : toResultViewItem(parent,NULL,name),Provider(prov)
  { }
  /** Create an item.
   * @param parent Parent of this item.
   * @param name Contents of the first column of the item.
   */
  toTemplateItem(toTemplateItem *parent,const QString &name)
    : toResultViewItem(parent,NULL,name),Provider(parent->provider())
  { }
  /** Get a reference to the provider of this item.
   * @return Reference to provider.
   */
  toTemplateProvider &provider(void)
  { return Provider; }
  /** This function is called when the item is expanded and can be used to fill up the
   * list when needed.
   */
  virtual void expand(void)
  { }
  /** This function is called when the item is collapsed.
   */
  virtual void collapse(void)
  { }
  /** This function can be used to return a widget that is displays extra information for
   * this item. Return NULL if no widget is to be displayed. The widget will be deleted
   * when this item is no longer shown.
   */
  virtual QWidget *selectedWidget(QWidget *parent)
  { return NULL; }
  /** Reimplemented for internal reasons.
   */
  virtual void setSelected(bool sel);
};

/** This class represent a template item that have an extra text to describe it if it is
 * selected.
 */

class toTemplateText : public toTemplateItem {
  /** The note to display.
   */
  const QString Note;
public:
  /** Create an item.
   * @param parent Parent of this item.
   * @param name Contents of the first column of the item.
   * @param note Extra text to display if item is selected.
   */
  toTemplateText(toTemplateItem *parent,const QString &name,const QString &note)
    : toTemplateItem(parent,name), Note(note)
  { }
  /** Reimplemented for internal reasons.
   */
  virtual QWidget *selectedWidget(QWidget *parent);
};

/** This class represent an item that when expanded will execute an SQL statement
 * and create child items which are the result of the query.
 */

class toTemplateSQL : public toTemplateItem {
  /** Connection to run statement in
   */
  toConnection &Connection;
  /** Statement to run.
   */
  QCString SQL;
public:
  /** Create an item.
   * @param conn Connection to query.
   * @param parent Parent of this item.
   * @param name Contents of the first column of the item.
   * @param sql SQL statement, observe that it is in @ref QCString format and you
   *            should use utf8 if converting from QString.
   */
  toTemplateSQL(toConnection &conn,toTemplateItem *parent,
		const QString &name,const QCString &sql)
    : toTemplateItem(parent,name),Connection(conn),SQL(sql)
  { setExpandable(true); }
  /** Get connection of this item.
   * @return Reference to connection.
   */
  toConnection &connection()
  { return Connection; }
  /** Create a child of this item.
   * @param name Name of the child.
   * @return A newly created item.
   */
  virtual toTemplateItem *createChild(const QString &name)
  { return new toTemplateItem(this,name); }
  /** Get parameters to pass to query.
   * @return List of strings to pass as input parameters to query
   */
  virtual list<QString> parameters(void)
  { list<QString> ret; return ret; }
  /** Reimplemented for internal reasons.
   */
  virtual void expand(void);
};

#endif
