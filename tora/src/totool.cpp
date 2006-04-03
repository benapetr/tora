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

#include "utils.h"

#include "tobackground.h"
#include "toconf.h"
#include "toconnection.h"
#include "tomain.h"
#include "totool.h"

#ifndef WIN32
#include <unistd.h>
#endif

#include <qapplication.h>
#include <qfile.h>
#include <qpixmap.h>
#include <qregexp.h>
#include <qstring.h>
#include <qtimer.h>
#include <qworkspace.h>

#ifdef WIN32
#  include "windows/cregistry.h"
#endif

#include "totool.moc"

// A little magic to get lrefresh to work and get a check on qApp

#undef QT_TRANSLATE_NOOP
#define QT_TRANSLATE_NOOP(x,y) QTRANS(x,y)

toConnectionWidget::toConnectionWidget(toConnection &conn, QWidget *widget)
        : Connection(&conn), Widget(widget)
{
    Connection->addWidget(Widget);
}

void toConnectionWidget::setConnection(toConnection &conn)
{
    if (Connection)
        Connection->delWidget(Widget);
    Connection = &conn;
    Connection->addWidget(Widget);
}

toConnectionWidget::toConnectionWidget(QWidget *widget)
        : Widget(widget)
{
    Connection = NULL;
}

toConnection &toConnectionWidget::connection()
{
    if (Connection)
        return *Connection;
    QWidget *widget = Widget;
    if (!widget)
        throw qApp->translate("toConnectionWidget", "toConnectionWidget not inherited with a QWidget");
    return toCurrentConnection(widget->parentWidget());
}

toToolWidget::toToolWidget(toTool &tool, const QString &ctx, QWidget *parent, toConnection &conn, const char *name)
        : QVBox(parent, name, WDestructiveClose), toHelpContext(ctx), toConnectionWidget(conn, this), Tool(tool)
{
    Timer = NULL;

    if (parent)
    {
        // Voodoo for making connection changing cascade to sub tools.
        try
        {
            toToolWidget *tool = toCurrentTool(parent);
            if (tool)
                connect(tool, SIGNAL(connectionChange()), this, SLOT(parentConnection()));
        }
        catch (...)
        {}
    }
}

toConnectionWidget::~toConnectionWidget()
{
    if (Connection)
        Connection->delWidget(Widget);
}

void toToolWidget::parentConnection(void)
{
    try
    {
        setConnection(toCurrentConnection(parentWidget()));
    }
    TOCATCH
}

toToolWidget::~toToolWidget()
{
    toMainWidget()->toolWidgetRemoved(this);
}

void toToolWidget::setConnection(toConnection &conn)
{
    bool connCap = false;
    QString name = connection().description();
    QString capt = caption();
    if (capt.startsWith(name))
    {
        connCap = true;
        capt = capt.mid(name.length());
    }
    toConnectionWidget::setConnection(conn);
    if (connCap)
    {
        capt.prepend(connection().description());
        setCaption(capt);
    }
    emit connectionChange();
}

toTimer *toToolWidget::timer(void)
{
    if (!Timer)
        Timer = new toTimer(this);
    return Timer;
}

void toToolWidget::exportData(std::map<QCString, QString> &data, const QCString &prefix)
{
    QWidget *par = parentWidget();
    if (!par)
        par = this;
    if (isMaximized() || par->width() >= toMainWidget()->workspace()->width())
        data[prefix + ":State"] = QString::fromLatin1("Maximized");
    else if (isMinimized())
        data[prefix + ":State"] = QString::fromLatin1("Minimized");

    data[prefix + ":X"] = QString::number(par->x());
    data[prefix + ":Y"] = QString::number(par->y());
    data[prefix + ":Width"] = QString::number(par->width());
    data[prefix + ":Height"] = QString::number(par->height());
}

void toToolWidget::importData(std::map<QCString, QString> &data, const QCString &prefix)
{
    QWidget *par = parentWidget();
    if (!par)
        par = this;
    if (data[prefix + ":State"] == QString::fromLatin1("Maximized"))
    {
        par->showMaximized();
        showMaximized();
    }
    else if (data[prefix + ":State"] == QString::fromLatin1("Minimized"))
    {
        par->showMinimized();
        showMinimized();
    }
    else
    {
        par->showNormal();
        par->setGeometry(data[prefix + ":X"].toInt(),
                         data[prefix + ":Y"].toInt(),
                         data[prefix + ":Width"].toInt(),
                         data[prefix + ":Height"].toInt());

    }
}

std::map<QCString, toTool *> *toTool::Tools;
// std::map<QCString, QString> *toTool::Configuration;


const char **toTool::pictureXPM(void)
{
    return (const char **) NULL;  // compiler warning
}

toTool::toTool(int priority, const char *name)
        : Name(name)
{
    if (!Tools)
        Tools = new std::map<QCString, toTool *>;
    Priority = priority;
    Key.sprintf("%05d%s", priority, name);
    (*Tools)[Key] = this;
    ButtonPicture = NULL;
}

toTool::~toTool()
{
    Tools->erase(Key);
}

const QPixmap *toTool::toolbarImage()
{
    if (!ButtonPicture)
    {
        const char **picture = pictureXPM();
        if (picture)
            ButtonPicture = new QPixmap(picture);
    }
    return ButtonPicture;
}

void toTool::createWindow(void)
{
    toMain *main = (toMain *)qApp->mainWidget();
    try
    {
        if (!canHandle(main->currentConnection()))
            throw QString("The tool %1 doesn't support the current database").arg(name());
        QWidget *newWin = toolWindow(main->workspace(), main->currentConnection());

        if (newWin)
        {
            const QPixmap *icon = toolbarImage();
            if (icon)
                newWin->setIcon(*icon);
            toToolWidget *tool = dynamic_cast<toToolWidget *>(newWin);
            if (tool)
                toToolCaption(tool, name());

            newWin->show();
            newWin->raise();
            newWin->setFocus();

            main->windowsMenu();

            if (tool)
                main->toolWidgetAdded(tool);

            // Maximize window if only window
            {
                bool max = true;
#if QT_VERSION < 0x030200

                for (unsigned int i = 0;i < toMainWidget()->workspace()->windowList().count();i++)
                {
                    QWidget *widget = toMainWidget()->workspace()->windowList().at(i);
#else

                for (unsigned int i = 0;i < toMainWidget()->workspace()->windowList(QWorkspace::CreationOrder).count();i++)
                {
                    QWidget *widget = toMainWidget()->workspace()->windowList(QWorkspace::CreationOrder).at(i);
#endif

                    if (widget && widget != newWin && !widget->isHidden())
                        max = false;
                }
                if (max)
                    newWin->showMaximized();
            }

#if QT_VERSION < 0x030100
            // This is a really ugly workaround for a Qt layout bug
            QWidget *tmp = NULL;
            QWidget *tmp2 = NULL;
            for (unsigned int i = 0;i < toMainWidget()->workspace()->windowList().count();i++)
            {
                QWidget *widget = toMainWidget()->workspace()->windowList().at(i);
                if (newWin != widget)
                    tmp2 = widget;
                else
                    tmp = newWin;
                if (tmp2 && tmp)
                    break;
            }
            if (tmp2 && tmp)
            {
                tmp2->setFocus();
                tmp->setFocus();
            }
#endif
        }
    }
    TOCATCH
}


bool toTool::canHandle(toConnection &conn)
{
    return (toIsOracle(conn));
}

void toTool::customSetup(int)
{}

QWidget *toTool::configurationTab(QWidget *)
{
    return NULL;
}

toTool *toTool::tool(const QCString &key)
{
    if (!Tools)
        Tools = new std::map<QCString, toTool *>;
    std::map<QCString, toTool *>::iterator i = Tools->find(key);
    if (i == Tools->end())
        return NULL;

    return (*i).second;
}

void toTool::about(QWidget *)
{}

 const QString& toTool::config(const QCString &tag,const QCString &def)
 {
     return toConfigurationSingle::Instance().config(tag, def, Name);
 }

  void toTool::setConfig(const QCString &tag,const QString &value)
  {
      toConfigurationSingle::Instance().setConfig(tag, value, Name);
  }
