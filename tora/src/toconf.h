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
*      Specifically you are not permitted to link this program with the
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

#ifndef TOCONF_H
#define TOCONF_H

#define CONF_PROVIDER  "Provider"
#define DEFAULT_PROVIDER "Oracle"
#define CONF_SAVE_PWD  "SavePassword"
#define DEFAULT_SAVE_PWD false
#define CONF_PASSWORD  "DefaultPassword"
#define DEFAULT_PASSWORD ""
#define CONF_DATABASE  "DefaultDatabase"
#define DEFAULT_DATABASE ""
#define CONF_USER  "DefaultUser"
#define DEFAULT_USER  "system"
#define CONF_HOST  "Host"
#define DEFAULT_HOST  "localhost"
#define CONF_OPTIONS  "Options"
#define DEFAULT_OPTIONS  ""
#define CONF_MAX_NUMBER  "MaxNumber"
#define DEFAULT_MAX_NUMBER 50
#define CONF_MAX_COL_SIZE "MaxColSize"
#define DEFAULT_MAX_COL_SIZE 30000
#define CONF_MAX_COL_DISP "MaxColDisp"
#define DEFAULT_MAX_COL_DISP 300
#define CONF_PLAN_TABLE  "PlanTable"
#define DEFAULT_PLAN_TABLE "PLAN_TABLE"
#define CONF_PLAN_CHECKPOINT "PlanCheckpoint"
#define DEFAULT_PLAN_CHECKPOINT "TORA_CHECKPOINT"
#define CONF_TEXT  "ConfText"
#define CONF_CODE  "ConfCode"
#define CONF_LIST  "ListText"
#define CONF_DATE_FORMAT "ConfDateFormat"
#define DEFAULT_DATE_FORMAT "YYYY-MM-DD HH24:MI:SS"
#define CONF_REFRESH  "Refresh"
#define DEFAULT_REFRESH  "None"
#define CONF_HIGHLIGHT  "SyntaxHighlight"
#define CONF_KEYWORD_UPPER "KeywordUpper"
#define DEFAULT_KEYWORD_UPPER false
#define CONF_COLOR  "KeywordColor"
#define CONF_PLUGIN_DIR  "PluginDir"
#define CONF_CACHE_DIR  "CacheDir"
#define CONF_CACHE_DISK  "CacheDiskUse"
#define DEFAULT_CACHE_DISK true
#define CONF_SQL_FILE  "SQLDictionary"
#define DEFAULT_SQL_FILE "$HOME/.torasql"
#define CONF_DESKTOP_AWARE "DesktopAware"
#define CONF_STATUS_MESSAGE     "StatusMessage"
#define DEFAULT_STATUS_MESSAGE  "5"
#define CONF_QT_XFT             "Xft"
#define DEFAULT_QT_XFT          "false"
#define CONF_DOCK_TOOLBAR "DockToolbar"
#define CONF_DB_TITLE  "DBTitle"
#define CONF_SIZE_UNIT  "SizeUnit"
#define DEFAULT_SIZE_UNIT "MB"
#define CONF_HELP_PATH  "HelpPath"
#define CONF_STATUS_SAVE "StatusSave"
#define DEFAULT_STATUS_SAVE 10
#define CONF_AUTO_COMMIT "AutoCommit"
#define CONF_CHANGE_CONNECTION "ChangeConnection"
#define CONF_CONNECT_HISTORY "ConnectHistory"
#define CONF_CONNECT_SIZE "ConnectSize"
#define CONF_CONNECT_CURRENT "ConnectCurrent"
#define DEFAULT_CONNECT_SIZE 20
#define CONF_DEFAULT_TOOL "DefaultTool"
#define CONF_TOOL_ENABLE "Enabled"
#define CONF_CHART_SAMPLES "ChartSamples"
#define DEFAULT_CHART_SAMPLES 100
#define CONF_DISPLAY_SAMPLES "DisplaySamples"
#define DEFAULT_DISPLAY_SAMPLES -1
#define CONF_CODE_COMPLETION "CodeComplete"
#define CONF_COMPLETION_SORT "CompleteSort"
#define CONF_AUTO_INDENT "AutoIndent"
#define CONF_DONT_REREAD "DontReread"
#define CONF_AUTO_LONG  "AutoLong"
#define CONF_MESSAGE_STATUSBAR "MessageStatusbar"
#define CONF_OBJECT_CACHE "ObjectCache"
#define DEFAULT_OBJECT_CACHE 1
#define CONF_BKGND_CONNECT "BackgroundConnect"
#define CONF_MAX_CONTENT "MaxContent"
#define DEFAULT_MAX_CONTENT 100
#define CONF_KEEP_PLANS  "KeepPlans"
#define CONF_RESTORE_SESSION "RestoreSession"
#define CONF_DEFAULT_SESSION "DefaultSession"
#define DEFAULT_SESSION  "$HOME/.tora.tse"
#define CONF_TOOLS_LEFT  "ToolsLeft"
#define CONF_DEFAULT_FORMAT "DefaultFormat"
#define CONF_CSV_SEPARATOR "CSVSeparator"
#define DEFAULT_CSV_SEPARATOR ";"
#define CONF_CSV_DELIMITER "CSVDelimiter"
#define DEFAULT_CSV_DELIMITER "\""
#define CONF_AUTO_INDENT_RO "AutoIndent"
#define CONF_INDICATE_EMPTY "IndicateEmpty"
#define CONF_INDICATE_EMPTY_COLOR "IndicateEmptyColor"
#define CONF_EXTENSIONS   "Extensions"
#define CONF_RECENT_FILES "RecentFiles"
#define CONF_RECENT_MAX  "RecentMax"
#define DEFAULT_RECENT_MAX 5
#define CONF_LAST_DIR  "LastDir"
#define CONF_LOCALE  "Locale"
#define CONF_KEEP_ALIVE  "KeepAlive"
#define DEFAULT_KEEP_ALIVE 300
#define CONF_NUMBER_FORMAT "NumberFormat"
#define DEFAULT_NUMBER_FORMAT 0
#define CONF_NUMBER_DECIMALS "NumberDecimals"
#define DEFAULT_NUMBER_DECIMALS 2
#define CONF_CACHE_TIMEOUT "CacheTimeout"
#define DEFAULT_CACHE_TIMEOUT 7
#define CONF_TAB_STOP  "TabStop"
#define DEFAULT_TAB_STOP 8
#define CONF_TAB_SPACES "TabSpaces"
#define DEFAULT_TAB_SPACES false
#define CONF_TOAD_BINDINGS "ToadBindings"
#define DEFAULT_TOAD_BINDINGS false
#define CONF_DISPLAY_GRIDLINES "Gridlines"
#define DEFAULT_DISPLAY_GRIDLINES true
#define DEFAULT_EXTENSIONS "SQL (*.sql *.pkg *.pkb), Text (*.txt), All (*)"
#define CONF_SMTP "SmtpServer"
#define DEFAULT_SMTP "localhost"
#define CONF_SMTP_PORT "SmtpServerPort"
#define DEFAULT_SMTP_PORT 25
#define CONF_STYLE  "Style"
// tooracleconnection.cpp
#define CONF_OPEN_CURSORS "OpenCursors"
#define DEFAULT_OPEN_CURSORS 40  // Defined to be able to update tuning view
#define CONF_MAX_LONG  "MaxLong"
#ifndef DEFAULT_MAX_LONG
#define DEFAULT_MAX_LONG 30000
#endif
// tosqlconnection
#define CONF_ONLY_FORWARD "OnlyForward"
#define DEFAULT_ONLY_FORWARD true
// main.cpp
#define CONF_LAST_VERSION "LastVersion"
#define CONF_FIRST_INSTALL "FirstInstall"
// tools
#define CONF_TOOLS "Tools"
// toeditextensions
#define CONF_COMMA_BEFORE "CommaBefore"
#define CONF_BLOCK_OPEN_LINE "BlockOpenLine"
#define CONF_OPERATOR_SPACE "OperatorSpace"
#define CONF_KEYWORD_UPPER "KeywordUpper"
#define CONF_RIGHT_SEPARATOR "RightSeparator"
#define CONF_END_BLOCK_NEWLINE "EndBlockNewline"
#define CONF_COMMENT_COLUMN "CommentColumn"
#define DEFAULT_COMMENT_COLUMN 60
// tobrowser
#define CONF_FILTER_IGNORE_CASE "FilterIgnoreCase"
#define CONF_FILTER_INVERT   "FilterInvert"
#define CONF_FILTER_TYPE   "FilterType"
#define CONF_FILTER_TABLESPACE_TYPE   "FilterTablespaceType"
#define CONF_FILTER_TEXT   "FilterText"
// tooutput
#define CONF_POLLING     "Refresh"
#define DEFAULT_POLLING  "10 seconds"
#define CONF_LOG_TYPE  "Type"
#define DEFAULT_LOG_TYPE 0
#define CONF_LOG_USER  "LogUser"
#define DEFAULT_LOG_USER "ULOG"
// torollback
#define CONF_NO_EXEC    "NoExec"
#define CONF_NEED_READ  "NeedRead"
#define CONF_NEED_TWO   "NeedTwo"
#define CONF_ALIGN_LEFT "AlignLeft"
#define CONF_OLD_ENABLE "OldEnable"
// tosgatrace
#define CONF_AUTO_UPDATE    "AutoUpdate"
// tostorage
#define CONF_DISP_TABLESPACES "DispTablespaces"
#define CONF_DISP_COALESCED "DispCoalesced"
#define CONF_DISP_EXTENTS "DispExtents"
#define CONF_DISP_AVAILABLEGRAPH "AvailableGraph"
// totuning
#define CONF_OVERVIEW "Overview"
#define CONF_FILEIO   "File I/O"
#define CONF_WAITS    "Wait events"
#define CONF_CHART    "chart"
// toworksheet
#define CONF_AUTO_SAVE    "AutoSave"
#define CONF_CHECK_SAVE   "CheckSave"
#define CONF_AUTO_LOAD    "AutoLoad"
#define CONF_LOG_AT_END   "LogAtEnd"
#define CONF_LOG_MULTI    "LogMulti"
#define CONF_STATISTICS   "Statistics"
#define CONF_TIMED_STATS  "TimedStats"
#define CONF_NUMBER   "Number"
#define CONF_MOVE_TO_ERR  "MoveToError"
#define CONF_HISTORY   "History"
#define CONF_EXEC_LOG     "ExecLog"
#define CONF_TOPLEVEL_DESCRIBE "ToplevelDescribe"


#  ifndef Q_OS_WIN32
// Undefining this will leave out the style selection option stuff from the program
#    define ENABLE_STYLE
// Undefining this will enable choosing Qt_Xft support
#    define ENABLE_QT_XFT
#  endif


// Version of TOra, must also change rpm files
#ifndef TOAPPNAME
#  define TOAPPNAME  "TOra"
#endif

#define TO_TOOLBAR_WIDGET_NAME "kde toolbar widget"

#endif
