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

#ifdef TO_KDE
#include <kfiledialog.h>
#include <kmenubar.h>
#endif

#include <qcombobox.h>
#include <qregexp.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qmultilineedit.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qsplitter.h>
#include <qtabwidget.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qmenubar.h>
#include <qworkspace.h>
#include <qgrid.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qtooltip.h>
#include <qlineedit.h>
#include <qfiledialog.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qheader.h>

#include "totool.h"
#include "toresultplan.h"
#include "toworksheet.h"
#include "toresultview.h"
#include "toresultresources.h"
#include "tohighlightedtext.h"
#include "toparamget.h"
#include "toresultlong.h"
#include "toresultstats.h"
#include "toresultcols.h"
#include "toconf.h"
#include "tochangeconnection.h"
#include "toworksheetsetupui.h"
#include "tosession.h"
#include "toresultbar.h"

#include "toworksheet.moc"
#include "toworksheetsetupui.moc"

#include "icons/toworksheet.xpm"
#include "icons/refresh.xpm"
#include "icons/execute.xpm"
#include "icons/executestep.xpm"
#include "icons/executeall.xpm"
#include "icons/commit.xpm"
#include "icons/rollback.xpm"
#include "icons/eraselog.xpm"
#include "icons/stop.xpm"
#include "icons/clock.xpm"

#define TO_ID_STATISTICS		(toMain::TO_TOOL_MENU_ID+ 0)
#define TO_ID_STOP			(toMain::TO_TOOL_MENU_ID+ 1)

#define CONF_AUTO_SAVE   "AutoSave"
#define CONF_CHECK_SAVE  "CheckSave"
#define CONF_AUTO_LOAD   "AutoLoad"
#define CONF_LOG_AT_END  "LogAtEnd"
#define CONF_LOG_MULTI   "LogMulti"
#define CONF_PLSQL_PARSE "PLSQLParse"
#define CONF_STATISTICS	 "Statistics"
#define CONF_TIMED_STATS "TimedStats"
#define CONF_NUMBER	 "Number"

static struct {
  int Pos;
  char *Start;
  bool WantEnd;
  bool WantSemi;
  bool CloseBlock;
  bool Comment;
  bool BeforeCode;
  bool StartCode;
  bool NoBinds;
} Blocks[] = { { 0,"begin",	true ,false,false,false,true ,true ,false},
	       { 0,"if",	true ,false,false,false,false,false,false},
	       { 0,"loop",	true ,false,false,false,false,false,false},
	       { 0,"while",	true ,false,false,false,false,false,false},
	       { 0,"declare",	false,false,false,false,true ,true ,false},
	       { 0,"create",	false,false,false,false,true ,false,false},
	       { 0,"package",	true ,false,false,false,false,true ,false},
	       { 0,"procedure",	false,false,false,false,false,true ,false},
	       { 0,"function",	false,false,false,false,false,true ,false},
	       { 0,"trigger",   false,false,false,false,false,true ,false},
	       { 0,"end",	false,true ,true ,false,false,false,false},
	       { 0,"rem",	false,false,false,true ,false,false,false},
	       { 0,"prompt",	false,false,false,true ,false,false,false},
	       { 0,"set",	false,false,false,true ,false,false,false},
	       { 0,NULL,	false,false,false,false,false,false,false}
};

class toWorksheetSetup : public toWorksheetSetupUI, public toSettingTab
{ 
  toTool *Tool;

public:
  toWorksheetSetup(toTool *tool,QWidget* parent = 0,const char* name = 0)
    : toWorksheetSetupUI(parent,name),toSettingTab("worksheet.html#preferences"),Tool(tool)
  {
    if (!tool->config(CONF_AUTO_SAVE,"").isEmpty())
      AutoSave->setChecked(true);
    if (!tool->config(CONF_CHECK_SAVE,"Yes").isEmpty())
      CheckSave->setChecked(true);
    if (!tool->config(CONF_LOG_AT_END,"Yes").isEmpty())
      LogAtEnd->setChecked(true);
    if (!tool->config(CONF_LOG_MULTI,"Yes").isEmpty())
      LogMulti->setChecked(true);
    if (!tool->config(CONF_PLSQL_PARSE,"Yes").isEmpty())
      PLSQLParse->setChecked(true);
    if (!tool->config(CONF_STATISTICS,"").isEmpty())
      Statistics->setChecked(true);
    TimedStatistics->setChecked(!tool->config(CONF_TIMED_STATS,"Yes").isEmpty());
    if (!tool->config(CONF_NUMBER,"Yes").isEmpty())
      DisplayNumber->setChecked(true);
    DefaultFile->setText(tool->config(CONF_AUTO_LOAD,""));
  }
  virtual void saveSetting(void)
  {
    if (AutoSave->isChecked())
      Tool->setConfig(CONF_AUTO_SAVE,"Yes");
    else
      Tool->setConfig(CONF_AUTO_SAVE,"");
    if (CheckSave->isChecked())
      Tool->setConfig(CONF_CHECK_SAVE,"Yes");
    else
      Tool->setConfig(CONF_CHECK_SAVE,"");
    if (LogAtEnd->isChecked())
      Tool->setConfig(CONF_LOG_AT_END,"Yes");
    else
      Tool->setConfig(CONF_LOG_AT_END,"");
    if (LogMulti->isChecked())
      Tool->setConfig(CONF_LOG_MULTI,"Yes");
    else
      Tool->setConfig(CONF_LOG_MULTI,"");
    if (PLSQLParse->isChecked())
      Tool->setConfig(CONF_PLSQL_PARSE,"Yes");
    else
      Tool->setConfig(CONF_PLSQL_PARSE,"");
    Tool->setConfig(CONF_STATISTICS,Statistics->isChecked()?"Yes":"");
    Tool->setConfig(CONF_TIMED_STATS,TimedStatistics->isChecked()?"Yes":"");
    Tool->setConfig(CONF_NUMBER,DisplayNumber->isChecked()?"Yes":"");
    Tool->setConfig(CONF_AUTO_LOAD,DefaultFile->text());
  }
public slots:
  void chooseFile(void)
  {
    QString str=toOpenFilename(DefaultFile->text(),"*.sql\n*.txt",this);
    if (!str.isEmpty())
      DefaultFile->setText(str);
  }
};

class toWorksheetTool : public toTool {
protected:
  virtual char **pictureXPM(void)
  { return toworksheet_xpm; }
public:
  toWorksheetTool()
    : toTool(1,"SQL Worksheet")
  { }
  virtual const char *menuItem()
  { return "SQL Worksheet"; }
  virtual QWidget *toolWindow(QWidget *main,toConnection &connection)
  {
    QWidget *window=new toWorksheet(main,connection);
    window->setIcon(*toolbarImage());
    return window;
  }
  virtual QWidget *configurationTab(QWidget *parent)
  {
    return new toWorksheetSetup(this,parent);
  }
};

static toWorksheetTool WorksheetTool;

static QPixmap *toRefreshPixmap;
static QPixmap *toExecutePixmap;
static QPixmap *toExecuteAllPixmap;
static QPixmap *toExecuteStepPixmap;
static QPixmap *toCommitPixmap;
static QPixmap *toRollbackPixmap;
static QPixmap *toEraseLogPixmap;
static QPixmap *toStopPixmap;
static QPixmap *toStatisticPixmap;

void toWorksheet::viewResources(void)
{
  try {
    QString address=toSQLToAddress(connection(),QueryString);

    Resources->changeParams(address);
  } TOCATCH
}

toWorksheet::toWorksheet(QWidget *main,toConnection &connection,bool autoLoad)
  : toToolWidget(WorksheetTool,"worksheet.html",main,connection)
{
  if (!toRefreshPixmap)
    toRefreshPixmap=new QPixmap((const char **)refresh_xpm);
  if (!toExecutePixmap)
    toExecutePixmap=new QPixmap((const char **)execute_xpm);
  if (!toExecuteAllPixmap)
    toExecuteAllPixmap=new QPixmap((const char **)executeall_xpm);
  if (!toExecuteStepPixmap)
    toExecuteStepPixmap=new QPixmap((const char **)executestep_xpm);
  if (!toCommitPixmap)
    toCommitPixmap=new QPixmap((const char **)commit_xpm);
  if (!toRollbackPixmap)
    toRollbackPixmap=new QPixmap((const char **)rollback_xpm);
  if (!toEraseLogPixmap)
    toEraseLogPixmap=new QPixmap((const char **)eraselog_xpm);
  if (!toStopPixmap)
    toStopPixmap=new QPixmap((const char **)stop_xpm);
  if (!toStatisticPixmap)
    toStatisticPixmap=new QPixmap((const char **)clock_xpm);

  QToolBar *toolbar=toAllocBar(this,"SQL worksheet",connection.description());

  new QToolButton(*toExecutePixmap,
		  "Execute current statement",
		  "Execute current statement",
		  this,SLOT(execute(void)),
		  toolbar);
  new QToolButton(*toExecuteAllPixmap,
		  "Execute all statements",
		  "Execute all statements",
		  this,SLOT(executeAll(void)),
		  toolbar);
  new QToolButton(*toExecuteStepPixmap,
		  "Step through statements",
		  "Step through statements",
		  this,SLOT(executeStep(void)),
		  toolbar);
  toolbar->addSeparator();
  new QToolButton(*toRefreshPixmap,
		  "Refresh",
		  "Update last executed statement",
		  this,SLOT(refresh(void)),
		  toolbar);
  toolbar->addSeparator();
  new QToolButton(*toCommitPixmap,
		  "Commit work",
		  "Commit work",
		  this,SLOT(commitButton(void)),
		  toolbar);
  new QToolButton(*toRollbackPixmap,
		  "Rollback work",
		  "Rollback work",
		  this,SLOT(rollbackButton(void)),
		  toolbar);

  QSplitter *splitter=new QSplitter(Vertical,this);

  Editor=new toHighlightedText(splitter);
  ResultTab=new QTabWidget(splitter);
  QVBox *box=new QVBox(ResultTab);
  ResultTab->addTab(box,"Result");

  Result=new toResultLong(box);
  connect(Result,SIGNAL(done(void)),this,SLOT(queryDone(void)));
  connect(Result,SIGNAL(firstResult(const QString &,const QString &)),
	  this,SLOT(addLog(const QString &,const QString &)));

  Columns=new toResultCols(box);
  Columns->hide();

  ResultTab->setTabEnabled(Columns,false);
  Plan=new toResultPlan(ResultTab);
  ResultTab->addTab(Plan,"Execution plan");
  Resources=new toResultResources(ResultTab);
  ResultTab->addTab(Resources,"Information");
  StatSplitter=new QSplitter(Horizontal,ResultTab);
  Statistics=new toResultStats(true,StatSplitter);
  toResultBar *bar=new toResultBar(StatSplitter);
  bar->setSQL(toSQL::string(TO_SESSION_WAIT,connection));
  bar->setTitle("Wait states");
  bar->setYPostfix("ms/s");
  bar->setSamples(-1);
  connect(Statistics,SIGNAL(sessionChanged(const QString &)),
	  bar,SLOT(changeParams(const QString &)));
  bar=new toResultBar(StatSplitter);
  bar->setSQL(toSQL::string(TO_SESSION_IO,connection));
  bar->setTitle("I/O");
  bar->setYPostfix("blocks/s");
  bar->setSamples(-1);
  connect(Statistics,SIGNAL(sessionChanged(const QString &)),
	  bar,SLOT(changeParams(const QString &)));
  ResultTab->addTab(StatSplitter,"Statistics");
  ResultTab->setTabEnabled(StatSplitter,false);

  Logging=new toResultView(true,false,ResultTab);
  ResultTab->addTab(Logging,"Logging");
  Logging->addColumn("SQL");
  Logging->addColumn("Result");
  Logging->addColumn("Timestamp");
  Logging->addColumn("Duration");
  Logging->setColumnAlignment(3,AlignRight);
  LastLogItem=NULL;

  toolbar->addSeparator();
  StopButton=new QToolButton(*toStopPixmap,
			     "Stop execution",
			     "Stop execution",
			     Result,SLOT(stop(void)),
			     toolbar);
  StopButton->setEnabled(false);
  toolbar->addSeparator();
  new QToolButton(*toEraseLogPixmap,
		  "Clear execution log",
		  "Clear execution log",
		  this,SLOT(eraseLogButton(void)),
		  toolbar);

  toolbar->addSeparator();
  StatisticButton=new QToolButton(toolbar);
  StatisticButton->setToggleButton(true);
  StatisticButton->setIconSet(QIconSet(*toStatisticPixmap));
  connect(StatisticButton,SIGNAL(toggled(bool)),this,SLOT(enableStatistic(bool)));
  QToolTip::add(StatisticButton,"Gather session statistic of execution");
  new QLabel("Refresh",toolbar);
  Refresh=toRefreshCreate(toolbar);
  connect(Refresh,SIGNAL(activated(const QString &)),this,SLOT(changeRefresh(const QString &)));
  connect(StatisticButton,SIGNAL(toggled(bool)),Refresh,SLOT(setEnabled(bool)));
  Refresh->setEnabled(false);

  toolbar->setStretchableWidget(new QLabel("",toolbar));
  new toChangeConnection(toolbar);

  connect(ResultTab,SIGNAL(currentChanged(QWidget *)),
	  this,SLOT(changeResult(QWidget *)));

  if (autoLoad) {
    Editor->setFilename(WorksheetTool.config(CONF_AUTO_LOAD,""));
    if (!Editor->filename().isEmpty()) {
      try {
	QCString data=toReadFile(Editor->filename());
	Editor->setText(QString::fromLocal8Bit(data));
	Editor->setEdited(false);
      } TOCATCH
    }
  }

  ToolMenu=NULL;
  connect(toMainWidget()->workspace(),SIGNAL(windowActivated(QWidget *)),
	  this,SLOT(windowActivated(QWidget *)));

  if (!WorksheetTool.config(CONF_STATISTICS,"").isEmpty()) {
    show();
    StatisticButton->setOn(true);
  }
}

void toWorksheet::changeRefresh(const QString &str)
{
  if (StopButton->isEnabled()&&StatisticButton->isOn())
    toRefreshParse(timer(),str);
}

void toWorksheet::windowActivated(QWidget *widget)
{
  QWidget *w=this;
  while(w&&w!=widget) {
    w=w->parentWidget();
  }

  if (widget==w) {
    if (!ToolMenu) {
      ToolMenu=new QPopupMenu(this);
      ToolMenu->insertItem(*toExecutePixmap,"&Execute Current",this,SLOT(execute(void)),
			   CTRL+Key_Return);
      ToolMenu->insertItem(*toExecuteAllPixmap,"Execute &All",this,SLOT(executeAll(void)),
			   Key_F8);
      ToolMenu->insertItem(*toExecuteStepPixmap,"Execute &Next",this,SLOT(executeStep(void)),
			   Key_F9);
      ToolMenu->insertItem("Execute &Newline Separated",this,
			   SLOT(executeNewline(void)),SHIFT+Key_F9);
      ToolMenu->insertItem(*toRefreshPixmap,"&Refresh",this,SLOT(refresh(void)),
			   Key_F5);
      ToolMenu->insertSeparator();
      ToolMenu->insertItem(*toCommitPixmap,"&Commit",this,SLOT(commitButton(void)),
			   Key_F2);
      ToolMenu->insertItem(*toRollbackPixmap,"&Rollback",this,SLOT(rollbackButton(void)),
			   CTRL+Key_Backspace);
      ToolMenu->insertSeparator();
      ToolMenu->insertItem("&Enable statistics",this,SLOT(toggleStatistic(void)),
			   0,TO_ID_STATISTICS);
      ToolMenu->insertItem(*toStopPixmap,"&Stop execution",Result,SLOT(stop(void)),
			   0,TO_ID_STOP);
      ToolMenu->insertSeparator();
      ToolMenu->insertItem(*toEraseLogPixmap,"Erase &Log",this,SLOT(eraseLogButton(void)));

      toMainWidget()->menuBar()->insertItem("W&orksheet",ToolMenu,-1,toToolMenuIndex());
      toMainWidget()->menuBar()->setItemEnabled(TO_ID_STOP,StopButton->isEnabled());
      toMainWidget()->menuBar()->setItemChecked(TO_ID_STATISTICS,
						StatisticButton->isOn());
    }
  } else {
    delete ToolMenu;
    ToolMenu=NULL;
  }
}

bool toWorksheet::checkSave(bool input)
{
  if (Editor->edited()) {
    if(WorksheetTool.config(CONF_AUTO_SAVE,"").isEmpty()||
       Editor->filename().isEmpty()) {
      if (!WorksheetTool.config(CONF_CHECK_SAVE,"Yes").isEmpty()) {
	if (input) {
	  QString str("Save changes to worksheet for ");
	  str.append(connection().description());
	  int ret=TOMessageBox::information(this,
					    "Save file",
					    str,
					    "&Yes","&No","&Cancel",0,2);
	  if (ret==1)
	    return true;
	  else if (ret==2)
	    return false;
	} else
	  return true;
      } else
	return true;
      if (Editor->filename().isEmpty()&&input)
	Editor->setFilename(toSaveFilename(Editor->filename(),
					   "*.sql\n*.txt",this));
      if (Editor->filename().isEmpty())
	return false;	
    }
    if (!toWriteFile(Editor->filename(),Editor->text()))
      return false;
    Editor->setEdited(false);
  }
  return true;
}

bool toWorksheet::close(bool del)
{
  if (checkSave(true)) {
    Result->stop();
    return QVBox::close(del);
  }
  return false;
}

toWorksheet::~toWorksheet()
{
  checkSave(false);
}

#define LARGE_BUFFER 4096

void toWorksheet::changeResult(QWidget *widget)
{
  CurrentTab=widget;
  if (QueryString.length()) {
    if (CurrentTab==Plan)
      Plan->query(QueryString);
    else if (CurrentTab==Resources)
      viewResources();
    else if (CurrentTab==Statistics&&Result->running())
      Statistics->refreshStats(false);
  }
}

void toWorksheet::refresh(void)
{
  if (!QueryString.isEmpty()) {
    query(QueryString,false);
    StopButton->setEnabled(true);
    toMainWidget()->menuBar()->setItemEnabled(TO_ID_STOP,true);
    if (CurrentTab==Plan)
      Plan->query(QueryString);
    else if (CurrentTab==Resources)
      viewResources();
  }
}

static QString unQuote(const QString &str)
{
  if (str.at(0)=='\"'&&str.at(str.length()-1)=='\"')
    return str.left(str.length()-1).right(str.length()-2);
  return str.upper();
}

bool toWorksheet::describe(const QString &query)
{
  QRegExp white("[ \r\n\t.]+");
  QStringList part=QStringList::split(white,query);
  if (part[0].upper()=="DESC"||
      part[0].upper()=="DESCRIBE") {
    if (part.count()==2) {
      Columns->changeParams(unQuote(part[1]));
    } else if (part.count()==3) {
      Columns->changeParams(unQuote(part[1]),unQuote(part[2]));
    } else
      throw QString("Wrong number of parameters for describe");
    QWidget *curr=ResultTab->currentPage();
    Columns->show();
    Result->hide();
    if (curr==Result)
      ResultTab->showPage(Columns);
    return true;
  } else {
    QWidget *curr=ResultTab->currentPage();
    Columns->hide();
    Result->show();
    if (curr==Columns)
      ResultTab->showPage(Result);
    return false;
  }
}

void toWorksheet::query(const QString &str,bool direct)
{
  if (!Timer.isNull()&&!QueryString.isEmpty())
    addLog(QueryString,"Aborted");

  QRegExp strq("'[^']*'");
  QString chk=str.lower();
  chk.replace(strq," ");
  bool code=false;
  int pos=chk.find("end",0);
  while (pos>0) {  // Ignore position 0, since that isn't a block anyway
    QChar c=chk[pos-1];
    QChar ec=chk[pos+3];
    if (!toIsIdent(c)&&!toIsIdent(ec)) {
      code=true;
      break;
    }
    pos=chk.find("end",pos+1);
  }
  QString execSql=str;
  if (!code&&execSql.length()>0&&execSql.at(execSql.length()-1)==';')
    execSql.truncate(execSql.length()-1);
  QueryString=execSql;
  
  bool nobinds=false;
  chk=str.lower();
  chk.replace(strq," ");
  chk=chk.simplifyWhiteSpace();
  chk.replace(QRegExp(" or replace ")," ");
  if(chk.startsWith("create trigger "))
    nobinds=true;
  
  if (!describe(execSql)) {
    toQList param;
    if (!nobinds)
      param=toParamGet::getParam(this,execSql);
    toStatusMessage("Processing query",true);
    if (direct) {
      try {
	Timer.start();
	
	toQuery query(connection(),execSql,param);
	
	char buffer[100];
	if (query.rowsProcessed()>0)
	  sprintf(buffer,"%d rows processed",(int)query.rowsProcessed());
	else
	  sprintf(buffer,"Query executed");
	addLog(execSql,buffer);
      } catch (const QString &exc) {
	addLog(execSql,exc);
      }
    } else {
      Result->setNumberColumn(!WorksheetTool.config(CONF_NUMBER,"Yes").isEmpty());
      try {
	Result->query(execSql,param);
      } catch (const QString &exc) {
	addLog(execSql,exc);
      }
      Timer.start();
      if (StatisticButton->isOn())
	toRefreshParse(timer(),Refresh->currentText());
      
      Result->setSQLName(execSql.simplifyWhiteSpace().left(40));
    }
    StopButton->setEnabled(true);
    toMainWidget()->menuBar()->setItemEnabled(TO_ID_STOP,true);
  }
}

void toWorksheet::addLog(const QString &sql,const QString &result)
{
  QString now=toNow(connection());
  toResultViewItem *item;

  int dur=0;
  if (!Timer.isNull()) {
    dur=Timer.elapsed();
    QTime null;
    Timer=null;
  }

  if (WorksheetTool.config(CONF_LOG_MULTI,"Yes").isEmpty()) {
    if (WorksheetTool.config(CONF_LOG_AT_END,"Yes").isEmpty())
      item=new toResultViewItem(Logging,NULL);
    else
      item=new toResultViewItem(Logging,LastLogItem);
  } else if (WorksheetTool.config(CONF_LOG_AT_END,"Yes").isEmpty())
    item=new toResultViewMLine(Logging,NULL);
  else
    item=new toResultViewMLine(Logging,LastLogItem);
  item->setText(0,sql);
  
  LastLogItem=item;
  item->setText(1,result);
  item->setText(2,now);
  char buf[100];
  if (dur>=3600000) {
    sprintf(buf,"%d:%02d:%02d.%02d",dur/3600000,(dur/60000)%60,(dur/1000)%60,(dur/10)%100);
  } else {
    sprintf(buf,"%d:%02d.%02d",dur/60000,(dur/1000)%60,(dur/10)%100);
  }
  item->setText(3,buf);

  Logging->setCurrentItem(item);
  Logging->ensureItemVisible(item);

  {
    QString str=result;
    str+=" (Duration ";
    str+=buf;
    str+=")";
    toStatusMessage(str);
  }

  if (!toTool::globalConfig(CONF_AUTO_COMMIT,"").isEmpty())
    connection().commit();
  else
    connection().setNeedCommit();
  saveDefaults();
  changeResult(CurrentTab);
}

static void NewStatement(void)
{
  for (int i=0;Blocks[i].Start;i++)
    Blocks[i].Pos=0;
}

void toWorksheet::execute(bool all,bool step)
{
  bool sqlparse=!WorksheetTool.config(CONF_PLSQL_PARSE,"Yes").isEmpty();
  bool code=true;
  bool beforeCode=false;
  TryStrip=true;
  if (!Editor->hasMarkedText()||all||step) {
    int cpos,cline,cbpos,cbline;
    if (!Editor->getMarkedRegion(&cbline,&cbpos,&cline,&cpos)) {
      Editor->getCursorPosition(&cline,&cpos);
      step=false;
    }
    enum {
      beginning,
      comment,
      inStatement,
      inString,
      inCode,
      endCode,
      done
    } state,lastState;

    int startLine=-1,startPos=-1;
    int endLine=-1,endPos=-1;
    lastState=state=beginning;
    NewStatement();
    int BlockCount=0;
    beforeCode=code=TryStrip=false;
    QChar lastChar;
    QChar c=' ';
    QChar nc;

    for (int line=0;line<Editor->numLines()&&state!=done;line++) {
      QString data=Editor->textLine(line);
      c='\n'; // Set correct previous character
      for (int i=0;i<(int)data.length()&&state!=done;i++) {
	lastChar=c;
	c=data[i];
	if (i+1<int(data.length()))
	  nc=data[i+1];
	else
	  nc=' ';
	if (state==comment) {
	  state=lastState;
	  break;
	} else if (state!=inString&&c=='\'') {
	  lastState=state;
	  state=inString;
	} else {
	  switch(state) {
	  case comment:
	    throw QString("Internal error, comment shouldn't have gotten here.");
	  case endCode:
	    if (c==';')
	      state=inCode;
	    break;
	  case inCode:
	    if (c=='-'&&nc=='-') {
	      lastState=state;
	      state=comment;
	    } else {
	      for (int j=0;Blocks[j].Start;j++) {
		int &pos=Blocks[j].Pos;
		if (c.lower()==Blocks[j].Start[pos]&&!Blocks[j].Comment) {
		  if (pos>0||!toIsIdent(lastChar)) {
		    pos++;
		    if (!Blocks[j].Start[pos]) {
		      if (!toIsIdent(nc)) {
			if (Blocks[j].CloseBlock) {
			  BlockCount--;
			  if (BlockCount<=0)
			    state=inStatement;
			} else if (Blocks[j].WantEnd)
			  BlockCount++;
			NewStatement();
			if (state==inCode) {
			  if (Blocks[j].WantSemi)
			    state=endCode;
			  else
			    state=inCode;
			}
			break;
		      } else
			pos=0;
		    }
		  } else
		    pos=0;
		} else
		  pos=0;
	      }
	    }
	    break;
	  case beginning:
	    {
	      QString rest=data.right(data.length()-i).lower();
	      for (int j=0;Blocks[j].Start;j++) {
		if (Blocks[j].Comment) {
		  unsigned int len=strlen(Blocks[j].Start);
		  if (rest.lower().startsWith(Blocks[j].Start)&&(rest.length()<=len||!toIsIdent(rest.at(len)))) {
		    lastState=state;
		    state=comment;
		    break;
		  }
		}
	      }
	      if (state==comment)
		break;
	    }
	    if (c=='-'&&nc=='-') {
	      lastState=state;
	      state=comment;
	      break;
	    } else if (!c.isSpace()&&(c!='/'||data.length()!=1)) {
	      if (((line==cline&&i>cpos)||(line>cline))&&!all&&!step&&startLine>=0&&startPos>=0) {
		state=done;
		break;
	      } else
		beforeCode=code=false;
	      startLine=line;
	      startPos=i;
	      endLine=-1;
	      endPos=-1;
	      state=inStatement;
	    } else
	      break;
	  case inStatement:
	    if (!code&&sqlparse) {
	      bool br=false;
	      for (int j=0;Blocks[j].Start&&!br;j++) {
		int &pos=Blocks[j].Pos;
		if (c.lower()==Blocks[j].Start[pos]&&!Blocks[j].Comment) {
		  if (pos>0||(!toIsIdent(lastChar))) {
		    pos++;
		    if (!Blocks[j].Start[pos]) {
		      if (!toIsIdent(nc)) {
			if (Blocks[j].BeforeCode) {
			  beforeCode=true;
			  pos=0;
			  br=true;
			}
			if (beforeCode) {
			  if (Blocks[j].CloseBlock) {
			    toStatusMessage("Ending unstarted block");
			    return;
			  } else if (Blocks[j].StartCode) {
			    if (Blocks[j].WantEnd)
			      BlockCount++;

			    code=true;
			    if (Blocks[j].WantSemi)
			      state=endCode;
			    else
			      state=inCode;
			    NewStatement();
			    br=true;
			  }
			}
		      } else
			pos=0;
		    } 
		  } else
		    pos=0;
		} else
		  pos=0;
	      }
	      if (br)
		break;
	    }
	    if (c==';') {
	      endLine=line;
	      endPos=i+1;
	      state=beginning;
	      if (all) {
		Editor->setCursorPosition(startLine,startPos,false);
		Editor->setCursorPosition(endLine,endPos,true);
		if (Editor->hasMarkedText()) {
		  query(Editor->markedText(),true);
		  qApp->processEvents();
		  NewStatement();
		  beforeCode=code=false;
		}
	      } else if (step&&
			 ((line==cline&&i>cpos)||(line>cline))) {
		state=done;
		break;
	      }
	    }
	    break;
	  case inString:
	    if (c=='\'') {
	      state=lastState;
	    }
	    break;
	  case done:
	    break;
	  }
	}
      }
    }
    if (endLine==-1) {
      endLine=Editor->numLines()-1;
      endPos=Editor->textLine(endLine).length();
      if (all&&endLine) {
	Editor->setCursorPosition(startLine,startPos,false);
	Editor->setCursorPosition(endLine,endPos,true);
	if (Editor->hasMarkedText()) {
	  query(Editor->markedText(),false);
	}
      }
    }
    if (all) {
      Editor->setCursorPosition(0,0,false);
      Editor->setCursorPosition(endLine,endPos,true);
    } else {
      Editor->setCursorPosition(startLine,startPos,false);
      Editor->setCursorPosition(endLine,endPos,true);
    }
  }
  if (Editor->hasMarkedText()&&!all) {
    query(Editor->markedText(),false);
    if (CurrentTab==Plan)
      Plan->query(QueryString);
    else if (CurrentTab==Resources)
      viewResources();
  }
}

void toWorksheet::eraseLogButton()
{
  Logging->clear();
  LastLogItem=NULL;
}

void toWorksheet::queryDone(void)
{
  if (!Timer.isNull()&&!QueryString.isEmpty())
    addLog(QueryString,"Aborted");

  timer()->stop();
  StopButton->setEnabled(false);
  toMainWidget()->menuBar()->setItemEnabled(TO_ID_STOP,false);
  saveDefaults();
}

void toWorksheet::saveDefaults(void)
{
  QListViewItem *item=Result->firstChild();
  if (item) {
    QHeader *head=Result->header();
    for (int i=0;i<Result->columns();i++) {
      toResultViewItem *resItem=dynamic_cast<toResultViewItem *>(item);
      QString str;
      if (resItem)
	str=resItem->allText(i);
      else if (item)
	str=item->text(i);

      if (str=="{null}")
	str=QString::null;
      toParamGet::setDefault(head->label(i).lower(),str);
    }
  }
}

static toSQL SQLTimedStatistics("toWorksheet:EnableTimed",
				"ALTER SESSION SET TIMED_STATISTICS = TRUE",
				"Enable timed statistics for the current session");

void toWorksheet::enableStatistic(bool ena)
{
  if (ena) {
    if (toTool::globalConfig(CONF_LONG_SESSION,"").isEmpty())
      TOMessageBox::warning(this,
			    "Enable statistics",
			    "Enabling statistics without enabling the long sessions option\n"
			    "is unreliable at bests.",
			    "Ok");
    Result->setStatistics(Statistics);
    ResultTab->setTabEnabled(StatSplitter,true);
    toMainWidget()->menuBar()->setItemChecked(TO_ID_STATISTICS,true);
    Statistics->clear();
    if (!WorksheetTool.config(CONF_TIMED_STATS,"Yes").isEmpty()) {
      try {
	connection().allExecute(SQLTimedStatistics);
	connection().addInit(SQLTimedStatistics(connection()));
      } TOCATCH
    }
  } else {
    connection().delInit(SQLTimedStatistics(connection()));
    Result->setStatistics(NULL);
    ResultTab->setTabEnabled(StatSplitter,false);
    toMainWidget()->menuBar()->setItemChecked(TO_ID_STATISTICS,false);
  }
}

void toWorksheet::executeNewline(void)
{
  int cline,epos;
  Editor->getCursorPosition(&cline,&epos);

  if (cline>0)
    cline--;
  while(cline>0) {
    QString data=Editor->textLine(cline).simplifyWhiteSpace();
    if (data.length()==0||data==" ") {
      cline++;
      break;
    }
    cline--;
  }

  int eline=cline;
  while(eline<Editor->numLines()) {
    QString data=Editor->textLine(eline).simplifyWhiteSpace();
    if (data.length()==0||data==" ") {
      eline--;
      epos=Editor->textLine(eline).length();
      break;
    }
    eline++;
  }
  Editor->setCursorPosition(cline,0,false);
  Editor->setCursorPosition(eline,epos,true);
  if (Editor->hasMarkedText())
    query(Editor->markedText(),false);
}

void toWorksheet::commitButton(void)
{
  connection().commit();
}

void toWorksheet::rollbackButton(void)
{
  connection().rollback();
}
