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

#ifndef __TOMARKEDTEXT_H
#define __TOMARKEDTEXT_H

#include <qglobal.h>

#include "tomain.h"

#if QT_VERSION < 300
#include <qmultilineedit.h>
#include "tomarkedtext.2.h"
#else
#include "qtlegacy/qttableview.h"
#include "qtlegacy/qtmultilineedit.h"
#include "tomarkedtext.3.h"
#endif
#include <qstring.h>

class TOPrinter;

/** This is the enhanced editor used in TOra. It mainly offers integration in the TOra
 * menues and printsupport in addition to normal QMultiLineEdit.
 */

class toMarkedText : public toMultiLineEdit {
  Q_OBJECT

  /** Indicates if redo is available.
   */
  bool RedoAvailable;
  /** Indicates if undo is available.
   */
  bool UndoAvailable;

  /** Filename of the file in this buffer.
   */
  QString Filename;
  /** Print one page to printer.
   * @param printer Printer to print to.
   * @param painter Painter to print to.
   * @param line Line at top of page.
   * @param offset Where one the drawn result this line starts.
   * @param pageNo Pagenumber.
   * @param paint Wether to paint or just test.
   */
  virtual int printPage(TOPrinter *printer,QPainter *painter,int line,int &offset,
			int pageNo,bool paint=true);
public:
  /** Create an editor.
   * @param parent Parent of this widget.
   * @param name Name of this widget.
   */
  toMarkedText(QWidget *parent,const char *name=NULL);
  ~toMarkedText();

  /** Get selected text. This function is now public.
   * @return The selected text.
   */
  QString markedText()
  { return toMultiLineEdit::markedText(); }
  /** Check if selection is available. This function is now public.
   * @return True if selection is available.
   */
  bool hasMarkedText()
  { return toMultiLineEdit::hasMarkedText(); }
  /** Erase the contents of the editor.
   */
  void clear(void)
  { Filename=""; RedoAvailable=false; UndoAvailable=false; setEdit(); toMultiLineEdit::clear(); setEdited(false); }

  /** Check if redo is available.
   * @return True if redo is available.
   */
  bool getRedoAvailable(void)
  { return RedoAvailable; }
  /** Check if undo is available.
   * @return True if undo is available.
   */
  bool getUndoAvailable(void)
  { return UndoAvailable; }
  /** Get location of the current selection. This function is now public. See the
   * Qt documentation for more information.
   */
  bool getMarkedRegion (int * line1,int * col1,int * line2,int * col2) const
  { return toMultiLineEdit::getMarkedRegion(line1,col1,line2,col2); }

  /** Get filename of current file in editor.
   * @return Filename of editor.
   */
  virtual const QString &filename(void) const
  { return Filename; }
  /** Set the current filename of the file in editor.
   * @param str String containing filename.
   */
  virtual void setFilename(const QString &str)
  { Filename=str; }
  /** Update user interface with availability of copy/paste etc.
   */
  virtual void setEdit(void);
  /** Reimplemented for internal reasons.
   */
  virtual void focusInEvent (QFocusEvent *e);
  /** Reimplemented for internal reasons.
   */
  virtual void focusOutEvent (QFocusEvent *e); 
  /** Reimplemented for internal reasons.
   */
  virtual void paintEvent(QPaintEvent *pe);
  /** Print this editor.
   */
  virtual void print(void);

private slots:
  void setRedoAvailable(bool avail)
  { RedoAvailable=avail; setEdit(); }
  void setUndoAvailable(bool avail)
  { UndoAvailable=avail; setEdit(); }
  void setCopyAvailable(bool avail)
  { setEdit(); }
};

#endif
