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

#ifndef __TOMEMOEDITOR_H
#define __TOMEMOEDITOR_H

#include <qdialog.h>

class toMarkedText;

/** Displays an text in a widget which can optionally be modified and saved back.
 */

class toMemoEditor : public QDialog {
  Q_OBJECT

  /** Editor of widget
   */
  toMarkedText *Editor;
  /** Row of this field
   */
  int Row;
  /** Column of this field
   */
  int Col;
private slots:
  void openFile(void);
  void saveFile(void); 
public:
  /** Create this editor. If row and col is specified, editing is posible.
   * @param parent Parent widget.
   * @param data Data of editor.
   * @param row Optional location specifier, pass on in @ref changeData call.
   * @param col Optional location specifier, pass on in @ref changeData call.
   * @param sql Use SQL syntax highlighting of widget.
   * @param modal Display in modal dialog
   */
  toMemoEditor(QWidget *parent,const QString &data,int row=-1,int col=-1,
	       bool sql=false,bool modal=false);
  /** Get editor text.
   * @return String of editor.
   */
  QString text(void);
signals:
  /** Emitted when data is to be stored back to the creator
   * @param row Pass on from creation.
   * @param col Pass on from creation.
   * @param data New data to save.
   */
  void changeData(int row,int col,const QString &data);
public slots:
  void store(void);
};

#endif
