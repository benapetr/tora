
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

    setSelected(sel);
    if (SelectedFound)
    	conn.setSchema(sel);
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

void toResultSchema::updateLastSchema(const QString &schema)
{
	if (schema.isEmpty())
		return;
    QSettings s;
    s.setValue("schema/" + ConnectionKey, schema);
    if (SelectedFound)
    	connection().setSchema(schema);
}

void toResultSchema::slotUsersFromCache(void)
{
	//userList.clear();
	clear();
	SelectedFound = false;
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
		{
			setCurrentIndex(count() - 1);
			SelectedFound = true;
		}
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
