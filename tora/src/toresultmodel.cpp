
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

#include "toresultmodel.h"
#include "toconf.h"
#include "utils.h"
#include "toconfiguration.h"
#include "toqvalue.h"
#include "toeventquery.h"


toResultModel::toResultModel(toEventQuery *query,
                             QObject *parent,
                             bool edit,
                             bool read)
    : QAbstractTableModel(parent)
{
    ReadableColumns = read;
    HeadersRead     = false;
    First           = true;
    Editable        = edit;
    ReadAll         = false;
    SortedOnColumn  = -1;
    
    MaxRead = MaxNumber = toConfigurationSingle::Instance().maxNumber();

    CurrRowKey = 1;

    Query = query;
    Query->setParent(this);

    connect(query,
            SIGNAL(descriptionAvailable()),
            this,
            SLOT(readHeaders()));
    connect(query,
            SIGNAL(dataAvailable()),
            this,
            SLOT(fetchMore()));
    connect(query,
            SIGNAL(error(const toConnection::exception &)),
            this,
            SLOT(queryError(const toConnection::exception &)));
    connect(query,
            SIGNAL(done()),
            this,
            SLOT(fetchMore()));

    if(Editable)
        setSupportedDragActions(Qt::CopyAction | Qt::MoveAction);
    else
        setSupportedDragActions(Qt::CopyAction);

    query->start();
}

toResultModel::toResultModel(const QString &owner,
                             const QString &type,
                             QObject *parent,
                             bool read)
    : QAbstractTableModel(parent)
{
    ReadableColumns = read;
    HeadersRead     = false;
    First           = true;
    Editable        = false;
    ReadAll         = false;
    SortedOnColumn  = -1;
    
    CurrRowKey = 1;

    Query = 0;

    setSupportedDragActions(Qt::CopyAction);

    // Manually add two columns (first one will be invisible)
    // NOTE: If this function is used to display say table columns from the cache
    // it will have to be modified to set header dynamically according to type of
    // data displayed.
    struct HeaderDesc d;
    d.name     = "#";
    d.align    = Qt::AlignRight;
    d.datatype = "INT";
    Headers.append(d);
    d.name = type + " name (cached)";
    d.align    = Qt::AlignLeft;
    d.datatype = "CHAR";
    Headers.append(d);
    HeadersRead = true;

    // Fetch list of objects from the cache
    toCache::RowList tmp = toCurrentConnection(this).Cache->getObjects(owner, type);

    beginInsertRows(QModelIndex(), 0, tmp.count());
    // Convert list of objects to appropriate type
    toResultModel::Row row;
    int counter = 1;
    for (toCache::RowList::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
        // For each row a mandatory rownumber integer should be added
    	toRowDesc rowDesc;
    	rowDesc.key=counter++;
    	rowDesc.status=EXISTED;
    	row.append(toQValue(rowDesc));
        //row.append(toQValue(counter++));
        // Copy all values of a record
        for (toCache::Row::iterator ii = (*i).begin(); ii != (*i).end(); ii++)
        {
            row.append((*ii).toString());
        }
        Rows.append(row);
        row.clear();
    }
    endInsertRows();
}

toResultModel::~toResultModel()
{
    cleanup();
}


void toResultModel::cleanup()
{
    if (Query)
    {
        disconnect(Query, 0, this, 0);

        Query->stop();
        delete Query;
        emit done();
    }

    Query = 0;
}


void toResultModel::queryError(const toConnection::exception &err)
{
    if(First)
    {
        emit firstResult(err, true);
        First = !First;
        toStatusMessage(err);
        cleanup();
    }
}


void toResultModel::readAll()
{
    ReadAll = true;
    if(Query)
        Query->readAll();
}


void toResultModel::readData()
{
    if (!Query)
    {
        cleanup();
        return;
    }

    try
    {
        // must check for errors
        Query->eof();

        int cols = Headers.size();
        if(cols < 1)
            return;

        // don't actually modify any data until we can call
        // beginInsertRows(). but to do that, we have to know how many
        // records we're going to add.
        RowList tmp;
        int     current = Rows.size();

        while(Query->hasMore() &&
                (MaxNumber < 0 || MaxNumber > current))
        {
            Row row;

            // The number column (rowKey). should never change
            toRowDesc rowDesc;
            rowDesc.key=CurrRowKey++;
            rowDesc.status=EXISTED;
            row.append(toQValue(rowDesc));
            //row.append(toQValue(CurrRowKey++));

            for (int j = 1; (j < cols || j == 0) && Query->hasMore(); j++)
                row.append(Query->readValue());

            tmp.append(row);
            current++;
        }

        // if we read some data, then go ahead and insert them now.
        if (tmp.size() > 0)
        {
            beginInsertRows(QModelIndex(), Rows.size(), current - 1);
            Rows << tmp;
            endInsertRows();
        }

        // not really first, but just be sure to emit before done()
        // must be emitted even if there's no data....
        if (First)
        {
            if (tmp.size() > 0 || !Query || Query->eof())
            {
                First = !First;

                // need to reset view(s) since we have to poll for data
                reset();
                if(Query && Query->rowsProcessed() > 0)
                {
                    emit firstResult(QString::number(Query->rowsProcessed()) +
                                     (Query->rowsProcessed() == 1 ?
                                      tr(" row processed") :
                                      tr(" rows processed")),
                                     false);
                }
                else
                    emit firstResult(tr("Statement executed"), false);
            }
        }

        if (!Query)
            return;
        if (!Query->hasMore() && Query->eof())
        {
            cleanup();
            return;
        }
    }
    catch (const toConnection::exception &str)
    {
        if (First)
        {
            First = !First;
            emit firstResult(str, true);
        }
        toStatusMessage(str);
        cleanup();
        return;
    }
    catch (const QString &str)
    {
        if (First)
        {
            First = !First;
            emit firstResult(str, true);
        }
        toStatusMessage(str);
        cleanup();
        return;
    }
}

int toResultModel::addRow(QModelIndex ind, bool duplicate)
{
    if (!Editable)
        return -1;

    int newRowPos;
    if (ind.isValid())
        newRowPos = ind.row() + 1; // new row is inserted right after the current one
    else
    {
        if (!duplicate || Rows.size() > 0)
            newRowPos = Rows.size() + 1; // new row is appended at the end
        else
            return -1; // unable to duplicate a record if there are no records
    }
    beginInsertRows(QModelIndex(), newRowPos, newRowPos);

    Row row;
    toRowDesc rowDesc;
    rowDesc.key= CurrRowKey++;
    rowDesc.status=ADDED;

    if (duplicate)
    {
        // Create a duplicate of current row
        row = Rows[ind.row()];
        // Reset a 0'th column
        row[0]=rowDesc;
    }
    else
    {
        // Create a new empty row
        row.append(toQValue(rowDesc));

        // null out the rest of the row
        int cols = Headers.size();
        for (int j = 1; j < cols; j++)
            row.append(toQValue());
    }

    Rows.insert(newRowPos, row);
    endInsertRows();
    emit rowAdded(row);
    return newRowPos;
} // addRow


void toResultModel::deleteRow(QModelIndex index)
{
    if (!Editable)
        return;

    if (!index.isValid() || index.row() >= Rows.size())
        return;

    Row deleted = Rows[index.row()];
    toRowDesc rowDesc=deleted[0].getRowDesc();

    if(rowDesc.status==REMOVED)
    {
    	//Make sure removed row can't be removed twice
    	return;
    }else if(rowDesc.status==ADDED)
    {
    	//Newly added record can be removed regularly
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    	Rows.takeAt(index.row());
    endRemoveRows();
    }else //Existed and Modified
    {
    	rowDesc.status=REMOVED;
    	Rows[index.row()][0]=toQValue(rowDesc);
    }
    emit rowDeleted(deleted);
}

void toResultModel::clearStatus()
{
	// Go through all records and set their status to be existed
	for(QList<Row>::iterator ite=Rows.begin();ite!=Rows.end();ite++)
	{
		toRowDesc rowDesc = ite->at(0).getRowDesc();
		if(rowDesc.status==REMOVED)
		{
			ite=Rows.erase(ite);
		}
		else if(rowDesc.status!=EXISTED)
		{
			rowDesc.status=EXISTED;
			(*ite)[0]=toQValue(rowDesc);
		}
	}
	emit headerDataChanged(Qt::Vertical,0,Rows.size()-1);
}


QStringList toResultModel::mimeTypes() const
{
    QStringList types;
    types << "text/plain";
    types << "application/vnd.tomodel.list";
    return types;
}


QMimeData* toResultModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData   *mimeData = new QMimeData();
    QByteArray   encodedData;
    QDataStream  stream(&encodedData, QIODevice::WriteOnly);
    QByteArray   stringData;
    QDataStream  ss(&stringData, QIODevice::WriteOnly);

    int         valid = 0;
    QModelIndex validIndex;
    QString     text;

    // qt sends list by column (all indexes from column 1, then column
    // column2). Need to figure out the number of columns and
    // rows. It'd be awesome if we could get the selection but there's
    // no good way to do that from the model.
    int rows       = 0;
    int columns    = 0;
    int currentRow = -1;
    int currentCol = -1;

    foreach (QModelIndex index, indexes)
    {
        if (index.isValid())
        {
            if(index.row() > currentRow)
            {
                currentRow = index.row();
                rows++;
            }

            if(currentCol != index.column())
            {
                currentCol = index.column();
                columns++;
            }

            valid++;
            validIndex = index;
            text = data(index, Qt::DisplayRole).toString();
            ss << text;
        }
    }

    if(valid < 1)
        return 0;

    if(valid == 1)
    {
        mimeData->setText(text);

        // set row and column in data so we can try to preserve the
        // columns, if needed

        QByteArray sourceData;
        QDataStream sourceStream(&sourceData, QIODevice::WriteOnly);
        sourceStream << validIndex.row();
        sourceStream << validIndex.column();
        mimeData->setData("application/vnd.int.list", sourceData);
    }
    else
    {
        // serialize selection shape
        stream << rows;
        stream << columns;
        mimeData->setData("application/vnd.tomodel.list", encodedData + stringData);
    }

    return mimeData;
}


bool toResultModel::dropMimeData(const QMimeData *data,
                                 Qt::DropAction action,
                                 int row,
                                 int column,
                                 const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;
    if (!Editable)
        return false;
    if (column == 0)
        return false;           // can't change row number

    if(row < 0 || column < 0)
    {
        // friggen qt expects me to figure it out.  you'd think -1
        // wouldn't happen often but you'd be wrong.
        row = parent.row();
        column = parent.column();
    }

    if(row < 0)
    {
        row = addRow();
        if(row < 0)
            return false;
    }

    if (data->hasText())
    {
        // can't do anything with an item without a valid column
        if(column < 0)
        {
            if (data->hasFormat("application/vnd.int.list"))
            {
                QByteArray source = data->data("application/vnd.int.list");
                QDataStream stream(&source, QIODevice::ReadOnly);
                int sourceRow = 0;
                int sourceCol = 0;
                stream >> sourceRow;
                stream >> sourceCol;

                if(sourceCol > 0)
                    column = sourceCol;
                else
                    return false;
            }
            else
                return false;
        }

        QModelIndex ind = index(row, column);
        setData(ind, QVariant(data->text()));
        return true;
    }
    else if (data->hasFormat("application/vnd.tomodel.list"))
    {
        QByteArray source = data->data("application/vnd.tomodel.list");
        QDataStream stream(&source, QIODevice::ReadOnly);

        if (column < 0)
            column = 1;

        int rows;
        int columns;
        stream >> rows;
        stream >> columns;

        // count of rows down we've gone so far
        int counter = 0;
        while (!stream.atEnd() && column <= columnCount())
        {
            QString text;
            stream >> text;

            QModelIndex ind = index(row, column);
            setData(ind, QVariant(text));

            row++;
            counter++;
            if (counter >= rows)
            {
                row -= counter;
                counter = 0;
                column++;
            }

            if (!stream.atEnd() && row >= rowCount())
                row = addRow();

            if (column >= columnCount())
                return true;    // drop data past end of columns
        }

        return true;
    }

    return false;
}


Qt::DropActions toResultModel::supportedDropActions() const
{
    if(Editable)
        return Qt::CopyAction | Qt::MoveAction;
    return Qt::IgnoreAction;
}


int toResultModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return Rows.size();
}


/**
 * Returns the data stored under the given role for the item
 * referred to by the index.
 */
QVariant toResultModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() > Rows.size() - 1 || index.column() > Headers.size() - 1)
        return QVariant();

    toQValue const &data = Rows.at(index.row()).at(index.column());

    toRowDesc rowDesc=Rows[index.row()][0].getRowDesc();
    QFont fontRet;

    switch(role)
    {
    case Qt::ToolTipRole:
        if (data.isNull())
            return data.toQVariant();
        if (data.isComplexType())
        {
            toQValue::complexType *i = data.toQVariant().value<toQValue::complexType*>();
            return QVariant(i->tooltipData());
        }
        return data.toQVariant();
    case Qt::EditRole:
        if (data.isComplexType())
        {
            toQValue::complexType *i = data.toQVariant().value<toQValue::complexType*>();
            return QVariant(i->editData());
        }
        return QVariant(data.editData());
    case Qt::DisplayRole:
        if (data.isComplexType())
        {
            toQValue::complexType *i = data.toQVariant().value<toQValue::complexType*>();
            return QVariant(i->displayData());
        }
        if (index.column() == 0)
            return index.row() + 1;
        return QVariant(data.displayData());
    case Qt::BackgroundRole:
        if (data.isNull() && toConfigurationSingle::Instance().indicateEmpty())
            return QVariant(QColor(toConfigurationSingle::Instance().indicateEmptyColor()));
        if (index.column() == 0) {
            return QPalette().color(QPalette::Window);
        }
        return QVariant();
    case Qt::TextAlignmentRole:
        return (int) Headers.at(index.column()).align;
    case Qt::UserRole:
        return data.toQVariant();
    case Qt::FontRole:
    	if (rowDesc.status==REMOVED)
    		fontRet.setStrikeOut(true);
    	else if (rowDesc.status==ADDED)
    		fontRet.setBold(true);
    	else if (rowDesc.status==MODIFIED)
    		fontRet.setItalic(true);
    	return fontRet;
    default:
        return QVariant();
    }
    return QVariant();
}


bool toResultModel::setData(const QModelIndex &index,
                            const QVariant &_value,
                            int role)
{
    if (!Editable)
        return false;

    if (role != Qt::EditRole)
        return false;

    if (index.column() == 0)
        return false;           // can't change number column

    if (index.row() > Rows.size() - 1 || index.column() > Headers.size() - 1)
        return false;

    Row &row = Rows[index.row()];
    Row oldRow = row;           // keep old version
    toQValue newValue = toQValue::fromVariant(_value);

    toRowDesc rowDesc = Rows.at(index.row())[0].getRowDesc();
    if (rowDesc.status == EXISTED && !(row[index.column()] == newValue))
    {
    	// leave row that's added as in status added
    	rowDesc.status = MODIFIED;
    	Rows[index.row()][0] = toQValue(rowDesc);
    }
    row[index.column()] = newValue;

    // for writing to the database
    emit columnChanged(index, newValue, oldRow);

    // for the view
    emit dataChanged(index, index);

    return true;
}


QVariant toResultModel::data(int row, int column) const
{
    QModelIndex ind = index(row, column);
    return data(ind, Qt::EditRole);
}


QVariant toResultModel::data(int row, QString column) const
{
    int c;
    for (c = 0; c < Headers.size(); c++)
    {
        if (Headers[c].name == column)
            break;
    }

    if (c < Headers.size())
        return data(row, c);
    return QVariant();
}


/**
 * Returns the data for the given role and section in the header
 * with the specified orientation.
 *
 */
QVariant toResultModel::headerData(int section,
                                   Qt::Orientation orientation,
                                   int role) const
{
    /*if (role != Qt::DisplayRole)
        return QVariant();*/

    if (orientation == Qt::Horizontal)
    {
        if (!HeadersRead)
            return QVariant();

        if (section > Headers.size() - 1)
            return QVariant();

        if (role == Qt::DisplayRole)
        return Headers[section].name;
        else
        	return QVariant();
    }

    if (orientation == Qt::Vertical)
    {
    	if (role == Qt::DisplayRole)
        return section + 1;
    	else if (role == Qt::ForegroundRole)
    	{
    		if(section<0 || section>Rows.size())
    			return QVariant();
			toRowDesc rowDesc = Rows[section][0].getRowDesc();
			switch (rowDesc.status) {
			case REMOVED:
				return QBrush(Qt::red);
			case ADDED:
				return QBrush(Qt::green);
			case MODIFIED:
				return QBrush(Qt::blue);
			case EXISTED:
			default:
				return QVariant();
			}
		}
    }

    return QVariant();
}

bool toResultModel::setHeaderData(int section,
                                  Qt::Orientation orientation,
                                  const QVariant & value,
                                  int role)
{
    if (role != Qt::DisplayRole)
        return false;

    if (orientation != Qt::Horizontal)
        return false;

    Headers[section].name = value.toString();
    return true;
}

void toResultModel::readHeaders()
{
    if (HeadersRead)
        return;
    if (!Query)
        return;

    // always add the number column. this makes adjusting for it in
    // the row data easier. it is not always displayed.
    struct HeaderDesc d;
    d.name     = "#";
    d.align    = Qt::AlignRight;
    d.datatype = "INT";
    Headers.append(d);

    toQDescList desc = Query->describe();
    for (toQDescList::iterator i = desc.begin(); i != desc.end(); i++)
    {
        struct HeaderDesc d;

        d.name = (*i).Name;
        if (ReadableColumns)
            toReadableColumn(d.name);

        d.datatype = (*i).Datatype;

        d.nullAllowed = (*i).Null;

        // AlignTop is used here to prevent Vert. centering of
        // data if there are more rows in the cell (CLOB etc.).
        // AlignVCenter makes it unreadable in this case. -- Petr Vanek
        if ((*i).AlignRight)
            d.align = Qt::AlignRight | Qt::AlignTop; //Qt::AlignVCenter;
        else
            d.align = Qt::AlignLeft | Qt::AlignTop; //Qt::AlignVCenter;

        Headers.append(d);
    }

    HeadersRead = true;
}


/**
 * Returns the number of columns for the children of the given
 * parent. When the parent is valid it means that rowCount is
 * returning the number of children of parent.
 */
int toResultModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return Headers.size();
}


bool toResultModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    // sometimes the view calls this before the query has even
    // run.
    if (First)
        return false;

    try
    {
        return Query && Query->hasMore();
    }
    catch (...)
    {
        // will catch later
        ;
    }

    return false;
}


void toResultModel::fetchMore()
{
    if(ReadAll)
    {
        MaxNumber = -1;
        readData();
    }
    else if(Rows.size() < MaxNumber)
    {
        QModelIndex ind;
        fetchMore(ind);
    }
}


void toResultModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    // sometimes the view calls this before the query has even
    // run. don't actually increase max until we've hit it.
    if (MaxNumber < 0 || MaxNumber <= Rows.size())
        MaxNumber += MaxRead;

    readData();
}


Qt::ItemFlags toResultModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);
    Qt::ItemFlags fl = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;

    if (index.column() == 0)
        return fl;              // row number column

    if (!index.isValid() || index.row() >= Rows.size())
    {
        if(Editable)
            return Qt::ItemIsDropEnabled | defaultFlags;
        else
            return defaultFlags;
    }

    toQValue const &data = Rows.at(index.row()).at(index.column());
    toRowDesc rowDesc = Rows.at(index.row()).at(0).getRowDesc();
    if (data.isComplexType())
    {
        return ( defaultFlags | fl ) & ~Qt::ItemIsEditable;
    }

    if (Editable)
        fl |= defaultFlags | Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
    else
    	fl |= defaultFlags;

    //Check the status of current record
    if (rowDesc.status==REMOVED)
    	fl &= ~Qt::ItemIsEditable;
    return fl;
}


void toResultModel::setAlignment(int col, Qt::AlignmentFlag fl)
{
    Headers[col].align = fl;
}


void toResultModel::sort(int column, Qt::SortOrder order)
{
    if (column > Headers.size() - 1)
        return;

    // Do nothing if data was already sorted in the requested way
    if (SortedOnColumn == column &&
        SortedOrder == order)
        return;

    Rows = mergesort(Rows, column, order);
    SortedOnColumn = column;
    SortedOrder = order;
    emit dataChanged(createIndex(0, 0),
                     createIndex(rowCount(), columnCount()));
}


toResultModel::RowList toResultModel::mergesort(RowList &rows,
        int column,
        Qt::SortOrder order)
{
    if(rows.size() <= 1)
        return rows;

    RowList left, right;

    int middle = (int) (rows.size() / 2);
    left = rows.mid(0, middle);
    right = rows.mid(middle);

    left = mergesort(left, column, order);
    right = mergesort(right, column, order);

    return merge(left, right, column, order);
}


toResultModel::RowList toResultModel::merge(RowList &left,
        RowList &right,
        int column,
        Qt::SortOrder order)
{
    RowList result;

    while(left.size() > 0 && right.size() > 0)
    {
        if((order == Qt::AscendingOrder && left.at(0).at(column) <= right.at(0).at(column)) ||
                (order == Qt::DescendingOrder && left.at(0).at(column) >= right.at(0).at(column)))
            result.append(left.takeAt(0));
        else
            result.append(right.takeAt(0));
    }

    if(left.size() > 0)
        result << left;
    if(right.size() > 0)
        result << right;
    return result;
}

toResultModel::RowList &toResultModel::getRawData(void)
{
    return Rows;
}
