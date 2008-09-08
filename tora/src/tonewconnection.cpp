/*****
 *
 * TOra - An Oracle Toolkit for DBA's and developers
 * Copyright (C) 2003-2008 Quest Software, Inc
 * Portions Copyright (C) 2008 Other Contributors
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

#include "toconf.h"
#include "toconnection.h"
#include "tonewconnection.h"
#include "toconnectionimport.h"
#include "tomain.h"
#include "totool.h"
#include "toconnectionmodel.h"

#include "icons/trash.xpm"

#include <QMessageBox>
#include <QHeaderView>
#include <QStringList>
#include <QMenu>
#include <QIcon>
#include <QSortFilterProxyModel>

// #define MAX_HISTORY 50

static const QString ORACLE_INSTANT = "Oracle (Instant Client)";
static const QString ORACLE_TNS     = "Oracle (TNS)";


toNewConnection::toNewConnection(
    QWidget* parent,
    Qt::WFlags fl) : QDialog(parent, fl),
        toHelpContext(QString::fromLatin1("newconnection.html"))
{
    setupUi(this);
    toHelp::connectDialog(this);

    m_connectionModel = new toConnectionModel();
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_connectionModel);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setFilterKeyColumn(-1);
    Previous->setModel(m_proxyModel);

    std::list<QString> lst = toConnectionProvider::providers();

    Q_FOREACH(QString s, lst)
    {
        if (s == "Oracle")
        {
            Provider->addItem(ORACLE_INSTANT);
            if (getenv("ORACLE_HOME"))
                Provider->addItem(ORACLE_TNS);
        }
        else
            Provider->addItem(s);
    }

    if (Provider->count() < 1)
    {
        TOMessageBox::information(
            this,
            tr("No connection provider"),
            tr("No available connection provider"));

        reject();
        return;
    }

    NewConnection = 0;

    Previous->setContextMenuPolicy(Qt::CustomContextMenu);

    PreviousContext = new QMenu(this);
    QAction *delact = PreviousContext->addAction(
                          QIcon(trash_xpm),
                          tr("&Delete"));
    connect(delact,
            SIGNAL(triggered()),
            this,
            SLOT(historyDelete()));

    connect(Previous,
            SIGNAL(customContextMenuRequested(const QPoint &)),
            this,
            SLOT(previousMenu(const QPoint &)));

    Settings.beginGroup("connections");
    readSettings();

	Previous->horizontalHeader()->setStretchLastSection(true);
    Previous->horizontalHeader()->setHighlightSections(false);
    Previous->verticalHeader()->setVisible(false);

    connect(Previous->selectionModel(),
            SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)),
            this,
            SLOT(previousCellChanged(const QModelIndex &)));

    connect(Provider,
            SIGNAL(currentIndexChanged(int)),
            this,
            SLOT(changeProvider(int)));

    connect(Previous,
            SIGNAL(doubleClicked(const QModelIndex &)),
            this,
            SLOT(accept()));

    connect(Host,
            SIGNAL(editTextChanged(QString)),
            this,
            SLOT(changeHost()));

    connect(ImportButton, SIGNAL(clicked()),
             this, SLOT(importButton_clicked()));

    connect(searchEdit, SIGNAL(textEdited(const QString &)),
             this, SLOT(searchEdit_textEdited(const QString &)));

    // must make sure this gets called manually.
    changeProvider(Provider->currentIndex());
    Previous->setFocus(Qt::OtherFocusReason);
}

QString toNewConnection::realProvider()
{
    QString p = Provider->currentText();
    if (p.startsWith("Oracle"))
        return "Oracle";

    return p;
}

void toNewConnection::readSettings()
{
    QMap<int, toConnectionOptions> OptionMap;
    restoreGeometry(Settings.value("geometry").toByteArray());

    Previous->setSortingEnabled(false);

    Settings.beginGroup("history");
    for (int pos = 0; pos < Settings.childGroups().count(); ++pos)
    {
        Settings.beginGroup(/*"history/" +*/ QString::number(pos)); // X
        if (!Settings.contains("provider"))
        {
            Settings.endGroup(); // X
            break;
        }

        QString provider = Settings.value("provider", "").toString();
        QString host     = Settings.value("host", "").toString();
        QString database = Settings.value("database", "").toString();
        QString username = Settings.value("username", "").toString();
        QString password = toUnobfuscate(Settings.value("password", "").toString());
        QString schema   = Settings.value("schema", "").toString();

        if ( provider == ORACLE_TNS )
            host = "";

        Settings.beginGroup("options");
        std::set<QString> options;
        QStringList keys = Settings.allKeys();
        Q_FOREACH(QString s, keys)
        {
            if (Settings.value(s, false).toBool())
                options.insert(s);
        }
        Settings.endGroup(); // options

        toConnectionOptions opt(
            provider,
            host,
            database,
            username,
            password,
            schema,
            Settings.value("port", 0).toInt(),
            options);
        OptionMap[pos] = opt;
        Settings.endGroup(); // X
    }
    Settings.endGroup(); // history

    m_connectionModel->setupData(OptionMap);
    Previous->resizeColumnsToContents();
    Previous->setSortingEnabled(true);
    Previous->hideColumn(0);
}


void toNewConnection::writeSettings(bool checkHistory)
{
    int r = 0;
    Settings.setValue("geometry", saveGeometry());

    Settings.remove("history");

    if (!Provider->currentText().isEmpty() && checkHistory)
    {
        Settings.beginGroup("history/0");
        Settings.setValue("provider", Provider->currentText());
        Settings.setValue("username", Username->text());
        Settings.setValue("password", toObfuscate(Password->text()));
        Settings.setValue("host", Host->currentText());
        Settings.setValue("port", Port->value());
        Settings.setValue("database", Database->currentText());
        Settings.setValue("schema", Schema->text());

        Settings.beginGroup("options");
        QList<QCheckBox *> widgets = OptionGroup->findChildren<QCheckBox *>();
        Q_FOREACH(QCheckBox *box, widgets)
        Settings.setValue(box->text(), box->isChecked());
        Settings.endGroup(); // options
        Settings.endGroup(); // history/0
        ++r;
    }

    // find history item with same options. will skip later.
    int skip = -1;
    if (checkHistory)
    {
        skip = findHistory(Provider->currentText(),
                           Username->text(),
                           Host->currentText(),
                           Database->currentText(),
                           Schema->text());
    }

    QMap<int,toConnectionOptions> c = m_connectionModel->availableConnections();
    foreach(int row, c.keys())
    {
        if (row == skip)// && ++skipped)
            continue;

        toConnectionOptions &opt = c[row];

        Settings.beginGroup("history/" + QString::number(r/*row*/));// + 1 - skipped));
        Settings.setValue("provider", opt.provider);
        Settings.setValue("username", opt.username);
        Settings.setValue("host", opt.host);
        Settings.setValue("database", opt.database);
        Settings.setValue("schema", opt.schema);
        Settings.setValue("port", opt.port);
        Settings.setValue("password", toObfuscate(opt.password));

        Settings.beginGroup("options");
        Q_FOREACH(QString s, opt.options)
        Settings.setValue(s, true);
        Settings.endGroup();

        Settings.endGroup();
        ++r;
    }
}


int toNewConnection::findHistory(const QString &provider,
                                 const QString &username,
                                 const QString &host,
                                 const QString &database,
                                 const QString &schema)
{
    QMapIterator<int,toConnectionOptions> i(m_connectionModel->availableConnections());
    while (i.hasNext())
    {
        i.next();
        toConnectionOptions opt = i.value();
        if (provider == opt.provider &&
                username == opt.username &&
                host == opt.host &&
                database == opt.database &&
                schema == opt.schema)
            return i.key();
    }
    return -1;
}

void toNewConnection::loadPrevious(const QModelIndex & current)
{
    if (!current.isValid())
        return;

    QModelIndex baseIndex = m_proxyModel->index(current.row(), 0);
    int index = m_proxyModel->data(baseIndex, Qt::DisplayRole).toInt();
    toConnectionOptions opt = m_connectionModel->availableConnection(index);// OptionMap[index];

    Provider->setCurrentIndex(Provider->findText(opt.provider));
    Host->lineEdit()->setText(opt.host);
    Database->lineEdit()->setText(opt.database);
    Username->setText(opt.username);
    Password->setText(opt.password);
    Port->setValue(opt.port);
    Schema->setText(opt.schema);

    QList<QCheckBox *> widgets = OptionGroup->findChildren<QCheckBox *>();
    Q_FOREACH(QCheckBox *box, widgets)
    box->setChecked(opt.options.find(box->text()) != opt.options.end());
}


void toNewConnection::done(int r)
{
    writeSettings();

    if (!r)
    {
        QDialog::done(r);
        return;
    }

    NewConnection = makeConnection();
    if (!NewConnection)
        return;

    QDialog::done(r);
}


void toNewConnection::previousCellChanged(const QModelIndex & current)
{
     loadPrevious(current);
}


void toNewConnection::changeProvider(int current)
{
    try
    {
        QString provider = realProvider();
        if (provider.isNull() || provider.isEmpty())
            return;

        std::list<QString> hosts = toConnectionProvider::hosts(provider);

        DefaultPort = 0;
        for (std::list<QString>::iterator i = hosts.begin(); i != hosts.end(); i++)
        {
            if ((*i).isEmpty())
                continue;
            else if ((*i).startsWith(":"))
                DefaultPort = (*i).mid(1).toInt();
            else
                Host->addItem(*i);
        }

        // seems i broke this for oracle
        if (!DefaultPort)
        {
            if (provider.startsWith("Oracle"))
                DefaultPort = 1521;
        }

        if (Provider->currentText() == ORACLE_TNS || Provider->currentText() == ORACLE_INSTANT )
        {
            SchemaLabel->show();
            Schema->show();
        }
        else
        {
            SchemaLabel->hide();
            Schema->hide();
        }

        Port->setValue(DefaultPort);

        if (Provider->currentText() == ORACLE_TNS)
        {
            HostLabel->hide();
            Host->hide();
            PortLabel->hide();
            Port->hide();
        }
        else
        {
            HostLabel->show();
            Host->show();
            PortLabel->show();
            Port->show();
        }

        QList<QWidget *> widgets = OptionGroup->findChildren<QWidget *>();
        Q_FOREACH(QWidget *w, widgets)
        delete w;

        std::list<QString> options = toConnectionProvider::options(provider);
        for (std::list<QString>::iterator j = options.begin();
                j != options.end();
                j++)
        {

            if (*j == "-")
                continue;

            QString option = *j;
            bool defOn = false;
            if (option.startsWith("*"))
            {
                defOn = true;
                option = option.mid(1);
            }

            QCheckBox *ow = new QCheckBox(OptionGroup);
            ow->setText(option);
            ow->setChecked(defOn);
            OptionGroup->layout()->addWidget(ow);
            ow->show();
        }

        if (options.empty())
            OptionGroup->hide();
        else
            OptionGroup->show();

        changeHost();
    }
    catch (const QString &str)
    {
        toStatusMessage(str);
    }
}


void toNewConnection::changeHost(void)
{
    QString prov = realProvider();

    try {
        if(Host->isVisible() || (prov == "Oracle"))
        {
            QString host = Host->currentText();
            std::list<QString> databases = toConnectionProvider::databases(
                prov,
                host,
                Username->text(),
                Password->text());
            QString current = Database->currentText();

            Database->clear();
            Q_FOREACH(QString s, databases)
                Database->addItem(s);
            Database->lineEdit()->setText(current);
        }
    }
    catch(const QString &str) {
        Database->clear();
        toStatusMessage(str);
    }
}


void toNewConnection::importButton_clicked()
{
    toConnectionImport dia;
    if (!dia.exec())
        return;

    Previous->setSortingEnabled(false);

    // find latest id (max+1)
    QList<int> keys = m_connectionModel->availableConnections().keys();
    qSort(keys);
    int max = 0;
    if (keys.count() > 0)
        max = keys.at(keys.count()-1) + 1;

    foreach (toConnectionOptions opt, dia.availableConnections().values())
    {
        if (findHistory(opt.provider, opt.username, opt.host, opt.database, opt.schema) != -1)
            continue;

        m_connectionModel->append(max, opt);
        ++max;
    }
    writeSettings();

    Previous->setSortingEnabled(true);
}

void toNewConnection::searchEdit_textEdited(const QString & text)
{
    m_proxyModel->setFilterWildcard(QString("*%1*").arg(text));
}

toConnection* toNewConnection::makeConnection(void)
{
    try
    {
        QString pass;
        QString host;
        if (!Host->isHidden())
            host = Host->currentText();

        QString schema;
        if (!Schema->isHidden())
            schema = Schema->text();

        QString optionstring;
        std::set<QString> options;

        QList<QCheckBox *> widgets = OptionGroup->findChildren<QCheckBox *>();
        Q_FOREACH(QCheckBox *box, widgets)
        {
            if (!optionstring.isEmpty())
                optionstring += ",";

            if (box->isChecked())
            {
                optionstring += "*";

                // ug. this is awesome. i broke it when i added
                // accelerators.
                options.insert(box->text().replace("&", ""));
            }

            optionstring += box->text();
        }

        QString provider = realProvider();

        if (Port->value() != 0 && Port->value())
            host += ":" + QString::number(Port->value());

        QString database = Database->currentText();

        if (Provider->currentText() == ORACLE_INSTANT)
        {
            // create the rest of the connect string. this will work
            // without an ORACLE_HOME.

            int port = Port->value();
            database = "//" + Host->currentText() +
                       ":" + QString::number(port) +
                       "/" + database;
            host = "";
        }

        // checks for existing connection
        std::list<QString> con = toMainWidget()->connections();
        for (std::list<QString>::iterator i = con.begin();i != con.end();i++)
        {
            try
            {
                toConnection &conn = toMainWidget()->connection(*i);
                if (conn.user() == Username->text() &&
                        conn.provider() == realProvider() &&
                        conn.host() == host &&
                        conn.database() == database &&
                        conn.schema() == schema)
                    return &conn;
            }
            catch (...) {}
        }

        toConnection *retCon = new toConnection(
            provider,
            Username->text(),
            Password->text(),
            host,
            database,
            schema,
            options);

        writeSettings(true);
        return retCon;
    }
    catch (const QString &exc)
    {
        QString str = tr("Unable to connect to the database.\n");
        str.append(exc);
        TOMessageBox::information(this->parentWidget(),
                                  tr("Unable to connect to the database"),
                                  str);
        return NULL;
    }
}


void toNewConnection::previousMenu(const QPoint &pos)
{
    Q_UNUSED(pos);
    PreviousContext->exec(QCursor::pos());
}

void toNewConnection::historyDelete()
{
//     QModelIndexList indexes = Previous->selectionModel()->selectedRows();
//     QModelIndex index;
//     foreach(index, indexes)   // loop through and remove them
//     {
//         int row = m_proxyModel->mapToSource(index).row();
//         m_connectionModel->removeRow(row, QModelIndex());
//     }
    QMap<int,toConnectionOptions> v = m_connectionModel->availableConnections();
    int ix = m_proxyModel->data(m_proxyModel->index(Previous->currentIndex().row(), 0)).toInt();
    v.remove(ix);
    m_connectionModel->setupData(v);

    writeSettings(false);
    readSettings();
}
