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

TO_NAMESPACE;

#include <stdlib.h>

#ifdef TO_KDE
#include <kurl.h>
#include <ktempfile.h>
#include <kfiledialog.h>
#include <kio/netaccess.h>
#endif

#include <qapplication.h>
#include <qfiledialog.h>
#include <qstatusbar.h>
#include <qcombobox.h>
#include <qtimer.h>
#include <qtooltip.h>
#include <qtoolbar.h>
#include <qworkspace.h>
#include <qvbox.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qregexp.h>

#ifdef TO_KDE
#include <kapp.h>
#endif

#include "tohighlightedtext.h"
#include "tonewconnection.h"
#include "tomain.h"
#include "toconf.h"
#include "totool.h"
#include "tosql.h"

#define CHUNK_SIZE 63

static toSQL SQLUserNames(toSQL::TOSQL_USERLIST,
			  "SELECT UserName FROM All_Users ORDER BY UserName",
			  "List users in the database");

QString toReadValue(const otl_column_desc &dsc,otl_stream &q,int maxSize)
{
  char *buffer;

  try {
    switch (dsc.otl_var_dbtype) {
    default:  // Try using char if all else fails
      {
	// The *2 is for raw columns, also dates and numbers are a bit tricky
	// but if someone specifies a dateformat longer than 100 bytes he
	// deserves everything he gets!
	buffer=new char[max(dsc.dbsize*2+1,100)];
	buffer[0]=0;
	q>>buffer;
	if (q.is_null()) {
	  delete buffer;
	  return "{null}";
	}
	QString buf(QString::fromUtf8(buffer));
	delete buffer;
        return buf;
      }
      break;
    case otl_var_varchar_long:
    case otl_var_raw_long:
    case otl_var_clob:
    case otl_var_blob:
      {
	buffer=new char[maxSize+1];
	buffer[0]=0;
	otl_long_string data(buffer,maxSize);
	q>>data;
	buffer[maxSize]=0;
	if (q.is_null()) {
	  delete buffer;
	  return "{null}";
	}
	buffer[data.len()]=0; // Not sure if this is needed
	QString buf(QString::fromUtf8(buffer));
	delete buffer;
	return buf;
      }
      break;
    }
  } catch (...) {
    delete buffer;
    throw;
  }
}

static toSQL SQLTextPiece("Global:SQLText",
			  "SELECT SQL_Text\n"
			  "  FROM V$SQLText_With_Newlines\n"
			  " WHERE Address||':'||Hash_Value = :f1<char[100]>\n"
			  " ORDER BY Piece",
			  "Get text of SQL statement.");

QString toSQLString(toConnection &conn,const QString &address)
{
  QString sql;
  otl_stream q(1,
	       SQLTextPiece(conn),
	       conn.connection());

  q<<address.utf8();

  while(!q.eof()) {
    char buffer[100];
    q>>buffer;
    sql.append(QString::fromUtf8(buffer));
  }
  if (sql.isEmpty())
    throw QString("SQL Address not found in SGA");
  return sql;
}

static toSQL SQLNow("Global:Now",
		    "SELECT TO_CHAR(SYSDATE) FROM DUAL",
		    "Get current date/time from database");

QString toNow(toConnection &conn)
{
  otl_stream q(1,
	       SQLNow(conn),
	       conn.connection());
  char buffer[1024];
  buffer[0] = 0;
  q>>buffer;
  return QString::fromUtf8(buffer);
}

static toSQL SQLAddress("Global:Address",
			"SELECT Address||':'||Hash_Value\n"
			"  FROM V$SQLText_With_Newlines\n"
			" WHERE SQL_Text LIKE :f1<char[150]>||'%'",
			"Get address of an SQL statement");

QString toSQLToAddress(toConnection &conn,const QString &sql)
{
  QString search;

  otl_stream q(1,SQLAddress(conn),conn.connection());

  q<<sql.left(CHUNK_SIZE).utf8();

  while(!q.eof()) {
    char buf[100];
    q>>buf;
    if (sql==toSQLString(conn,QString::fromUtf8(buf)))
      return QString::fromUtf8(buf);
  }
  throw QString("SQL Query not found in SGA");
}

void toStatusMessage(const QString &str,bool save)
{
  toMain *main=dynamic_cast<toMain *>(qApp->mainWidget());
  if (main) {
    int sec=toTool::globalConfig(CONF_STATUS_MESSAGE,DEFAULT_STATUS_MESSAGE).toInt();
    if (save||sec==0)
      main->statusBar()->message(str);
    else
      main->statusBar()->message(str,sec*1000);
    QToolTip::add(main->statusBar(),str);
  }
}

toMain *toMainWidget(void)
{
  return dynamic_cast<toMain *>(qApp->mainWidget());
}

QComboBox *toRefreshCreate(QWidget *parent,const char *name,const char *def,QComboBox *item)
{
  QComboBox *refresh;
  if (item)
    refresh=item;
  else
    refresh=new QComboBox(false,parent);

  refresh->insertItem("None");
  refresh->insertItem("10 seconds");
  refresh->insertItem("30 seconds");
  refresh->insertItem("1 min");
  refresh->insertItem("10 min");
  QString str;
  if (def)
    str=def;
  else
    str=toTool::globalConfig(CONF_REFRESH,DEFAULT_REFRESH);
  if (str=="10 seconds")
    refresh->setCurrentItem(1);
  else if (str=="30 seconds")
    refresh->setCurrentItem(2);
  else if (str=="1 min")
    refresh->setCurrentItem(3);
  else if (str=="10 min")
    refresh->setCurrentItem(4);
  else
    refresh->setCurrentItem(0);
  return refresh;
}

void toRefreshParse(QTimer *timer,const QString &str)
{
  if (str=="None")
    timer->stop();
  else if (str=="10 seconds")
    timer->start(10*1000);
  else if (str=="30 seconds")
    timer->start(30*1000);
  else if (str=="1 min")
    timer->start(60*1000);
  else if (str=="10 min")
    timer->start(600*1000);
  else
    throw QString("Unknown timer value");
}

QString toDeepCopy(const QString &str)
{
  QString ret=str;
  ret+=" ";
  return ret.left(ret.length()-1);
}

#ifdef ENABLE_STYLE
#include <qmotifstyle.h>
#include <qmotifplusstyle.h>
#include <qsgistyle.h>
#include <qcdestyle.h>
#include <qwindowsstyle.h>
#include <qplatinumstyle.h>

void toSetSessionType(const QString &str)
{
  if (str=="Motif")
    qApp->setStyle(new QMotifStyle());
  else if (str=="Motif Plus")
    qApp->setStyle(new QMotifPlusStyle());
  else if (str=="SGI")
    qApp->setStyle(new QSGIStyle());
  else if (str=="CDE")
    qApp->setStyle(new QCDEStyle());
  else if (str=="Windows")
    qApp->setStyle(new QWindowsStyle());
  else if (str=="Platinum")
    qApp->setStyle(new QPlatinumStyle());
  else
    throw QString("Failed to find style match");
}

QString toGetSessionType(void)
{
  QStyle *style=&qApp->style();
  if (dynamic_cast<QMotifPlusStyle *>(style))
    return "Motif Plus";
  else if (dynamic_cast<QSGIStyle *>(style))
    return "SGI";
  else if (dynamic_cast<QCDEStyle *>(style))
    return "CDE";
  else if (dynamic_cast<QMotifStyle *>(style))
    return "Motif";
  else if (dynamic_cast<QPlatinumStyle *>(style))
    return "Platinum";
  else if (dynamic_cast<QWindowsStyle *>(style))
    return "Windows";
  throw QString("Failed to find style match");
}
#endif

QToolBar *toAllocBar(QWidget *parent,const QString &str,const QString &db)
{
  QString name=str;
  if (db.isEmpty()&&!toTool::globalConfig(CONF_DB_TITLE,"Yes").isEmpty()) {
    name+=" ";
    name+=db;
  }
  QToolBar *tool;
#ifdef TO_KDE // Will only work after KDE2.2
  if (parent==toMainWidget())
    tool=new KToolBar(toMainWidget(),QMainWindow::Top);
  else {
#if KDE_VERSION < 220
    tool=new QToolBar(name,toMainWidget(),parent);
#else
    tool=new KToolBar(toMainWidget(),parent);
#endif
  }
#else
  if (parent==toMainWidget())
    tool=new QToolBar(name,toMainWidget());
  else
    tool=new QToolBar(name,toMainWidget(),parent);
#endif
  return tool;
}

TODock *toAllocDock(const QString &name,
		    const QString &db,
		    const QPixmap &icon)
{
  QString str=name;
  if (db.isEmpty()&&!toTool::globalConfig(CONF_DB_TITLE,"Yes").isEmpty()) {
    str+=" ";
    str+=db;
  }
#ifdef TO_KDE
  KDockMainWindow *main=dynamic_cast<KDockMainWindow *>(toMainWidget());
  if (main)
    return main->createDockWidget(str,icon);
  else
    throw QString("Main widget not KDockMainWindow");
#else
#  if QT_VERSION < 300
  if (toTool::globalConfig(CONF_DOCK_TOOLBAR,"Yes").isEmpty()) {
    QVBox *frm=new QVBox(toMainWidget()->workspace());
    frm->setCaption(str);
    return frm;
  } else {
    QToolBar *toolbar=toAllocBar(toMainWidget(),name,db);
    return toolbar;
  }
#  else
  return new QDockWindow(QDockWindow::InDock,toMainWidget());
#  endif
#endif
}

void toAttachDock(TODock *dock,QWidget *container,QMainWindow::ToolBarDock place)
{
#ifdef TO_KDE
  KDockMainWindow *main=dynamic_cast<KDockMainWindow *>(toMainWidget());
  if (main) {
    KDockWidget::DockPosition pos;
    switch (place) {
    case QMainWindow::Top:
      pos=KDockWidget::DockTop;
      break;
    case QMainWindow::Bottom:
      pos=KDockWidget::DockBottom;
      break;
    case QMainWindow::Left:
      pos=KDockWidget::DockLeft;
      break;
    case QMainWindow::Right:
      pos=KDockWidget::DockRight;
      break;
    default:
      throw QString("Unknown dock position");
    }
    KDockWidget *dw=dynamic_cast<KDockWidget *>(dock);
    if (dw) {
      dw->setWidget(container);
      dw->manualDock(main->getMainDockWidget(),pos,20);
    }
  } else
    throw QString("Main widget not KDockMainWindow");
#else
#  if QT_VERSION < 300
  QToolBar *bar=dynamic_cast<QToolBar *>(dock);
  if (bar) {
    toMainWidget()->moveToolBar(bar,place);
    bar->setStretchableWidget(container);
  }
#  else
  QDockWindow *d=dynamic_cast<QDockWindow *>(dock);
  if (d) {
    d->setResizeEnabled(true);
    d->setWidget(container);
    toMainWidget()->moveDockWindow(d,place);
  }
#  endif
#endif
}

list<QString> toReadQuery(otl_stream &str,list<QString> &args)
{
  otl_null null;
  for (list<QString>::iterator i=((list<QString> &)args).begin();i!=((list<QString> &)args).end();i++) {
    if ((*i).isNull())
      str<<null;
    else
      str<<(*i).utf8();
  }

  int len;
  otl_column_desc *dsc=str.describe_select(len);
  list<QString> ret;
  int MaxColSize=toTool::globalConfig(CONF_MAX_COL_SIZE,DEFAULT_MAX_COL_SIZE).toInt();
  for (int col=0;!str.eof();col++) {
    if (col==len)
      col=0;
    QString dat=toReadValue(dsc[col],str,MaxColSize);
    if (dat=="{null}")
      dat=QString::null;
    ret.insert(ret.end(),dat);
  }
  return ret;
}

list<QString> toReadQuery(otl_stream &str,
			  const QString &arg1,const QString &arg2,
			  const QString &arg3,const QString &arg4,
			  const QString &arg5,const QString &arg6,
			  const QString &arg7,const QString &arg8,
			  const QString &arg9)
{
  int numArgs;
  if (!arg9.isNull())
    numArgs=9;
  else if (!arg8.isNull())
    numArgs=8;
  else if (!arg7.isNull())
    numArgs=7;
  else if (!arg6.isNull())
    numArgs=6;
  else if (!arg5.isNull())
    numArgs=5;
  else if (!arg4.isNull())
    numArgs=4;
  else if (!arg3.isNull())
    numArgs=3;
  else if (!arg2.isNull())
    numArgs=2;
  else if (!arg1.isNull())
    numArgs=1;
  else
    numArgs=0;

  list<QString> args;
  if (numArgs>0)
    args.insert(args.end(),arg1);
  if (numArgs>1)
    args.insert(args.end(),arg2);
  if (numArgs>2)
    args.insert(args.end(),arg3);
  if (numArgs>3)
    args.insert(args.end(),arg4);
  if (numArgs>4)
    args.insert(args.end(),arg5);
  if (numArgs>5)
    args.insert(args.end(),arg6);
  if (numArgs>6)
    args.insert(args.end(),arg7);
  if (numArgs>7)
    args.insert(args.end(),arg8);
  if (numArgs>8)
    args.insert(args.end(),arg9);

  return toReadQuery(str,args);
}

list<QString> toReadQuery(toConnection &conn,const QCString &query,list<QString> &args)
{
  otl_stream str;
  str.set_all_column_types(otl_all_num2str|otl_all_date2str);
  str.open(1,
	   query,
	   conn.connection());
  return toReadQuery(str,args);
}

list<QString> toReadQuery(toConnection &conn,const QCString &query,
			  const QString &arg1,const QString &arg2,
			  const QString &arg3,const QString &arg4,
			  const QString &arg5,const QString &arg6,
			  const QString &arg7,const QString &arg8,
			  const QString &arg9)
{
  int numArgs;
  if (!arg9.isNull())
    numArgs=9;
  else if (!arg8.isNull())
    numArgs=8;
  else if (!arg7.isNull())
    numArgs=7;
  else if (!arg6.isNull())
    numArgs=6;
  else if (!arg5.isNull())
    numArgs=5;
  else if (!arg4.isNull())
    numArgs=4;
  else if (!arg3.isNull())
    numArgs=3;
  else if (!arg2.isNull())
    numArgs=2;
  else if (!arg1.isNull())
    numArgs=1;
  else
    numArgs=0;

  list<QString> args;
  if (numArgs>0)
    args.insert(args.end(),arg1);
  if (numArgs>1)
    args.insert(args.end(),arg2);
  if (numArgs>2)
    args.insert(args.end(),arg3);
  if (numArgs>3)
    args.insert(args.end(),arg4);
  if (numArgs>4)
    args.insert(args.end(),arg5);
  if (numArgs>5)
    args.insert(args.end(),arg6);
  if (numArgs>6)
    args.insert(args.end(),arg7);
  if (numArgs>7)
    args.insert(args.end(),arg8);
  if (numArgs>8)
    args.insert(args.end(),arg9);

  return toReadQuery(conn,query,args);
}

QString toShift(list<QString> &lst)
{
  if (lst.begin()==lst.end())
    return QString::null;
  QString ret=(*lst.begin());
  lst.erase(lst.begin());
  return ret;
}

void toUnShift(list<QString> &lst,const QString &str)
{
  lst.insert(lst.begin(),str);
}

QString toPop(list<QString> &lst)
{
  if (lst.begin()==lst.end())
    return QString::null;
  QString ret=(*lst.rbegin());
  lst.pop_back();
  return ret;
}

void toPush(list<QString> &lst,const QString &str)
{
  lst.push_back(str);
}

QString toFontToString(const QFont &fnt)
{
#if QT_VERSION >= 300
  return fnt.toString();
#else
#  ifdef TO_FONT_RAW_NAME
  return fnt.rawName();
#  else
  QStringList lst;
  lst.insert(lst.end(),fnt.family());
  lst.insert(lst.end(),QString::number(fnt.pointSize()));
  lst.insert(lst.end(),QString::number(fnt.weight()));
  lst.insert(lst.end(),QString::number(fnt.italic()));
  lst.insert(lst.end(),QString::number(fnt.charSet()));
  return lst.join("/");
#  endif
#endif
}

QFont toStringToFont(const QString &str)
{
  if (str.isEmpty())
    return QFont("Courier",12);
#if QT_VERSION >= 300
  QFont fnt;
  fnt.fromString(str);
  return fnt;
#else
#  ifdef TO_FONT_RAW_NAME
  QFont fnt;
  fnt.setRawName(str);
  return fnt;
#  else
  QStringList lst=QStringList::split("/",str);
  if (lst.count()!=5)
    return QFont("Courier",12);
  QFont font(lst[0],lst[1].toInt(),lst[2].toInt(),
	     bool(lst[3].toInt()),QFont::CharSet(lst[4].toInt()));
  return font;
#  endif
#endif
}

int toSizeDecode(const QString &str)
{
  if (str=="KB")
    return 1024;
  if (str=="MB")
    return 1024*1024;
  return 1;
}

QString toHelpPath(void)
{
  QString str=toTool::globalConfig(CONF_HELP_PATH,"");
  if (str.isEmpty()) {
    str=DEFAULT_PLUGIN_DIR;
    str+="/help";
  }
  return str;
}

static QString toExpandFile(const QString &file)
{
  QString ret(file);
  const char *home=getenv("HOME");
  if (!home)
    home="";
  ret.replace(QRegExp("\\$HOME"),home);
  return ret;
}

QCString toReadFile(const QString &filename)
{
  QString expanded=toExpandFile(filename);
#ifdef TO_KDE
  KURL url(expanded);
  if (!url.isLocalFile()) {
    QString tmpFile;
    if(KIO::NetAccess::download(url,tmpFile)) {
      QFile file(tmpFile);
      if (!file.open(IO_ReadOnly)) {
	KIO::NetAccess::removeTempFile(tmpFile);
	throw QString("Couldn't open file %1.").arg(filename);
      }

      int size=file.size();

      char *buf=new char[size+1];
      if (file.readBlock(buf,size)==-1) {
	delete buf;
	KIO::NetAccess::removeTempFile(tmpFile);
	throw QString("Encountered problems read configuration");
      }
      buf[size]=0;
      QCString ret(buf,size+1);
      delete buf;
      KIO::NetAccess::removeTempFile(tmpFile);
      return ret;
    }
    throw QString("Couldn't download file");
  }
#endif
  QFile file(expanded);
  if (!file.open(IO_ReadOnly))
    throw QString("Couldn't open file %1.").arg(filename);
	    
  int size=file.size();
	    
  char *buf=new char[size+1];
  if (file.readBlock(buf,size)==-1) {
    delete buf;
    throw QString("Encountered problems read configuration");
  }
  buf[size]=0;
  QCString ret(buf,size+1);
  delete buf;
  return ret;
}

bool toWriteFile(const QString &filename,const QCString &data)
{
  QString expanded=toExpandFile(filename);
#ifdef TO_KDE
  KURL url(expanded);
  if (!url.isLocalFile()) {
    KTempFile file;
    file.file()->writeBlock(data,data.length());
    if (file.status()!=IO_Ok) {
      TOMessageBox::warning(toMainWidget(),"File error","Couldn't write data to tempfile");
      file.unlink();
      return false;
    }
    file.close();
    if (!KIO::NetAccess::upload(file.name(),url)) {
      file.unlink();
      TOMessageBox::warning(toMainWidget(),"File error","Couldn't upload data to URL");
      return false;
    }
    file.unlink();
    toStatusMessage("File saved successfully");
    return true;
  }
#endif

  QFile file(expanded);
  if (!file.open(IO_WriteOnly)) {
    TOMessageBox::warning(toMainWidget(),"File error","Couldn't open file for writing");
    return false;
  }
  file.writeBlock(data,data.length());
  if (file.status()!=IO_Ok) {
    TOMessageBox::warning(toMainWidget(),"File error","Couldn't write data to file");
    return false;
  }
  toStatusMessage("File saved successfully");
  return true;
}

bool toWriteFile(const QString &filename,const QString &data)
{
  return toWriteFile(filename,data.local8Bit());
}

bool toCompareLists(QStringList &lst1,QStringList &lst2,unsigned int len)
{
  if (lst1.count()<len||lst2.count()<len)
    return false;
  for (unsigned int i=0;i<len;i++)
    if (lst1[i]!=lst2[i])
      return false;
  return true;
}

QString toOpenFilename(const QString &filename,const QString &filter,QWidget *parent)
{
#ifdef TO_KDE
  KURL url=TOFileDialog::getOpenURL(filename,filter,parent);
  if (url.isEmpty())
    return QString::null;
  if (url.isLocalFile())
    return url.path();
  return url.url();
#else
  return TOFileDialog::getOpenFileName(filename,filter,parent);
#endif
}

QString toSaveFilename(const QString &filename,const QString &filter,QWidget *parent)
{
#ifdef TO_KDE
  KURL url=TOFileDialog::getSaveURL(filename,filter,parent);
  if (url.hasPass())
    TOMessageBox::warning(toMainWidget(),"File open password",url.pass());
  if (url.isEmpty())
    return QString::null;
  if (url.isLocalFile())
    return url.path();
  return url.url();
#else
  return TOFileDialog::getSaveFileName(filename,filter,parent);
#endif
}

void toSetEnv(const QCString &var,const QCString &val)
{
#ifndef TO_HAS_SETENV
  // Has a memory leak, but just a minor one.

  char *env=new char[var.length()+val.length()+2];
  strcpy(env,var);
  strcat(env,"=");
  strcat(env,val);
  putenv(env);
#else
  setenv(var,val,1);
#endif
}

void toUnSetEnv(const QCString &var)
{
#ifndef TO_HAS_SETENV
  toSetEnv(var,"");
#else
  unsetenv(var);
#endif
}

#ifdef TO_DEBUG_MEMORY
#  ifndef LARGE_NUMBER
#    define LARGE_NUMBER 1024
#  endif

#include "tothread.h"

static toLock Lock;
static void **Pointers=NULL;
static size_t *PointerInd=NULL;
static size_t *PointerSize=NULL;
static size_t PointerNum=0;
static size_t PointerAlloc=0;
static size_t PointerIndex=0;
static size_t PointerStartup=0;
static bool PointerError=false;

struct toPointerInfo toGetPointerInfo(int ind)
{
  toLocker current(Lock);
  toPointerInfo data;
  if (ind<-int(PointerStartup)||
      ind>=int((PointerNum-PointerStartup)/sizeof(void *))) {
    data.index=ind;
    data.allocIndex=0;
    data.size=0;
    data.ptr=NULL;
  } else {
    data.index=ind;
    data.allocIndex=PointerInd[ind+PointerStartup/sizeof(void *)];
    data.size=PointerSize[ind+PointerStartup/sizeof(void *)];
    data.ptr=Pointers[ind+PointerStartup/sizeof(void *)];
  }
  return data;
}

void toInitPointers(void)
{
  toLocker current(Lock);
  PointerStartup=PointerNum;
}

void toPointerExpectLost(int num)
{
  toLocker current(Lock);
  PointerStartup+=num*sizeof(void *);
}

struct toPointerInfo toGetPointerInfo(void *p)
{
  int i;
  Lock.lock();
  for (i=0;i<int(PointerNum/sizeof(void *));i++)
    if (Pointers[i]==p)
      break;
  int ind=i-PointerStartup;
  Lock.unlock();
  return toGetPointerInfo(ind);
}

size_t toCurrentIndex(void)
{
  return PointerIndex;
}

size_t toGetAllocNum(void)
{
  toLocker current(Lock);
  return (PointerNum-PointerStartup)/sizeof(void *);
}

static void *getmem(size_t l)
{
  toLocker current(Lock);
  void *p;
  if (l==0) {
    printf ("Tried to allocate 0 bytes of memory\n");
    l=1;
  }
  p=malloc(l);
  if (!p) {
    fprintf (stderr,"Failed to allocated %lu bytes of memory\n",
	     (unsigned long)l);
    PointerError=true;
    return NULL;
  }
  if (PointerNum==PointerAlloc) {
    void **t;
    size_t *s;
    PointerAlloc+=LARGE_NUMBER;
    t=(void **)malloc(PointerAlloc);
    if(Pointers) {
      memmove(t,Pointers,PointerAlloc-LARGE_NUMBER);
      free(Pointers);
    }
    Pointers=t;
    s=(size_t *)malloc(PointerAlloc*sizeof(size_t)/sizeof(void *));
    if(PointerSize) {
      memmove(s,PointerSize,(PointerAlloc-LARGE_NUMBER)*sizeof(size_t)/
	      sizeof(void *));
      free(PointerSize);
    }
    PointerSize=s;
    s=(size_t *)malloc(PointerAlloc*sizeof(size_t)/sizeof(void *));
    if(PointerInd) {
      memmove(s,PointerInd,(PointerAlloc-LARGE_NUMBER)*sizeof(size_t)/
	      sizeof(void *));
      free(PointerInd);
    }
    PointerInd=s;
  }
  Pointers[PointerNum/sizeof(void *)]=p;
  PointerInd[PointerNum/sizeof(void *)]=PointerIndex;
  PointerIndex++;
  PointerSize[PointerNum/sizeof(void *)]=l;
  PointerNum+=sizeof(void *);
  return p;
}

void *operator new[](size_t l)
{
  return getmem(l);
}

void *operator new(size_t l)
{
  return getmem(l);
}

void del (void *p)
{
  if (!p)
    return;
  toLocker current(Lock);
  unsigned int i;
  for (i=0;i<PointerNum/sizeof(void *);i++)
    if (Pointers[i]==p)
      break;
  if(i==(PointerNum/sizeof(void *))) {
    for (i=0;i<PointerNum/sizeof(void *);i++)
      if (Pointers[i]>p&&(char *)(Pointers[i])+PointerSize[i]<p)
	break;
    if(PointerNum==(PointerNum/sizeof(void *)))
      fprintf (stderr,"Tried to dealloc unalloced memory (Withing block with index %d)\n",i);
    else
      fprintf (stderr,"Tried to dealloc unalloced memory\n");
    PointerError=true;
    return;
  }
  if(PointerNum>=(i+1)*sizeof(void *)) {
    memmove(Pointers+i,Pointers+i+1,PointerNum-(i+1)*sizeof(void *));
    memmove(PointerSize+i,PointerSize+i+1,(PointerNum-(i+1)*sizeof(void *))*
	    sizeof(size_t)/sizeof(void *));
    memmove(PointerInd+i,PointerInd+i+1,(PointerNum-(i+1)*sizeof(void *))*
	    sizeof(size_t)/sizeof(void *));
  }
  PointerNum-=sizeof(void *);
  free(p);
}

void operator delete[](void *p)
{
  del (p);
}

void operator delete(void *p)
{
  del(p);
}

static void DisplayLast(char *p,size_t ind)
{
  int start;
  if (!ind)
    return;
  printf ("  ");
  for (start=int((ind-1)&~0xf);start<int(ind);start++) {
    if (!(start%8))
      printf (" ");
    if (isprint(p[start]))
      printf ("%c",p[start]);
    else
      printf (".");
  }
}

int toDisplayMemLost(void)
{
  printf("Lost %lu blocks of memory\n",(unsigned long)(toGetAllocNum()));
  size_t i;
  for (i=0;i<toGetAllocNum();i++) {
    toPointerInfo ptr=toGetPointerInfo(i);
    toLocker current(Lock);    
    printf ("  Index    :%lu\n",(unsigned long)(ptr.index));
    printf ("    Alloced:%lu\n",(unsigned long)(ptr.allocIndex));
    printf ("    Size   :%lu\n",(unsigned long)(ptr.size));
    printf ("    Content:");
    size_t j;
    for (j=0;j<ptr.size;j++) {
      if (!(j%16)) {
	DisplayLast((char *)ptr.ptr,j);
	printf ("\n");
      } else if (!(j%8)) {
	printf ("  ");
      }
      printf ("%02x ",((unsigned char *)ptr.ptr)[j]);
    }
    DisplayLast((char *)ptr.ptr,j);
    printf("\n");
  }
  if (toGetAllocNum())
    return 2;
  if (PointerError)
    return 3;
  return 0;
}

int toToolMenuIndex(void)
{
  return toMainWidget()->menuBar()->indexOf(TO_WINDOWS_MENU);
}
#endif
