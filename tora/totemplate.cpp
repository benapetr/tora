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

#include "utils.h"

#include "toconf.h"
#include "toconnection.h"
#include "tohelp.h"
#include "tomarkedtext.h"
#include "tonoblockquery.h"
#include "toresultview.h"
#include "totemplate.h"
#include "totool.h"

#ifdef TO_KDE
#include <kfiledialog.h>
#endif

#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qsplitter.h>
#include <qtextview.h>
#include <qtimer.h>
#include <qtoolbar.h>

#include "totemplate.moc"
#include "totemplateaddfileui.moc"
#include "totemplateeditui.moc"
#include "totemplatesetupui.moc"

#include "icons/totemplate.xpm"

static std::map<QCString,QString> DefaultText(void)
{
  std::map<QCString,QString> def;
  QString file=toPluginPath();
#ifndef OAS
  file+=QString::fromLatin1("/sqlfunctions.tpl");
  def["PL/SQL Functions"]=file;
  file=toPluginPath();
  file+=QString::fromLatin1("/hints.tpl");
  def["Optimizer Hints"]=file;
  file=toPluginPath();
  file+=QString::fromLatin1("/log4plsql.tpl");
  def["Log4PL/SQL"]=file;
#endif
  return def;
}

class toTemplateEdit : public toTemplateEditUI, public toHelpContext {
  std::map<QCString,QString> &TemplateMap;
  std::map<QCString,QString>::iterator LastTemplate;
  void connectList(bool conn)
  {
    if (conn)
      connect(Templates,SIGNAL(selectionChanged()),this,SLOT(changeSelection()));
    else
      disconnect(Templates,SIGNAL(selectionChanged()),this,SLOT(changeSelection()));
  }
  QListViewItem *findLast(void)
  {
    QString name=(*LastTemplate).first;
    return toFindItem(Templates,name);
  }
  void allocateItem(void)
  {
    QStringList lst=QStringList::split(QString::fromLatin1(":"),Name->text());
    unsigned int li=0;
    QListViewItem *parent=NULL;
    for(QListViewItem *item=Templates->firstChild();item&&li<lst.count();) {
      if (item->text(0)==lst[li]) {
	li++;
	parent=item;
	item=item->firstChild();
      } else
	item=item->nextSibling();
    }
    while(li<lst.count()) {
      if (parent)
	parent=new QListViewItem(parent,lst[li]);
      else
	parent=new QListViewItem(Templates,lst[li]);
      li++;
    }
  }
  bool clearUnused(QListViewItem *first,const QCString &pre)
  {
    bool ret=false;
    while(first) {
      QListViewItem *delitem=first;
      QCString str=pre;
      if (!str.isEmpty())
	str+=":";
      str+=first->text(0).latin1();
      if (first->firstChild()&&clearUnused(first->firstChild(),str))
	delitem=NULL;
      if (delitem&&TemplateMap.find(str)!=TemplateMap.end())
	delitem=NULL;
      first=first->nextSibling();
      if (!delitem)
	ret=true;
      else
	delete delitem;
    }
    return ret;
  }
public:
  virtual void updateFromMap(void)
  {
    try {
      while(Templates->firstChild())
	delete Templates->firstChild();
      QListViewItem *last=NULL;
      int lastLevel=0;
      QStringList lstCtx;
      for(std::map<QCString,QString>::iterator i=TemplateMap.begin();i!=TemplateMap.end();i++) {
	QStringList ctx=QStringList::split(QString::fromLatin1(":"),QString::fromLatin1((*i).first));
	if (last) {
	  while(last&&lastLevel>=int(ctx.count())) {
	    last=last->parent();
	    lastLevel--;
	  }
	  while(last&&lastLevel>=0&&!toCompareLists(lstCtx,ctx,(unsigned int)lastLevel)) {
	    last=last->parent();
	    lastLevel--;
	  }
	}
	if (lastLevel<0)
	  throw qApp->translate("toTemplateEdit","Internal error, lastLevel < 0");
	while(lastLevel<int(ctx.count())-1) {
	  if (last)
	    last=new QListViewItem(last,ctx[lastLevel]);
	  else
	    last=new QListViewItem(Templates,ctx[lastLevel]);
	  last->setOpen(true);
	  lastLevel++;
	}
	if (last)
	  last=new QListViewItem(last,ctx[lastLevel]);
	else
	  last=new QListViewItem(Templates,ctx[lastLevel]);
	last->setOpen(true);
	if (i==LastTemplate)
	  last->setSelected(true);
	lstCtx=ctx;
	lastLevel++;
      }
    } catch(const QString &str) {
      toStatusMessage(str);
      reject();
    }
  }
  toTemplateEdit(std::map<QCString,QString> &pairs,QWidget *parent,const char *name=0)
    : toTemplateEditUI(parent,name,true,WStyle_Maximize),
      toHelpContext(QString::fromLatin1("template.html#editor")),
      TemplateMap(pairs)
  {
    toHelp::connectDialog(this);
    LastTemplate=TemplateMap.end();
    updateFromMap();
    Description->setWordWrap(toMarkedText::WidgetWidth);
  }
  virtual void remove(void)
  {
    if (LastTemplate!=TemplateMap.end()) {
      QListViewItem *item=findLast();
      TemplateMap.erase(LastTemplate);
      LastTemplate=TemplateMap.end();
      Name->setText(QString::null);
      Description->setText(QString::null);
      if (item) {
	connectList(false);
	clearUnused(Templates->firstChild(),"");
	connectList(true);
      }
    }
  }
  virtual void preview(void)
  {
    Preview->setText(Description->text());
  }
  QCString name(QListViewItem *item)
  {
    QCString str=item->text(0).latin1();
    for(item=item->parent();item;item=item->parent()) {
      str.prepend(":");
      str.prepend(item->text(0).latin1());
    }
    return str;
  }
  virtual void newTemplate(void)
  {
    changeSelection();
    LastTemplate=TemplateMap.end();
    Description->setText(QString::null);
    QListViewItem *item=Templates->selectedItem();
    if (item) {
      connectList(false);
      Templates->setSelected(item,false);
      connectList(true);
      item=item->parent();
    }
    QCString str;
    if (item) {
      str=name(item);
      str+=":";
    }
    Name->setText(str);
  }
  virtual void changeSelection(void)
  {
    bool update=false;
    if (LastTemplate!=TemplateMap.end()) {
      if (Name->text().latin1()!=(*LastTemplate).first||
	  Description->text()!=(*LastTemplate).second) {
	TemplateMap.erase(LastTemplate);
	TemplateMap[Name->text().latin1()]=Description->text();
	allocateItem();
	update=true;
      }
    } else if (!Name->text().isEmpty()) {
      TemplateMap[Name->text().latin1()]=Description->text();
      allocateItem();
      update=true;
    }
    LastTemplate=TemplateMap.end();

    QListViewItem *item=Templates->selectedItem();
    if (item) {
      QCString str=name(item);
      LastTemplate=TemplateMap.find(str);
      if (LastTemplate!=TemplateMap.end()) {
	Name->setText(QString::fromLatin1((*LastTemplate).first));
	Description->setText((*LastTemplate).second);
	Preview->setText((*LastTemplate).second);
      } else {
	Name->setText(QString::null);
	Description->clear();
	Preview->setText(QString::null);
      }
    } else
      LastTemplate=TemplateMap.end();
    clearUnused(Templates->firstChild(),"");
  }
};

class toTemplateAddFile : public toTemplateAddFileUI {
public:
  toTemplateAddFile(QWidget *parent,const char *name=0)
    : toTemplateAddFileUI(parent,name,true)
  {
    OkButton->setEnabled(false);
    toHelp::connectDialog(this);
  }
  virtual void browse(void)
  {
    QFileInfo file(Filename->text());
    QString filename=toOpenFilename(file.dirPath(),QString::fromLatin1("*.tpl"),this);
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
    : toTemplateSetupUI(parent,name),toSettingTab("template.html#setup"),Tool(tool)
  {
    std::map<QCString,QString> def=DefaultText();

    int tot=Tool->config("Number","-1").toInt();
    {
      for(int i=0;i<tot;i++) {
	QCString num=QString::number(i).latin1();
        QCString root=Tool->config(num,"").latin1();
        num+="file";
        QString file=Tool->config(num,"");
	new QListViewItem(FileList,root,file);
	if (def.find(root)!=def.end())
	  def.erase(def.find(root));
      }
    }
    for (std::map<QCString,QString>::iterator i=def.begin();i!=def.end();i++)
      new QListViewItem(FileList,(*i).first,(*i).second);
  }
  virtual void saveSetting(void)
  {
    int i=0;
    for(QListViewItem *item=FileList->firstChild();item;item=item->nextSibling()) {
      QCString nam=QString::number(i).latin1();
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
  virtual void editFile(void)
  {
    QListViewItem *item=FileList->selectedItem();
    if (item) {
      try {
	QString file=item->text(1);
	std::map<QCString,QString> pairs;
	try {
	  toTool::loadMap(file,pairs);
	} catch(...) {
	  if (TOMessageBox::warning(this,
				    qApp->translate("toTemplatePrefs","Couldn't open file."),
				    qApp->translate("toTemplatePrefs","Couldn't open file. Start on new file?"),
				    qApp->translate("toTemplatePrefs","&Ok"),
				    qApp->translate("toTemplatePrefs","Cancel"))==1)
	    return;
	}
	toTemplateEdit edit(pairs,this);
	if (edit.exec()) {
	  edit.changeSelection();
	  if (!toTool::saveMap(file,pairs))
	    throw qApp->translate("toTemplatePrefs","Couldn't write file");
	}
      } catch (const QString &str) {
	TOMessageBox::warning(this,
			      qApp->translate("toTemplatePrefs","Couldn't open file"),
			      str,
			      qApp->translate("toTemplatePrefs","&Ok"));
      }
    }
  }
  virtual void delFile(void)
  {
    delete FileList->selectedItem();
  }
};

class toTemplateTool : public toTool {
  TODock *Dock;
  toTemplate *Window;
protected:
  virtual char **pictureXPM(void)
  { return totemplate_xpm; }
public:
  toTemplateTool()
    : toTool(410,"SQL Template")
  { Dock=NULL; Window=NULL; }
  virtual const char *menuItem()
  { return "SQL Template"; }
  virtual QWidget *toolWindow(QWidget *,toConnection &)
  {
    if (!Dock||!Window) {
      Dock=toAllocDock(qApp->translate("toTemplateTool","Template"),QString::null,*toolbarImage());
      Window=new toTemplate(Dock);
    } else if (Dock->isHidden()) {
#if TO_KDE
      toAttachDock(Dock,Window,QMainWindow::Left);
#else
      Dock->show();
#endif
      Window->showResult(true);
    } else {
#if TO_KDE
      toAttachDock(Dock,Window,QMainWindow::Minimized);
#else
      Dock->hide();
#endif
      Window->showResult(false);
    }
    return NULL;
  }
  void closeWindow()
  { Dock=NULL; Window=NULL; }
  virtual QWidget *configurationTab(QWidget *parent)
  { return new toTemplatePrefs(this,parent); }
  virtual bool canHandle(toConnection &)
  { return true; }
};

static toTemplateTool TemplateTool;

QWidget *toTemplate::parentWidget(QListViewItem *item)
{
  try {
    return templateWidget(item)->frame();
  } catch(...) {
    return NULL;
  }
}

toTemplate *toTemplate::templateWidget(QListView *obj)
{
  QObject *lst=obj;
  while(lst) {
    toTemplate *tpl=dynamic_cast<toTemplate *>(lst);
    if (tpl)
      return tpl;
    lst=lst->parent();
  }
  throw tr("Not a toTemplate parent");
}

toTemplate *toTemplate::templateWidget(QListViewItem *item)
{
  return templateWidget(item->listView());
}

class toTemplateResult : public QVBox {
  toTemplate *Template;
public:
  toTemplateResult(TODock *parent,toTemplate *temp)
    : QVBox(parent),Template(temp)
  { }
  virtual ~toTemplateResult()
  { Template->closeFrame(); }
};

toTemplate::toTemplate(TODock *parent)
  : QVBox(parent),toHelpContext(QString::fromLatin1("template.html"))
{
  Toolbar=toAllocBar(this,tr("Template Toolbar"));

  List=new toListView(this);
  List->addColumn(tr("Template"));
  List->setRootIsDecorated(true);
  List->setSorting(0);
  List->setShowSortIndicator(false);
  List->setTreeStepSize(10);
  List->setSelectionMode(QListView::Single);
  List->header()->setStretchEnabled(true);
  TODock *dock;
  dock=Result=toAllocDock(tr("Template result"),
			  QString::null,
			  *TemplateTool.toolbarImage());
  Frame=new toTemplateResult(dock,this);

  connect(List,SIGNAL(expanded(QListViewItem *)),this,SLOT(expand(QListViewItem *)));
  connect(List,SIGNAL(collapsed(QListViewItem *)),this,SLOT(collapse(QListViewItem *)));
  connect(List,SIGNAL(doubleClicked(QListViewItem *)),this,SLOT(selected(QListViewItem *)));
  connect(List,SIGNAL(returnPressed(QListViewItem *)),this,SLOT(selected(QListViewItem *)));

  if (toTemplateProvider::Providers)
    for (std::list<toTemplateProvider *>::iterator i=toTemplateProvider::Providers->begin();
	 i!=toTemplateProvider::Providers->end();
	 i++)
      (*i)->insertItems(List,Toolbar);

  Toolbar->setStretchableWidget(new QLabel(Toolbar,TO_KDE_TOOLBAR_WIDGET));

  WidgetExtra=NULL;
  setWidget(NULL);

  setFocusProxy(List);
  toAttachDock(parent,this,QMainWindow::Left);
  toAttachDock((TODock *)Result,Frame,QMainWindow::Bottom);
}

toTemplate::~toTemplate()
{
  TemplateTool.closeWindow();
  delete Result;
}

void toTemplate::showResult(bool show)
{
  if (!Result)
    return;

  if (show) {
#if TO_KDE
    toAttachDock((TODock *)Result,Frame,QMainWindow::Bottom);
#else
    Result->show();
#endif
  } else {
#if TO_KDE
    toAttachDock((TODock *)Result,Frame,QMainWindow::Minimized);
#else
    Result->hide();
#endif
  }
}

void toTemplate::closeFrame(void)
{
  Result=NULL;
  Frame=NULL;
}

void toTemplate::expand(QListViewItem *item)
{
  try {
    toTemplateItem *ti=dynamic_cast<toTemplateItem *>(item);
    if (ti)
      ti->expand();
  } catch (...) {
  }
}

void toTemplate::selected(QListViewItem *item)
{
  try {
    toTemplateItem *ti=dynamic_cast<toTemplateItem *>(item);
    if (ti)
      ti->selected();
  } catch (...) {
  }
}

QWidget *toTemplate::frame(void)
{
  if (Result->isHidden())
    showResult(true);
  return Frame;
}

void toTemplate::collapse(QListViewItem *item)
{
  toTemplateItem *ti=dynamic_cast<toTemplateItem *>(item);
  if (ti)
    ti->collapse();
}


QWidget *toTemplateItem::selectedWidget(QWidget *)
{
  return NULL;
}

void toTemplateItem::setSelected(bool sel)
{
  try {
    toTemplate *temp=toTemplate::templateWidget(this);
    if (sel&&temp) {
      QWidget *frame=toTemplate::parentWidget(this);
      if (frame)
	temp->setWidget(selectedWidget(frame));
    }
  } TOCATCH
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

  if (WidgetExtra!=widget) {
    widget->show();
    if (WidgetExtra)
      delete WidgetExtra;

    WidgetExtra=widget;
  }
}

class toTextTemplate : toTemplateProvider {
public:
  toTextTemplate()
    : toTemplateProvider("Text")
  { }
  void addFile(QListView *parent,const QString &root,const QString &file);
  virtual void insertItems(QListView *parent,QToolBar *toolbar);
};

void toTextTemplate::insertItems(QListView *parent,QToolBar *)
{
  int tot=TemplateTool.config("Number","-1").toInt();
  std::map<QCString,QString> def=DefaultText();

  {
    for(int i=0;i<tot;i++) {
      QCString num=QString::number(i).latin1();
      QCString root=TemplateTool.config(num,"").latin1();
      num+="file";
      QString file=TemplateTool.config(num,"");
      addFile(parent,root,file);
      if (def.find(root)!=def.end())
	def.erase(def.find(root));
    }
  }
  for (std::map<QCString,QString>::iterator i=def.begin();i!=def.end();i++)
    addFile(parent,(*i).first,(*i).second);
}

void toTextTemplate::addFile(QListView *parent,const QString &root,const QString &file)
{
  std::map<QCString,QString> pairs;
  try {
    toTool::loadMap(file,pairs);
    toTemplateItem *last=new toTemplateItem(*this,parent,root);
    int lastLevel=0;
    QStringList lstCtx;
    for(std::map<QCString,QString>::iterator i=pairs.begin();i!=pairs.end();i++) {
      QStringList ctx=QStringList::split(QString::fromLatin1(":"),(*i).first);
      if (last) {
	while(last&&lastLevel>=int(ctx.count())) {
	  last=dynamic_cast<toTemplateItem *>(last->parent());
	  lastLevel--;
	}
	while(last&&lastLevel>=0&&!toCompareLists(lstCtx,ctx,(unsigned int)lastLevel)) {
	  last=dynamic_cast<toTemplateItem *>(last->parent());
	  lastLevel--;
	}
      }
      if (lastLevel<0)
	throw qApp->translate("toTemplate","Internal error, lastLevel < 0");
      while(lastLevel<int(ctx.count())-1) {
	last=new toTemplateItem(last,ctx[lastLevel]);
	lastLevel++;
      }
      last=new toTemplateText(last,ctx[lastLevel],(*i).second);
      lstCtx=ctx;
      lastLevel++;
    }
  } TOCATCH
}

toTemplateSQL::toTemplateSQL(toConnection &conn,toTemplateItem *parent,
			     const QString &name,const QString &sql)
  : toTemplateItem(parent,name),Object(this),Connection(conn),SQL(sql)
{
  setExpandable(true);
}

toTemplateSQLObject::toTemplateSQLObject(toTemplateSQL *parent)
  : Parent(parent)
{
  Query=NULL;
  connect(&Poll,SIGNAL(timeout()),this,SLOT(poll()));
}

void toTemplateSQL::expand(void)
{
  while(firstChild())
    delete firstChild();
  Object.expand();
}

void toTemplateSQLObject::expand(void)
{
  try {
    delete Query;
    Query=NULL;
    Query=new toNoBlockQuery(Parent->connection(),toQuery::Background,
			     Parent->SQL,Parent->parameters());
    Poll.start(100);
  } TOCATCH
}

void toTemplateSQLObject::poll(void)
{
  try {
    if (QApplication::activeModalWidget()) // Template is never in widget
      return;
    if (Query&&Query->poll()) {
      toQDescList desc=Query->describe();
      while(Query->poll()&&!Query->eof()) {
	Parent->createChild(Query->readValue());
	for (unsigned int j=1;j<desc.size();j++)
	  Query->readValue();
      }
      if (Query->eof()) {
	delete Query;
	Query=NULL;
	Poll.stop();
      }
    }
  } catch (const QString &str) {
    delete Query;
    Query=NULL;
    Poll.stop();
    toStatusMessage(str);
  }
}

toTemplateSQLObject::~toTemplateSQLObject()
{
  delete Query;
}

static toTextTemplate TextTemplate;
