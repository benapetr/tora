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

#ifndef UTILS_H
#define UTILS_H

#include <list>
#include <map>

#include <qmainwindow.h>
#include <qtoolbutton.h>

#ifdef TO_KDE
#  include <kapp.h>
#  if KDE_VERSION < 220
#    define TOPrinter QPrinter
#  else
#    define TOPrinter KPrinter
#    define TO_HAS_KPRINT
#  endif
#  define TOFileDialog KFileDialog
#else
#  define TOPrinter QPrinter
#  define TOFileDialog QFileDialog
#endif
#define TOMessageBox QMessageBox
#define TODock QWidget

#if 0
/**
 * The class to use for a printer object.
 */
class TOPrinter : public QPrinter {

};
/**
 * The class to use for a dock widget.
 */
class TODock : public QWidget {

};
/**
 * The class to use for a file dialog.
 */
class TOFileDialog : public QFileDialog {

};
/**
 * The class to use for a messagebox.
 */
class TOMessageBox : public QMessageBox {

};
#endif

class QComboBox;
class QListView;
class QListViewItem;
class QToolBar;
class QWidget;
class toMain;
class toConnection;
class toTimer;
class toToolWidget;

/** Display a message in the statusbar of the main window.
 * @param str Message to display
 * @param save If true don't remove the message after a specified interval.
 * @param log Log message. Will never log saved messages.
 */
void toStatusMessage(const QString &str,bool save=false,bool log=true);
/** Get an address to a SQL statement in the SGA. The address has the form
 * 'address:hash_value' which are resolved from the v$sqltext_with_newlines
 * view in Oracle.
 * @param conn Connection to get address from
 * @param sql Statement to get address for.
 * @return String with address in.
 * @exception QString if address not found.
 */
QString toSQLToAddress(toConnection &conn,const QString &sql);
/** Get the full SQL of an address (See @ref toSQLToAddress) from the
 * SGA.
 * @param conn Connection to get address from
 * @param sql Address of SQL.
 * @return String with SQL of statement.
 * @exception QString if address not found.
 */
QString toSQLString(toConnection &conn,const QString &address);
/** Make a column name more readable.
 * @param col Name of column name, will be modified.
 */
void toReadableColumn(QString &col);
/** Get the current database time in the current sessions dateformat.
 * @param conn Connection to get address from.
 * @return String with the current date and time.
 */
QString toNow(toConnection &conn);
/** Set the current session type (Style)
 * @param str Session to set, can be any of Motif, Motif Plus, SGI, CDE, Windows and Platinum
 * @exception QString if style not available.
 */
void toSetSessionType(const QString &str);
/** Get current session type (Style)
 * @return A string describing the current style.
 * @see toSetSessionType
 */
QString toGetSessionType(void);
/** Get the available styles.
 */
QStringList toGetSessionTypes(void);
/** Create or fill a combobox with refresh intervals.
 * @param parent Parent of created combobox.
 * @param name Name of created combobox.
 * @param def Default value of the combobox.
 * @param item Combo box to fill. If not specified a new combobox is created.
 */
QComboBox *toRefreshCreate(QWidget *parent,const char *name=NULL,const QString &def=QString::null,
			   QComboBox *item=NULL);
/** Set a timer with the value from a refresh combobox (See @ref toRefreshCreate).
 * @param timer Timer to set timeout in.
 * @param str String from currentText of combobox. If empty, set to default.
 */
void toRefreshParse(toTimer *timer,const QString &str=QString::null);
/** Get information about wether this TOra has plugin support or not.
 * @return True if plugin support is enabled.
 */
bool toMonolithic(void);
/** Make a deep copy of a string. Usefull when sharing copying strings between threads.
 * @param str String to copy
 * @return Copied string.
 */
QString toDeepCopy(const QString &str);
/** Allocate a toolbar. This is needed since Qt and KDE use different toolbars.
 * @param parent Parent of toolbar.
 * @param name Name of toolbar.
 * @return A newly created toolbar.
 */
QToolBar *toAllocBar(QWidget *parent,const QString &name);
/** Allocate a new docked window. This is needed since Qt and KDE docks windows differently
 * (Qt 2.x doesn't even have support for docked windows). Observe that you must attach a
 * dock using @ref toAttachDock after allocating it.
 * @param name Name of window.
 * @param db Database name or empty if N/A.
 * @param icon Icon of new dock.
 * @return A newly allocated fock.
 */
TODock *toAllocDock(const QString &name,
		    const QString &db,
		    const QPixmap &icon);
/** Attach a dock to a specified position.
 * @param dock Dock to attach
 * @param container Whatever container is supposed to be in the dock window.
 * @param place Where to place the dock.
 */
void toAttachDock(TODock *dock,QWidget *container,QMainWindow::ToolBarDock place);
/** Decode a size string this basically converts "KB" to 1024, "MB" to 1024KB and
 * everything else to 1.
 * @return Multiplier specified by string.
 */
int toSizeDecode(const QString &str);

/** Shift the first value out of a list.
 * @param lst List to shift value from (Also modified).
 * @return The first value in the list.
 */
template <class T> T toShift(std::list<T> &lst)
{
  if (lst.begin()==lst.end()) {
    T ret;
    return ret;
  }
  T ret=(*lst.begin());
  lst.erase(lst.begin());
  return ret;
}

/** Push an object to the beginning of a list.
 * @param lst List to push value in from of.
 * @param str Object to push.
 */
template <class T> void toUnShift(std::list<T> &lst,const T &str)
{
  lst.insert(lst.begin(),str);
}

/** Pop the last value out of a list.
 * @param lst List to pop value from (Also modified).
 * @return The value in the list of objects.
 */
template <class T> T toPop(std::list<T> &lst)
{
  if (lst.begin()==lst.end()) {
    T ret;
    return ret;
  }
  T ret=(*lst.rbegin());
  lst.pop_back();
  return ret;
}

/** Push an object to the end of a list.
 * @param lst List to push value in from of.
 * @param str Object to push.
 */
template <class T> void toPush(std::list<T> &lst,const T &str)
{
  lst.push_back(str);
}

/** Find if an item exists in a list.
 * @param lst List to search for value.
 * @param str Object to search for.
 */
template <class T> std::list<T>::iterator toFind(std::list<T> &lst,const T &str)
{
  for(std::list<T>::iterator i=lst.begin();i!=lst.end();i++)
    if (*i==str)
      return i;
  return lst.end();
}

/** Convert a string representation to a font structure.
 * @param str String representing the font.
 * @return Font structure represented by the string.
 * @see toFontToString
 */
QFont toStringToFont(const QString &str);
/** Convert a font to a string representation.
 * @param fnt Font to convert.
 * @return String representation of font.
 */
QString toFontToString(const QFont &fnt);
/** Get the path to the help directory.
 * @return Path to the help directory.
 */
QString toHelpPath(void);
/** Read file from filename.
 * @param filename Filename to read file from.
 * @return Contents of file.
 * @exception QString describing I/O problem.
 */
QCString toReadFile(const QString &filename);
/** Write file to filename.
 * @param filename Filename to write file to.
 * @param data Data to write to file.
 */
bool toWriteFile(const QString &filename,const QCString &data);
/** Write file to filename.
 * @param filename Filename to write file to.
 * @param data Data to write to file.
 */
bool toWriteFile(const QString &filename,const QString &data);
/** Compare two string lists.
 * @param l1 First list to compare.
 * @param l2 Second list to compare.
 * @param len Length of lists to compare.
 * @return True if all first len elements match.
 */
bool toCompareLists(QStringList &l1,QStringList &l2,unsigned int len);
/** Set environment variable.
 * @param var Variable name to set.
 * @param val Value to set variable to.
 */
void toSetEnv(const QCString &var,const QCString &val);
/** Delete an environment variable.
 * @param var Environment variable to delete.
 */
void toUnSetEnv(const QCString &var);
/** Pop up a dialog and choose a file to open.
 * @param filename Default filename to open.
 * @param filter Filter of filenames (See @ref QFileDialog)
 * @param parent Parent of dialog.
 * @return Selected filename.
 */
QString toOpenFilename(const QString &filename,const QString &filter,QWidget *parent);
/** Pop up a dialog and choose a file to save to.
 * @param filename Default filename to open.
 * @param filter Filter of filenames (See @ref QFileDialog)
 * @param parent Parent of dialog.
 * @return Selected filename.
 */
QString toSaveFilename(const QString &filename,const QString &filter,QWidget *parent);
/**
 * Check if this is a registered TOra or freeware, otherwise quit.
 */
void toCheckCommercial(const QString &reason);

/** Get index of menu to insert tool specific menu at.
 * @return Index of the tool menu entry.
 */
int toToolMenuIndex(void);
/** Check if a character is valid for an identifier in Oracle.
 * @param c Character to check
 * @return True if it is a valid Oracle identifier.
 */
bool inline toIsIdent(QChar c)
{
  return c.isLetterOrNumber()||c=='_'||c=='%'||c=='$'||c=='#';
}
/** Get installation directory of application (Plugin directory on unix, installation
 * target on windows).
 * @return String containing directory
 */
QString toPluginPath(void);
/** Get a list of the latest status messages that have been shown.
 */
std::list<QString> toStatusMessages(void);
/** Get a brush for a chart item.
 * @param index Indicating which chart item to get color for.
 */
QBrush toChartBrush(int index);
/** Return the connection most closely associated with a widget. Currently connections are
 * only stored in toToolWidgets.
 * @return Reference toConnection object closest to the current.
 */
toConnection &toCurrentConnection(QObject *widget);
/** Return the tool widget most closely associated with a widget.
 * @return Pointer to tool widget.
 */
toToolWidget *toCurrentTool(QObject *widget);
/** Check if this connection is an oracle connection.
 */
bool toIsOracle(const toConnection &);
/** Strip extra bind specifier from an SQL statement. (That means the extra <***> part after
 * the bind variable.
 * @param sql The sql to strip.
 * @return Return a string containing the same statement without qualifiers, which means the
 *         sql sent to Oracle and available in the SGA.
 */
QString toSQLStripSpecifier(const QString &sql);
/** Strip extra binds and replace with empty strings.
 * @param sql The sql to strip.
 * @return Return a string containing the same statement without binds.
 */
QString toSQLStripBind(const QString &sql);
/** Expand filename with $HOME to be replaced with home directory or my documents.
 */
QString toExpandFile(const QString &file);
/** Convert string read by readValue to value read by readValueNull
 */
QString toUnnull(const QString &str);
/** Convert string read by readValueNull to value to be read by readValue.
 */
QString toNull(const QString &str);
/** Check if the current widget is within the active modal widget (Or no modal widget exists)
 */
bool toCheckModal(QWidget *widget);

/** Export the contents of a map into another map.
 * @param data The destination map.
 * @param prefix Prefix to save map using.
 * @param src The map to save.
 */
void toMapExport(std::map<QString,QString> &data,const QString &prefix,
		 std::map<QString,QString> &src);
/** Import the contents of a map from another map.
 * @param data The source map.
 * @param prefix Prefix to use for restoring from map.
 * @param dst The map to save into.
 */
void toMapImport(std::map<QString,QString> &data,const QString &prefix,
		 std::map<QString,QString> &dst);

/** Find an item in a listview.
 * @param list The list to search for the item.
 * @param str The string to search for. You can specify parent/child with : in the string.
 */
QListViewItem *toFindItem(QListView *list,const QString &str);
/** Whenever this class is instantiated the window will display a busy cursor. You
 * can instantiate this function as many time as you want, only when all of them are
 * destructed the curser will revert back to normal.
 */
class toBusy {
  static unsigned int Count;
public:
  static void clear(void);
  toBusy();
  ~toBusy();
};
/**
 * Set or change title of a tool window.
 * @param tool Widget of tool.
 * @param caption Caption to set to the tool.
 */
void toToolCaption(toToolWidget *tool,const QString &caption);

/**
 * Get maximum number of a and b.
 */
template <class T>
T max(T a, T b)
{
    return a > b ? a : b ;
}

/**
 * Get minimum number of a and b.
 */
template <class T>
T min(T a, T b)
{
    return a < b ? a : b ;
}

/** Popup toolbutton that works like I think they should under Qt 3.0 as well.
 * this means they will pop up a tool menu when you click on the button as well.
 */
class toPopupButton : public QToolButton {
  Q_OBJECT
public:
  /** Create button, same constructor as toolbutton except no slots
   */
  toPopupButton(const QIconSet &iconSet,const QString &textLabel,
                const QString &grouptext,QToolBar *parent,const char *name=0);
  /** Create button somewhere else than in a toolbar.
   */
  toPopupButton(QWidget *parent,const char *name=0);
private slots:
  void click(void);
};

/* This can't be documented in KDoc, anyway it is an easy way to catch any exception that
 * might be sent by TOra or OTL and display the message in the statusbar of the main window.
 */
#define TOCATCH \
    catch (const QString &str) {\
      toStatusMessage(str);\
    }

#endif
