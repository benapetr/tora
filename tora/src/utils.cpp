/*****
*
* TOra - An Oracle Toolkit for DBA's and developers
* Copyright (C) 2003-2005 Quest Software, Inc
* Portions Copyright (C) 2005 Other Contributors
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
*      Specifically you are not permittgetOpenFileNameed to link this program with the
*      Qt/UNIX, Qt/Windows or Qt Non Commercial products of TrollTech.
*      And you are not permitted to distribute binaries compiled against
*      these libraries without written consent from Quest Software, Inc.
*      Observe that this does not disallow linking to the Qt Free Edition.
*
*      You may link this product with any GPL'd Qt library such as Qt/Free
*
* All trademarks belong to their respective owners.
*
*****/

#include "utils.h"

#include "tobackground.h"
#include "toconf.h"
#include "toconnection.h"
#include "tohighlightedtext.h"
#include "tomain.h"
#include "toresult.h"
#include "tosql.h"
#include "tothread.h"
#include "totool.h"

#include <stdlib.h>
#include <algorithm>

#include <qcombobox.h>
#include <qcursor.h>
#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>
#include <totreewidget.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qtimer.h>

#include <QApplication>
#include <QToolBar>
#include <QStatusBar>
#include <qworkspace.h>         // todo, obsolete
#include <QKeyEvent>
#include <QPixmap>
#include <QBrush>
#include <QColor>
#include <QFileDialog>
#include <QDockWidget>
#include <QTextStream>

#ifdef Q_OS_WIN32
#  include "windows/cregistry.h"
# include "windows.h"
#endif

#if defined(Q_OS_MACX)
#include <sys/param.h>
#include <CoreServices/CoreServices.h>
#endif // Q_OS_MACX


#define CHUNK_SIZE 31

// A little magic to get lrefresh to work and get a check on qApp

#undef QT_TRANSLATE_NOOP
#define QT_TRANSLATE_NOOP(x,y) QTRANS(x,y)

static toSQL SQLUserNamesMySQL(toSQL::TOSQL_USERLIST,
                               "SHOW DATABASES",
                               "List users in the database",
                               "3.0",
                               "MySQL");

static toSQL SQLUserNames(toSQL::TOSQL_USERLIST,
                          "SELECT UserName FROM sys.All_Users ORDER BY UserName",
                          "");

static toSQL SQLUserNamesPgSQL(toSQL::TOSQL_USERLIST,
                               "SELECT nspname AS UserName FROM pg_namespace ORDER BY nspname",
                               "",
                               "7.1",
                               "PostgreSQL");

static toSQL SQLUserNamesSapDB(toSQL::TOSQL_USERLIST,
                               "SELECT username \"UserName\" FROM users ORDER BY username",
                               "",
                               "",
                               "SapDB");

static toSQL SQLTextPiece("Global:SQLText",
                          "SELECT SQL_Text\n"
                          "  FROM V$SQLText_With_Newlines\n"
                          " WHERE Address||':'||Hash_Value = :f1<char[100]>\n"
                          " ORDER BY Piece",
                          "Get text of SQL statement.");

QString toSQLString(toConnection &conn, const QString &address)
{
    QString sql;

    toQList vals = toQuery::readQuery(conn, SQLTextPiece, address);

    for (toQList::iterator i = vals.begin();i != vals.end();i++)
    {
        sql.append(*i);
    }
    if (sql.isEmpty())
        throw qApp->translate("toSQLString", "SQL Address not found in SGA");
    return sql;
}

static toSQL SQLNowMySQL("Global:Now",
                         "SELECT now()",
                         "Get current date/time from database",
                         "3.0",
                         "MySQL");
static toSQL SQLNow("Global:Now",
                    "SELECT TO_CHAR(SYSDATE) FROM sys.DUAL",
                    "");

static toSQL SQLNowPgSQL("Global:Now",
                         "SELECT now()",
                         "",
                         "7.1",
                         "PostgreSQL");

QString toNow(toConnection &conn)
{
    try
    {
        toQList vals = toQuery::readQuery(conn, SQLNow);
        return toPop(vals);
    }
    catch (...)
    {
        return qApp->translate("toNow", "Unexpected error");
    }
}

QString toSQLStripSpecifier(const QString &sql)
{
    QString ret;
    char inString = 0;
    for (int i = 0;i < sql.length();i++)
    {
        QChar rc = sql.at(i);
        char c = rc.toLatin1();
        if (inString)
        {
            if (c == inString)
            {
                inString = 0;
            }
            ret += c;
        }
        else
        {
            switch (c)
            {
            case '\'':
                inString = '\'';
                ret += rc;
                break;
            case '\"':
                inString = '\"';
                ret += rc;
                break;
            case ':':
                ret += rc;
                for (i++;i < sql.length();i++)
                {
                    rc = sql.at(i);
                    c = rc.toLatin1();
                    if (!rc.isLetterOrNumber())
                        break;
                    ret += rc;
                }
                if (c == '<')
                {
                    ret += QString::fromLatin1(" ");
                    for (i++;i < sql.length();i++)
                    {
                        rc = sql.at(i);
                        c = rc.toLatin1();
                        ret += QString::fromLatin1(" ");
                        if (c == '>')
                        {
                            i++;
                            break;
                        }
                    }
                }
                i--;
                break;
            default:
                ret += rc;
            }
        }
    }
    return ret;
}

QString toSQLStripBind(const QString &sql)
{
    QString ret;
    char inString = 0;
    for (int i = 0;i < sql.length();i++) {
        QChar rc = sql.at(i);
        char  c  = rc.toLatin1(); // current
        char  n  = 0;           // next
        if(i + 1 < sql.length())
            n = sql.at(i + 1).toLatin1();

        if (inString)
        {
            if (c == inString)
            {
                inString = 0;
            }
            ret += rc;
        }
        else
        {
            switch (char(c))
            {
            case '\'':
                inString = '\'';
                ret += rc;
                break;
            case '\"':
                inString = '\"';
                ret += rc;
                break;
            case ':':
                // don't nuke my postgres-style casts
                if(n == ':') {
                    ret += rc;
                    ret += n;
                    i++;
                    break;
                }

                ret += QString::fromLatin1("''");
                for (i++;i < sql.length();i++)
                {
                    rc = sql.at(i);
                    c = rc.toLatin1();
                    if (!rc.isLetterOrNumber())
                        break;
                }
                if (c == '<')
                {
                    for (i++;i < sql.length();i++)
                    {
                        rc = sql.at(i);
                        c = rc.toLatin1();
                        if (c == '>')
                        {
                            i++;
                            break;
                        }
                    }
                }
                i--;
                break;
            default:
                ret += rc;
            }
        }
    }
    return ret;
}

static toSQL SQLAddress("Global:Address",
                        "SELECT Address||':'||Hash_Value\n"
                        "  FROM V$SQLText_With_Newlines\n"
                        " WHERE SQL_Text LIKE :f1<char[150]>||'%'",
                        "Get address of an SQL statement");

QString toSQLToAddress(toConnection &conn, const QString &sql)
{
    QString search = toSQLStripSpecifier(sql);

    toQList vals = toQuery::readQuery(conn, SQLAddress, search.left(CHUNK_SIZE));

    for (toQList::iterator i = vals.begin();i != vals.end();i++)
    {
        if (search == toSQLString(conn, *i))
            return *i;
    }
    throw qApp->translate("toSQLToAddress", "SQL Query not found in SGA");
}

static std::list<QString> LastMessages;

void toStatusMessage(const QString &str, bool save, bool log) {

    // causes no errors to display.  if we need to we may have to post
    // a message to the statusbar with the queued flag instead.
// #ifndef Q_OS_WIN32
//     if (!toThread::mainThread()) {
//         return;
//     }
// #endif

    toMain *main = toMainWidget();
    if (main)
    {
        if (!str.isEmpty())
        {
            int sec = toConfigurationSingle::Instance().globalConfig(CONF_STATUS_MESSAGE, DEFAULT_STATUS_MESSAGE).toInt();
            if (save || sec == 0)
                main->statusBar()->showMessage(str.simplified());
            else
                main->statusBar()->showMessage(str.simplified(), sec*1000);
            if (!save && log)
            {
                if (toConfigurationSingle::Instance().globalConfig(CONF_MESSAGE_STATUSBAR, "").isEmpty())
                    main->displayMessage(str);
                toPush(LastMessages, str);
                if (int(LastMessages.size()) > toConfigurationSingle::Instance().globalConfig(CONF_STATUS_SAVE,
                        DEFAULT_STATUS_SAVE).toInt())
                    toShift(LastMessages);
            }
            main->statusBar()->setToolTip(str);
        }
    }

}

std::list<QString> toStatusMessages(void)
{
    return LastMessages;
}

QComboBox *toRefreshCreate(QWidget *parent, const char *name, const QString &def, QComboBox *item)
{
    QComboBox *refresh;
    if (item)
        refresh = item;
    else {
        refresh = new QComboBox(parent);
        refresh->setObjectName(name);
        refresh->setEditable(false);
    }

    refresh->addItem(qApp->translate("toRefreshCreate", "None"));
    refresh->addItem(qApp->translate("toRefreshCreate", "2 seconds"));
    refresh->addItem(qApp->translate("toRefreshCreate", "5 seconds"));
    refresh->addItem(qApp->translate("toRefreshCreate", "10 seconds"));
    refresh->addItem(qApp->translate("toRefreshCreate", "30 seconds"));
    refresh->addItem(qApp->translate("toRefreshCreate", "1 min"));
    refresh->addItem(qApp->translate("toRefreshCreate", "5 min"));
    refresh->addItem(qApp->translate("toRefreshCreate", "10 min"));
    QString str;
    if (!def.isNull())
        str = def;
    else
        str = toConfigurationSingle::Instance().globalConfig(CONF_REFRESH, DEFAULT_REFRESH);
    if (str == "2 seconds")
        refresh->setCurrentIndex(1);
    else if (str == "5 seconds")
        refresh->setCurrentIndex(2);
    else if (str == "10 seconds")
        refresh->setCurrentIndex(3);
    else if (str == "30 seconds")
        refresh->setCurrentIndex(4);
    else if (str == "1 min")
        refresh->setCurrentIndex(5);
    else if (str == "5 min")
        refresh->setCurrentIndex(6);
    else if (str == "10 min")
        refresh->setCurrentIndex(7);
    else
        refresh->setCurrentIndex(0);
    return refresh;
}

void toRefreshParse(toTimer *timer, const QString &str)
{
    QString t = str;
    if (t.isEmpty())
        t = toConfigurationSingle::Instance().globalConfig(CONF_REFRESH, DEFAULT_REFRESH);

    if (t == qApp->translate("toRefreshCreate", "None") || t == "None")
        timer->stop();
    else if (t == qApp->translate("toRefreshCreate", "2 seconds") || t == "2 seconds")
        timer->start(2*1000);
    else if (t == qApp->translate("toRefreshCreate", "5 seconds") || t == "5 seconds")
        timer->start(5*1000);
    else if (t == qApp->translate("toRefreshCreate", "10 seconds") || t == "10 seconds")
        timer->start(10*1000);
    else if (t == qApp->translate("toRefreshCreate", "30 seconds") || t == "30 seconds")
        timer->start(30*1000);
    else if (t == qApp->translate("toRefreshCreate", "1 min") || t == "1 min")
        timer->start(60*1000);
    else if (t == qApp->translate("toRefreshCreate", "5 min") || t == "5 min")
        timer->start(300*1000);
    else if (t == qApp->translate("toRefreshCreate", "10 min") || t == "10 min")
        timer->start(600*1000);
    else
        throw qApp->translate("toRefreshParse", "Unknown timer value");
}

QString toDeepCopy(const QString &str) {
    return QString(str.data(), str.length());
}

// Why is this optional?
#ifdef ENABLE_STYLE
#    include <qstylefactory.h>
#    include <qstyle.h>

QStringList toGetSessionTypes(void)
{
    return QStyleFactory::keys();
}

QString toGetSessionType(void)
{
    QStyle  *style = qApp->style();
    QString  sname = style->metaObject()->className();

    // This is probably really slow, but who cares.

    QStringList keys = QStyleFactory::keys();
    for (int i = 0;i < keys.size();i++)
    {
        QString name = keys[i];
        QStyle *t = QStyleFactory::create(name);
        if (!t)
            continue;
        if (sname == t->metaObject()->className())
        {
            delete t;
            return name;
        }
        delete t;
    }

    // Weird should never get here.
    return sname;
}

void toSetSessionType(const QString &str)
{
    QStyle *style = QStyleFactory::create(str);
    if (style)
        qApp->setStyle(style);
    else
        toStatusMessage(qApp->translate("toSetSessionType", "Failed to find style %1").arg(str));
}
#endif

QToolBar *toAllocBar(QWidget *parent, const QString &str)
{
    QString db;
    try {
        db = toCurrentConnection(parent).description(false);
    }
    catch (...)
    {}

    QString name = str;
    if (!db.isEmpty() && !toConfigurationSingle::Instance().globalConfig(
            CONF_DB_TITLE, "Yes").isEmpty()) {
        name += QString::fromLatin1(" ");
        name += db;
    }

    QToolBar *tool;

    QMainWindow *main = dynamic_cast<QMainWindow *>(parent);
    if(main)
        tool = toMainWidget()->addToolBar(name);
    else
        tool = new QToolBar(parent);

    tool->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                    QSizePolicy::Fixed));
    return tool;
}

TODock *toAllocDock(const QString &name,
                    const QString &db,
                    const QPixmap &pix) {
    QString str = name;
    if (!db.isEmpty() &&
        !toConfigurationSingle::Instance().globalConfig(
            CONF_DB_TITLE, "Yes").isEmpty()) {
        str += QString::fromLatin1(" ");
        str += db;
    }
    QDockWidget *dock = new QDockWidget(str, toMainWidget());
    dock->setFeatures(QDockWidget::DockWidgetClosable |
                      QDockWidget::DockWidgetMovable |
                      QDockWidget::DockWidgetFloatable);
    if(!pix.isNull())
        dock->setWindowIcon(QIcon(pix));
    return dock;
}

void toAttachDock(TODock *dock, QWidget *container, Qt::DockWidgetArea area)
{
    QDockWidget *d = dynamic_cast<QDockWidget *>(dock);
    if (d) {
        toMainWidget()->addDockWidget(area, d);
        d->setWidget(container);
        container->show();
    }
}

QString toFontToString(const QFont &fnt)
{
    return fnt.toString();
}

QFont toStringToFont(const QString &str)
{
    if (str.isEmpty())
        return QFont(QString::fromLatin1("Courier New"), 12);
    QFont fnt;
    if (fnt.fromString(str))
        return fnt;

    return QFont(QString::fromLatin1("Courier New"), 12);
}

int toSizeDecode(const QString &str)
{
    if (str == QString::fromLatin1("KB"))
        return 1024;
    if (str == QString::fromLatin1("MB"))
        return 1024*1024;
    return 1;
}

QString toPluginPath(void)
{
    QString str;

#ifdef Q_OS_WIN32

    CRegistry registry;
	DWORD siz = 1024;
    char buffer[1024];

    try
    {
        if (registry.GetStringValue(HKEY_LOCAL_MACHINE,
						"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\TOra",
						"UninstallString",
						buffer, siz))
		{
			if (siz> 0)
			{
				str = buffer;
				static QRegExp findQuotes("\"([^\"]*)\"");
				if (findQuotes.indexIn(str) >= 0)
				str = findQuotes.cap(1);
				int ind = str.lastIndexOf('\\');
				if (ind >= 0)
				str = str.mid(0, ind);
				str += "\\templates";
			}
		}
    }
    catch (...)
        {}

#elif defined( Q_OS_MACX )
    {
        // MacOS
        char resourcePath[MAXPATHLEN];
        memset( &resourcePath[0], 0, MAXPATHLEN );
        CFBundleRef appBundle = ::CFBundleGetMainBundle();
        if ( appBundle )
        {
            CFURLRef urlRef = CFBundleCopyResourcesDirectoryURL( appBundle );
            if ( urlRef )
            {
                UInt8* _p = (UInt8*) & resourcePath[0];
                bool isOK = CFURLGetFileSystemRepresentation(
                                urlRef, TRUE, _p, MAXPATHLEN );
                if ( !isOK )
                {
                    // QMessageBox::warning( 0, "File error",
                    //     QString( "Unexpected: no file system representation") );
                }
            }
            else
            {
                // QMessageBox::warning( 0, "File error",
                //     QString( "Unexpected: unable to get resource directory") );
            }
            CFRelease( urlRef );
            str = &resourcePath[0];
        }
        else
        {
            // QMessageBox::warning( 0, "File error",
            //     QString( "Unexpected: unable to get main bundle") );
        }
    } // MacOS

#else
    str = toConfigurationSingle::Instance().globalConfig(CONF_PLUGIN_DIR, DEFAULT_PLUGIN_DIR);
#endif

    return str;
}

QString toHelpPath(void)
{
    QString str = toConfigurationSingle::Instance().globalConfig(CONF_HELP_PATH, "");
    if (str.isEmpty())
    {
        str = toPluginPath();
        str += QString::fromLatin1("/help/toc.html");
    }
    return str;
}

QByteArray toReadFile(const QString &filename)
{
    QString expanded = toExpandFile(filename);
    QFile file(expanded);
    if(!file.open(QIODevice::ReadOnly))
        throw QT_TRANSLATE_NOOP("toReadFile", "Couldn't open file %1.").arg(filename);

    QTextStream in(&file);
    return in.readAll().toUtf8();
}

QString toExpandFile(const QString &file)
{
    QString ret(file);
    QString home;

#ifdef Q_OS_WIN32

	CRegistry registry;
    DWORD siz = 1024;
    char buffer[1024];
    try
    {
		if (registry.GetStringValue(HKEY_LOCAL_MACHINE,
						"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
						"Personal",
						buffer, siz))
		{
			if (siz> 0)
			home = buffer;
		}
    }
    catch (...)
        {}

#else
    const char *homet = getenv("HOME");
    if (!homet)
        home = "";
    else
        home = homet;
#endif

    ret.replace(QRegExp(QString::fromLatin1("\\$HOME")), home);
    return ret;
}

bool toWriteFile(const QString &filename, const QByteArray &data)
{
    QString expanded = toExpandFile(filename);
    QFile file(expanded);
    if (!file.open(QIODevice::WriteOnly))
    {
        TOMessageBox::warning(
            toMainWidget(),
            QT_TRANSLATE_NOOP("toWriteFile", "File error"),
            QT_TRANSLATE_NOOP(
                "toWriteFile",
                QString("Couldn't open %1 for writing").arg(filename).toAscii().constData()));
        return false;
    }
    file.write(data);
    if (file.error() != QFile::NoError)
    {
        TOMessageBox::warning(
            toMainWidget(),
            QT_TRANSLATE_NOOP("toWriteFile", "File error"),
            QT_TRANSLATE_NOOP("toWriteFile", "Couldn't write data to file"));
        return false;
    }
    toStatusMessage(QT_TRANSLATE_NOOP("toWriteFile", "File saved successfully"), false, false);
    return true;
}

bool toWriteFile(const QString &filename, const QString &data)
{
    return toWriteFile(filename, data.toUtf8());
}

bool toCompareLists(QStringList &lsta, QStringList &lstb, int len)
{
    if (lsta.count() < len || lstb.count() < len)
        return false;
    for (int i = 0;i < len;i++)
        if (lsta[i] != lstb[i])
            return false;
    return true;
}

static QString GetExtensions(void)
{
    static QRegExp repl(QString::fromLatin1("\\s*,\\s*"));
    QString t = toConfigurationSingle::Instance().globalConfig(CONF_EXTENSIONS, DEFAULT_EXTENSIONS);
    t.replace(repl, QString::fromLatin1("\n"));
    return t;
}

static QString AddExt(QString t, const QString &filter)
{
    static QRegExp hasext(QString::fromLatin1("\\.[^\\/]*$"));
    if (t.isEmpty())
        return t;

    toConfigurationSingle::Instance().globalSetConfig(CONF_LAST_DIR, t);

    if (hasext.indexIn(t) < 0)
    {
        static QRegExp findext(QString::fromLatin1("\\.[^ \t\r\n\\)\\|]*"));
        int len = 0;
        int pos = findext.indexIn(filter, 0);
        len = findext.matchedLength();
        if (pos >= 0)
            t += filter.mid(pos, len);
        else
        {
            QFile file(t);
            if (!file.exists())
                t += QString::fromLatin1(".sql");
        }
    }
    return t;
}

QString toOpenFilename(const QString &filename, const QString &filter, QWidget *parent)
{
    QString t = filter;
    if (t.isEmpty())
        t = GetExtensions();

    QString dir = filename;
    if (dir.isNull())
        dir = toConfigurationSingle::Instance().globalConfig(CONF_LAST_DIR, "");

	return AddExt(TOFileDialog::getOpenFileName(parent, QObject::tr("Open File", "utils/toOpenFilename"), dir, t), t);
}

QString toSaveFilename(const QString &filename, const QString &filter, QWidget *parent)
{
    QString t = filter;
    if (t.isEmpty())
        t = GetExtensions();

    QString dir = filename;
    if (dir.isNull())
        dir = toConfigurationSingle::Instance().globalConfig(CONF_LAST_DIR, "");

	return AddExt(TOFileDialog::getSaveFileName(parent, QObject::tr("Save File", "utils/toSaveFilename"), dir, t), t);
}

void toSetEnv(const QString &var, const QString &val)
{
#if HAVE_SETENV && !defined(Q_OS_WIN32)
    setenv(var.toAscii().constData(), val.toAscii().constData(), 1);
#else
    // Has a memory leak, but just a minor one.

    char *env = new char[var.length() + val.length() + 2];
    strcpy(env, var.toUtf8());
    strcat(env, "=");
    strcat(env, val.toUtf8());
    putenv(env);
#endif
}

void toUnSetEnv(const QString &var)
{
#if HAVE_SETENV && !defined(Q_OS_WIN32)
    unsetenv(var.toAscii().constData());
#else
    toSetEnv(var.toAscii().constData(), "");
#endif
}

#define COLORS 2

#define TYPES 7

#define STYLES 5

QBrush toChartBrush(int index)
{
    index %= (COLORS * TYPES * STYLES);
    index = COLORS * TYPES * STYLES - 1 - index;
    int type = index % TYPES;
    int color = (index / TYPES) % COLORS;
    int style = (index / TYPES / COLORS);
    int r = 0, g = 0, b = 0;
    int offset = 1;
    switch (type)
    {
    case 6:
        r = 1;
        break;
    case 5:
        g = 1;
        break;
    case 4:
        b = 1;
        break;
    case 3:
        r = g = 1;
        break;
    case 2:
        r = b = 1;
        break;
    case 1:
        b = g = 1;
        break;
    case 0:
        r = b = g = 1;
        offset = 0;
        break;
    }

    Qt::BrushStyle rets;

    switch (style)
    {
    default:
        rets = Qt::SolidPattern;
        break;
    case 3:
        rets = Qt::BDiagPattern;
        break;
    case 2:
        rets = Qt::FDiagPattern;
        break;
    case 1:
        rets = Qt::DiagCrossPattern;
        break;
    case 0:
        rets = Qt::CrossPattern;
        break;
    }

    color += offset;

    QBrush brush(QColor(TO_MIN(255, r*color*256 / (COLORS)),
                        TO_MIN(255, g*color*256 / (COLORS)),
                        TO_MIN(255, b*color*256 / (COLORS))),
                 rets);
    return brush;
}

toToolWidget *toCurrentTool(QObject *cur)
{
    while (cur)
    {
        toToolWidget *tool = dynamic_cast<toToolWidget *>(cur);
        if (tool)
            return tool;
        cur = cur->parent();
    }
    throw qApp->translate("toCurrentTool", "Couldn't find parent tool. Internal error.");
}

toConnection &toCurrentConnection(QObject *cur)
{
    while (cur)
    {
        toConnectionWidget *conn = dynamic_cast<toConnectionWidget *>(cur);
        if (conn)
            return conn->connection();
        cur = cur->parent();
    }
    throw qApp->translate("toCurrentConnection", "Couldn't find parent connection. Internal error.");
}

unsigned int toBusy::Count = 0;

static toLock BusyLock;

void toBusy::clear()
{
    BusyLock.lock();
    Count = 0;
    BusyLock.unlock();
    qApp->restoreOverrideCursor();
}

toBusy::toBusy()
{
    BusyLock.lock();
    if (toThread::mainThread())
    {
        if (!Count)
            qApp->setOverrideCursor(Qt::WaitCursor);
        Count++;
    }
    BusyLock.unlock();
}

toBusy::~toBusy()
{
    BusyLock.lock();
    if (toThread::mainThread())
    {
        if (Count > 0)
            Count--;
        if (!Count)
            qApp->restoreOverrideCursor();
    }
    BusyLock.unlock();
}

void toReadableColumn(QString &name)
{
    bool inWord = false;
    for (int i = 0;i < name.length();i++)
    {
        if (name.at(i) == '_')
        {
            name[i] = ' ';
            inWord = false;
        }
        else if (name.at(i).isSpace())
        {
            inWord = false;
        }
        else if (name.at(i).isLetter())
        {
            if (inWord)
                name[i] = name.at(i).toLower();
            else
                name[i] = name.at(i).toUpper();
            inWord = true;
        }
    }
}

bool toIsOracle(const toConnection &conn)
{
    return conn.provider() == "Oracle";
}

bool toIsSapDB(const toConnection &conn)
{
    return conn.provider() == "SapDB";
}

bool toIsMySQL(const toConnection &conn)
{
    return conn.provider() == "MySQL";
}

bool toIsPostgreSQL(const toConnection &conn)
{
    return conn.provider() == "PostgreSQL";
}

static toTreeWidgetItem *FindItem(toTreeWidget *lst, toTreeWidgetItem *first, const QString &str)
{
    while (first)
    {
        QString tmp = first->text(0);
        if (tmp == str)
            return first;
        else
        {
            tmp += QString::fromLatin1(":");
            if (str.startsWith(tmp))
            {
                toTreeWidgetItem *ret = FindItem(lst, first->firstChild(), str.mid(tmp.length()));
                if (ret)
                    return ret;
            }
        }
        first = first->nextSibling();
    }
    return NULL;
}

toTreeWidgetItem *toFindItem(toTreeWidget *lst, const QString &str)
{
    return FindItem(lst, lst->firstChild(), str);
}

void toToolCaption(toToolWidget *widget, const QString &caption)
{
    QString title;
    if (!toConfigurationSingle::Instance().globalConfig(CONF_DB_TITLE, "Yes").isEmpty())
    {
        try
        {
            title = widget->connection().description();
            title += QString::fromLatin1(" ");
        }
        catch (...)
            {}
    }
    title += caption;

    widget->setWindowTitle(title);
    toMainWidget()->updateWindowsMenu();
}

void toMapExport(std::map<QString, QString> &data, const QString &prefix,
                 std::map<QString, QString> &src)
{
    std::map<QString, QString>::iterator i = src.begin();
    if (i != src.end())
    {
        data[prefix + ":First"] = QString((*i).first);
        QString key = prefix + ":d:";
        do
        {
            data[key + (*i).first] = (*i).second;
            i++;
        }
        while (i != src.end());
    }
}

void toMapImport(std::map<QString, QString> &data, const QString &prefix,
                 std::map<QString, QString> &dst)
{
    dst.clear();
    std::map<QString, QString>::iterator i = data.find(prefix + ":First");
    if (i != data.end())
    {
        QString key = prefix + ":d:";
        i = data.find(key + (*i).second.toLatin1());
        while (i != data.end() && (*i).first.mid(0, key.length()) == key)
        {
            QString t = (*i).first.mid(key.length());
            if (t.isNull())
                t = "";
            dst[t] = (*i).second;
            i++;
        }
    }
}

bool toCheckModal(QWidget *widget)
{
    QWidget *parent = QApplication::activeModalWidget();
    if (!parent)
        return true;
    while (widget && widget != parent)
        widget = widget->parentWidget();
    if (widget == parent)
        return true;
    return false;
}

static bool IndicateEmpty = false;

void toUpdateIndicateEmpty(void)
{
    IndicateEmpty = !toConfigurationSingle::Instance().globalConfig(CONF_INDICATE_EMPTY, "").isEmpty();
}


toQValue toNull(const toQValue &str)
{
    if (IndicateEmpty)
    {
        if (str.isNull())
            return str;
        if (str.toString().length() == 0)
            return QString::fromLatin1("''");
    }
    else if (str.isNull())
        return QString::fromLatin1("{null}");
    return str;
}

toQValue toUnnull(const toQValue &str)
{
    if (IndicateEmpty)
    {
        if (QString(str) == QString::fromLatin1("''"))
            return QString::fromLatin1("");
    }
    else if (QString(str) == QString::fromLatin1("{null}"))
        return QString();
    return str;
}

QString toTranslateMayby(const QString &ctx, const QString &text)
{
    if (ctx.contains(QString::fromLatin1(" ")) || ctx.toLatin1() != ctx.toUtf8() || text.toLatin1() != text.toUtf8() || ctx.isEmpty() || text.isEmpty())
        return text;
    return QT_TRANSLATE_NOOP(ctx.toLatin1(), text.toLatin1());
}

toPopupButton::toPopupButton(const QIcon &iconSet,
                             const QString &textLabel,
                             const QString &grouptext,
                             QToolBar *parent,
                             const char *name)
    : QToolButton(parent) {

    setObjectName(name);
    setIcon(iconSet);
    setText(textLabel);
    setToolTip(grouptext);
}

toPopupButton::toPopupButton(QWidget *parent, const char *name)
    : QToolButton(parent) {

    setObjectName(name);
}

QString toObfuscate(const QString &str)
{
    if (str.isEmpty())
        return str;

    QByteArray arr = qCompress(str.toUtf8());
    QString ret = "\002";

    char buf[100]; // Just to be on the safe side
    for (int i = 0;i < arr.size();i++)
    {
        sprintf(buf, "%02x", ((unsigned int)arr.at(i)) % 0xff);
        ret += buf;
    }
    return ret;
}

QString toUnobfuscate(const QString &str)
{
    if (str.isEmpty())
        return str;

    if (str.at(0) != '\001' && str.at(0) != '\002')
        return str;

    QByteArray arr;
    for (int i = 1;i < str.length();i += 2)
// qt4        arr.at(i / 2) = str.mid(i, 2).toInt(0, 16);
        arr[i / 2] = str.mid(i, 2).toInt(0, 16);
    if (str.at(0) == '\002')
    {
        QByteArray ret = qUncompress(arr);
        return QString::fromUtf8(ret);
    }
    else
        return QString::fromUtf8(arr);
}

QKeySequence toKeySequence(const QString &key)
{
    QKeySequence ret = key;
    if (key.isEmpty() && ret.isEmpty())
        printf("Key sequence %s is not valid\n", key.toAscii().constData());
    return ret;
}

bool toCheckKeyEvent(QKeyEvent *event, const QKeySequence &key)
{
    int state = 0;
    if (key.count() != 1)
        return false;
    int val = key[0];
    if ((val&Qt::META) == Qt::META)
        state |= Qt::META;
    if ((val&Qt::SHIFT) == Qt::SHIFT)
        state |= Qt::SHIFT;
    if ((val&Qt::CTRL) == Qt::CTRL)
        state |= Qt::CTRL;
    if ((val&Qt::ALT) == Qt::ALT)
        state |= Qt::ALT;

    val &= 0xfffff;

    return (event->modifiers() == Qt::NoModifier && event->key() == val);
}

int countChars(const QString &source, const char find)
{
    int found = 0;

    for (int i = 0; i < source.length(); i++)
    {
        if (source[i] == find)
            found++;
    }

    return found;
}

toSpacer::toSpacer(QWidget *parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                              QSizePolicy::Minimum));
}
