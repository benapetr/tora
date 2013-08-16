
/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * TOra - An Oracle Toolkit for DBA's and developers
 *
 * Shared/mixed copyright is held throughout files in this product
 *
 * Portions Copyright (C) 2000-2001 Underscore AB
 * Portions Copyright (C) 2003-2005 Quest Software, Inc.
 * Portions Copyright (C) 2004-2009 Numerous Other Contributors
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
 *      these libraries.
 *
 *      You may link this product with any GPL'd Qt library.
 *
 * All trademarks belong to their respective owners.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "utils.h"
#include "toquerymodel.h"

#include "tologger.h"

//#include "tomain.h"
//#include "toconnectionmodel.h"

//#include <QtGui/QHeaderView>
#include <QtGui/QTabWidget>
#include <QtGui/QListView>

#include <QtCore/QTimerEvent>

#include "core/tsqlparse.h"
#include "docklets/toastwalk.h"

#include "ermodel/dotgraphview.h"
#include "ermodel/dotgraph.h"

#define TOOL_NAME "QueryModel"

char* workaround(char *buf, int n, FILE* fp)
{
    return fgets(buf, n, fp);
};

REGISTER_VIEW(TOOL_NAME, toQueryModel);

toQueryModel::toQueryModel(QWidget *parent, Qt::WindowFlags flags)
    : toDocklet(tr(TOOL_NAME), parent, flags)
    , m_widget(NULL)
    , m_currentEditor(new editHandlerHolder())
    , m_timerID(-1)
{
    setObjectName("Query model");
    toEditWidget::addHandler(m_currentEditor);

    {
        m_widget = new DotGraphView( NULL /*actionCollection()*/, this);
        m_widget->initEmpty();
        m_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        // connect( m_widget, SIGNAL( graphLoaded() ),
        //   this, SIGNAL( graphLoaded() ) );
        // connect( m_widget, SIGNAL( newEdgeAdded(const QString&, const QString&) ),
        //   this, SIGNAL( newEdgeAdded(const QString&, const QString&) ) );
        // connect( m_widget, SIGNAL( newNodeAdded(const QString&) ),
        //   this, SIGNAL( newNodeAdded(const QString&) ) );
        // connect( m_widget, SIGNAL( removeEdge(const QString&) ),
        //   this, SIGNAL( removeEdge(const QString&) ) );
        // connect( m_widget, SIGNAL( removeElement(const QString&) ),
        //   this, SIGNAL( removeElement(const QString&) ) );
        // connect( m_widget, SIGNAL( selectionIs(const QList<QString>, const QPoint&) ),
        //   this, SIGNAL( selectionIs(const QList<QString>, const QPoint&) ) );
        // connect( m_widget,
        //   SIGNAL( contextMenuEvent(const QString&, const QPoint&) ),
        //   this,
        //   SIGNAL( contextMenuEvent(const QString&, const QPoint&) ) );
        // connect( m_widget,
        //   SIGNAL( newEdgeFinished(const QString&, const QString&, const QMap<QString, QString>&) ),
        //   this,
        //   SIGNAL( newEdgeFinished(const QString&, const QString&, const QMap<QString, QString>&) ) );
        // connect( m_widget, SIGNAL( hoverEnter(const QString&)) ,
        //   this, SIGNAL( hoverEnter(const QString&) ) );
        // connect( m_widget, SIGNAL( hoverLeave(const QString&)) ,
        //   this, SIGNAL( hoverLeave(const QString&)) );
        ///m_widget->loadLibrary(QString("/tmp/example1.dot"));

        setFocusProxy(m_widget); // TODO ?? What is this??
        setWidget(m_widget); // TODO ?? What is this??
    };

    {
        //GVC_t *gvc = gvContext();

        ////graph_t *g;
        ////g = agopen("TST", AGRAPH);
        ////FILE* fp = fopen("D:\\DEVEL\\tc.dot", "r");
        ////g = agread(fp);
        ////g=agread_usergets(fp,workaround);

        ////m_widget->loadLibrary(g);

        /////QString file("D:\\DEVEL\\tc.dot");
        /////m_widget->loadDot(file);
        /////m_widget->loadLibrary(file);

        //QMap<QString,QString> ga;
        //ga["id"] = "Query";
        //ga["compound"] = "true";
        //ga["shape"] = "circle";

        //m_widget->graph()->setGraphAttributes(ga);

        //QMap<QString,QString> na;
        //na["name"] = "Acko";
        //na["label"] = "AckoL";
        //na["fontsize"] = "5";
        //na["comment"]="ACKO";
        //na["id"]="A";
        //QMap<QString,QString> nb;
        //nb["name"] = "Becko";
        //nb["label"] = "laBel";
        //nb["fontsize"] = "10";
        //nb["comment"]="BEEE";
        //nb["id"]="B";


        //m_widget->graph()->addNewNode(na);
        //m_widget->graph()->addNewNode(nb);

        //m_widget->graph()->update();
    }

    m_timerID = startTimer(5000);
    ///TLOG(0,toDecorator,__HERE__) << "void toQueryModel::timerEvent(QTimerEvent *e) fired" << std::endl;
}

void toQueryModel::timerEvent(QTimerEvent *e)
{
    QString newText;
    ///TLOG(0,toDecorator,__HERE__) << "void toQueryModel::timerEvent(QTimerEvent *e) fired" << std::endl;

    if( m_timerID == e->timerId())
    {
        // if( m_currentEditor->m_current)
        //  TLOG(0,toDecorator,__HERE__) << m_currentEditor->m_current->editText().toStdString() << std::endl;
    }
    else
    {
        toDocklet::timerEvent(e);
    }

    if( !isVisible())
        return;

    if( m_currentEditor->m_current == NULL)
        return;

    newText = m_currentEditor->m_current->editText();
    if( newText == m_lastText)
        return;

    m_lastText = newText;

    if(newText.isEmpty())
        return;

    try
    {
        std::auto_ptr <SQLParser::Statement> stat = StatementFactTwoParmSing::Instance().create("OracleSQL", m_lastText, "");
        TLOG(0, toDecorator, __HERE__) << "Parsing ok:" << std::endl
                                       << stat->root()->toStringRecursive().toStdString() << std::endl;

        //delete(m_widget);
        //m_widget = new KGraphViewer::DotGraphView( NULL /*actionCollection()*/, this);
        m_widget->initEmpty();
        //m_widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        //setFocusProxy(m_widget); // TODO ?? What is this??
        //setWidget(m_widget); // TODO ?? What is this??
        toASTWalk(stat.get(), m_widget->graph());

    }
    catch ( SQLParser::ParseException const &e)
    {

    }
    catch (...)
    {

    }
}

QIcon toQueryModel::icon() const
{
    return QIcon(":/icons/connect.xpm");
}


QString toQueryModel::name() const
{
    return tr(TOOL_NAME);
}

void toQueryModel::describeSlot(void)
{

    GVC_t *gvc = gvContext();
    graph_t *g = agopen("TST", AGRAPH);

    m_widget->loadLibrary(g);

    QMap<QString, QString> ga;
    ga["compound"] = "true";

    m_widget->graph()->setGraphAttributes(ga);

    QMap<QString, QString> na;
    na["name"] = "Acko";
    na["label"] = "AckoL";
    na["fontsize"] = "5";
    na["comment"] = "ACKO";

    m_widget->graph()->addNewNode(na);

    toASTWalk(NULL, m_widget->graph());

    m_widget->graph()->update();
}
