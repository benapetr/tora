
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

#include "core/tolistviewformattercsv.h"
#include "core/tolistviewformatterfactory.h"
#include "core/tolistviewformatteridentifier.h"
#include "core/toresultview.h"
#include "core/toresultmodel.h"

#include <QtCore/QRegExp>

#include <iostream>
#include <vector>

namespace
{
toListViewFormatter* createCSV()
{
    return new toListViewFormatterCSV();
}
const bool registered = toListViewFormatterFactory::Instance().Register(
                            toListViewFormatterIdentifier::CSV, createCSV);
}


toListViewFormatterCSV::toListViewFormatterCSV() : toListViewFormatter()
{
}

toListViewFormatterCSV::~toListViewFormatterCSV()
{
}

QString toListViewFormatterCSV::QuoteString(const QString &str)
{
    static QRegExp quote(QString::fromLatin1("\""));
    QString t = str;
    t.replace(quote, QString::fromLatin1("\"\""));
    return t;
}

QString toListViewFormatterCSV::getFormattedString(toExportSettings &settings,
        //const toResultModel *model);
        const QAbstractItemModel * model)
{
    int     columns   = model->columnCount();
    int     rows      = model->rowCount();
    QString separator = settings.separator;
    QString delimiter = settings.delimiter;

    QString output;
    QString indent;

    QVector<int> rlist = selectedRows(settings.selected);
    QVector<int> clist = selectedColumns(settings.selected);

    if (settings.columnsHeader)
    {
        for (int j = (settings.rowsHeader ? 0 : 1); j < columns; j++)
        {
            if (settings.columnsExport == toExportSettings::ColumnsSelected && !clist.contains(j))
                continue;
            output += QString::fromLatin1("%1%2%3%4").
                      arg(delimiter).
                      arg(QuoteString(model->headerData(
                                          j,
                                          Qt::Horizontal,
                                          Qt::DisplayRole).toString())).
                      arg(delimiter).
                      arg(separator);
        }
        if (output.length() > 0)
            output = output.left(output.length() - separator.length());

        endLine(output);
    }

    QModelIndex mi;
    for (int row = 0; row < rows; row++)
    {
        if (settings.rowsExport == toExportSettings::RowsSelected && !rlist.contains(row))
            continue;

        QString line;
        for (int i = 0; i < columns; i++)
        {
            if (settings.columnsExport == toExportSettings::ColumnsSelected && !clist.contains(i))
                continue;
            if (!settings.rowsHeader && i == 0)
                continue;

            mi = model->index(row, i);
            QString text = model->data(mi, Qt::EditRole).toString();
            line += indent;

            line += QString::fromLatin1("%1%2%3%4").
                    arg(delimiter,
                        QuoteString(text),
                        delimiter,
                        separator);
        }

        line = line.left(line.length() - separator.length());
        output += line;
        endLine(output);
    }

    return output;
}
