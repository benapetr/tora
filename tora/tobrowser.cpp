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

TO_NAMESPACE;

#include <qheader.h>
#include <qtabwidget.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qsplitter.h>
#include <qworkspace.h>
#include <qvaluelist.h>
#include <qbuttongroup.h>
#include <qregexp.h>
#include <qpopupmenu.h>
#include <qmenubar.h>

#ifdef TO_KDE
#  include <kmenubar.h>
#endif

#include "totool.h"
#include "tomain.h"
#include "tobrowser.h"
#include "toresultview.h"
#include "toresultcols.h"
#include "toresultconstraint.h"
#include "toresultreferences.h"
#include "toresultitem.h"
#include "toresultfield.h"
#include "toresultindexes.h"
#include "toresultcontent.h"
#include "toresultcontent.h"
#include "toresultdepend.h"
#include "tosql.h"
#include "tobrowserfilterui.h"
#include "tohelp.h"

#include "tobrowser.moc"
#include "tobrowserfilterui.moc"

#include "icons/tobrowser.xpm"
#include "icons/refresh.xpm"
#include "icons/schema.xpm"
#include "icons/index.xpm"
#include "icons/view.xpm"
#include "icons/table.xpm"
#include "icons/function.xpm"
#include "icons/sequence.xpm"
#include "icons/synonym.xpm"
#include "icons/filter.xpm"
#include "icons/nofilter.xpm"

class toBrowserTool : public toTool {
protected:
  virtual char **pictureXPM(void)
  { return tobrowser_xpm; }
public:
  toBrowserTool()
    : toTool(2,"Schema Browser")
  { }
  virtual const char *menuItem()
  { return "Schema Browser"; }
  virtual QWidget *toolWindow(QWidget *parent,toConnection &connection)
  {
    QWidget *window=new toBrowser(parent,connection);
    window->setIcon(*toolbarImage());
    return window;
  }
};

static toBrowserTool BrowserTool;

class toBrowserFilter : public toBrowserFilterUI {
public:
  class setting : public toResultFilter {
    int Type;
    bool IgnoreCase;
    bool Invert;
    QString Text;
    QRegExp Match;
  public:
    setting(int type=0,bool cas=false,bool invert=false,const QString &str=QString::null)
      : Type(type),IgnoreCase(cas),Invert(invert),Text(cas?str.upper():str)
    {
      if (!str.isEmpty()) {
	Match.setPattern(str);
	Match.setCaseSensitive(cas);
      }
    }
    virtual bool check(const QListViewItem *item)
    {
      QString str=item->text(0);
      switch(Type) {
      case 0:
	return true;
      case 1:
	if (IgnoreCase) {
	  if (str.upper().startsWith(Text))
	    return !Invert;
	} else if (str.startsWith(Text))
	  return !Invert;
	break;
      case 2:
	if (IgnoreCase) {
	  if (str.right(Text.length()).upper()==Text)
	    return !Invert;
	} else if (str.right(Text.length())==Text)
	  return !Invert;
	break;
      case 3:
	if (str.contains(Text,!IgnoreCase))
	  return !Invert;
	break;
      case 4:
	if (Match.match(str)>=0)
	  return !Invert;
	break;
      }
      return Invert;
    }
    virtual toResultFilter *clone(void)
    { return new setting(*this); }
    friend class toBrowserFilter;
  };
  toBrowserFilter(QWidget *parent)
    : toBrowserFilterUI(parent,"Filter Setting",true)
  {
    toHelp::connectDialog(this);
  }
  toBrowserFilter(const setting &cur,QWidget *parent)
    : toBrowserFilterUI(parent,"Filter Setting",true)
  {
    Buttons->setButton(cur.Type);
    String->setText(cur.Text);
    Invert->setChecked(cur.Invert);
    IgnoreCase->setChecked(cur.IgnoreCase);
  }
  setting *getSetting(void)
  {
    return new setting(Buttons->id(Buttons->selected()),
		       IgnoreCase->isChecked(),
		       Invert->isChecked(),
		       String->text());
  }
};

#define FIRST_WIDTH 150

#define TAB_TABLES		"Tables"
#define TAB_TABLE_COLUMNS	"TablesColumns"
#define TAB_TABLE_CONS		"TablesConstraint"
#define TAB_TABLE_DEPEND	"TablesDepend"
#define TAB_TABLE_INDEXES	"TablesIndexes"
#define TAB_TABLE_DATA		"TablesData"
#define TAB_TABLE_GRANTS	"TablesGrants"
#define TAB_TABLE_TRIGGERS	"TablesTriggers"
#define TAB_TABLE_INFO		"TablesInfo"
#define TAB_TABLE_COMMENT	"TablesComment"
#define TAB_VIEWS		"Views"
#define TAB_VIEW_COLUMNS	"ViewColumns"
#define TAB_VIEW_SQL		"ViewSQL"
#define TAB_VIEW_DATA		"ViewData"
#define TAB_VIEW_GRANTS		"ViewGrants"
#define TAB_VIEW_DEPEND		"ViewDepend"
#define TAB_VIEW_COMMENT	"ViewComment"
#define TAB_SEQUENCES		"Sequences"
#define TAB_SEQUENCES_INFO	"SequencesInfo"
#define TAB_INDEX		"Index"
#define TAB_INDEX_COLS		"IndexCols"
#define TAB_INDEX_INFO		"IndexInfo"
#define TAB_SYNONYM		"Synonym"
#define TAB_SYNONYM_INFO	"SynonymInfo"
#define TAB_PLSQL		"PLSQL"
#define TAB_PLSQL_SOURCE	"PLSQLSource"
#define TAB_PLSQL_BODY		"PLSQLBody"
#define TAB_PLSQL_DEPEND	"PLSQLDepend"
#define TAB_TRIGGER		"Trigger"
#define TAB_TRIGGER_INFO	"TriggerInfo"
#define TAB_TRIGGER_SOURCE	"TriggerSource"
#define TAB_TRIGGER_COLS	"TriggerCols"
#define TAB_TRIGGER_DEPEND	"TriggerDepend"

static toSQL SQLListTables("toBrowser:ListTables",
			   "SELECT Table_Name FROM ALL_TABLES WHERE OWNER = :f1<char[31]>\n"
			   " ORDER BY Table_Name",
			   "List the available tables in a schema.");
static toSQL SQLTableGrants("toBrowser:TableGrants",
			    "SELECT Privilege,Grantee,Grantor,Grantable FROM ALL_TAB_PRIVS\n"
			    " WHERE Table_Schema = :f1<char[31]> AND Table_Name = :f2<char[31]>\n"
			    " ORDER BY Privilege,Grantee",
			    "Display the grants on a table");
static toSQL SQLTableTrigger("toBrowser:TableTrigger",
			     "SELECT Trigger_Name,Triggering_Event,Column_Name,Status,Description \n"
			     "  FROM ALL_TRIGGERS\n"
			     " WHERE Table_Owner = :f1<char[31]> AND Table_Name = :f2<char[31]>",
			     "Display the triggers operating on a table");
static toSQL SQLTableInfo("toBrowser:TableInformation",
			  "SELECT *\n"
			  "  FROM ALL_TABLES\n"
			  " WHERE OWNER = :f1<char[31]> AND Table_Name = :f2<char[31]>",
			  "Display information about a table");
static toSQL SQLTableComment("toBrowser:TableComment",
			     "SELECT Comments FROM ALL_TAB_COMMENTS\n"
			     " WHERE Owner = :f1<char[31]> AND Table_Name = :f2<char[31]>",
			     "Display comment on a table");

static toSQL SQLListView("toBrowser:ListView",
			 "SELECT View_Name FROM ALL_VIEWS WHERE OWNER = :f1<char[31]>\n"
			 " ORDER BY View_Name",
			 "List the available views in a schema");
static toSQL SQLViewSQL("toBrowser:ViewSQL",	
			"SELECT Text SQL\n"
			"  FROM ALL_Views\n"
			" WHERE Owner = :f1<char[31]> AND View_Name = :f2<char[31]>",
			"Display SQL of a specified view");
static toSQL SQLViewComment("toBrowser:ViewComment",
			    "SELECT Comments FROM ALL_TAB_COMMENTS\n"
			    " WHERE Owner = :f1<char[31]> AND Table_Name = :f2<char[31]>",
			    "Display comment on a view");

static toSQL SQLListIndex("toBrowser:ListIndex",
			  "SELECT Index_Name\n"
			  "  FROM ALL_INDEXES\n"
			  " WHERE OWNER = :f1<char[31]>\n"
			  " ORDER BY Index_Name\n",
			  "List the available indexes in a schema");
static toSQL SQLIndexCols("toBrowser:IndexCols",
			  "SELECT Table_Name,Column_Name,Column_Length,Descend\n"
			  "  FROM ALL_IND_COLUMNS\n"
			  " WHERE Index_Owner = :f1<char[31]> AND Index_Name = :f2<char[31]>\n"
			  " ORDER BY Column_Position",
			  "Display columns on which an index is built");
static toSQL SQLIndexInfo("toBrowser:IndexInformation",
			  "SELECT * FROM ALL_INDEXES\n"
			  " WHERE Owner = :f1<char[31]> AND Index_Name = :f2<char[31]>",
			  "Display information about an index");

static toSQL SQLListSequence("toBrowser:ListSequence",
			     "SELECT Sequence_Name FROM ALL_SEQUENCES\n"
			     " WHERE SEQUENCE_OWNER = :f1<char[31]>\n"
			     " ORDER BY Sequence_Name",
			     "List the available sequences in a schema");
static toSQL SQLSequenceInfo("toBrowser:SequenceInformation",
			     "SELECT * FROM ALL_SEQUENCES\n"
			     " WHERE Sequence_Owner = :f1<char[31]>\n"
			     "   AND Sequence_Name = :f2<char[31]>",
			     "Display information about a sequence");

static toSQL SQLListSynonym("toBrowser:ListSynonym",
			    "SELECT Synonym_Name FROM ALL_SYNONYMS\n"
			    " WHERE Table_Owner = :f1<char[31]>"
			    " ORDER BY Synonym_Name",
			    "List the available synonyms in a schema");
static toSQL SQLSynonymInfo("toBrowser:SynonymInformation",
			    "SELECT * FROM ALL_SYNONYMS\n"
			    " WHERE Table_Owner = :f1<char[31]> AND Synonym_Name = :f2<char[31]>",
			    "Display information about a synonym");

static toSQL SQLListSQL("toBrowser:ListPL/SQL",
			"SELECT Object_Name,Object_Type Type FROM ALL_OBJECTS\n"
			" WHERE OWNER = :f1<char[31]>\n"
			"   AND Object_Type IN ('FUNCTION','PACKAGE',\n"
			"                       'PROCEDURE','TYPE')\n"
			" ORDER BY Object_Name",
			"List the available PL/SQL objects in a schema");
static toSQL SQLListSQLShort("toBrowser:ListPL/SQLShort",
			     "SELECT Object_Name Type FROM ALL_OBJECTS\n"
		 	     " WHERE OWNER = :f1<char[31]>\n"
			     "   AND Object_Type IN ('FUNCTION','PACKAGE',\n"
			     "                       'PROCEDURE','TYPE')\n"
			     " ORDER BY Object_Name",
			     "List the available PL/SQL objects in a schema, one column version");
static toSQL SQLSQLTemplate("toBrowser:PL/SQLTemplate",
			    "SELECT Text FROM ALL_SOURCE\n"
			    " WHERE Owner = :f1<char[31]> AND Name = :f2<char[31]>\n"
			    "   AND Type IN ('PACKAGE','PROCEDURE','FUNCTION','PACKAGE','TYPE')",
			    "Declaration of object displayed in template window");
static toSQL SQLSQLHead("toBrowser:PL/SQLHead",
			"SELECT Text FROM ALL_SOURCE\n"
			" WHERE Owner = :f1<char[31]> AND Name = :f2<char[31]>\n"
			"   AND Type IN ('PACKAGE','TYPE')",
			"Declaration of object");
static toSQL SQLSQLBody("toBrowser:PL/SQLBody",
			"SELECT Text FROM ALL_SOURCE\n"
			" WHERE Owner = :f1<char[31]> AND Name = :f2<char[31]>\n"
			"   AND Type IN ('PROCEDURE','FUNCTION','PACKAGE BODY','TYPE BODY')",
			"Implementation of object");

static toSQL SQLListTrigger("toBrowser:ListTrigger",
			    "SELECT Trigger_Name FROM ALL_TRIGGERS\n"
			    " WHERE OWNER = :f1<char[31]>\n"
			    " ORDER BY Trigger_Name",
			    "List the available triggers in a schema");
static toSQL SQLTriggerInfo("toBrowser:TriggerInfo",
			    "SELECT Owner,Trigger_Name,\n"
			    "       Trigger_Type,Triggering_Event,\n"
			    "       Table_Owner,Base_Object_Type,Table_Name,Column_Name,\n"
			    "       Referencing_Names,When_Clause,Status,\n"
			    "       Description,Action_Type\n"
			    "  FROM ALL_TRIGGERS\n"
			    "WHERE Owner = :f1<char[31]> AND Trigger_Name = :f2<char[31]>",
			    "Display information about a trigger");
static toSQL SQLTriggerBody("toBrowser:TriggerBody",
			    "SELECT Trigger_Body FROM ALL_TRIGGERS\n"
			    " WHERE Owner = :f1<char[31]> AND Trigger_Name = :f2<char[31]>",
			    "Implementation of trigger");
static toSQL SQLTriggerCols("toBrowser:TriggerCols",
			    "SELECT Column_Name,Column_List \"In Update\",Column_Usage Usage\n"
			    "  FROM ALL_TRIGGER_COLS\n"
			    " WHERE Trigger_Owner = :f1<char[31]> AND Trigger_Name = :f2<char[31]>",
			    "Columns used by trigger");

static QPixmap *RefreshPixmap;
static QPixmap *FilterPixmap;
static QPixmap *NoFilterPixmap;

void toBrowser::setNewFilter(toResultFilter *filter)
{
  if (filter)
    Filter=filter;
  for(map<QString,toResultView *>::iterator i=Map.begin();i!=Map.end();i++)
    (*i).second->setFilter(filter?filter->clone():NULL);
  refresh();
}

toBrowser::toBrowser(QWidget *parent,toConnection &connection)
  : toToolWidget("browser.html",parent,connection)
{
  if (!RefreshPixmap)
    RefreshPixmap=new QPixmap((const char **)refresh_xpm);
  if (!FilterPixmap)
    FilterPixmap=new QPixmap((const char **)filter_xpm);
  if (!NoFilterPixmap)
    NoFilterPixmap=new QPixmap((const char **)nofilter_xpm);
  Filter=NULL;

  QToolBar *toolbar=toAllocBar(this,"DB Browser",connection.connectString());

  new QToolButton(*RefreshPixmap,
		  "Update from DB",
		  "Update from DB",
		  this,SLOT(refresh(void)),
		  toolbar);
  toolbar->addSeparator();
  new QToolButton(*FilterPixmap,
		  "Define the object filter",
		  "Define the object filter",
		  this,SLOT(defineFilter(void)),
		  toolbar);
  new QToolButton(*NoFilterPixmap,
		  "Remove any object filter",
		  "Remove any object filter",
		  this,SLOT(clearFilter(void)),
		  toolbar);
  Schema=new QComboBox(toolbar);
  connect(Schema,SIGNAL(activated(int)),
	  this,SLOT(changeSchema(int)));
  toolbar->setStretchableWidget(new QLabel("",toolbar));
  
  TopTab=new QTabWidget(this);
  QSplitter *splitter=new QSplitter(Horizontal,TopTab,TAB_TABLES);
  TopTab->addTab(splitter,"T&ables");
  toResultView *resultView=new toResultView(true,false,connection,splitter);
  resultView->setReadAll(true);
  resultView->setSQL(SQLListTables);
  resultView->resize(FIRST_WIDTH,resultView->height());
  splitter->setResizeMode(resultView,QSplitter::KeepSize);
  FirstTab=resultView;
  Map[TAB_TABLES]=resultView;
  connect(resultView,SIGNAL(selectionChanged(QListViewItem *)),
	  this,SLOT(changeItem(QListViewItem *)));

  QTabWidget *curr=new QTabWidget(splitter);
  splitter->setResizeMode(curr,QSplitter::Stretch);
  resultView=new toResultCols(connection,curr,TAB_TABLE_COLUMNS);
  curr->addTab(resultView,"&Columns");
  connect(curr,SIGNAL(currentChanged(QWidget *)),this,SLOT(changeSecondTab(QWidget *)));
  SecondTab=resultView;
  SecondMap[TAB_TABLES]=resultView;
  SecondMap[TAB_TABLE_COLUMNS]=resultView;

  resultView=new toResultIndexes(connection,curr,TAB_TABLE_INDEXES);
  curr->addTab(resultView,"&Indexes");
  SecondMap[TAB_TABLE_INDEXES]=resultView;

  toResultConstraint *resultConstraint=new toResultConstraint(connection,curr,
							      TAB_TABLE_CONS);
  curr->addTab(resultConstraint,"C&onstraints");
  SecondMap[TAB_TABLE_CONS]=resultConstraint;

  toResultReferences *resultReferences=new toResultReferences(connection,curr,
							      TAB_TABLE_DEPEND);
  curr->addTab(resultReferences,"&References");
  SecondMap[TAB_TABLE_DEPEND]=resultReferences;

  resultView=new toResultView(true,false,connection,curr,TAB_TABLE_GRANTS);
  resultView->setReadAll(true);
  resultView->setSQL(SQLTableGrants);
  curr->addTab(resultView,"&Grants");
  SecondMap[TAB_TABLE_GRANTS]=resultView;

  resultView=new toResultView(true,false,connection,curr,TAB_TABLE_TRIGGERS);
  resultView->setReadAll(true);
  resultView->setSQL(SQLTableTrigger);
  curr->addTab(resultView,"Triggers");
  SecondMap[TAB_TABLE_TRIGGERS]=resultView;

  toResultContent *content=new toResultContent(connection,curr,TAB_TABLE_DATA);
  curr->addTab(content,"&Data");
  SecondMap[TAB_TABLE_DATA]=content;

  toResultItem *resultItem=new toResultItem(2,true,connection,curr,TAB_TABLE_INFO);
  resultItem->setSQL(SQLTableInfo(connection));
  curr->addTab(resultItem,"Information");
  SecondMap[TAB_TABLE_INFO]=resultItem;

  resultItem=new toResultItem(1,true,connection,curr,TAB_TABLE_COMMENT);
  resultItem->showTitle(false);
  resultItem->setSQL(SQLTableComment(connection));
  curr->addTab(resultItem,"Co&mment");
  SecondMap[TAB_TABLE_COMMENT]=resultItem;

  splitter=new QSplitter(Horizontal,TopTab,TAB_VIEWS);
  TopTab->addTab(splitter,"&Views");
  resultView=new toResultView(true,false,connection,splitter);
  resultView->setReadAll(true);
  Map[TAB_VIEWS]=resultView;
  resultView->setSQL(SQLListView);
  resultView->resize(FIRST_WIDTH,resultView->height());
  connect(resultView,SIGNAL(selectionChanged(QListViewItem *)),
	  this,SLOT(changeItem(QListViewItem *)));
  splitter->setResizeMode(resultView,QSplitter::KeepSize);
  curr=new QTabWidget(splitter);
  splitter->setResizeMode(curr,QSplitter::Stretch);
  resultView=new toResultCols(connection,curr,TAB_VIEW_COLUMNS);
  curr->addTab(resultView,"&Columns");
  SecondMap[TAB_VIEWS]=resultView;
  SecondMap[TAB_VIEW_COLUMNS]=resultView;

  toResultField *resultField=new toResultField(connection,curr,TAB_VIEW_SQL);
  resultField->setSQL(SQLViewSQL);
  curr->addTab(resultField,"SQL");
  connect(curr,SIGNAL(currentChanged(QWidget *)),this,SLOT(changeSecondTab(QWidget *)));
  SecondMap[TAB_VIEW_SQL]=resultField;

  content=new toResultContent(connection,curr,TAB_VIEW_DATA);
  curr->addTab(content,"&Data");
  SecondMap[TAB_VIEW_DATA]=content;

  resultView=new toResultView(true,false,connection,curr,TAB_VIEW_GRANTS);
  resultView->setReadAll(true);
  resultView->setSQL(SQLTableGrants);
  curr->addTab(resultView,"&Grants");
  SecondMap[TAB_VIEW_GRANTS]=resultView;

  toResultDepend *resultDepend=new toResultDepend(connection,curr,TAB_VIEW_DEPEND);
  curr->addTab(resultDepend,"De&pendencies");
  SecondMap[TAB_VIEW_DEPEND]=resultDepend;

  resultItem=new toResultItem(1,true,connection,curr,TAB_VIEW_COMMENT);
  resultItem->showTitle(false);
  resultItem->setSQL(SQLViewComment(connection));
  curr->addTab(resultItem,"Co&mment");
  SecondMap[TAB_VIEW_COMMENT]=resultItem;

  splitter=new QSplitter(Horizontal,TopTab,TAB_INDEX);
  TopTab->addTab(splitter,"Inde&xes");
  resultView=new toResultView(true,false,connection,splitter);
  resultView->setReadAll(true);
  Map[TAB_INDEX]=resultView;
  resultView->setSQL(SQLListIndex);
  resultView->resize(FIRST_WIDTH,resultView->height());
  connect(resultView,SIGNAL(selectionChanged(QListViewItem *)),
	  this,SLOT(changeItem(QListViewItem *)));
  splitter->setResizeMode(resultView,QSplitter::KeepSize);
  curr=new QTabWidget(splitter);
  connect(curr,SIGNAL(currentChanged(QWidget *)),this,SLOT(changeSecondTab(QWidget *)));
  splitter->setResizeMode(curr,QSplitter::Stretch);

  resultView=new toResultView(true,false,connection,curr,TAB_INDEX_COLS);
  resultView->setSQL(SQLIndexCols);
  curr->addTab(resultView,"&Columns");
  SecondMap[TAB_INDEX]=resultView;
  SecondMap[TAB_INDEX_COLS]=resultView;

  resultItem=new toResultItem(2,true,connection,curr,TAB_INDEX_INFO);
  resultItem->setSQL(SQLIndexInfo(connection));
  curr->addTab(resultItem,"Info");
  SecondMap[TAB_INDEX_INFO]=resultItem;

  splitter=new QSplitter(Horizontal,TopTab,TAB_SEQUENCES);
  TopTab->addTab(splitter,"&Sequences");
  resultView=new toResultView(true,false,connection,splitter);
  resultView->setReadAll(true);
  Map[TAB_SEQUENCES]=resultView;
  resultView->setSQL(SQLListSequence);
  resultView->resize(FIRST_WIDTH,resultView->height());
  connect(resultView,SIGNAL(selectionChanged(QListViewItem *)),
	  this,SLOT(changeItem(QListViewItem *)));
  splitter->setResizeMode(resultView,QSplitter::KeepSize);
  curr=new QTabWidget(splitter);
  connect(curr,SIGNAL(currentChanged(QWidget *)),this,SLOT(changeSecondTab(QWidget *)));
  splitter->setResizeMode(curr,QSplitter::Stretch);
  resultItem=new toResultItem(2,true,connection,curr,TAB_SEQUENCES_INFO);
  resultItem->setSQL(SQLSequenceInfo(connection));
  curr->addTab(resultItem,"Info");
  SecondMap[TAB_SEQUENCES]=resultItem;
  SecondMap[TAB_SEQUENCES_INFO]=resultItem;

  splitter=new QSplitter(Horizontal,TopTab,TAB_SYNONYM);
  TopTab->addTab(splitter,"S&ynonyms");
  resultView=new toResultView(true,false,connection,splitter);
  resultView->setReadAll(true);
  Map[TAB_SYNONYM]=resultView;
  resultView->setSQL(SQLListSynonym);
  resultView->resize(FIRST_WIDTH,resultView->height());
  connect(resultView,SIGNAL(selectionChanged(QListViewItem *)),
	  this,SLOT(changeItem(QListViewItem *)));
  splitter->setResizeMode(resultView,QSplitter::KeepSize);
  curr=new QTabWidget(splitter);
  connect(curr,SIGNAL(currentChanged(QWidget *)),this,SLOT(changeSecondTab(QWidget *)));
  splitter->setResizeMode(curr,QSplitter::Stretch);
  resultItem=new toResultItem(2,true,connection,curr,TAB_SYNONYM_INFO);
  resultItem->setSQL(SQLSynonymInfo(connection));
  curr->addTab(resultItem,"Info");
  SecondMap[TAB_SYNONYM]=resultItem;
  SecondMap[TAB_SYNONYM_INFO]=resultItem;

  splitter=new QSplitter(Horizontal,TopTab,TAB_PLSQL);
  TopTab->addTab(splitter,"&PL/SQL");
  resultView=new toResultView(true,false,connection,splitter);
  resultView->setReadAll(true);
  Map[TAB_PLSQL]=resultView;
  resultView->setSQL(SQLListSQL);
  resultView->resize(FIRST_WIDTH*2,resultView->height());
  connect(resultView,SIGNAL(selectionChanged(QListViewItem *)),
	  this,SLOT(changeItem(QListViewItem *)));
  splitter->setResizeMode(resultView,QSplitter::KeepSize);
  curr=new QTabWidget(splitter);
  connect(curr,SIGNAL(currentChanged(QWidget *)),this,SLOT(changeSecondTab(QWidget *)));
  splitter->setResizeMode(curr,QSplitter::Stretch);

  resultField=new toResultField(connection,curr,TAB_PLSQL_SOURCE);
  resultField->setSQL(SQLSQLHead);
  curr->addTab(resultField,"&Declaration");
  SecondMap[TAB_PLSQL]=resultField;
  SecondMap[TAB_PLSQL_SOURCE]=resultField;

  resultField=new toResultField(connection,curr,TAB_PLSQL_BODY);
  resultField->setSQL(SQLSQLBody);
  curr->addTab(resultField,"B&ody");
  SecondMap[TAB_PLSQL_BODY]=resultField;

  resultDepend=new toResultDepend(connection,curr,TAB_PLSQL_DEPEND);
  curr->addTab(resultDepend,"De&pendencies");
  SecondMap[TAB_PLSQL_DEPEND]=resultDepend;

  splitter=new QSplitter(Horizontal,TopTab,TAB_TRIGGER);
  TopTab->addTab(splitter,"Tri&ggers");
  resultView=new toResultView(true,false,connection,splitter);
  resultView->setReadAll(true);
  Map[TAB_TRIGGER]=resultView;
  resultView->setSQL(SQLListTrigger);
  resultView->resize(FIRST_WIDTH,resultView->height());
  connect(resultView,SIGNAL(selectionChanged(QListViewItem *)),
	  this,SLOT(changeItem(QListViewItem *)));
  splitter->setResizeMode(resultView,QSplitter::KeepSize);
  curr=new QTabWidget(splitter);
  connect(curr,SIGNAL(currentChanged(QWidget *)),this,SLOT(changeSecondTab(QWidget *)));
  splitter->setResizeMode(curr,QSplitter::Stretch);

  resultItem=new toResultItem(2,true,connection,curr,TAB_TRIGGER_INFO);
  resultItem->setSQL(SQLTriggerInfo(connection));
  curr->addTab(resultItem,"Info");
  SecondMap[TAB_TRIGGER]=resultItem;
  SecondMap[TAB_TRIGGER_INFO]=resultItem;

  resultField=new toResultField(connection,curr,TAB_TRIGGER_SOURCE);
  resultField->setSQL(SQLTriggerBody);
  curr->addTab(resultField,"C&ode");
  SecondMap[TAB_TRIGGER_SOURCE]=resultField;

  resultView=new toResultView(true,false,connection,curr,TAB_TRIGGER_COLS);
  resultView->setSQL(SQLTriggerCols);
  curr->addTab(resultView,"&Columns");
  SecondMap[TAB_TRIGGER_COLS]=resultView;

  resultDepend=new toResultDepend(connection,curr,TAB_TRIGGER_DEPEND);
  curr->addTab(resultDepend,"De&pendencies");
  SecondMap[TAB_TRIGGER_DEPEND]=resultDepend;

  ToolMenu=NULL;
  connect(toMainWidget()->workspace(),SIGNAL(windowActivated(QWidget *)),
	  this,SLOT(windowActivated(QWidget *)));

  refresh();

  connect(TopTab,SIGNAL(currentChanged(QWidget *)),this,SLOT(changeTab(QWidget *)));
  Schema->setFocus();
}

void toBrowser::windowActivated(QWidget *widget)
{
  if (widget==this) {
    if (!ToolMenu) {
      ToolMenu=new QPopupMenu(this);
      ToolMenu->insertItem(*RefreshPixmap,"&Refresh",this,SLOT(refresh(void)),
			   Key_F5);
      ToolMenu->insertItem("&Change Schema",Schema,SLOT(setFocus(void)),
			   Key_S+ALT);
      ToolMenu->insertSeparator();
      ToolMenu->insertItem(*FilterPixmap,"&Define filter",this,SLOT(defineFilter(void)),
			   CTRL+SHIFT+Key_G);
      ToolMenu->insertItem(*NoFilterPixmap,"&Clear filter",this,SLOT(clearFilter(void)),
			   CTRL+SHIFT+Key_H);
      toMainWidget()->menuBar()->insertItem("&Browser",ToolMenu,-1,toToolMenuIndex());
    }
  } else {
    delete ToolMenu;
    ToolMenu=NULL;
  }
}

toBrowser::~toBrowser()
{
  delete Filter;
}

void toBrowser::refresh(void)
{
  try {
    QString selected=Schema->currentText();
    if (selected.isEmpty())
      selected=connection().user().upper();

    Schema->clear();
    otl_stream users(1,
		     toSQL::sql(toSQL::TOSQL_USERLIST,connection()),
		     otlConnect());
    for(int i=0;!users.eof();i++) {
      char buffer[31];
      users>>buffer;
      Schema->insertItem(QString::fromUtf8(buffer));
      if (selected==QString::fromUtf8(buffer))
	Schema->setCurrentItem(i);
    }
    if (!Schema->currentText().isEmpty()) {
      QListViewItem *item=FirstTab->currentItem();
      QString str;
      if (item)
	str=item->text(0);
      FirstTab->changeParams(Schema->currentText());
      if (!str.isEmpty()) {
	for (item=FirstTab->firstChild();item;item=item->nextSibling()) {
	  if (item->text(0)==str) {
	    FirstTab->setSelected(item,true);
	    break;
	  }
	}
      }
    }
    if (SecondTab&&!SecondText.isEmpty())
      SecondTab->changeParams(Schema->currentText(),SecondText);
  } TOCATCH
}

void toBrowser::changeItem(QListViewItem *item)
{
  if (item) {
    SecondText=item->text(0);
    if (SecondTab&&!SecondText.isEmpty())
      SecondTab->changeParams(Schema->currentText(),
			      SecondText);
  }
}

void toBrowser::changeSecondTab(QWidget *tab)
{
  if (tab) {
    SecondTab=SecondMap[tab->name()];
    SecondMap[TopTab->currentPage()->name()]=SecondTab;
    if (SecondTab&&!SecondText.isEmpty()) {
      try {
	for(QWidget *widget=dynamic_cast<QWidget *>(SecondTab)->parentWidget();
	    widget;
	    widget=widget->parentWidget())
	  if (widget->isA("QTabWidget")) {
#if 0
	    widget->setFocus();
#endif
	    break;
	  }
      } catch(...) {
      }
      SecondTab->changeParams(Schema->currentText(),
			      SecondText);
    }
  }
}

void toBrowser::changeTab(QWidget *tab)
{
  if (tab&&this==toMainWidget()->workspace()->activeWindow()) {
    FirstTab=Map[tab->name()];
    SecondTab=SecondMap[tab->name()];
    SecondText="";
    TopTab->setFocus();
    if (FirstTab&&SecondTab)
      refresh();
  }
}

void toBrowser::clearFilter(void)
{
  setNewFilter(NULL);
}

void toBrowser::defineFilter(void)
{
  toBrowserFilter::setting *setting=dynamic_cast<toBrowserFilter::setting *>(Filter);
  if (setting) {
    toBrowserFilter filt(*setting,this);
    if (filt.exec())
      setNewFilter(filt.getSetting());
  } else {
    toBrowserFilter filt(this);
    if (filt.exec())
      setNewFilter(filt.getSetting());
  }
}

void toBrowseTemplate::addDatabase(const QString &name)
{
  for(list<toTemplateItem *>::iterator i=Parents.begin();i!=Parents.end();i++)
    new toTemplateItem(*i,name);
}

void toBrowseTemplate::removeDatabase(const QString &name)
{
  for(list<toTemplateItem *>::iterator i=Parents.begin();i!=Parents.end();i++) {
    for (QListViewItem *item=(*i)->firstChild();item;item=item->nextSibling())
      if (item->text(0)==name) {
	delete item;
	break;
      }
  }
}

void toBrowseTemplate::removeItem(QListViewItem *item)
{
  for(list<toTemplateItem *>::iterator i=Parents.begin();i!=Parents.end();i++)
    if ((*i)==item) {
      Parents.erase(i);
      break;
    }
}

class toTemplateTableItem : public toTemplateItem {
  toConnection &Connection;
public:
  toTemplateTableItem(toConnection &conn,toTemplateItem *parent,
		       const QString &name)
    : toTemplateItem(parent,name),Connection(conn)
  {
  }
  virtual QWidget *selectedWidget(QWidget *par)
  {
    QString object=parent()->text(0);
    QString typ=text(0);
    QString schema=parent()->parent()->parent()->text(0);

    toResultView *res;

    if (typ=="Constraints") {
      res=new toResultConstraint(Connection,par);
    } else if (typ=="References") {
      res=new toResultReferences(Connection,par);
    } else if (typ=="Grants") {
      res=new toResultView(true,false,Connection,par);
      res->setSQL(SQLTableGrants);
    } else
      return NULL;
    res->changeParams(schema,object);
    return res;
  }
};

class toTemplateSchemaItem : public toTemplateItem {
  toConnection &Connection;
public:
  toTemplateSchemaItem(toConnection &conn,toTemplateItem *parent,
		       const QString &name)
    : toTemplateItem(parent,name),Connection(conn)
  {
    QString typ=parent->text(0);
    if (typ=="Tables") {
      QPixmap image((const char **)table_xpm);
      setPixmap(0,image);
      new toTemplateTableItem(conn,this,"Constraints");
      new toTemplateTableItem(conn,this,"References");
      new toTemplateTableItem(conn,this,"Grants");
    } else if (typ=="Views") {
      QPixmap image((const char **)view_xpm);
      setPixmap(0,image);
      new toTemplateTableItem(conn,this,"Grants");
    } else if (typ=="Sequences") {
      QPixmap image((const char **)sequence_xpm);
      setPixmap(0,image);
    } else if (typ=="Code" || typ =="Triggers") {
      QPixmap image((const char **)function_xpm);
      setPixmap(0,image);
    } else if (typ=="Indexes") {
      QPixmap image((const char **)index_xpm);
      setPixmap(0,image);
    } else if (typ=="Synonyms") {
      QPixmap image((const char **)synonym_xpm);
      setPixmap(0,image);
    }
  }

  virtual QString allText(int col) const
  {
    QString txt=parent()->parent()->text(0);
    txt+=".";
    txt+=text(col);
    return txt;
  }

  virtual QWidget *selectedWidget(QWidget *par)
  {
    QString object=text(0);
    QString typ=parent()->text(0);
    QString schema=parent()->parent()->text(0);

    if (typ=="Code"||typ=="Triggers") {
      toResultField *fld=new toResultField(Connection,par);
      if(typ=="Code")
	fld->setSQL(SQLSQLTemplate);
      else
	fld->setSQL(SQLTriggerBody);
      fld->changeParams(schema,object);
      return fld;
    } else if (typ=="Tables"||typ=="Views") {
      toResultCols *cols=new toResultCols(Connection,par);
      cols->changeParams(schema,object);
      return cols;
    } else if (typ=="Indexes") {
      toResultView *resultView=new toResultView(true,false,Connection,par);
      resultView->setSQL(SQLIndexCols);
      resultView->changeParams(schema,object);
      return resultView;
    } else if (typ=="Synonyms"||typ=="Sequences") {
      toResultItem *resultItem=new toResultItem(1,true,Connection,par);
      if (typ=="Synonyms")
	resultItem->setSQL(SQLSynonymInfo(Connection));
      else
	resultItem->setSQL(SQLSequenceInfo(Connection));
      resultItem->changeParams(schema,object);
      return resultItem;
    } else
      return NULL;
  }
};

class toTemplateSchemaList : public toTemplateSQL {
public:
  toTemplateSchemaList(toConnection &conn,toTemplateItem *parent,
		       const QString &name,const QCString &sql)
    : toTemplateSQL(conn,parent,name,sql)
  { }
  virtual toTemplateItem *createChild(const QString &name)
  {
    return new toTemplateSchemaItem(connection(),this,name);
  }
  virtual list<QString> parameters(void)
  {
    list<QString> ret;
    ret.insert(ret.end(),parent()->text(0));
    return ret;
  }
};

class toTemplateDBItem : public toTemplateSQL {
public:
  toTemplateDBItem(toConnection &conn,toTemplateItem *parent,
		   const QString &name)
    : toTemplateSQL(conn,parent,name,toSQL::sql(toSQL::TOSQL_USERLIST,conn))
  {
  }
  virtual ~toTemplateDBItem()
  {
    toBrowseTemplate *prov=dynamic_cast<toBrowseTemplate *>(&provider());
    if (prov)
      prov->removeItem(this);
  }
  virtual toTemplateItem *createChild(const QString &name)
  {
    toTemplateItem *item=new toTemplateItem(this,name);
    QPixmap image(schema_xpm);
    item->setPixmap(0,image);
    QPixmap table((const char **)table_xpm);
    QPixmap view((const char **)view_xpm);
    QPixmap sequence((const char **)sequence_xpm);
    QPixmap function((const char **)function_xpm);
    QPixmap index((const char **)index_xpm);
    QPixmap synonym((const char **)synonym_xpm);

    (new toTemplateSchemaList(connection(),
			      item,
			      "Tables",
			      SQLListTables(connection())))->setPixmap(0,table);
    (new toTemplateSchemaList(connection(),
			      item,
			      "Views",
			      SQLListView(connection())))->setPixmap(0,view);
    (new toTemplateSchemaList(connection(),
			      item,
			      "Indexes",
			      SQLListIndex(connection())))->setPixmap(0,index);
    (new toTemplateSchemaList(connection(),
			      item,
			      "Sequences",
			      SQLListSequence(connection())))->setPixmap(0,sequence);
    (new toTemplateSchemaList(connection(),
			      item,
			      "Synonyms",
			      SQLListSynonym(connection())))->setPixmap(0,synonym);
    (new toTemplateSchemaList(connection(),
			      item,
			      "Code",
			      SQLListSQLShort(connection())))->setPixmap(0,function);
    (new toTemplateSchemaList(connection(),
			      item,
			      "Triggers",
			      SQLListTrigger(connection())))->setPixmap(0,function);
    return item;
  }
};

class toBrowseTemplateItem : public toTemplateItem {
public:
  toBrowseTemplateItem(toTemplateProvider &prov,QListView *parent,const QString &name)
    : toTemplateItem(prov,parent,name)
  { }
  virtual ~toBrowseTemplateItem()
  {
    dynamic_cast<toBrowseTemplate &>(provider()).removeItem(this);
  }
};

void toBrowseTemplate::insertItems(QListView *parent)
{
  if (!Registered) {
    connect(toMainWidget(),SIGNAL(addedConnection(const QString &)),
	    this,SLOT(addDatabase(const QString &)));
    connect(toMainWidget(),SIGNAL(removedConnection(const QString &)),
	    this,SLOT(removeDatabase(const QString &)));
  }
  toTemplateItem *dbitem=new toBrowseTemplateItem(*this,parent,"DB Browser");
  list<QString> conn=toMainWidget()->connections();
  for (list<QString>::iterator i=conn.begin();i!=conn.end();i++) {
    toConnection &conn=toMainWidget()->connection(*i);
    new toTemplateDBItem(conn,dbitem,*i);
  }
  Parents.insert(Parents.end(),dbitem);
}

static toBrowseTemplate BrowseTemplate;
