
#pragma once

// QT headers first
#include <Qsci/qsciapis.h>
#include <Qsci/qscilexer.h>
#include <Qsci/qscilexercustom.h>
#include <Qsci/qscilexersql.h>
#include <Qsci/qsciprinter.h>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscistyle.h>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QAbstractListModel>
#include <QtCore/QAbstractTableModel>
#include <QtCore/QAtomicInt>
#include <QtCore/QByteArray>
#include <QtCore/QChar>
#include <QtCore/QCoreApplication>
#include <QtCore/QDataStream>
#include <QtCore/QDate>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QLibrary>
#include <QtCore/QList>
#include <QtCore/QLocale>
#include <QtCore/QMap>
#include <QtCore/QMetaType>
#include <QtCore/QMimeData>
#include <QtCore/QModelIndex>
#include <QtCore/QModelIndexList>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtCore/QPoint>
#include <QtCore/QPointer>
#include <QtCore/QReadWriteLock>
#include <QtCore/QRect>
#include <QtCore/QRegExp>
#include <QtCore/QSemaphore>
#include <QtCore/QSet>
#include <QtCore/QSettings>
#include <QtCore/QSharedPointer>
#include <QtCore/QSignalMapper>
#include <QtCore/QSize>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtCore/QTimerEvent>
#include <QtCore/QTranslator>
#include <QtCore/QVariant>
#include <QtCore/QVector>
#include <QtCore/QWaitCondition>
#include <QtCore/QtDebug>
#include <QtCore/qglobal.h>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QBoxLayout>
#include <QtGui/QCheckBox>
#include <QtGui/QClipboard>
#include <QtGui/QCloseEvent>
#include <QtGui/QColor>
#include <QtGui/QColorDialog>
#include <QtGui/QComboBox>
#include <QtGui/QCompleter>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QDateEdit>
#include <QtGui/QDesktopWidget>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDirModel>
#include <QtGui/QDockWidget>
#include <QtGui/QDropEvent>
#include <QtGui/QFileDialog>
#include <QtGui/QFocusEvent>
#include <QtGui/QFont>
#include <QtGui/QFontDialog>
#include <QtGui/QFontMetrics>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QHideEvent>
#include <QtGui/QIcon>
#include <QtGui/QInputDialog>
#include <QtGui/QItemDelegate>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>
#include <QtGui/QMouseEvent>
#include <QtGui/QMovie>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QPalette>
#include <QtGui/QPixmap>
#include <QtGui/QPixmapCache>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPolygon>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrinter>
#include <QtGui/QProgressBar>
#include <QtGui/QProgressDialog>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QResizeEvent>
#include <QtGui/QScrollArea>
#include <QtGui/QScrollBar>
#include <QtGui/QShortcut>
#include <QtGui/QSlider>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QSpinBox>
#include <QtGui/QSplitter>
#include <QtGui/QStackedWidget>
#include <QtGui/QStatusBar>
#include <QtGui/QStyleFactory>
#include <QtGui/QStyleOptionButton>
#include <QtGui/QStylePainter>
#include <QtGui/QTabBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTableView>
#include <QtGui/QTextBrowser>
#include <QtGui/QTextDocument>
#include <QtGui/QTextEdit>
#include <QtGui/QToolBar>
#include <QtGui/QToolBar>
#include <QtGui/QToolButton>
#include <QtGui/QToolTip>
#include <QtGui/QTreeView>
#include <QtGui/QTreeView>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QTreeWidgetItemIterator>
#include <QtGui/QVBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtXml/QDomDocument>
#include <QtXml/QXmlStreamReader>

// STL headers
#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <ostream>
#include <queue>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

// Boost headers
#include <boost/iostreams/device/null.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iterator/iterator_facade.hpp>

// Loki headers
#include <loki/Factory.h>
#include <loki/Singleton.h>

// OS headers
#include <windows.h>
//#include <dlfcn.h> Linux
//#include <stdlib.h> Linux
//#include <sys/prctl.h> Linux
//#include <unistd.h> Linux OSX
//#include <sys/param.h> OSX

// Other headers
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// TOra headers
//#include "toresultview.h"
//#include "tomain.h"
//#include "tosql.h"
//#include "toconf.h"
//#include "totool.h"
//#include "toconnection.h"
//#include "utils.h"
//#include "config.h"
//#include "tologger.h"

