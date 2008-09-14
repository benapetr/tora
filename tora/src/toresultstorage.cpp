/* BEGIN_COMMON_COPYRIGHT_HEADER 
* END_COMMON_COPYRIGHT_HEADER */

#include "utils.h"

#include "toconf.h"
#include "toconnection.h"
#include "tonoblockquery.h"
#include "toresulttableview.h"
#include "toresultstorage.h"
#include "tosql.h"
#include "totool.h"

#include <qpainter.h>
//Added by qt3to4:
#include <QList>
#include <QPaintEvent>
#include <QProgressDialog>

class toResultStorageItem : public toResultViewItem
{
    bool AvailableGraph;
public:
    toResultStorageItem(bool available, toTreeWidget *parent, toTreeWidgetItem *after,
                        const QString &buf = QString::null)
            : toResultViewItem(parent, after, buf), AvailableGraph(available)
    { }
    toResultStorageItem(bool available, toTreeWidgetItem *parent, toTreeWidgetItem *after,
                        const QString &buf = QString::null)
            : toResultViewItem(parent, after, buf), AvailableGraph(available)
    { }
    virtual void paintCell(QPainter * p, const QColorGroup & cg, int column, int width, int align)
    {
#if 0                           // disabled, wrong override
        if (column == 8)
        {
            QString ct = text(column);
            if (ct.isEmpty())
            {
                toTreeWidgetItem::paintCell(p, cg, column, width, align);
                return ;
            }
            ct = ct.left(ct.length() - 1); // Strip last %
            double total = text(7).toDouble();
            double user = text(5).toDouble();
            double free = text(6).toDouble();
            if (total < user || !AvailableGraph)
                total = user;

            QString t;

            user /= total;
            free /= total;
            if (AvailableGraph)
                t.sprintf("%0.1f / %0.1f / %0.1f %%", (user - free)*100, free*100, (1 - user)*100);
            else
                t.sprintf("%0.1f %%", free*100);
            p->fillRect(0, 0, int((user - free)*width), height(),
                        QBrush(Qt::red));
            p->fillRect(int((user - free)*width), 0, int(user*width), height(),
                        QBrush(Qt::blue));
            if (AvailableGraph)
                p->fillRect(int(user*width), 0, width, height(),
                            QBrush(Qt::green));

            QPen pen(Qt::white);
            p->setPen(pen);
            p->drawText(0, 0, width, height(), Qt::AlignCenter, t);
        }
        else if (column == 9)
        {
            QString ct = text(column);
            if (ct.isEmpty())
            {
                toTreeWidgetItem::paintCell(p, cg, column, width, align);
                return ;
            }
            ct = ct.left(ct.length() - 1); // Strip last %
            double val = ct.toDouble();

            p->fillRect(0, 0, int(val*width / 100), height(), QBrush(Qt::blue));
            p->fillRect(int(val*width / 100), 0, width, height(),
                        QBrush(isSelected() ? cg.highlight() : cg.base()));

            QPen pen(isSelected() ? cg.highlightedText() : cg.foreground());
            p->setPen(pen);
            p->drawText(0, 0, width, height(), Qt::AlignCenter, text(column));
        }
        else
        {
            toTreeWidgetItem::paintCell(p, cg, column, width, align);
        }
#endif
    }
};

bool toResultStorage::canHandle(toConnection &conn)
{
    return toIsOracle(conn);
}

toResultStorage::toResultStorage(bool available, QWidget *parent, const char *name)
        : toResultView(false, false, parent, name), AvailableGraph(available)
{
    Unit = toConfigurationSingle::Instance().sizeUnit();
    setAllColumnsShowFocus(true);
    setSorting(0);
    setRootIsDecorated(true);
    addColumn(tr("Name"));
    addColumn(tr("Status"));
    addColumn(tr("Information"));
    addColumn(tr("Contents"));
    addColumn(tr("Logging"));
    addColumn(tr("Size (%1)").arg(Unit));
    addColumn(tr("Free (%1)").arg(Unit));
    addColumn(tr("Autoextend (%1)").arg(Unit));
    if (available)
        addColumn(tr("Used/Free/Autoextend"));
    else
        addColumn(tr("Available"));
    addColumn(tr("Coalesced"));
    addColumn(tr("Max free (%1)").arg(Unit));
    addColumn(tr("Free fragments"));
    setSQLName(tr("toResultStorage"));

    setColumnAlignment(5, Qt::AlignRight);
    setColumnAlignment(6, Qt::AlignRight);
    setColumnAlignment(7, Qt::AlignRight);
    setColumnAlignment(8, Qt::AlignCenter);
    setColumnAlignment(9, Qt::AlignCenter);
    setColumnAlignment(10, Qt::AlignRight);
    setColumnAlignment(11, Qt::AlignRight);

    ShowCoalesced = false;
    OnlyFiles = false;

    Tablespaces = Files = NULL;

    connect(&Poll, SIGNAL(timeout()), this, SLOT(poll()));
}

toResultStorage::~toResultStorage()
{
    delete Tablespaces;
    delete Files;
}

static toSQL SQLShowCoalesced("toResultStorage:ShowCoalesced",
                              "SELECT d.tablespace_name,\n"
                              "       d.status,\n"
                              "       d.extent_management,\n"
                              "       d.contents,\n"
                              "       d.logging,\n"
                              "       TO_CHAR(ROUND(NVL(a.tbs_size,0) / b.unit,2)),\n"
                              "       TO_CHAR(ROUND(NVL(f.free_size,0) / b.unit,2)),\n"
                              "       TO_CHAR(ROUND(NVL(a.autoextend_size,0) / b.unit,2)),\n"
                              "       '-',\n"
                              "       TO_CHAR(ROUND(f.percent_extents_coalesced,1))||'%',\n"
                              "       '-',\n"
                              "       TO_CHAR(f.free_extents)\n"
                              "  FROM (SELECT :unt<int> unit FROM sys.dual) b,\n"
                              "       sys.dba_tablespaces d\n"
                              "  LEFT JOIN (SELECT tablespace_name,\n"
                              "                    SUM(bytes) tbs_size,\n"
                              "       SUM(DECODE(autoextensible,'YES',maxbytes,bytes)) autoextend_size\n"
                              "               FROM sys.dba_data_files\n"
                              "        GROUP BY tablespace_name) a\n"
                              "       ON d.tablespace_name = a.tablespace_name\n"
                              "  LEFT JOIN (SELECT tablespace_name,\n"
                              "                    total_bytes free_size,\n"
                              "                    total_extents free_extents,\n"
                              "                    percent_extents_coalesced\n"
                              "               FROM sys.dba_free_space_coalesced) f\n"
                              "       ON d.tablespace_name = f.tablespace_name\n"
                              " WHERE NOT (d.extent_management like 'LOCAL' AND d.contents like 'TEMPORARY')\n"
                              " UNION ALL\n"
                              "SELECT d.tablespace_name,\n"
                              "       d.status,\n"
                              "       d.extent_management,\n"
                              "       d.contents,\n"
                              "       d.logging,\n"
                              "       TO_CHAR(ROUND(NVL(a.tbs_size,0) / b.unit,2)),\n"
                              "       TO_CHAR(ROUND(NVL(f.free_size,0) / b.unit,2)),\n"
                              "       TO_CHAR(ROUND(NVL(a.autoextend_size,0) / b.unit,2)),\n"
                              "       '-',\n"
                              "       '-',\n"
                              "       TO_CHAR(ROUND(NVL(f.max_free,0) / b.unit,2)),\n"
                              "       TO_CHAR(f.total_extents)\n"
                              "  FROM (SELECT :unt<int> unit FROM sys.dual) b,\n"
                              "       sys.dba_tablespaces d\n"
                              "  LEFT JOIN (SELECT tablespace_name,\n"
                              "                    SUM(bytes) tbs_size,\n"
                              "                    SUM(DECODE(autoextensible,'YES',maxbytes,bytes)) autoextend_size\n"
                              "               FROM sys.dba_temp_files\n"
                              "              GROUP BY tablespace_name) a\n"
                              "       ON d.tablespace_name = a.tablespace_name\n"
                              "  LEFT JOIN (SELECT tablespace_name,\n"
                              "                    SUM(bytes_cached) free_size,\n"
                              "                    COUNT(1) total_extents,\n"
                              "                    MAX(bytes_cached) max_free\n"
                              "               FROM v$temp_extent_pool\n"
                              "              GROUP BY tablespace_name) f\n"
                              "       ON d.tablespace_name = f.tablespace_name\n"
                              " WHERE d.extent_management = 'LOCAL' AND d.contents = 'TEMPORARY'\n",
                              "Display storage usage of database. This includes the coalesced columns which may make the query sluggish on some DB:s. "
                              "All columns must be present in output (Should be 12)",
                              "0801");

static toSQL SQLShowCoalesced8("toResultStorage:ShowCoalesced",
                               "SELECT d.tablespace_name,\n"
                               "       d.status,\n"
                               "       ' ',\n"
                               "       d.contents,\n"
                               "       d.logging,\n"
                               "       TO_CHAR(ROUND(NVL(a.bytes / b.unit, 0),2)),\n"
                               "       TO_CHAR(ROUND(NVL(f.bytes,0) / b.unit,2)),\n"
                               "       TO_CHAR(ROUND(NVL(a.maxbytes / b.unit, 0),2)),\n"
                               "       '-',\n"
                               "       TO_CHAR(ROUND(f.percent_extents_coalesced,1))||'%',\n"
                               "       '-',\n"
                               "       TO_CHAR(f.total_extents)\n"
                               "  FROM sys.dba_tablespaces d,\n"
                               "       (select tablespace_name, sum(bytes) bytes, sum(maxbytes) maxbytes from sys.dba_data_files group by tablespace_name) a,\n"
                               "       (select tablespace_name, total_bytes bytes, total_extents, percent_extents_coalesced from sys.dba_free_space_coalesced) f,\n"
                               "       (select :unt<int> unit from sys.dual) b\n"
                               " WHERE d.tablespace_name = a.tablespace_name(+)\n"
                               "   AND d.tablespace_name = f.tablespace_name(+)\n"
                               " ORDER BY d.tablespace_name",
                               "",
                               "0800");

static toSQL SQLShowCoalesced7("toResultStorage:ShowCoalesced",
                               "SELECT d.tablespace_name,\n"
                               "       d.status,\n"
                               "       ' ',\n"
                               "       d.contents,\n"
                               "       'N/A',\n"
                               "       TO_CHAR(ROUND(NVL(a.bytes / b.unit, 0),2)),\n"
                               "       TO_CHAR(ROUND(NVL(f.bytes,0) / b.unit,2)),\n"
                               "       '-',\n"
                               "       '-',\n"
                               "       TO_CHAR(ROUND(f.percent_extents_coalesced,1))||'%',\n"
                               "       '-',\n"
                               "       TO_CHAR(f.total_extents)\n"
                               "  FROM sys.dba_tablespaces d,\n"
                               "       (select tablespace_name, sum(bytes) bytes from sys.dba_data_files group by tablespace_name) a,\n"
                               "       (select tablespace_name, total_bytes bytes, total_extents, percent_extents_coalesced from sys.dba_free_space_coalesced) f,\n"
                               "       (select :unt<int> unit from sys.dual) b\n"
                               " WHERE d.tablespace_name = a.tablespace_name(+)\n"
                               "   AND d.tablespace_name = f.tablespace_name(+)\n"
                               " ORDER BY d.tablespace_name",
                               "",
                               "0703");

static toSQL SQLNoShowCoalesced(
    "toResultStorage:NoCoalesced",
    "SELECT d.tablespace_name,\n"
    "       d.status,\n"
    "       d.extent_management,\n"
    "       d.CONTENTS,\n"
    "       d.LOGGING,\n"
    "       TO_CHAR ( ROUND ( NVL ( s.tbs_size,\n"
    "                               0 ) / b.unit,\n"
    "                         2 ) ),\n"
    "       TO_CHAR ( ROUND ( NVL ( f.free_size,\n"
    "                               0 ) / b.unit,\n"
    "                         2 ) ),\n"
    "       TO_CHAR ( ROUND ( NVL ( s.autoextend_size,\n"
    "                               0 ) / b.unit,\n"
    "                         2 ) ),\n"
    "       '-',\n"
    "       '-',\n"
    "       TO_CHAR ( ROUND ( NVL ( f.max_free,\n"
    "                               0 ) / b.unit,\n"
    "                         2 ) ),\n"
    "       TO_CHAR ( f.free_extents )\n"
    "  FROM ( SELECT :unt<int> unit\n"
    "           FROM sys.dual ) b,\n"
    "       sys.dba_tablespaces d,\n"
    "       ( SELECT tablespace_name,\n"
    "                SUM ( bytes ) tbs_size,\n"
    "                SUM ( DECODE ( autoextensible,\n"
    "                               'YES',\n"
    "                               maxbytes,\n"
    "                               bytes ) ) autoextend_size\n"
    "           FROM sys.dba_data_files\n"
    "          GROUP BY tablespace_name\n"
    "          UNION ALL SELECT tablespace_name,\n"
    "                SUM ( bytes ) tbs_size,\n"
    "                SUM ( DECODE ( autoextensible,\n"
    "                               'YES',\n"
    "                               maxbytes,\n"
    "                               bytes ) ) autoextend_size\n"
    "           FROM sys.dba_temp_files\n"
    "          GROUP BY tablespace_name ) s,\n"
    "       ( SELECT tablespace_name,\n"
    "                SUM ( bytes ) free_size,\n"
    "                MAX ( bytes ) max_free,\n"
    "                COUNT ( 1 ) free_extents\n"
    "           FROM sys.dba_free_space\n"
    "          GROUP BY tablespace_name\n"
    "          UNION ALL SELECT tablespace_name,\n"
    "                SUM ( bytes_cached ) free_size,\n"
    "                MAX ( bytes_cached ) max_free,\n"
    "                COUNT ( 1 ) free_extents\n"
    "           FROM v$temp_extent_pool\n"
    "          GROUP BY tablespace_name ) f\n"
    " WHERE d.tablespace_name = s.tablespace_name ( + )\n"
    "   AND d.tablespace_name = f.tablespace_name ( + )",
    "Display storage usage of database. This does not include the coalesced "
    "columns which may make the query sluggish on some DB:s. "
    "All columns must be present in output (Should be 12)",
    "0801");

static toSQL SQLNoShowCoalesced8("toResultStorage:NoCoalesced",
                                 "SELECT d.tablespace_name,\n"
                                 "       d.status,\n"
                                 "       ' ',\n"
                                 "       d.contents,\n"
                                 "       d.logging,\n"
                                 "       TO_CHAR(ROUND(NVL(a.bytes,0) / b.unit,2)),\n"
                                 "       TO_CHAR(ROUND(NVL(f.bytes,0) / b.unit,2)),\n"
                                 "       TO_CHAR(ROUND(NVL(a.maxbytes,0) / b.unit,2)),\n"
                                 "       '-',\n"
                                 "       '-',\n"
                                 "       TO_CHAR(ROUND(NVL(f.maxbytes,0) / b.unit,2)),\n"
                                 "       TO_CHAR(f.total_extents)\n"
                                 "  FROM sys.dba_tablespaces d,\n"
                                 "       (select tablespace_name, sum(bytes) bytes, sum(maxbytes) maxbytes from sys.dba_data_files group by tablespace_name) a,\n"
                                 "       (select tablespace_name, NVL(sum(bytes),0) bytes, count(1) total_extents, NVL(max(bytes),0) maxbytes from sys.dba_free_space group by tablespace_name) f,\n"
                                 "       (select :unt<int> unit from sys.dual) b\n"
                                 " WHERE d.tablespace_name = a.tablespace_name(+)\n"
                                 "   AND d.tablespace_name = f.tablespace_name(+)\n"
                                 " ORDER BY d.tablespace_name",
                                 "",
                                 "0800");

static toSQL SQLNoShowCoalesced7("toResultStorage:NoCoalesced",
                                 "SELECT d.tablespace_name,\n"
                                 "       d.status,\n"
                                 "       ' ',\n"
                                 "       d.contents,\n"
                                 "       'N/A',\n"
                                 "       TO_CHAR(ROUND(NVL(a.bytes,0) / b.unit,2)),\n"
                                 "       TO_CHAR(ROUND(NVL(f.bytes,0) / b.unit,2)),\n"
                                 "       '-',\n"
                                 "       '-',\n"
                                 "       '-',\n"
                                 "       TO_CHAR(ROUND(NVL(f.maxbytes,0) / b.unit,2)),\n"
                                 "       TO_CHAR(f.total_extents)\n"
                                 "  FROM sys.dba_tablespaces d,\n"
                                 "       (select tablespace_name, sum(bytes) bytes from sys.dba_data_files group by tablespace_name) a,\n"
                                 "       (select tablespace_name, NVL(sum(bytes),0) bytes, count(1) total_extents, NVL(max(bytes),0) maxbytes from sys.dba_free_space group by tablespace_name) f,\n"
                                 "       (select :unt<int> unit from sys.dual) b\n"
                                 " WHERE d.tablespace_name = a.tablespace_name(+)\n"
                                 "   AND d.tablespace_name = f.tablespace_name(+)\n"
                                 " ORDER BY d.tablespace_name",
                                 "",
                                 "0703");

static toSQL SQLDatafile(
    "toResultStorage:Datafile",
    "SELECT d.tablespace_name,\n"
    "       v.name,\n"
    "       v.status,\n"
    "       v.enabled,\n"
    "       ' ',\n"
    "       ' ',\n"
    "       to_char ( round ( d.bytes / b.unit,\n"
    "                         2 ) ),\n"
    "       to_char ( round ( s.bytes / b.unit,\n"
    "                         2 ) ),\n"
    "       to_char ( round ( d.maxbytes / b.unit,\n"
    "                         2 ) ),\n"
    "       '-',\n"
    "       ' ',\n"
    "       to_char ( round ( s.maxbytes / b.unit,\n"
    "                         2 ) ),\n"
    "       to_char ( s.num ),\n"
    "       NULL,\n"            // Used to fill in tablespace name
    "       v.file#\n"          // Needed by toStorage to work
    "  FROM ( SELECT :unt<int> unit\n"
    "           FROM sys.dual ) b,\n"
    "       sys.dba_data_files d\n"
    "       JOIN v$datafile v\n"
    "       ON      d.file_id = v.file#\n"
    "       LEFT JOIN\n"
    "       ( SELECT file_id,\n"
    "                NVL ( SUM ( bytes ),\n"
    "                      0 ) bytes,\n"
    "                COUNT ( 1 ) num,\n"
    "                NVL ( MAX ( bytes ),\n"
    "                      0 ) maxbytes\n"
    "           FROM sys.dba_free_space\n"
    "          GROUP BY file_id ) s\n"
    "       ON      s.file_id = d.file_id\n"
    " UNION ALL SELECT d.tablespace_name,\n"
    "       v.name,\n"
    "       v.status,\n"
    "       v.enabled,\n"
    "       ' ',\n"
    "       ' ',\n"
    "       to_char ( round ( d.bytes / b.unit,\n"
    "                         2 ) ),\n"
    "       to_char ( round ( ( d.user_bytes - t.bytes_cached ) / b.unit,\n"
    "                         2 ) ),\n"
    "       to_char ( round ( d.maxbytes / b.unit,\n"
    "                         2 ) ),\n"
    "       '-',\n"
    "       ' ',\n"
    "       ' ',\n"
    "       '1',\n"
    "       NULL,\n"
    "       v.file#\n"
    "  FROM ( SELECT :unt<int> unit\n"
    "           FROM sys.dual ) b,\n"
    "       sys.dba_temp_files d\n"
    "       JOIN v$tempfile v\n"
    "       ON      d.file_id = v.file#\n"
    "       LEFT JOIN v$temp_extent_pool t\n"
    "       ON      t.file_id = d.file_id",
    "Display information about a datafile in a tablespace. "
    "All columns must be present in the output (Should be 14)",
    "0900");

static toSQL SQLDatafile8(
"toResultStorage:Datafile",
    "SELECT d.tablespace_name,\n"
    "       v.name,\n"
    "       v.status,\n"
    "       v.enabled,\n"
    "       ' ',\n"
    "       ' ',\n"
    "       to_char ( round ( d.bytes / b.unit,\n"
    "                         2 ) ),\n"
    "       to_char ( round ( s.bytes / b.unit,\n"
    "                         2 ) ),\n"
    "       to_char ( round ( d.maxbytes / b.unit,\n"
    "                         2 ) ),\n"
    "       '-',\n"
    "       ' ',\n"
    "       to_char ( round ( s.maxbytes / b.unit,\n"
    "                         2 ) ),\n"
    "       to_char ( s.num ),\n"
    "       NULL,\n"            // Used to fill in tablespace name
    "       v.file#\n"          // Needed by toStorage to work
    "  FROM ( SELECT :unt<int> unit\n"
    "           FROM sys.dual ) b,\n"
    "       sys.dba_data_files d,\n"
    "       v$datafile v,\n"
    "       ( SELECT file_id,\n"
    "                NVL ( SUM ( bytes ),\n"
    "                      0 ) bytes,\n"
    "                COUNT ( 1 ) num,\n"
    "                NVL ( MAX ( bytes ),\n"
    "                      0 ) maxbytes\n"
    "           FROM sys.dba_free_space\n"
    "          GROUP BY file_id ) s\n"
    " WHERE d.file_id = v.file#\n"
    "   AND s.file_id = d.file_id ( + )\n"
    " UNION ALL SELECT d.tablespace_name,\n"
    "       v.name,\n"
    "       v.status,\n"
    "       v.enabled,\n"
    "       ' ',\n"
    "       ' ',\n"
    "       to_char ( round ( d.bytes / b.unit,\n"
    "                         2 ) ),\n"
    "       to_char ( round ( ( d.user_bytes - t.bytes_cached ) / b.unit,\n"
    "                         2 ) ),\n"
    "       to_char ( round ( d.maxbytes / b.unit,\n"
    "                         2 ) ),\n"
    "       '-',\n"
    "       ' ',\n"
    "       ' ',\n"
    "       '1',\n"
    "       NULL,\n"
    "       v.file#\n"
    "  FROM ( SELECT :unt<int> unit\n"
    "           FROM sys.dual ) b,\n"
    "       sys.dba_temp_files d,\n"
    "       v$tempfile v,\n"
    "       v$temp_extent_pool t\n"
    " WHERE d.file_id = v.file#\n"
    "   AND t.file_id = d.file_id ( + )",
    "",
        "0800");

static toSQL SQLDatafile7("toResultStorage:Datafile",
                          "SELECT  d.tablespace_name,\n"
                          "    v.name,\n"
                          "    v.status,\n"
                          "    v.enabled,\n"
                          "    ' ',\n"
                          "    ' ',\n"
                          "        to_char(round(d.bytes/b.unit,2)),\n"
                          "        to_char(round(s.bytes/b.unit,2)),\n"
                          "        '-',\n"
                          "        '-',\n"
                          "    ' ',\n"
                          "        to_char(round(s.maxbytes/b.unit,2)),\n"
                          "    to_char(s.num),\n"
                          "        NULL,\n"
                          "        v.file#\n"
                          "  FROM  sys.dba_data_files d,\n"
                          "    v$datafile v,\n"
                          "    (SELECT file_id, NVL(SUM(bytes),0) bytes, COUNT(1) num,NVL(MAX(bytes),0) maxbytes FROM sys.dba_free_space  GROUP BY file_id) s,\n"
                          "        (select :unt<int> unit from sys.dual) b\n"
                          " WHERE  (s.file_id (+)= d.file_id)\n"
                          "   AND  (d.file_name = v.name)",
                          "",
                          "0703");

#define FILECOLUMNS 14
#define COLUMNS (FILECOLUMNS-2)

void toResultStorage::saveSelected(void)
{
    toTreeWidgetItem *item = selectedItem();
    if (item)
    {
        if (item->parent() || OnlyFiles)
        {
            CurrentSpace = item->text(COLUMNS);
            CurrentFile = item->text(0);
        }
        else
        {
            CurrentSpace = item->text(0);
            CurrentFile = QString::null;
        }
    }
}


void toResultStorage::query(void)
{
    if (!handled() || Tablespaces || Files)
        return ;

    try
    {
        saveSelected();
        clear();

        toConnection &conn = connection();

        toQList args;
        toPush(args, toQValue(toSizeDecode(Unit)));

        TablespaceValues.clear();
        FileValues.clear();

        Tablespaces = new toNoBlockQuery(conn, toQuery::Background,
                                         toSQL::string(ShowCoalesced ? SQLShowCoalesced : SQLNoShowCoalesced, connection()), args);
        Files = NULL;
        Files = new toNoBlockQuery(conn, toQuery::Background,
                                   toSQL::string(SQLDatafile, connection()), args);

        Poll.start(100);
    }
    TOCATCH
}

void toResultStorage::updateList(void)
{
    clear();
    if (!OnlyFiles)
    {
        for (std::list<QString>::iterator j = TablespaceValues.begin();j != TablespaceValues.end();)
        {
            toTreeWidgetItem *tablespace = new toResultStorageItem(AvailableGraph, this, NULL);
            for (int i = 0;i < COLUMNS && j != TablespaceValues.end();i++, j++)
                tablespace->setText(i, *j);

            if (CurrentSpace == tablespace->text(0))
            {
                if (CurrentFile.isEmpty())
                    setSelected(tablespace, true);
            }
        }
    }

    for (std::list<QString>::iterator k = FileValues.begin();k != FileValues.end();)
    {
        QString name = *k;
        k++;

        toTreeWidgetItem *file;
        toTreeWidgetItem *tablespace = NULL;
        if (OnlyFiles)
        {
            file = new toResultStorageItem(AvailableGraph, this, NULL);
        }
        else
        {
            for (tablespace = firstChild();tablespace && tablespace->text(0) != name;tablespace = tablespace->nextSibling())
                ;
            if (!tablespace)
            {
                if (Files)
                    break;
            }
            file = new toResultStorageItem(AvailableGraph, tablespace, NULL);
        }
        for (int i = 0;i < FILECOLUMNS && k != FileValues.end();i++, k++)
            file->setText(i, *k);

        file->setText(COLUMNS, name);
        if (CurrentSpace == file->text(COLUMNS) &&
                CurrentFile == file->text(0))
        {
            if (tablespace)
                tablespace->setOpen(true);
            setSelected(file, true);
        }
    }
}

void toResultStorage::poll(void)
{
    try
    {
        if (!toCheckModal(this))
            return ;
        if (Tablespaces && Tablespaces->poll())
        {
            int cols = Tablespaces->describe().size();
            while (Tablespaces->poll() && !Tablespaces->eof())
            {
                for (int i = 0;i < cols && !Tablespaces->eof();i++)
                    toPush(TablespaceValues, QString(Tablespaces->readValue()));
            }
            updateList();
            if (Tablespaces->eof())
            {
                delete Tablespaces;
                Tablespaces = NULL;
            }
        }

        if (Files && Files->poll())
        {
            int cols = Files->describe().size();
            while (Files->poll() && !Files->eof())
            {
                for (int i = 0;i < cols && !Files->eof();i++)
                    toPush(FileValues, QString(Files->readValue()));
            }
            if (Files->eof())
            {
                delete Files;
                Files = NULL;
            }
        }

        if (Tablespaces == NULL && Files == NULL)
        {
            updateList();
            Poll.stop();
        }
    }
    catch (const QString &exc)
    {
        delete Tablespaces;
        Tablespaces = NULL;
        delete Files;
        Files = NULL;
        Poll.stop();
        toStatusMessage(exc);
    }
}

QString toResultStorage::currentTablespace(void)
{
    toTreeWidgetItem *item = selectedItem();
    if (!item)
        throw tr("No tablespace selected");
    QString name;
    if (item->parent() || OnlyFiles)
        name = item->text(COLUMNS);
    else
        name = item->text(0);
    if (name.isEmpty())
        throw tr("Weird, empty tablespace name");
    return name;
}

QString toResultStorage::currentFilename(void)
{
    toTreeWidgetItem *item = selectedItem();
    if (!item || (!item->parent() && !OnlyFiles))
        throw tr("No file selected");
    QString name = item->text(0);
    return name;
}

void toResultStorage::setOnlyFiles(bool only)
{
    saveSelected();
    if (only && !OnlyFiles)
    {
        addColumn(tr("Tablespace"));
        setRootIsDecorated(false);
    }
    else if (!only && OnlyFiles)
    {
        // removing columns doesn't work with qtreewidget
//         removeColumn(columns() - 1); todo
        setRootIsDecorated(true);
    }
    OnlyFiles = only;
    updateList();
}

toStorageExtent::extentName::extentName(const QString &owner, const QString &table,
                                        const QString &partition, int size)
        : Owner(owner), Table(table), Partition(partition)
{
    Size = size;
}

bool toStorageExtent::extentName::operator < (const toStorageExtent::extentName &ext) const
{
    if (Owner < ext.Owner)
        return true;
    if (Owner > ext.Owner)
        return false;
    if (Table < ext.Table)
        return true;
    if (Table > ext.Table)
        return false;
    if (Partition < ext.Partition)
        return true;
    return false;
}

bool toStorageExtent::extentName::operator == (const toStorageExtent::extentName &ext) const
{
    return Owner == ext.Owner && Table == ext.Table && (Partition == ext.Partition || ext.Partition.isNull());
}

toStorageExtent::extent::extent(const QString &owner, const QString &table,
                                const QString &partition,
                                int file, int block, int size)
        : extentName(owner, table, partition, size)
{
    File = file;
    Block = block;
}

bool toStorageExtent::extent::operator < (const toStorageExtent::extent &ext) const
{
    if (File < ext.File)
        return true;
    if (File > ext.File)
        return false;
    if (Block < ext.Block)
        return true;
    return false;
}

bool toStorageExtent::extent::operator == (const toStorageExtent::extent &ext) const
{
    return Owner == ext.Owner && Table == ext.Table && Partition == ext.Partition &&
           File == ext.File && Block == ext.Block && Size == ext.Size;
}

toStorageExtent::toStorageExtent(QWidget *parent, const char *name)
        : QWidget(parent)
{
    setObjectName(name);
    QPalette pal = palette();
    pal.setColor(backgroundRole(), Qt::white);
    setPalette(pal);
}

void toStorageExtent::highlight(const QString &owner, const QString &table,
                                const QString &partition)
{
    Highlight.Owner = owner;
    Highlight.Table = table;
    Highlight.Partition = partition;
    update();
}

static toSQL SQLObjectsFile("toStorageExtent:ObjectsFile",
                            "SELECT owner,\n"
                            "       segment_name,\n"
                            "       partition_name,\n"
                            "       file_id,\n"
                            "       block_id,\n"
                            "       blocks\n"
                            "  FROM sys.dba_extents\n"
                            " WHERE tablespace_name = :tab<char[101]>\n"
                            "   AND file_id = :fil<int>",
                            "Get objects in a datafile, must have same columns and binds",
                            "0800");

static toSQL SQLObjectsFile7("toStorageExtent:ObjectsFile",
                             "SELECT owner,\n"
                             "       segment_name,\n"
                             "       NULL,\n"
                             "       file_id,\n"
                             "       block_id,\n"
                             "       blocks\n"
                             "  FROM sys.dba_extents\n"
                             " WHERE tablespace_name = :tab<char[101]>\n"
                             "   AND file_id = :fil<int>",
                             "",
                             "0703");

static toSQL SQLObjectsTablespace("toStorageExtent:ObjectsTablespace",
                                  "SELECT owner,\n"
                                  "       segment_name,\n"
                                  "       partition_name,\n"
                                  "       file_id,\n"
                                  "       block_id,\n"
                                  "       blocks\n"
                                  "  FROM sys.dba_extents WHERE tablespace_name = :tab<char[101]>",
                                  "Get objects in a tablespace, must have same columns and binds",
                                  "0800");

static toSQL SQLObjectsTablespace7("toStorageExtent:ObjectsTablespace",
                                   "SELECT owner,\n"
                                   "       segment_name,\n"
                                   "       NULL,\n"
                                   "       file_id,\n"
                                   "       block_id,\n"
                                   "       blocks\n"
                                   "  FROM sys.dba_extents WHERE tablespace_name = :tab<char[101]>",
                                   "",
                                   "0703");

static toSQL SQLTableTablespace("toStorageExtent:TableTablespace",
                                "SELECT MAX(tablespace_name)\n"
                                "  FROM (SELECT tablespace_name FROM sys.all_all_tables\n"
                                "         WHERE owner = :own<char[101]> AND table_name = :tab<char[101]>\n"
                                "        UNION\n"
                                "        SELECT i.tablespace_name\n"
                                "          FROM sys.all_indexes i,\n"
                                "               sys.all_constraints c,\n"
                                "               sys.all_all_tables t\n"
                                "         WHERE t.owner = :own<char[101]> AND t.table_name = :tab<char[101]>\n"
                                "           AND i.table_name = t.table_name AND i.owner = t.owner\n"
                                "           AND c.constraint_name = i.index_name AND c.owner = i.owner\n"
                                "           AND c.constraint_type = 'P')",
                                "Get the tablespace of a table");

static toSQL SQLTableTablespace7("toStorageExtent:TableTablespace",
                                 "SELECT tablespace_name FROM sys.all_tables\n"
                                 " WHERE owner = :own<char[101]> AND table_name = :tab<char[101]>",
                                 "",
                                 "0703");

static toSQL SQLFileBlocks("toStorageExtent:FileSize",
                           "SELECT file_id,blocks FROM sys.dba_data_files\n"
                           " WHERE tablespace_name = :tab<char[101]>\n"
                           "   AND file_id = :fil<int>\n"
                           " ORDER BY file_id",
                           "Get blocks for datafiles, must have same columns and binds");

static toSQL SQLTablespaceBlocks("toStorageExtent:TablespaceSize",
                                 "SELECT file_id,blocks FROM sys.dba_data_files\n"
                                 " WHERE tablespace_name = :tab<char[101]>"
                                 " ORDER BY file_id",
                                 "Get blocks for tablespace datafiles, must have same columns and binds");

void toStorageExtent::setTablespace(const QString &tablespace)
{
    try
    {
        if (Tablespace == tablespace)
            return ;
        Tablespace = tablespace;
        toBusy busy;
        Extents.clear();
        FileOffset.clear();
        toQuery query(toCurrentConnection(this), SQLObjectsTablespace, tablespace);

        // It's used to keep UI "non-freezed".
        // OK, there are better query classes for it but it's enough for now.
        // TODO: rewrite data fetching for this tool
        int progressMax = 1000;
        int progressCurr = 1;
        QProgressDialog progress("Reading Objects...", "Abort", 0, progressMax, this);

        extent cur;
        while (!query.eof())
        {
            if (progressCurr > (progressMax-1))
                progressCurr = 0;
            if (progressCurr % 200 == 0)
            {
                progress.setValue(progressCurr);
                QCoreApplication::processEvents();
            }
            if (progress.wasCanceled())
            {
                Extents.clear();
                break;
            }
            ++progressCurr;

            cur.Owner = query.readValueNull();
            cur.Table = query.readValueNull();
            cur.Partition = query.readValueNull();
            cur.File = query.readValueNull().toInt();
            cur.Block = query.readValueNull().toInt();
            cur.Size = query.readValueNull().toInt();
            toPush(Extents, cur);
        }
        toQuery blocks(toCurrentConnection(this), SQLTablespaceBlocks, tablespace);
        Total = 0;
        while (!blocks.eof())
        {
            int id = blocks.readValueNull().toInt();
            FileOffset[id] = Total;
            Total += blocks.readValueNull().toInt();
        }
    }
    TOCATCH
    Extents.sort();
    update();
}

void toStorageExtent::setFile(const QString &tablespace, int file)
{
    try
    {
        toBusy busy;
        Extents.clear();
        FileOffset.clear();
        toQuery query(toCurrentConnection(this), SQLObjectsFile, tablespace, QString::number(file));
        extent cur;

        int progressMax = 1000;
        int progressCurr = 1;
        QProgressDialog progress("Reading Objects...", "Abort", 0, progressMax, this);

        while (!query.eof())
        {
            if (progressCurr > (progressMax-1))
                progressCurr = 0;
            if (progressCurr % 200 == 0)
            {
                progress.setValue(progressCurr);
                QCoreApplication::processEvents();
            }
            if (progress.wasCanceled())
            {
                Extents.clear();
                break;
            }
            ++progressCurr;

            cur.Owner = query.readValueNull();
            cur.Table = query.readValueNull();
            cur.Partition = query.readValueNull();
            cur.File = query.readValueNull().toInt();
            cur.Block = query.readValueNull().toInt();
            cur.Size = query.readValueNull().toInt();
            toPush(Extents, cur);
        }
        toQuery blocks(toCurrentConnection(this), SQLFileBlocks, tablespace, QString::number(file));
        Total = 0;
        while (!blocks.eof())
        {
            int id = blocks.readValueNull().toInt();
            FileOffset[id] = Total;
            Total += blocks.readValueNull().toInt();
        }
    }
    TOCATCH
    Extents.sort();
    update();
}

void toStorageExtent::paintEvent(QPaintEvent *)
{
    QPainter paint(this);
    if ( FileOffset.empty() )
        return ;
    QFontMetrics fm = paint.fontMetrics();

    int offset = 2 * fm.lineSpacing();
    // prevent the crash when user wants it smaller (by splitter)
    setMinimumHeight(offset + 20);

    double lineblocks = Total / (height() - offset - FileOffset.size() + 1);

    paint.fillRect(0, 0, width(), offset, palette().window());
    paint.drawText(0, 0, width(), offset, Qt::AlignLeft | Qt::AlignTop, tr("Files: %1").arg(FileOffset.size()));
    paint.drawText(0, 0, width(), offset, Qt::AlignRight | Qt::AlignTop, tr("Extents: %1").arg(Extents.size()));
    if (!Tablespace.isNull())
        paint.drawText(0, 0, width(), offset, Qt::AlignCenter | Qt::AlignTop, tr("Tablespace: %1").arg(Tablespace));
    paint.drawText(0, 0, width(), offset, Qt::AlignLeft | Qt::AlignBottom, tr("Blocks: %1").arg(Total));
    paint.drawText(0, 0, width(), offset, Qt::AlignRight | Qt::AlignBottom, tr("Blocks/line: %1").arg(int(lineblocks)));

    for (std::list<extent>::iterator i = Extents.begin();i != Extents.end();i++)
    {
        QColor col("#469446"); //= Qt::darkGreen;
        if (extentName(*i) == Highlight)
            col = Qt::red;
        int fileo = 0;
        for (std::map<int, int>::iterator j = FileOffset.begin();j != FileOffset.end();j++, fileo++)
            if ((*j).first == (*i).File)
                break;
        int block = FileOffset[(*i).File] + (*i).Block;

        int y1 = int(block / lineblocks);
        int x1 = int((block / lineblocks - y1) * width());
        block += (*i).Size;
        int y2 = int(block / lineblocks);
        int x2 = int((block / lineblocks - y2) * width());
        paint.setPen(col);
        if (y1 != y2)
        {
            paint.drawLine(x1, y1 + offset + fileo, width() - 1, y1 + offset + fileo);
            paint.drawLine(0, y2 + offset + fileo, x2 - 1, y2 + offset + fileo);
            if (y1 + 1 != y2)
                paint.fillRect(0, y1 + 1 + offset + fileo, width(), y2 - y1 - 1, col);
        }
        else
            paint.drawLine(x1, y1 + offset + fileo, x2, y2 + offset + fileo);
    }
    std::map<int, int>::iterator j = FileOffset.begin();
    j++;
    paint.setPen(Qt::black);
    int fileo = offset;
    while (j != FileOffset.end())
    {
        int block = (*j).second;
        int y1 = int(block / lineblocks);
        int x1 = int((block / lineblocks - y1) * width());
        paint.drawLine(x1, y1 + fileo, width() - 1, y1 + fileo);
        if (x1 != 0)
            paint.drawLine(0, y1 + 1 + fileo, x1 - 1, y1 + fileo + 1);
        j++;
        fileo++;
    }
}


std::list<toStorageExtent::extentTotal> toStorageExtent::objects(void)
{
    std::list<extentTotal> ret;

    for (std::list<extent>::iterator i = Extents.begin();i != Extents.end();i++)
    {
        bool dup = false;
        for (std::list<extentTotal>::iterator j = ret.begin();j != ret.end();j++)
        {
            if ((*j) == (*i))
            {
                (*j).Size += (*i).Size;
                (*j).Extents++;
                dup = true;
                break;
            }
        }
        if (!dup)
            toPush(ret, extentTotal((*i).Owner, (*i).Table, (*i).Partition, (*i).Size));
    }

    ret.sort();

    return ret;
}

static toSQL SQLListExtents("toResultStorage:ListExtents",
                            "SELECT * \n"
                            "  FROM SYS.DBA_EXTENTS WHERE OWNER = :f1<char[101]> AND SEGMENT_NAME = :f2<char[101]>\n"
                            " ORDER BY extent_id",
                            "List the extents of a table in a schema.",
                            "" ,
                            "Oracle");

toResultExtent::toResultExtent(QWidget *parent, const char *name)
        : QSplitter(Qt::Vertical, parent)
{
    setObjectName(name);
    Graph = new toStorageExtent(this);
    List = new toResultTableView(true, false, this);
    List->setSQL(SQLListExtents);

    setChildrenCollapsible(false);
}

bool toResultExtent::canHandle(toConnection &conn)
{
    return toIsOracle(conn);
}

void toResultExtent::query(const QString &sql, const toQList &params)
{
    try
    {
        if (!handled())
            return ;

        if (!setSQLParams(sql, params))
            return ;

        toQList::const_iterator i = params.begin();
        if (i == params.end())
            return ;
        QString owner = (*i);

        i++;
        if (i == params.end())
            return ;
        QString table = (*i);

        List->changeParams(owner, table);

        toQList res = toQuery::readQueryNull(connection(), SQLTableTablespace, owner, table);

        Graph->setTablespace(toShift(res));
        Graph->highlight(owner, table, QString::null);
    }
    TOCATCH
}
