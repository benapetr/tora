//***************************************************************************
/*
 * TOra - An Oracle Toolkit for DBA's and developers
 * Copyright (C) 2000-2001,2001 GlobeCom AB
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

#ifndef __TORESULTCONTENT_H
#define __TORESULTCONTENT_H

#include <list>
#include <qtable.h>
#include <qvbox.h>

#include "toresult.h"
#include "toeditwidget.h"

class toResultContent;
class toNoBlockQuery;
class toSearchReplace;

/** This widget allows the user to browse the contents of a table and also edit
 * the content. The table is specified by the first and second parameter in the query.
 * The sql is not used in the query.
 */

class toResultContentEditor : public QTable,public toEditWidget {
  Q_OBJECT

  /** Owner of table.
   */
  QString Owner;
  /** Tablename.
   */
  QString Table;
  /** Original values of rows currently being edited.
   */
  std::list<QString> OrigValues;
  /** Stream to read data from.
   */
  toNoBlockQuery *Query;
  toBackground Poll;
  int MaxNumber;
  /** Number of rows read from stream.
   */
  int Row;
  /** Current row of editing.
   */
  int CurrentRow;
  /** Current row of editing.
   */
  int NewRecordRow;
  /** Used to detect drag.
   */
  QPoint LastMove;
  
  /** Popup menu if available.
   */
  QPopupMenu *Menu;
  /** Column of item selected when popup menu displayed.
   */
  int MenuColumn;
  /** Row of item selected when popup menu displayed.
   */
  int MenuRow;
  
  /** Current sorting row.
   */
  int SortRow;
  /** Indicate if sorting ascending or descending.
   */
  bool SortRowAsc;
  
  /** Filter selection criteria
   */
  QString Criteria;
  /** Filter retrieve order
   */
  QString Order;

  /** Throw an exception about wrong usage.
   */
  void wrongUsage(void);
  
  /** Reimplemented for internal reasons.
   */
  virtual void drawContents(QPainter * p,int cx,int cy,int cw,int ch);
  /** Reimplemented for internal reasons.
   */
  virtual QWidget *beginEdit(int row,int col,bool replace);
  /** Reimplemented for internal reasons.
   */
  virtual void paintCell(QPainter *p,int row,int col,const QRect &cr,bool selected);
  /** Reimplemented for internal reasons.
  */
  virtual void keyPressEvent(QKeyEvent *e);
  /** Reimplemented for internal reasons.
   */
  virtual void activateNextCell();
  
  /** Reimplemented for internal reasons.
   */
  virtual void dragEnterEvent(QDragEnterEvent *event);
  /** Reimplemented for internal reasons.
   */
  virtual void dropEvent(QDropEvent *event);
  /** Reimplemented for internal reasons.
   */
  virtual void contentsMousePressEvent(QMouseEvent *e);
  /** Reimplemented for internal reasons.
   */
  virtual void contentsMouseReleaseEvent(QMouseEvent *e);
  /** Reimplemented for internal reasons.
   */
  virtual void contentsMouseMoveEvent (QMouseEvent *e);
  /** Reimplemented for internal reasons.
   */
  virtual void focusInEvent (QFocusEvent *e);
  /** A setCurrentCell() replacement; makes sure that we have focus.
   */
  virtual void setCurrentCellFocus(int row, int col);

  QString table(void);

  toConnection &connection()
  { return toCurrentConnection(this); }

public:
  /** Create the widget.
   * @param parent Parent widget.
   * @param name Name of widget.
   */
  toResultContentEditor(QWidget *parent,const char *name=NULL);
  /** Destruct object
   */
  ~toResultContentEditor();
  /** Reimplemented for internal reasons.
   */
  virtual void query(const QString &sql,const toQList &param)
  { wrongUsage(); }
  /** Reimplemented for internal reasons.
   */
  virtual void changeParams(const QString &Param1)
  { wrongUsage(); }
  /** Reimplemented for internal reasons.
   */
  virtual void changeParams(const QString &Param1,const QString &Param2,const QString &Param3)
  { wrongUsage(); }
  
  /** Print this editor.
   */
  virtual void editPrint(void);
  /** Reimplemented for internal reasons.
   */
  virtual void editSave(bool ask);
  /** Reimplemented for internal reasons.
   */
  virtual void editSearch(toSearchReplace *search);
  /** Reimplemented for internal reasons.
   */
  virtual void editReadAll(void);

  virtual void changeFilter(const QString &criteria,const QString &order);

  friend class toResultContent;
public slots:
  /** Change sorting column
   * @param col Column selected to change as sorting.
   */
  virtual void changeSort(int col); 
  /** Reimplemented for internal reasons.
   */
  virtual void refresh(void)
  { QString t=Owner; Owner=QString::null; changeParams(t,Table); }
  /** Reimplemented for internal reasons.
   */
  virtual void changeParams(const QString &Param1,const QString &Param2);
  /** Current cell changed.
   * @param row New row.
   * @param col New column.
   */
  void changePosition(int row,int col);
  
  /** Display popup menu
   * @param p Point to display popup at.
   */
  virtual void displayMenu(const QPoint &p);
  /** Display editable memo viewer at current position.
   */
  virtual void displayMemo(void);
  /** Save unsaved changes in the editor
   */
  virtual void saveUnsaved(void);
  /** Delete the current row from the table.
   */
  virtual void deleteCurrent(void);
  /** Add a new record to the table.
   */
  virtual void addRecord(void);
  /** Discard the changes made to the table.
   */
  virtual void cancelEdit(void);
  /** Goto the last record in the table.
   */
  virtual void gotoLastRecord(void);
  /** Goto the first record in the table.
   */
  virtual void gotoFirstRecord(void);
  /** Goto the previous record in the table.
   */
  virtual void gotoPreviousRecord(void);
  /** Goto the next record in the table.
   */
  virtual void gotoNextRecord(void);
protected slots:
  /** Callback from popup menu.
   * @param cmd Command ID.
   */
  virtual void menuCallback(int cmd);
  /** Change data at specified position.
   * @param row Row to change.
   * @param col Column to change.
   * @param data New contents of data.
   */ 
  virtual void changeData(int row,int col,const QString &data); 
private slots:
  virtual void poll(void);
};

/** This widget allows the user to browse the contents of a table and also edit
 * the content. The table is specified by the first and second parameter in the query.
 * The sql is not used in the query.
 */

class toResultContent : public QVBox, public toResult {
  Q_OBJECT

  toResultContentEditor *Editor;
public:
  /** Create the widget.
   * @param parent Parent widget.
   * @param name Name of widget.
   */
  toResultContent(QWidget *parent,const char *name=NULL);

  /** Get content editor table widget
   * @return Pointer to editor.
   */
  toResultContentEditor *editor(void)
  { return Editor; }

  /** Read all rows from the table.
   */
  virtual void editReadAll(void)
  { Editor->editReadAll(); }
  /** Print the contents.
   */
  virtual void editPrint(void)
  { Editor->editPrint(); }
  /** Export contents to file.
   */
  virtual void editSave(bool ask)
  { Editor->editSave(ask); }
private slots:
  void changeFilter(void);
  void removeFilter(void)
  { Editor->changeFilter(QString::null,QString::null); }
public slots:
  /** Reimplemented for internal reasons.
   */
  virtual void refresh(void)
  { Editor->refresh(); }
  /** Reimplemented for internal reasons.
   */
  virtual void query(const QString &sql,const toQList &param)
  { Editor->query(sql,param); }
  /** Reimplemented for internal reasons.
   */
  virtual void changeParams(const QString &Param1)
  { Editor->changeParams(Param1); }
  /** Reimplemented for internal reasons.
   */
  virtual void changeParams(const QString &Param1,const QString &Param2)
  { Editor->changeParams(Param1,Param2); }
  /** Reimplemented for internal reasons.
   */
  virtual void changeParams(const QString &Param1,const QString &Param2,const QString &Param3)
  { Editor->changeParams(Param1,Param2,Param3); }
  /** Save unsaved changes in the editor
   */
  virtual void saveUnsaved(void)
  { Editor->saveUnsaved(); }
  /** Commit connection
   * @param conn Connection commit is made on.
   * @param cmt If commit or rollback
   */
  virtual void saveUnsaved(toConnection &conn,bool cmt);

  /** Handle Oracle & MySQL
   */
  virtual bool canHandle(toConnection &);
};

#endif
