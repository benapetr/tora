//***************************************************************************
/*
 * TOra - An Oracle Toolkit for DBA's and developers
 * Copyright (C) 2000-2001,2001 Underscore AB
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
 *      these libraries without written consent from Underscore AB. Observe
 *      that this does not disallow linking to the Qt Free Edition.
 *
 * All trademarks belong to their respective owners.
 *
 ****************************************************************************/

#include "utils.h"

#include "toconf.h"
#include "toconnection.h"
#include "tonoblockquery.h"
#include "toresultstorage.h"
#include "tosql.h"
#include "totool.h"

#include <qheader.h>
#include <qpainter.h>

#include "toresultstorage.moc"

class toResultStorageItem : public toResultViewItem {
  bool AvailableGraph;
public:
  toResultStorageItem(bool available,QListView *parent,QListViewItem *after,
		      const QString &buf=QString::null)
    : toResultViewItem(parent,after,buf),AvailableGraph(available)
  { }
  toResultStorageItem(bool available,QListViewItem *parent,QListViewItem *after,
		      const QString &buf=QString::null)
    : toResultViewItem(parent,after,buf),AvailableGraph(available)
  { }
  virtual void paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align)
  {
    if (column==8) {
      QString ct=text(column);
      if (ct.isEmpty()) {
	QListViewItem::paintCell(p,cg,column,width,align);
	return;
      }
      ct=ct.left(ct.length()-1); // Strip last %
      double total=text(7).toDouble();
      double user=text(5).toDouble();
      double free=text(6).toDouble();
      if (total<user||!AvailableGraph)
	total=user;

      QString t;

      user/=total;
      free/=total;
      if (AvailableGraph)
	t.sprintf("%0.1f / %0.1f %%",free*100,(total-user)/total*100);
      else
	t.sprintf("%0.1f %%",free*100);
      p->fillRect(0,0,int((user-free)*width),height(),
		  QBrush(red));
      p->fillRect(int((user-free)*width),0,int(user*width),height(),
		  QBrush(blue));
      if (AvailableGraph)
	p->fillRect(int(user*width),0,width,height(),
		    QBrush(green));

      QPen pen(white);
      p->setPen(pen);
      p->drawText(0,0,width,height(),AlignCenter,t);
    } else if (column==9) {
      QString ct=text(column);
      if (ct.isEmpty()) {
	QListViewItem::paintCell(p,cg,column,width,align);
	return;
      }
      ct=ct.left(ct.length()-1); // Strip last %
      double val=ct.toDouble();

      p->fillRect(0,0,int(val*width/100),height(),QBrush(blue));
      p->fillRect(int(val*width/100),0,width,height(),
		  QBrush(isSelected()?cg.highlight():cg.base()));

      QPen pen(isSelected()?cg.highlightedText():cg.foreground());
      p->setPen(pen);
      p->drawText(0,0,width,height(),AlignCenter,text(column));
    } else {
      QListViewItem::paintCell(p,cg,column,width,align);
    }
  }
};

bool toResultStorage::canHandle(toConnection &conn)
{
  return toIsOracle(conn);
}

toResultStorage::toResultStorage(bool available,QWidget *parent,const char *name)
  : toResultView(false,false,parent,name),AvailableGraph(available)
{
  Unit=toTool::globalConfig(CONF_SIZE_UNIT,DEFAULT_SIZE_UNIT);
  setAllColumnsShowFocus(true);
  setSorting(0);
  setRootIsDecorated(true);
  addColumn("Name");
  addColumn("Status");
  addColumn("Information");
  addColumn("Contents");
  addColumn("Logging");
  addColumn(QString("Size (%1)").arg(Unit));
  addColumn(QString("Free (%1)").arg(Unit));
  addColumn(QString("Autoextend (%1)").arg(Unit));
  if (available)
    addColumn("Used/Free/Autoextend");
  else
    addColumn("Available");
  addColumn("Coalesced");
  addColumn(QString("Max free (%1)").arg(Unit));
  addColumn("Free fragments");
  setSQLName("toResultStorage");

  setColumnAlignment(5,AlignRight);
  setColumnAlignment(6,AlignRight);
  setColumnAlignment(7,AlignRight);
  setColumnAlignment(8,AlignCenter);
  setColumnAlignment(9,AlignCenter);
  setColumnAlignment(10,AlignRight);
  setColumnAlignment(11,AlignRight);

  ShowCoalesced=false;
  OnlyFiles=false;

  Tablespaces=Files=NULL;

  connect(&Poll,SIGNAL(timeout()),this,SLOT(poll()));
}

toResultStorage::~toResultStorage()
{
  delete Tablespaces;
  delete Files;
}

static toSQL SQLShowCoalesced("toResultStorage:ShowCoalesced",
			      "SELECT d.tablespace_name, \n"
			      "       d.status,\n"
                              "       d.extent_management,\n"
                              "       d.contents,\n"
                              "       d.logging,\n"
                              "       TO_CHAR(ROUND(NVL(a.bytes / b.unit, 0),2)),\n"
                              "       TO_CHAR(ROUND(NVL(f.bytes,0) / b.unit,2)), \n"
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
                              "   AND NOT (d.extent_management like 'LOCAL' AND d.contents like 'TEMPORARY')\n"
                              " UNION ALL\n"
                              "SELECT d.tablespace_name, \n"
                              "       d.status,\n"
                              "       d.extent_management,\n"
                              "       d.contents,\n"
                              "       d.logging,\n"
                              "       TO_CHAR(ROUND(NVL(a.bytes / b.unit, 0),2)),\n"
                              "       TO_CHAR(ROUND(NVL(f.bytes,0) / b.unit,2)),\n"
                              "       TO_CHAR(ROUND(NVL(a.maxbytes / b.unit, 0),2)),\n"
                              "       '-',\n"
                              "       '-',\n"
                              "       TO_CHAR(ROUND(NVL(f.maxbytes,0) / b.unit,2)), \n"
                              "       TO_CHAR(f.total_extents)\n"
                              "  FROM sys.dba_tablespaces d,\n"
                              "       (select tablespace_name, sum(bytes) bytes, sum(maxbytes) maxbytes from sys.dba_temp_files group by tablespace_name) a,\n"
                              "       (select tablespace_name, nvl(sum(bytes_cached),0) bytes, count(1) total_extents,nvl(max(bytes_cached),0) maxbytes from v$temp_extent_pool group by tablespace_name) f,\n"
                              "       (select :unt<int> unit from sys.dual) b\n"
                              " WHERE d.tablespace_name = a.tablespace_name(+)\n"
                              "   AND d.tablespace_name = f.tablespace_name(+)\n"
                              "   AND d.extent_management = 'LOCAL'\n"
                              "   AND d.contents = 'TEMPORARY'",
                              "Display storage usage of database. This includes the coalesced columns which may make the query sluggish on some DB:s. "
                              "All columns must be present in output (Should be 12)",
                              "8.1");

static toSQL SQLShowCoalesced8("toResultStorage:ShowCoalesced",
                               "SELECT d.tablespace_name, \n"
                               "       d.status,\n"
			       "       d.extent_management,\n"
                               "       d.contents,\n"
                               "       d.logging,\n"
                               "       TO_CHAR(ROUND(NVL(a.bytes / b.unit, 0),2)),\n"
                               "       TO_CHAR(ROUND(NVL(f.bytes,0) / b.unit,2)), \n"
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
                               QString::null,
                               "8.0");

static toSQL SQLShowCoalesced7("toResultStorage:ShowCoalesced",
                               "SELECT d.tablespace_name, \n"
                               "       d.status,\n"
                               "       ' ',\n"
                               "       d.contents,\n"
                               "       'N/A',\n"
                               "       TO_CHAR(ROUND(NVL(a.bytes / b.unit, 0),2)),\n"
                               "       TO_CHAR(ROUND(NVL(f.bytes,0) / b.unit,2)), \n"
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
                               QString::null,
                               "7.3");

static toSQL SQLNoShowCoalesced("toResultStorage:NoCoalesced",
                                "SELECT d.tablespace_name, \n"
                                "       d.status,\n"
				"       d.extent_management,\n"
                                "       d.contents,\n"
                                "       d.logging,\n"
                                "       TO_CHAR(ROUND(NVL(a.bytes / b.unit, 0),2)),\n"
                                "       TO_CHAR(ROUND(NVL(f.bytes,0) / b.unit,2)), \n"
                                "       TO_CHAR(ROUND(NVL(a.maxbytes / b.unit, 0),2)),\n"
				"       '-',\n"
                                "       '-',\n"
                                "       TO_CHAR(ROUND(NVL(f.maxbytes,0) / b.unit,2)), \n"
                                "       TO_CHAR(f.total_extents)\n"
                                "  FROM sys.dba_tablespaces d,\n"
                                "       (select tablespace_name, sum(bytes) bytes, sum(maxbytes) maxbytes from sys.dba_data_files group by tablespace_name) a,\n"
                                "       (select tablespace_name, NVL(sum(bytes),0) bytes, count(1) total_extents, NVL(max(bytes),0) maxbytes from sys.dba_free_space group by tablespace_name) f,\n"
                                "       (select :unt<int> unit from sys.dual) b\n"
                                " WHERE d.tablespace_name = a.tablespace_name(+)\n"
                                "   AND d.tablespace_name = f.tablespace_name(+)\n"
                                "   AND NOT (d.extent_management like 'LOCAL' AND d.contents like 'TEMPORARY')\n"
                                " UNION ALL\n"
                                "SELECT d.tablespace_name, \n"
                                "       d.status,\n"
				"       d.extent_management,\n"
                                "       d.contents,\n"
                                "       d.logging,\n"
                                "       TO_CHAR(ROUND(NVL(a.bytes / b.unit, 0),2)),\n"
                                "       TO_CHAR(ROUND(NVL(f.bytes,0) / b.unit,2)),\n"
                                "       TO_CHAR(ROUND(NVL(a.maxbytes / b.unit, 0),2)),\n"
				"       '-',\n"
                                "       '-',\n"
                                "       TO_CHAR(ROUND(NVL(f.maxbytes,0) / b.unit,2)), \n"
                                "       TO_CHAR(f.total_extents)\n"
                                "  FROM sys.dba_tablespaces d,\n"
                                "       (select tablespace_name, sum(bytes) bytes, sum(maxbytes) maxbytes from sys.dba_temp_files group by tablespace_name) a,\n"
                                "       (select tablespace_name, NVL(sum(bytes_cached),0) bytes, count(1) total_extents, NVL(max(bytes_cached),0) maxbytes from v$temp_extent_pool group by tablespace_name) f,\n"
                                "       (select :unt<int> unit from sys.dual) b\n"
                                " WHERE d.tablespace_name = a.tablespace_name(+)\n"
                                "   AND d.tablespace_name = f.tablespace_name(+)\n"
                                "   AND d.extent_management = 'LOCAL'\n"
                                "   AND d.contents = 'TEMPORARY'",
                                "Display storage usage of database. This does not include the coalesced columns which may make the query sluggish on some DB:s. "
                                "All columns must be present in output (Should be 12)",
                                "8.1");

static toSQL SQLNoShowCoalesced8("toResultStorage:NoCoalesced",
                                 "SELECT d.tablespace_name, \n"
                                 "       d.status,\n"
				 "       d.extent_management,\n"
                                 "       d.contents,\n"
                                 "       d.logging,\n"
                                 "       TO_CHAR(ROUND(NVL(a.bytes / b.unit, 0),2)),\n"
                                 "       TO_CHAR(ROUND(NVL(f.bytes,0) / b.unit,2)), \n"
                                 "       TO_CHAR(ROUND(NVL(a.maxbytes / b.unit, 0),2)),\n"
				 "       '-',\n"
                                 "       '-',\n"
                                 "       TO_CHAR(ROUND(NVL(f.maxbytes,0) / b.unit,2)), \n"
                                 "       TO_CHAR(f.total_extents)\n"
                                 "  FROM sys.dba_tablespaces d,\n"
                                 "       (select tablespace_name, sum(bytes) bytes, sum(maxbytes) maxbytes from sys.dba_data_files group by tablespace_name) a,\n"
                                 "       (select tablespace_name, NVL(sum(bytes),0) bytes, count(1) total_extents, NVL(max(bytes),0) maxbytes from sys.dba_free_space group by tablespace_name) f,\n"
                                 "       (select :unt<int> unit from sys.dual) b\n"
                                 " WHERE d.tablespace_name = a.tablespace_name(+)\n"
                                 "   AND d.tablespace_name = f.tablespace_name(+)\n"
                                 " ORDER BY d.tablespace_name",
                                 QString::null,
                                 "8.0");

static toSQL SQLNoShowCoalesced7("toResultStorage:NoCoalesced",
                                 "SELECT d.tablespace_name, \n"
                                 "       d.status,\n"
                                 "       ' ',\n"
                                 "       d.contents,\n"
                                 "       'N/A',\n"
                                 "       TO_CHAR(ROUND(NVL(a.bytes / b.unit, 0),2)),\n"
                                 "       TO_CHAR(ROUND(NVL(f.bytes,0) / b.unit,2)), \n"
                                 "       '-',\n"
				 "       '-',\n"
                                 "       '-',\n"
                                 "       TO_CHAR(ROUND(NVL(f.maxbytes,0) / b.unit,2)), \n"
                                 "       TO_CHAR(f.total_extents)\n"
                                 "  FROM sys.dba_tablespaces d,\n"
                                 "       (select tablespace_name, sum(bytes) bytes from sys.dba_data_files group by tablespace_name) a,\n"
                                 "       (select tablespace_name, NVL(sum(bytes),0) bytes, count(1) total_extents, NVL(max(bytes),0) maxbytes from sys.dba_free_space group by tablespace_name) f,\n"
                                 "       (select :unt<int> unit from sys.dual) b\n"
                                 " WHERE d.tablespace_name = a.tablespace_name(+)\n"
                                 "   AND d.tablespace_name = f.tablespace_name(+)\n"
                                 " ORDER BY d.tablespace_name",
                                 QString::null,
                                 "7.3");

static toSQL SQLDatafile("toResultStorage:Datafile",
                         "SELECT d.tablespace_name,\n"
                         "       v.name,\n"
                         "       v.status,\n"
                         "       v.enabled,\n"
                         "         ' ',\n"
                         "         ' ',\n"
                         "       to_char(round(d.bytes/b.unit,2)),\n"
                         "       to_char(round(s.bytes/b.unit,2)),\n"
                         "       to_char(round(d.maxbytes/b.unit,2)),\n"
			 "       '-',\n"
                         "       ' ',\n"
                         "       to_char(round(s.maxbytes/b.unit,2)),\n"
                         "       to_char(s.num),\n"
			 "       NULL,\n" // Used to fill in tablespace name
                         "       v.file#\n"
                         "  FROM sys.dba_data_files d,\n"
                         "       v$datafile v,\n"
                         "       (SELECT file_id, NVL(SUM(bytes),0) bytes, COUNT(1) num, NVL(MAX(bytes),0) maxbytes FROM sys.dba_free_space  GROUP BY file_id) s,\n"
                         "       (select :unt<int> unit from sys.dual) b\n"
                         " WHERE (s.file_id (+)= d.file_id)\n"
                         "   AND (d.file_name = v.name)\n"
                         " UNION ALL\n"
                         "SELECT d.tablespace_name,\n"
                         "       v.name,\n"
                         "       v.status,\n"
                         "       v.enabled,\n"
                         "       ' ',\n"
                         "       ' ',\n"
                         "       to_char(round(d.bytes/b.unit,2)),\n"
                         "       to_char(round((d.user_bytes-t.bytes_cached)/b.unit,2)),\n"
                         "       to_char(round(d.maxbytes/b.unit,2)),\n"
			 "       '-',\n"
                         "       ' ',\n"
                         "       ' ',\n"
                         "       '1',\n"
			 "       NULL,\n" // Used to fill in tablespace name
                         "       v.file#\n" // Needed by toStorage to work
                         "  FROM sys.dba_temp_files d,\n"
                         "       v$tempfile v,\n"
                         "       v$temp_extent_pool t,\n"
                         "       (select :unt<int> unit from sys.dual) b\n"
                         " WHERE (t.file_id (+)= d.file_id)\n"
                         "   AND (d.file_id = v.file#)",
                         "Display information about a datafile in a tablespace. "
                         "All columns must be present in the output (Should be 14)",
                         "8.1");

static toSQL SQLDatafile8("toResultStorage:Datafile",
                          "SELECT d.tablespace_name,\n"
                          "       v.name,\n"
                          "       v.status,\n"
                          "       v.enabled,\n"
                          "       ' ',\n"
                          "       ' ',\n"
                          "       to_char(round(d.bytes/b.unit,2)),\n"
                          "       to_char(round(s.bytes/b.unit,2)),\n"
                          "       to_char(round(d.maxbytes/b.unit,2)),\n"
			  "       '-',\n"
                          "       ' ',\n"
                          "       to_char(round(s.maxbytes/b.unit,2)),\n"
                          "       to_char(s.num),\n"
			  "       NULL,\n"
                          "       v.file#\n"
                          "  FROM sys.dba_data_files d,\n"
                          "       v$datafile v,\n"
                          "       (SELECT file_id, NVL(SUM(bytes),0) bytes, COUNT(1) num,NVL(MAX(bytes),0) maxbytes FROM sys.dba_free_space  GROUP BY file_id) s,\n"
                          "       (select :unt<int> unit from sys.dual) b\n"
                          " WHERE (s.file_id (+)= d.file_id)\n"
                          "   AND (d.file_name = v.name)",
                          QString::null,
                          "8.0");

static toSQL SQLDatafile7("toResultStorage:Datafile",
			  "SELECT  d.tablespace_name,\n"
			  "	   v.name,\n"
			  "	   v.status,\n"
			  "	   v.enabled,\n"
			  "	   ' ',\n"
			  "	   ' ',\n"
			  "        to_char(round(d.bytes/b.unit,2)),\n"
			  "        to_char(round(s.bytes/b.unit,2)),\n"
			  "        '-',\n"
			  "        '-',\n"
			  "	   ' ',\n"
			  "        to_char(round(s.maxbytes/b.unit,2)),\n"
			  "	   to_char(s.num),\n"
			  "        NULL,\n"
			  "        v.file#\n"
			  "  FROM  sys.dba_data_files d,\n"
			  "	   v$datafile v,\n"
			  "	   (SELECT file_id, NVL(SUM(bytes),0) bytes, COUNT(1) num,NVL(MAX(bytes),0) maxbytes FROM sys.dba_free_space  GROUP BY file_id) s,\n"
			  "        (select :unt<int> unit from sys.dual) b\n"
			  " WHERE  (s.file_id (+)= d.file_id)\n"
			  "   AND  (d.file_name = v.name)",
			  QString::null,
			  "7.3");

#define FILECOLUMNS 14
#define COLUMNS (FILECOLUMNS-2)

void toResultStorage::saveSelected(void)
{
  QListViewItem *item=selectedItem();
  if (item) {
    if (item->parent()||OnlyFiles) {
      CurrentSpace=item->text(COLUMNS);
      CurrentFile=item->text(0);
    } else {
      CurrentSpace=item->text(0);
      CurrentFile=QString::null;
    }
  }
}


void toResultStorage::query(void)
{
  if (!handled()||Tablespaces||Files)
    return;

  try {
    saveSelected();
    clear();

    toConnection &conn=connection();

    toQList args;
    toPush(args,toQValue(toSizeDecode(Unit)));

    TablespaceValues.clear();
    FileValues.clear();

    Tablespaces=new toNoBlockQuery(conn,toQuery::Background,
                                   toSQL::string(ShowCoalesced?SQLShowCoalesced:SQLNoShowCoalesced,connection()),args);
    Files=NULL;
    Files=new toNoBlockQuery(conn,toQuery::Background,
                             toSQL::string(SQLDatafile,connection()),args);
  
    Poll.start(100);
  } TOCATCH
}

void toResultStorage::updateList(void)
{
  clear();
  if (!OnlyFiles) {
    for(std::list<QString>::iterator j=TablespaceValues.begin();j!=TablespaceValues.end();) {
      QListViewItem *tablespace=new toResultStorageItem(AvailableGraph,this,NULL);
      for (int i=0;i<COLUMNS&&j!=TablespaceValues.end();i++,j++)
	tablespace->setText(i,*j);
    
      if (CurrentSpace==tablespace->text(0)) {
	if (CurrentFile.isEmpty())
	  setSelected(tablespace,true);
      }
    }
  }

  for(std::list<QString>::iterator k=FileValues.begin();k!=FileValues.end();) {
    QString name=*k;
    k++;

    QListViewItem *file;
    QListViewItem *tablespace=NULL;
    if (OnlyFiles) {
      file=new toResultStorageItem(AvailableGraph,this,NULL);
    } else {
      for (tablespace=firstChild();tablespace&&tablespace->text(0)!=name;tablespace=tablespace->nextSibling())
	;
      if (!tablespace) {
	if (Files)
	  break;
	else
	  throw QString("Couldn't find tablespace parent %1 for datafile").arg(name);
      }
      file=new toResultStorageItem(AvailableGraph,tablespace,NULL);
    }
    for (int i=0;i<FILECOLUMNS&&k!=FileValues.end();i++,k++)
      file->setText(i,*k);
    
    file->setText(COLUMNS,name);
    if (CurrentSpace==file->text(COLUMNS)&&
	CurrentFile==file->text(0)) {
      if (tablespace)
	tablespace->setOpen(true);
      setSelected(file,true);
    }
  }
}

void toResultStorage::poll(void)
{
  try {
    if (!toCheckModal(this))
      return;
    if (Tablespaces&&Tablespaces->poll()) {
      int cols=Tablespaces->describe().size();
      while(Tablespaces->poll()&&!Tablespaces->eof()) {
        for (int i=0;i<cols&&!Tablespaces->eof();i++)
          toPush(TablespaceValues,QString(Tablespaces->readValue()));
      }
      updateList();
      if (Tablespaces->eof()) {
        delete Tablespaces;
        Tablespaces=NULL;
      }
    }

    if (Files&&Files->poll()) {
      int cols=Files->describe().size();
       while(Files->poll()&&!Files->eof()) {
        for (int i=0;i<cols&&!Files->eof();i++)
          toPush(FileValues,QString(Files->readValue()));
      }
      if (Files->eof()) {
        delete Files;
        Files=NULL;
      }
    }

    if (Tablespaces==NULL&&Files==NULL) {
      updateList();
      Poll.stop();
    }
  } catch(const QString &exc) {
    delete Tablespaces;
    Tablespaces=NULL;
    delete Files;
    Files=NULL;
    Poll.stop();
    toStatusMessage(exc);
  }
}

QString toResultStorage::currentTablespace(void)
{
  QListViewItem *item=selectedItem();
  if (!item)
    throw QString("No tablespace selected");
  QString name;
  if (item->parent()||OnlyFiles)
    name=item->text(COLUMNS);
  else
    name=item->text(0);
  if (name.isEmpty())
    throw QString("Weird, empty tablespace name");
  return name;
}

QString toResultStorage::currentFilename(void)
{
  QListViewItem *item=selectedItem();
  if (!item||(!item->parent()&&!OnlyFiles))
    throw QString("No file selected");
  QString name=item->text(0);
  return name;
}

void toResultStorage::setOnlyFiles(bool only)
{
  saveSelected();
  if (only&&!OnlyFiles) {
    addColumn("Tablespace");
    setRootIsDecorated(false);
  } else if (!only&&OnlyFiles) {
    removeColumn(columns()-1);
    setRootIsDecorated(true);
  }
  OnlyFiles=only;
  updateList();
}
