
/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * TOra - An Oracle Toolkit for DBA's and developers
 *
 * Shared/mixed copyright is held throughout files in this product
 *
 * Portions Copyright (C) 2000-2001 Underscore AB
 * Portions Copyright (C) 2003-2005 Quest Software, Inc.
 * Portions Copyright (C) 2004-2008 Numerous Other Contributors
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

#ifndef TOCONNECTION_OPTIONS_H
#define TOCONNECTION_OPTIONS_H

#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QSet>

class toConnection;

/**
 * Simple class for storing connection options and comparisons
 */
class toConnectionOptions
{
public:
    // must have for qmap
    toConnectionOptions()
    	: port(0)
    	{}

    toConnectionOptions(const QString &_prov,
                        const QString &_host,
                        const QString &_data,
                        const QString &_user,
                        const QString &_pass,
                        const QString &_schema,
                        const QString &_color,
                        int _port,
                        QSet<QString> _options);

    toConnectionOptions(toConnectionOptions const& other);
    toConnectionOptions& operator=(const toConnectionOptions &other);

    bool operator==(const toConnectionOptions &other);
    bool operator==(const toConnection &conn);

    QString provider, host, database, username, password, schema, color;
    quint16 port;
    QSet<QString> options;
};

Q_DECLARE_METATYPE(toConnectionOptions)

#endif

