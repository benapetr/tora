//***************************************************************************
/*
 * TOra - An Oracle Toolkit for DBA's and developers
 * Copyright (C) 2000-2001,2001 Underscore AB
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
 *      these libraries without written consent from Underscore AB. Observe
 *      that this does not disallow linking to the Qt Free Edition.
 *
 * All trademarks belong to their respective owners.
 *
 ****************************************************************************/

#ifndef TORESULTVIEW_H
#define TORESULTVIEW_H

#include "toeditwidget.h"
#include "toresult.h"

#include <qlistview.h>

#include <map>

class QListViewItem;
class QPopupMenu;
class TOPrinter;
class toListTip;
class toQuery;
class toResultCols;
class toResultView;
class toSQL;
class toSearchReplace;

/** Baseclass for filters to apply to the @ref toResultView to filter out
 * rows that you don't want to add as items to the list.
 */
class toResultFilter {
public:
  toResultFilter()
  { }
  virtual ~toResultFilter()
  { }
  /** This function can inspect the item to be added and decide if it is
   * valid for adding or not.
   * @param item Item to inspect.
   * @return If false is returned the item isn't added.
   */
  virtual bool check(const QListViewItem *item) = 0;
  /** Create a copy of this filter.
   * @return A newly created copy of this filter.
   */
  virtual toResultFilter *clone(void) = 0;
  /** Export data to a map.
   * @param data A map that can be used to recreate the data of a chart.
   * @param prefix Prefix to add to the map.
   */
  virtual void exportData(std::map<QString,QString> &data,const QString &prefix)
  { }
  /** Import data
   * @param data Data to read from a map.
   * @param prefix Prefix to read data from.
   */
  virtual void importData(std::map<QString,QString> &data,const QString &prefix)
  { }
};

/** An item to display in a toListView or toResultView. They differ from normal
 * QListViewItems in that they can have a tooltip and actually contain more text
 * than is displayed in the cell of the listview.
 */
class toResultViewItem : public QListViewItem {
public:
  /** Create a new item.
   * @param parent Parent list view.
   * @param after Insert after this item.
   * @param buffer String to set as first column
   */
  toResultViewItem(QListView *parent,QListViewItem *after,const QString &buf=QString::null)
    : QListViewItem(parent,after,QString::null)
  { if (buf) setText(0,buf); }
  /** Create a new item.
   * @param parent Parent to this item.
   * @param after Insert after this item.
   * @param buffer String to set as first column
   */
  toResultViewItem(QListViewItem *parent,QListViewItem *after,const QString &buf=QString::null)
    : QListViewItem(parent,after,QString::null)
  { if (buf) setText(0,buf); }
  /** Reimplemented for internal reasons.
   */
  virtual void paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align);
  /** Reimplemented for internal reasons.
   */
  virtual QString text(int col) const;
  /** String to sort the data on. This is reimplemented so that numbers are sorted as numbers
   * and not as strings.
   * @param col Column
   * @param asc Wether to sort ascending or not.
   */
  virtual QString key(int col,bool asc) const;
  /** Reimplemented for internal reasons.
   */
  virtual int width(const QFontMetrics &fm, const QListView *top, int column) const;
  /** Get all text for this item. This is used for copying, drag & drop and memo editing etc.
   * @param col Column.
   * @return All of the text.
   */
  virtual QString allText(int col) const
  { return QListViewItem::text(col); }
  /** Get the text to be displayed as tooltip for this item.
   * @param col Column.
   * @return The text to display as tooltip.
   */
  virtual QString tooltip(int col) const
  { return allText(col); }
};

/** This item expands the height to commodate all lines in the input buffer.
 */
class toResultViewMLine : public toResultViewItem {
private:
  /** Number of lines in the largest row.
   */
  int Lines;
public:
  /** Create a new item.
   * @param parent Parent list view.
   * @param after Insert after this item.
   * @param buffer String to set as first column
   */
  toResultViewMLine(QListView *parent,QListViewItem *after,const QString &buf=QString::null)
    : toResultViewItem(parent,after,QString::null)
  { Lines=1; if (buf) setText(0,buf); }
  /** Create a new item.
   * @param parent Parent to this item.
   * @param after Insert after this item.
   * @param buffer String to set as first column
   */
  toResultViewMLine(QListViewItem *parent,QListViewItem *after,const QString &buf=QString::null)
    : toResultViewItem(parent,after,QString::null)
  { Lines=1; if (buf) setText(0,buf); }
  /** Reimplemented for internal reasons.
   */
  virtual void setText (int,const QString &);
  /** Reimplemented for internal reasons.
   */
  virtual void setup(void);
  /** Reimplemented for internal reasons.
   */
  virtual QString text(int col) const
  { return QListViewItem::text(col); }
  /** Reimplemented for internal reasons.
   */
  virtual QString allText(int col) const
  { return QListViewItem::text(col); }
  /** Reimplemented for internal reasons.
   */
  virtual int width(const QFontMetrics &fm, const QListView *top, int column) const;
  /** Reimplemented for internal reasons.
   */
  virtual void paintCell (QPainter *pnt,const QColorGroup & cg,int column,int width,int alignment);
};

/** An item to display in a toListView or toResultView. They differ from normal
 * QListViewItems in that they can have a tooltip and actually contain more text
 * than is displayed in the cell of the listview.
 */
class toResultViewCheck : public QCheckListItem {
public:
  /** Create a new item.
   * @param parent Parent list view.
   * @param text Text of first column.
   * @param type Type of check on this item.
   */
  toResultViewCheck(QListView *parent,const QString &text,QCheckListItem::Type type=Controller)
    : QCheckListItem(parent,QString::null,type)
  { if (!text.isNull()) setText(0,text); }
  /** Create a new item.
   * @param parent Parent item.
   * @param text Text of first column.
   * @param type Type of check on this item.
   */
  toResultViewCheck(QListViewItem *parent,const QString &text,QCheckListItem::Type type=Controller)
    : QCheckListItem(parent,QString::null,type)
  { if (!text.isNull()) setText(0,text); }
  /** Reimplemented for internal reasons.
   */
  virtual void paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align);
  /** Reimplemented for internal reasons.
   */
  virtual QString text(int col) const;
  /** String to sort the data on. This is reimplemented so that numbers are sorted as numbers
   * and not as strings.
   * @param col Column
   * @param asc Wether to sort ascending or not.
   */
  virtual QString key(int col,bool) const;
  /** Reimplemented for internal reasons.
   */
  virtual int width(const QFontMetrics &fm, const QListView *top, int column) const;
  /** Get all text for this item. This is used for copying, drag & drop and memo editing etc.
   * @param col Column.
   * @return All of the text.
   */
  virtual QString allText(int col) const
  { return QCheckListItem::text(col); }
  /** Get the text to be displayed as tooltip for this item.
   * @param col Column.
   * @return The text to display as tooltip.
   */
  virtual QString tooltip(int col) const
  { return allText(col); }
};

/** This item expands the height to commodate all lines in the input buffer.
 */
class toResultViewMLCheck : public toResultViewCheck {
private:
  /** Number of lines in the largest row.
   */
  int Lines;
public:
  /** Create a new item.
   * @param parent Parent list view.
   * @param text Text of first column.
   * @param type Type of check on this item.
   */
  toResultViewMLCheck(QListView *parent,const QString &text,QCheckListItem::Type type=Controller)
    : toResultViewCheck(parent,QString::null,type)
  { Lines=1; if (!text.isNull()) setText(0,text); }
  /** Create a new item.
   * @param parent Parent item.
   * @param text Text of first column.
   * @param type Type of check on this item.
   */
  toResultViewMLCheck(QListViewItem *parent,const QString &text,QCheckListItem::Type type=Controller)
    : toResultViewCheck(parent,QString::null,type)
  { Lines=1; if (!text.isNull()) setText(0,text); }
  /** Reimplemented for internal reasons.
   */
  virtual void setup(void);
  /** Reimplemented for internal reasons.
   */
  virtual void setText (int,const QString &);
  /** Reimplemented for internal reasons.
   */
  virtual QString text(int col) const
  { return QListViewItem::text(col); }
  /** Reimplemented for internal reasons.
   */
  virtual QString allText(int col) const
  { return QListViewItem::text(col); }
  /** Reimplemented for internal reasons.
   */
  virtual int width(const QFontMetrics &fm, const QListView *top, int column) const;
  /** Reimplemented for internal reasons.
   */
  virtual void paintCell (QPainter *pnt,const QColorGroup & cg,int column,int width,int alignment);
};

/**
 * The TOra implementation of a listview which offers a few extra goodies to the baseclass.
 * First of all tooltip which can display contents that doesn't fit in the list, printing,
 * integration into toMain with Edit menu etc, drag & drop, export as file, display item
 * as memo and context menu.
 */
class toListView : public QListView,public toEditWidget {
  Q_OBJECT

  /** Name of this list, used primarily when printing. Also used to be able to edit
   * SQL displayed in @ref toResultView.
   */
  QString Name;
  /** Used to display tip on fields.
   */
  toListTip *AllTip;
  /** Item selected when popup menu displayed.
   */
  QListViewItem *MenuItem;
  /** Column of item selected when popup menu displayed.
   */
  int MenuColumn;
  /** Popup menu if available.
   */
  QPopupMenu *Menu;
  /** Last move, used to determine if drag has started.
   */
  QPoint LastMove;

  /** Reimplemented for internal reasons.
   */
  virtual void contentsMouseDoubleClickEvent (QMouseEvent *e);
  /** Reimplemented for internal reasons.
   */
  virtual void contentsMousePressEvent(QMouseEvent *e);
  /** Reimplemented for internal reasons.
   */
  virtual void contentsMouseReleaseEvent(QMouseEvent *e);
  /** Reimplemented for internal reasons.
   */
  virtual void contentsMouseMoveEvent (QMouseEvent *e);

  /** Used to print one page of the list.
   * @param printer Printer to print to.
   * @param painter Painter to print page to.
   * @param top Item at top of page.
   * @param column Column to start printing at. Will be changed to where you are when done.
   * @param level The indentation level of the top item.
   * @param pageNo Page number.
   * @param paint If just testing to determine how many pages are needed set this to false.
   * @return The next item to print to (Pass as top to this function).
   */
  virtual QListViewItem *printPage(TOPrinter *printer,QPainter *painter,QListViewItem *top,
				   int &column,int &level,int pageNo,bool paint=true);
  int exportType(void);
public:
  /** Create new list view.
   * @param parent Parent of list.
   * @param name Name of list.
   */
  toListView(QWidget *parent,const char *name=NULL);

  /** Get SQL name of list.
   */
  virtual QString sqlName(void)
  { return Name; }
  /** Set SQL name of list.
   */
  virtual void setSQLName(const QString &name)
  { Name=name; }
  /** Get the whole text for the item and column selected when menu was poped up.
   */
  QString menuText(void);

  /** Print this list
   */
  virtual void editPrint(void);
  /** Reimplemented for internal reasons.
   */
  virtual void editSearch(toSearchReplace *);
  /** Reimplemented for internal reasons.
   */
  virtual void focusInEvent (QFocusEvent *e);
  /** The string to be displayed in the middle of the footer when printing.
   * @return String to be placed in middle.
   */
  virtual QString middleString()
  { return QString::null; }
  /** Adds option to add menues to the popup menu before it is displayed.
   * @param menu Menu to add entries to.
   */
  virtual void addMenues(QPopupMenu *menu)
  { }
  /** Export list as a string.
   * @param includeHeader Include header.
   * @param onlySelection Only include selection.
   * @param type Format of exported list.
   */
  virtual QString exportAsText(bool includeHeader,bool onlySelection,int type=-1);
  /** Export list as file.
   */
  virtual bool editSave(bool ask);

  /** Export data to a map.
   * @param data A map that can be used to recreate the data of a chart.
   * @param prefix Prefix to add to the map.
   */
  virtual void exportData(std::map<QString,QString> &data,const QString &prefix);
  /** Import data
   * @param data Data to read from a map.
   * @param prefix Prefix to read data from.
   */
  virtual void importData(std::map<QString,QString> &data,const QString &prefix);
  /** Create transposed copy of list
   * @return Pointer to newly allocated transposed listview.
   */
  virtual toListView *copyTransposed(void);
public slots:
  /** Display the menu at the given point and column.
   * @param item Item to display.
   * @param pnt Point to display menu at.
   * @param col Column to display menu for.
   */
  virtual void displayMenu(QListViewItem *item,const QPoint &pnt,int col);
  /** Display memo of selected menu column
   */
  virtual void displayMemo(void);
protected slots:
  /** Callback when menu is selected. If you override this make sure you
   * call the parents function when you have parsed your entries.
   * @param id ID of the menu item selected.
   */
  virtual void menuCallback(int id);
};

/**
 * This class defines a list which displays the result of a query.
 *
 * One special thing to know about this class is that columns at the end in which the
 * description start with a '-' characters are not displayed.
 */

class toResultView : public toListView, public toResult {
  Q_OBJECT

  /** Reimplemented for internal reasons.
   */
  virtual void keyPressEvent (QKeyEvent * e);
protected:
  /** Connection to execute statement on.
   */
  toQuery *Query;
  /** Last added item.
   */
  QListViewItem *LastItem;

  /** Number of rows in list.
   */
  int RowNumber;
  /** If column names are to be made more readable.
   */
  bool ReadableColumns;
  /** Wether to display first number column or not.
   */
  bool NumberColumn;
  /** If all the available data should be read at once.
   */
  bool ReadAll;
  /** Input filter if any.
   */
  toResultFilter *Filter;

  /** Setup the list.
   * @param readable Wether to display first number column or not.
   * @param dispCol Wether to display first number column or not.
   */
  void setup(bool readable,bool dispCol);

  /** Check if end of query is detected yet or not.
   */
  virtual bool eof(void);

public:
  /** Create list.
   * @param readable Indicate if columns are to be made more readable. This means that the
   * descriptions are capitalised and '_' are converted to ' '.
   * @param numCol If number column is to be displayed.
   * @param parent Parent of list.
   * @param name Name of widget.
   */
  toResultView(bool readable,bool numCol,QWidget *parent,const char *name=NULL);
  /** Create list. The columns are not readable and the number column is displayed.
   * @param parent Parent of list.
   * @param name Name of widget.
   */
  toResultView(QWidget *parent,const char *name=NULL);
  ~toResultView();

  /** Set the read all flag.
   * @param all New value of flag.
   */
  void setReadAll(bool all)
  { ReadAll=all;}

  /** Get read all flag
   * @return Value of read all flag.
   */
  virtual void editReadAll(void);

  /** Get the number of columns in query.
   * @return Columns in query.
   */
  int queryColumns() const;

  /** Set a filter to this list.
   * @param filter The new filter or NULL if no filter is to be used.
   */
  void setFilter(toResultFilter *filter)
  { Filter=filter; }
  /** Get the current filter.
   * @return Current filter or NULL if no filter.
   */
  toResultFilter *filter(void)
  { return Filter; }

  /** Get number column flag.
   * @return Wether or not the numbercolumn is displayed.
   */
  bool numberColumn() const
  { return NumberColumn; }
  /** Set number column flag. Don't change this while a query is running. Observe
   * that not all descendants of this class support changing this on the fly. The base
   * class and @ref toResultLong does though.
   * @param val New value of number column.
   */
  void setNumberColumn(bool val)
  { NumberColumn=val; }

  /** Get readable column flag.
   * @return Wether or not the readable column names.
   */
  bool readableColumn() const
  { return ReadableColumns; }
  /** Set readable column flag.
   */
  void setReadableColumns(bool val)
  { ReadableColumns=val; }

  /** Create a new item in this list. Can be used if a special kind of item is wanted
   * in the list. The rest of the columns will be filled with setText.
   * @param last Where to insert the item.
   * @param str String to set first column to.
   * @return Allocated item.
   */
  virtual QListViewItem *createItem(QListViewItem *last,const QString &str)
  { return new toResultViewItem(this,last,str); }

  /** Reimplemented for internal reasons.
   */
  virtual void query(const QString &sql,const toQList &param);

  /** Get SQL name of list.
   */
  virtual QString sqlName(void)
  { return toListView::sqlName(); }
  /** Set SQL name of list.
   */
  virtual void setSQLName(const QString &name)
  { toListView::setSQLName(name); }

  // Why are these needed?
#if 1
  /** Set the SQL statement of this list
   * @param sql String containing statement.
   */
  void setSQL(const QString &sql)
  { toResult::setSQL(sql); }
  /** Set the SQL statement of this list. This will also affect @ref Name.
   * @param sql SQL containing statement.
   */
  void setSQL(const toSQL &sql)
  { toResult::setSQL(sql); }
  /** Set new SQL and run query.
   * @param sql New sql.
   * @see setSQL
   */
  void query(const QString &sql)
  { toResult::query(sql); }
  /** Set new SQL and run query.
   * @param sql New sql.
   * @see setSQL
   */
  void query(const toSQL &sql)
  { toResult::query(sql); }
  /** Set new SQL and run query.
   * @param sql New sql.
   * @see setSQL
   */
  void query(const toSQL &sql,toQList &par)
  { toResult::query(sql,par); }
#endif

  /** Reimplemented for internal reasons.
   */
  virtual void editPrint(void)
  { editReadAll(); toListView::editPrint(); }
  /** Reimplemented for internal reasons.
   */
  virtual QString middleString();

  /** Reimplemented for internal reasons.
   */
  virtual void addMenues(QPopupMenu *);
public slots:
  /** Reimplemented for internal reasons.
   */
  virtual void refresh(void)
  { toResult::refresh(); }
  /** Reimplemented for internal reasons.
   */
  virtual void changeParams(const QString &Param1)
  { toResult::changeParams(Param1); }
  /** Reimplemented For internal reasons.
   */
  virtual void changeParams(const QString &Param1,const QString &Param2)
  { toResult::changeParams(Param1,Param2); }
  /** Reimplemented for internal reasons.
   */
  virtual void changeParams(const QString &Param1,const QString &Param2,const QString &Param3)
  { toResult::changeParams(Param1,Param2,Param3); }
  /** Try to add an item to the list if available.
   */
  virtual void addItem(void);
  /** Handle any connection by default
   */
  virtual bool canHandle(toConnection &)
  { return true; }
protected slots:
  /** Reimplemented for internal reasons.
   */
  virtual void menuCallback(int);
};

#endif
