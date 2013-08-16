
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

#include "core/toresultschema.h"
#include "core/utils.h"
#include "core/toconnection.h"

#include <QtCore/QSettings>


toResultSchema::toResultSchema(QWidget *parent,
                               const char *name)
    : toResultCombo(parent, name)
{
    setSQL(toSQL::sql(toSQL::TOSQL_USERLIST));

    toConnection &conn = toConnection::currentConnection(parent);
    ConnectionKey =
        conn.provider() + "-" +
        conn.host() + "-" +
        conn.database();

    QString sel = conn.schema();

    if (sel.isEmpty())
    {
        QSettings s;
        sel = s.value("schema/" + ConnectionKey).toString();
    }

    if (sel.isEmpty())
    {
        if (conn.providerIs("QMYSQL"))
            sel = conn.database();
        else
            sel = conn.user();
    }

    // Oracle usernames are always in upper case
    if (conn.providerIs("Oracle") || conn.providerIs("SapDB"))
        sel = sel.toUpper();

    conn.setSchema(sel);
    setSelected(sel);
    connect(this, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(updateLastSchema(const QString &)));

    connect(&(connection().getCache()), SIGNAL(userListRefreshed()),
	    this, SLOT(slotUsersFromCache()));
    setFocusPolicy(Qt::NoFocus);
}

void toResultSchema::query(const QString &sql, toQueryParams const& param)
{
	// do not call setSqlAndParams twice in a row. (also called from toResultCombo::query)
	//if (!setSqlAndParams(sql, param))
	//	return ;

	if(connection().getCache().userListExists(toCache::USERS))
	{
		slotUsersFromCache();
	} else {
		//userList.clear();
		toResultCombo::query(sql, param);
	}
}

#define CHANGE_CURRENT_SCHEMA QString("ALTER SESSION SET CURRENT_SCHEMA = \"%1\"")
#define CHANGE_CURRENT_SCHEMA_PG QString("SET search_path TO %1,\"$user\",public")
#define CHANGE_CURRENT_SCHEMA_TD QString("DATABASE \"%1\"")

void toResultSchema::update()
{
    update(toResultCombo::currentText());
}


void toResultSchema::update(const QString &schema)
{
    if(schema.isEmpty())
        return;

    try
    {
        toConnection &conn = connection();

        if (schema != conn.schema())
        {
            if (conn.providerIs("Oracle"))
            {
                /* remove current schema initstring */
                conn.delInit(CHANGE_CURRENT_SCHEMA.arg(conn.user()));

                /* set the new one with selected schema */
                QString sql = CHANGE_CURRENT_SCHEMA.arg(schema);
                conn.allExecute(sql);

                conn.addInit(sql);
            }
            else if (conn.providerIs("QMYSQL"))
            {
                conn.allExecute(QString("USE `%1`").arg(schema));
                //mysql conn.setDatabase(schema);
            }
            else if (conn.providerIs("PostgreSQL"))
                conn.allExecute(CHANGE_CURRENT_SCHEMA_PG.arg(schema));
            else if (conn.providerIs("Teradata"))
                conn.allExecute(CHANGE_CURRENT_SCHEMA_TD.arg(schema));
            else
                throw QString("No support for changing schema for this database");

            conn.setSchema(schema);
        }
    }
    TOCATCH;
}


void toResultSchema::updateLastSchema(const QString &schema)
{
	if (schema.isEmpty())
		return;
    QSettings s;
    s.setValue("schema/" + ConnectionKey, schema);
    connection().setSchema(schema);
}

void toResultSchema::slotUsersFromCache(void)
{
	//userList.clear();
	clear();
	addItems(Additional);
	for (int i = 0; i < Additional.count(); i++)
		if (Additional[i] == Selected)
			setCurrentIndex(i);

	QStringList users = connection().getCache().userList(toCache::USERS);
	for (QStringList::iterator i = users.begin(); i != users.end(); i++)
	{
		QString t = (*i);
		addItem(t);
		if (t == Selected)
			setCurrentIndex(count() - 1);
	}
	slotQueryDone();
}

void toResultSchema::refresh(void)
{
       try {
               toResultCombo::refresh();
       }
       catch (...)
       {
               TLOG(1,toDecorator,__HERE__) << "       Ignored exception." << std::endl;
       }
}
