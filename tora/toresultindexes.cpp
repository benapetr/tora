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

#include "toresultindexes.h"
#include "tomain.h"
#include "totool.h"
#include "toconf.h"
#include "tosql.h"
#include "toconnection.h"
#include "tonoblockquery.h"

#include "toresultindexes.moc"

bool toResultIndexes::canHandle(toConnection &conn)
{
  return toIsOracle(conn)||conn.provider()=="MySQL";
}

toResultIndexes::toResultIndexes(QWidget *parent,const char *name)
  : toResultView(false,false,parent,name)
{
  setReadAll(true);
  addColumn("Index Name");
  addColumn("Columns");
  addColumn("Type");
  addColumn("Unique");
  setSQLName("toResultIndexes");

  Query=NULL;
  connect(&Poll,SIGNAL(timeout()),this,SLOT(poll()));
}

toResultIndexes::~toResultIndexes()
{
  delete Query;
}

static toSQL SQLColumns("toResultIndexes:Columns",
			"SELECT Column_Name FROM sys.All_Ind_Columns\n"
			" WHERE Index_Owner = :f1<char[101]> AND Index_Name = :f2<char[101]>\n"
			" ORDER BY Column_Position",
			"List columns an index is built on");

QString toResultIndexes::indexCols(const QString &indOwner,const QString &indName)
{
  toQuery query(connection(),SQLColumns,indOwner,indName);

  QString ret;
  while(!query.eof()) {
    if (!ret.isEmpty())
      ret.append(",");
    ret.append(query.readValue());
  }
  return ret;
}

static toSQL SQLListIndex("toResultIndexes:ListIndex",
			  "SELECT Owner,\n"
			  "       Index_Name,\n"
			  "       Index_Type,\n"
			  "       Uniqueness\n"
			  "  FROM sys.All_Indexes\n"
			  " WHERE Table_Owner = :f1<char[101]>\n"
			  "   AND Table_Name = :f2<char[101]>\n"
			  " ORDER BY Index_Name",
			  "List the indexes available on a table",
			  "8.0");
static toSQL SQLListIndex7("toResultIndexes:ListIndex",
			   "SELECT Owner,\n"
			   "       Index_Name,\n"
			   "       'NORMAL',\n"
			   "       Uniqueness\n"
			   "  FROM sys.All_Indexes\n"
			   " WHERE Table_Owner = :f1<char[101]>\n"
			   "   AND Table_Name = :f2<char[101]>\n"
			   " ORDER BY Index_Name",
			   QString::null,
			   "7.3");

static toSQL SQLListIndexMySQL("toResultIndexes:ListIndex",
			       "SHOW INDEX FROM :tab<noquote>",
			       QString::null,
			       "3.0",
			       "MySQL");

void toResultIndexes::query(const QString &sql,const toQList &param)
{
  if (!handled())
    return;

  if (Query)
    delete Query;
  Query=NULL;

  toConnection &conn=connection();
  if(conn.provider()=="Oracle")
    Type=Oracle;
  else if (conn.provider()=="MySQL")
    Type=MySQL;
  else
    return;
    
  toQList::iterator cp=((toQList &)param).begin();
  if (cp!=((toQList &)param).end())
    Owner=*cp;
  cp++;
  if (cp!=((toQList &)param).end())
    TableName=(*cp);

  RowNumber=0;

  clear();

  try {
    toQuery query(connection());
    toQList par;
    if (Type==Oracle)
      par.insert(par.end(),Owner);
    par.insert(par.end(),TableName);

    Query=new toNoBlockQuery(connection(),toQuery::Normal,
			     toSQL::string(SQLListIndex,conn),par);
    Poll.start(100);
  } TOCATCH
}

void toResultIndexes::poll(void)
{
  try {
    if (Query&&Query->poll()) {
      while(Query->poll()&&!Query->eof()) {
	if (Type==Oracle) {
	  Last=new toResultViewItem(this,NULL);
	  
	  QString indexOwner(Query->readValue());
	  QString indexName(Query->readValue());
	  Last->setText(0,indexName);
	  Last->setText(1,indexCols(indexOwner,indexName));
	  Last->setText(2,Query->readValue());
	  Last->setText(3,Query->readValue());
	} else {
	  Query->readValue(); // Tablename
	  int unique=Query->readValue().toInt();
	  QString name=Query->readValue();
	  Query->readValue(); // SeqID
	  QString col=Query->readValue();
	  Query->readValue();
	  Query->readValue();
	  Query->readValue();
	  Query->readValue();
	  Query->readValue();
	  if (Last&&Last->text(0)==name)
	    Last->setText(1,Last->text(1)+","+col);
	  else {
	    Last=new toResultViewItem(this,NULL);
	    Last->setText(0,name);
	    Last->setText(1,col);
	    Last->setText(2,(name=="PRIMARY")?"PRIMARY":"INDEX");
	    Last->setText(3,unique?"UNIQUE":"NONUNIQUE");
	  }
	}
      }
      if (Query->eof()) {
	delete Query;
	Query=NULL;
	Poll.stop();
      }
    }
  } catch(const QString &exc) {
    delete Query;
    Query=NULL;
    Poll.stop();
    toStatusMessage(exc);
  }
}
