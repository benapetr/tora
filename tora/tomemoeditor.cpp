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

#include <qpixmap.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qheader.h>

#include "tomemoeditor.h"
#include "tohighlightedtext.h"
#include "tomarkedtext.h"
#include "tomain.h"
#include "toresultview.h"

#include "tomemoeditor.moc"

#include "icons/commit.xpm"
#include "icons/fileopen.xpm"
#include "icons/filesave.xpm"
#include "icons/cut.xpm"
#include "icons/copy.xpm"
#include "icons/paste.xpm"

#include "icons/forward.xpm"
#include "icons/next.xpm"
#include "icons/previous.xpm"
#include "icons/rewind.xpm"

class toMemoText : public toMarkedText {
  toMemoEditor *MemoEditor;
public:
  toMemoText(toMemoEditor *edit,QWidget *parent,const char *name=NULL)
    : toMarkedText(parent,name),MemoEditor(edit)
  { }
  /** Reimplemented for internal reasons.
   */
  virtual void keyPressEvent(QKeyEvent *e)
  {
    if (e->state()==ControlButton&&
	e->key()==Key_Return) {
      MemoEditor->store();
      e->accept();
    } else {
      toMarkedText::keyPressEvent(e);
    }
  }
};

class toMemoSQL : public toHighlightedText {
  toMemoEditor *MemoEditor;
public:
  toMemoSQL(toMemoEditor *edit,QWidget *parent,const char *name=NULL)
    : toHighlightedText(parent,name),MemoEditor(edit)
  { }
  /** Reimplemented for internal reasons.
   */
  virtual void keyPressEvent(QKeyEvent *e)
  {
    if (e->state()==ControlButton&&
	e->key()==Key_Return) {
      MemoEditor->store();
      e->accept();
    } else {
      toHighlightedText::keyPressEvent(e);
    }
  }
};

void toMemoEditor::openFile(void)
{
  Editor->editOpen();
}

void toMemoEditor::saveFile(void)
{
  Editor->editSave(true);
}

void toMemoEditor::setup(const QString &str,bool sql,bool modal)
{
  setMinimumSize(400,300);

  QBoxLayout *l=new QVBoxLayout(this);

  Toolbar=toAllocBar(this,"Memo Editor",QString::null);
  l->addWidget(Toolbar);

  if (sql)
    Editor=new toMemoSQL(this,this);
  else
    Editor=new toMemoText(this,this);
  l->addWidget(Editor);
  Editor->setText(str);
  Editor->setEdited(false);
  Editor->setReadOnly(Row<0||Col<0||listView());
  Editor->setFocus();

  QToolButton *btn;
  if (Row>=0&&Col>=0&&!listView()) {
    new QToolButton(QPixmap((const char **)commit_xpm),
		    "Save changes",
		    "Save changes",
		    this,SLOT(store(void)),
		    Toolbar);
    Toolbar->addSeparator();
    new QToolButton(QPixmap((const char **)fileopen_xpm),
		    "Open file",
		    "Open file",
		    this,SLOT(openFile()),Toolbar);
    new QToolButton(QPixmap((const char **)filesave_xpm),
		    "Save file",
		    "Save file",
		    this,SLOT(saveFile()),Toolbar);
    Toolbar->addSeparator();
    btn=new QToolButton(QPixmap((const char **)cut_xpm),
			"Cut to clipboard",
			"Cut to clipboard",
			Editor,SLOT(cut()),Toolbar);
    connect(Editor,SIGNAL(copyAvailable(bool)),
	    btn,SLOT(setEnabled(bool)));
    btn->setEnabled(false);
  } else {
    new QToolButton(QPixmap((const char **)filesave_xpm),
		    "Save file",
		    "Save file",
		    this,SLOT(saveFile()),Toolbar);
    Toolbar->addSeparator();
  }
  btn=new QToolButton(QPixmap((const char **)copy_xpm),
			     "Copy to clipboard",
			     "Copy to clipboard",
			     Editor,SLOT(copy()),Toolbar);
  connect(Editor,SIGNAL(copyAvailable(bool)),
	  btn,SLOT(setEnabled(bool)));
  btn->setEnabled(false);
  if (Row>=0&&Col>=0&&!listView())
    new QToolButton(QPixmap((const char **)paste_xpm),
		    "Paste from clipboard",
		    "Paste from clipboard",
		    Editor,SLOT(paste()),Toolbar);
  
  if (!modal)
    show();
}

toMemoEditor::toMemoEditor(QWidget *parent,const QString &str,int row,int col,
			   bool sql,bool modal,bool navigation)
  : QDialog(parent,NULL,modal,modal?0:WDestructiveClose),Row(row),Col(col)
{
  setup(str,sql,modal);

  toListView *lst=listView();
  if (lst||navigation) {

    new QToolButton(QPixmap((const char **)rewind_xpm),
		    "First column",
		    "First column",
		    this,SLOT(firstColumn()),Toolbar);
    new QToolButton(QPixmap((const char **)previous_xpm),
		    "Previous column",
		    "Previous column",
		    this,SLOT(previousColumn()),Toolbar);
    new QToolButton(QPixmap((const char **)next_xpm),
		    "Next column",
		    "Next column",
		    this,SLOT(nextColumn()),Toolbar);
    new QToolButton(QPixmap((const char **)forward_xpm),
		    "Last column",
		    "Last column",
		    this,SLOT(lastColumn()),Toolbar);
  }
  Label=new QLabel("",Toolbar);
  Label->setAlignment(AlignRight);
  Toolbar->setStretchableWidget(Label);

  if (lst) {
    connect(parent,SIGNAL(currentChanged(QListViewItem *)),
	    this,SLOT(changeCurrent(QListViewItem *)));
    Label->setText(lst->header()->label(Col));
  }
}

QString toMemoEditor::text(void)
{
  return Editor->text();
}

void toMemoEditor::store(void)
{
  if (Editor->isReadOnly())
    return;
  if (Editor->edited())
    emit changeData(Row,Col,Editor->text());
  accept();
}

void toMemoEditor::changePosition(int row,int cols)
{
  if (Editor->edited()) {
    emit changeData(Row,Col,Editor->text());
    Editor->setEdited(false);
  }

  toListView *lst=listView();
  if (lst)
    Label->setText(lst->header()->label(Col));

  Row=row;
  Col=cols;
}

toListView *toMemoEditor::listView(void)
{
  return dynamic_cast<toListView *>(parentWidget());
}

void toMemoEditor::firstColumn(void)
{
  toListView *lst=listView();
  if (lst) {
    QListViewItem *cur=lst->currentItem();
    if (!cur)
      return;

    if (Col==0) {
      QListViewItem *item=lst->firstChild();
      QListViewItem *next=item;
      while(next&&next!=cur) {
	item=next;
	if (item->firstChild())
	  next=item->firstChild();
	else if (item->nextSibling())
	  next=item->nextSibling();
	else {
	  next=item;
	  do {
	    next=next->parent();
	  } while(next&&!next->nextSibling());
	  if (next)
	    next=next->nextSibling();
	}
      }
      lst->setCurrentItem(item);
    } else {
      Col=0;
      Label->setText(lst->header()->label(Col));

      toResultViewItem *resItem=dynamic_cast<toResultViewItem *>(cur);
      toResultViewCheck *chkItem=dynamic_cast<toResultViewCheck *>(cur);
      if (resItem)
	Editor->setText(resItem->allText(Col));
      else if (chkItem)
	Editor->setText(chkItem->allText(Col));
      else
	Editor->setText(cur->text(Col));
    }
  }
}

void toMemoEditor::previousColumn(void)
{
  toListView *lst=listView();
  if (lst) {
    QListViewItem *cur=lst->currentItem();
    if (!cur)
      return;

    if (Col==0) {
      Col=lst->columns()-1;
      QListViewItem *item=lst->firstChild();
      QListViewItem *next=item;
      while(next&&next!=cur) {
	item=next;
	if (item->firstChild())
	  next=item->firstChild();
	else if (item->nextSibling())
	  next=item->nextSibling();
	else {
	  next=item;
	  do {
	    next=next->parent();
	  } while(next&&!next->nextSibling());
	  if (next)
	    next=next->nextSibling();
	}
      }
      lst->setCurrentItem(item);
    } else {
      Col--;
      toResultViewItem *resItem=dynamic_cast<toResultViewItem *>(cur);
      toResultViewCheck *chkItem=dynamic_cast<toResultViewCheck *>(cur);
      if (resItem)
	Editor->setText(resItem->allText(Col));
      else if (chkItem)
	Editor->setText(chkItem->allText(Col));
      else
	Editor->setText(cur->text(Col));
    }
    Label->setText(lst->header()->label(Col));
  }
}

void toMemoEditor::nextColumn(void)
{
  toListView *lst=listView();
  if (lst) {
    QListViewItem *cur=lst->currentItem();
    if (!cur)
      return;

    if (Col==lst->columns()-1) {
      Col=0;
      QListViewItem *next=cur;
      if (cur->firstChild())
	next=cur->firstChild();
      else if (cur->nextSibling())
	next=cur->nextSibling();
      else {
	next=cur;
	do {
	  next=next->parent();
	} while(next&&!next->nextSibling());
	if (next)
	  next=next->nextSibling();
      }
      if (next)
	lst->setCurrentItem(next);
    } else {
      Col++;
      toResultViewItem *resItem=dynamic_cast<toResultViewItem *>(cur);
      toResultViewCheck *chkItem=dynamic_cast<toResultViewCheck *>(cur);
      if (resItem)
	Editor->setText(resItem->allText(Col));
      else if (chkItem)
	Editor->setText(chkItem->allText(Col));
      else
	Editor->setText(cur->text(Col));
    }
    Label->setText(lst->header()->label(Col));
  }
}

void toMemoEditor::lastColumn(void)
{
  toListView *lst=listView();
  if (lst) {
    QListViewItem *cur=lst->currentItem();
    if (!cur)
      return;

    if (Col==lst->columns()-1) {
      QListViewItem *next=cur;
      if (cur->firstChild())
	next=cur->firstChild();
      else if (cur->nextSibling())
	next=cur->nextSibling();
      else {
	next=cur;
	do {
	  next=next->parent();
	} while(next&&!next->nextSibling());
	if (next)
	  next=next->nextSibling();
      }
      if (next)
	lst->setCurrentItem(next);
    } else {
      Col=lst->columns()-1;
      Label->setText(lst->header()->label(Col));
      toResultViewItem *resItem=dynamic_cast<toResultViewItem *>(cur);
      toResultViewCheck *chkItem=dynamic_cast<toResultViewCheck *>(cur);
      if (resItem)
	Editor->setText(resItem->allText(Col));
      else if (chkItem)
	Editor->setText(chkItem->allText(Col));
      else
	Editor->setText(cur->text(Col));
    }
  }
}

void toMemoEditor::changeCurrent(QListViewItem *item)
{
  toListView *lst=listView();
  if (lst) {
    QListViewItem *cur=lst->currentItem();
    if (!cur)
      return;

    toResultViewItem *resItem=dynamic_cast<toResultViewItem *>(cur);
    toResultViewCheck *chkItem=dynamic_cast<toResultViewCheck *>(cur);
    if (resItem)
      Editor->setText(resItem->allText(Col));
    else if (chkItem)
      Editor->setText(chkItem->allText(Col));
    else
      Editor->setText(cur->text(Col));
  }
}
