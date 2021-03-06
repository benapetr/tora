
/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * TOra - An Oracle Toolkit for DBA's and developers
 * 
 * Shared/mixed copyright is held throughout files in this product
 * 
 * Portions Copyright (C) 2000-2001 Underscore AB
 * Portions Copyright (C) 2003-2005 Quest Software, Inc.
 * Portions Copyright (C) 2004-2013 Numerous Other Contributors
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
 * along with this program as the file COPYING.txt; if not, please see
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
 * 
 *      As a special exception, you have permission to link this program
 *      with the Oracle Client libraries and distribute executables, as long
 *      as you follow the requirements of the GNU GPL in regard to all of the
 *      software in the executable aside from Oracle client libraries.
 * 
 * All trademarks belong to their respective owners.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef TOSCRIPTTREEITEM_H
#define TOSCRIPTTREEITEM_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QString>

/*! \brief A leaf item for toScriptTreeModel.
It contains required data for DB objects re-creation.
Read Qt4 documentation to understand undocumented
methods in this class.
*/
class toScriptTreeItem : public QObject
{
	Q_OBJECT;

public:
    /*!
    \param parent a parent item. When it's 0, it's a root one.
    \param data main data - object name usually.
    \param type object type. See SQL statements for its content.
    \param schema a schema/owner name. See SQL statements for its content.
    */
    toScriptTreeItem(
        toScriptTreeItem *parent = 0,
        const QString & data = 0,
        const QString & type = 0,
        const QString & schema = 0
    );
    ~toScriptTreeItem();

    void appendChild(toScriptTreeItem *child);

    toScriptTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    int row() const;
    toScriptTreeItem *parent();

    //! Object name
    QString data() const;
    //! Object type
    QString type() const;
    //! Object owner/its schema name
    QString schema() const;

private:
    QList<toScriptTreeItem*> childItems;
    QString itemData;
    QString itemType;
    QString itemSchema;
    toScriptTreeItem *parentItem;
};

#endif
