//***************************************************************************
/*
 * TOra - An Oracle Toolkit for DBA's and developers
 * Copyright (C) 2000 GlobeCom AB
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
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
 *      Qt/UNIX or Qt/Windows products of TrollTech. And you are not
 *      permitted to distribute binaries compiled against these libraries
 *      without written consent from GlobeCom AB. Observe that this does not
 *      disallow linking to the Qt Free Edition.
 *
 * All trademarks belong to their respective owners.
 *
 ****************************************************************************/

TO_NAMESPACE;

#include "toresultreferences.h"
#include "tomain.h"
#include "totool.h"
#include "toconf.h"
#include "tosql.h"

toResultReferences::toResultReferences(toConnection &conn,QWidget *parent,const char *name)
  : toResultView(false,false,conn,parent,name)
{
  setReadAll(true);
  addColumn("Owner");
  addColumn("Object");
  addColumn("Constraint");
  addColumn("Condition");
  addColumn("Enabled");
  addColumn("Delete Rule");
  setSQLName("toResultReferences");
}

static toSQL SQLConsColumns("toResultReferences:ForeignColumns",
			    "SELECT Column_Name FROM All_Cons_Columns\n"
			    " WHERE Owner = :f1<char[31]> AND Constraint_Name = :f2<char[31]>\n"
			    " ORDER BY Position",
			    "Get columns of foreign constraint, must return same number of cols");

QString toResultReferences::constraintCols(const QString &conOwner,const QString &conName)
{
  otl_stream Query(1,
		   SQLConsColumns(Connection),
		   Connection.connection());

  Query<<conOwner.utf8();
  Query<<conName.utf8();

  QString ret;
  while(!Query.eof()) {
    char buffer[31];
    Query>>buffer;
    if (!ret.isEmpty())
      ret.append(",");
    ret.append(QString::fromUtf8(buffer));
  }
  return ret;
}

static toSQL SQLConstraints("toResultReferences:References",
			    "SELECT Owner,\n"
			    "       Table_Name,\n"
			    "       Constraint_Name,\n"
			    "       R_Owner,\n"
			    "       R_Constraint_Name,\n"
			    "       Status,\n"
			    "       Delete_Rule\n"
			    "  FROM all_constraints a\n"
			    " WHERE constraint_type = 'R'\n"
			    "   AND (r_owner,r_constraint_name) IN (SELECT b.owner,b.constraint_name\n"
			    "                                         FROM all_constraints b\n"
			    "                                        WHERE b.OWNER = :owner<char[31]>\n"
			    "                                          AND b.TABLE_NAME = :tab<char[31]>)\n"
			    " ORDER BY Constraint_Name",
			    "List the references from foreign constraints to specified table, must return same columns");
static toSQL SQLDependencies("toResultReferences:Dependencies",
			     "SELECT owner,name,type||' '||dependency_type\n"
			     "  FROM dba_dependencies\n"
			     " WHERE referenced_owner = :owner<char[31]>\n"
			     "   AND referenced_name = :tab<char[31]>\n"
			     " ORDER BY owner,type,name",
			     "List the dependencies from other objects to this object, must return same number of columns");

void toResultReferences::query(const QString &sql,const list<QString> &param)
{
  QString Owner;
  QString TableName;
  list<QString>::iterator cp=((list<QString> &)param).begin();
  if (cp!=((list<QString> &)param).end())
    Owner=*cp;
  cp++;
  if (cp!=((list<QString> &)param).end())
    TableName=(*cp);

  Query=NULL;
  RowNumber=0;

  clear();

  try {
    int MaxColSize=toTool::globalConfig(CONF_MAX_COL_SIZE,DEFAULT_MAX_COL_SIZE).toInt();

    otl_stream Query(1,
		     SQLConstraints(Connection),
		     Connection.connection());

    Description=Query.describe_select(DescriptionLen);

    Query<<Owner.utf8();
    Query<<TableName.utf8();

    QListViewItem *item=NULL;
    char *buffer=new char[MaxColSize+1];
    buffer[MaxColSize]=0;
    try {
      while(!Query.eof()) {
        item=new QListViewItem(this,item,NULL);

        Query>>buffer;
        QString consOwner(QString::fromUtf8(buffer));
        Query>>buffer;
        item->setText(1,QString::fromUtf8(buffer));
        Query>>buffer;
        QString consName(QString::fromUtf8(buffer));
        QString colNames(constraintCols(Owner,QString::fromUtf8(buffer)));
        item->setText(0,consOwner);
        item->setText(2,consName);
        Query>>buffer;
        QString rConsOwner(QString::fromUtf8(buffer));
        Query>>buffer;
        QString rConsName(QString::fromUtf8(buffer));
        Query>>buffer;
        item->setText(4,QString::fromUtf8(buffer));
        QString Condition;

        Condition="foreign key (";
        Condition.append(colNames);
        Condition.append(") references ");
        Condition.append(rConsOwner);
        Condition.append(".");
        QString cols(constraintCols(rConsOwner,rConsName));

        Condition.append(TableName);
        Condition.append("(");
        Condition.append(cols);
        Condition.append(")");

        item->setText(3,Condition);
        Query>>buffer;
        item->setText(5,QString::fromUtf8(buffer));
      }

      otl_stream Deps(1,
	  	      SQLDependencies(Connection),
		      Connection.connection());
      Deps<<Owner.utf8();
      Deps<<TableName.utf8();
      while(!Deps.eof()) {
        item=new QListViewItem(this,item,NULL);
        Deps>>buffer;
        item->setText(0,QString::fromUtf8(buffer));
        Deps>>buffer;
        item->setText(1,QString::fromUtf8(buffer));
        Deps>>buffer;
        item->setText(3,QString::fromUtf8(buffer));
        item->setText(4,"DEPENDENCY");
      }
    } catch (...) {
      delete buffer;
      throw;
    }
    delete buffer;
  } TOCATCH
  updateContents();
}
