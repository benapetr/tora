
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

#include <QtCore/QStringList>

#include "tosecuritytreeitem.h"


toSecurityTreeItem::toSecurityTreeItem(
    toSecurityTreeItem *parent,
    const QString & data,
    const QString & name,
    const QString & type,
    const QString & schema)
    : m_changed(false),
      m_checkable(false),
      m_checked(Qt::Unchecked)
{
    parentItem = parent;
    itemData = data;
    itemName = name;
    itemType = type;
    itemSchema = schema;
}

toSecurityTreeItem::~toSecurityTreeItem()
{
    qDeleteAll(childItems);
}

void toSecurityTreeItem::appendChild(toSecurityTreeItem *item)
{
    childItems.append(item);
}

toSecurityTreeItem *toSecurityTreeItem::child(int row)
{
    return childItems.value(row);
}

int toSecurityTreeItem::childCount() const
{
    return childItems.count();
}

int toSecurityTreeItem::columnCount() const
{
    return 1;//itemData.count();
}

QString toSecurityTreeItem::data() const
{
    return itemData;
}

QString toSecurityTreeItem::name() const
{
    return itemName;
}

QString toSecurityTreeItem::type() const
{
    return itemType;
}

QString toSecurityTreeItem::schema() const
{
    return itemSchema;
}

toSecurityTreeItem *toSecurityTreeItem::parent()
{
    return parentItem;
}

int toSecurityTreeItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<toSecurityTreeItem*>(this));

    return 0;
}
