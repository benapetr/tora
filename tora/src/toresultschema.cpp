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
#include "toresultschema.h"
#include "toconnection.h"

#include <QSettings>


toResultSchema::toResultSchema(toConnection &conn,
                               QWidget *parent,
                               const char *name)
    : toResultCombo(parent, name)
{
    setSQL(toSQL::sql(toSQL::TOSQL_USERLIST));

    ConnectionKey =
        conn.provider() + "-" +
        conn.host() + "-" +
        conn.database();

    QString sel = conn.schema();

    if(sel.isEmpty())
    {
        QSettings s;
        sel = s.value("schema/" + ConnectionKey).toString();
    }

    if(sel.isEmpty())
    {
        if (toIsMySQL(conn))
            sel = conn.database();
        else if (toIsOracle(conn) || toIsSapDB(conn))
            sel = conn.user().toUpper();
        else
            sel = conn.user();
    }

    conn.setSchema(sel);
    setSelected(sel);
    connect(this,
            SIGNAL(currentIndexChanged(const QString &)),
            this,
            SLOT(updateLastSchema(const QString &)));
}


#define CHANGE_CURRENT_SCHEMA QString("ALTER SESSION SET CURRENT_SCHEMA = %1")
#define CHANGE_CURRENT_SCHEMA_PG QString("SET search_path TO %1,\"$user\",public")

void toResultSchema::update() {
    update(toResultCombo::currentText());
}


void toResultSchema::update(const QString &schema) {
    if(schema.isEmpty())
        return;

    try {
        toConnection &conn = connection();

        if (toIsOracle(conn)) {
            /* remove current schema initstring */
            conn.delInit(CHANGE_CURRENT_SCHEMA.arg(conn.user()));

            /* set the new one with selected schema */
            QString sql = CHANGE_CURRENT_SCHEMA.arg(schema);
            conn.allExecute(sql);

            conn.addInit(sql);
        }
        else if (toIsMySQL(conn)) {
            conn.allExecute(QString("USE `%1`").arg(schema));
            conn.setDatabase(schema);
        }
        else if (toIsPostgreSQL(conn))
            conn.allExecute(CHANGE_CURRENT_SCHEMA_PG.arg(schema));
        else
            throw QString("No support for changing schema for this database");

        conn.setSchema(schema);
    }
    TOCATCH;
}


void toResultSchema::updateLastSchema(const QString &schema) {
    QSettings s;
    s.setValue("schema/" + ConnectionKey, schema);
}
