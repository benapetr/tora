/****************************************************************************
 *
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
 *      with the Qt and Oracle Client libraries and distribute executables,
 *      as long as you follow the requirements of the GNU GPL in regard to
 *      all of the software in the executable aside from Qt and Oracle client
 *      libraries.
 *
 ****************************************************************************/

TO_NAMESPACE;

#include "toresultreferences.h"
#include "tomain.h"
#include "totool.h"
#include "toconf.h"
#include "tosql.h"

toResultReferences::toResultReferences(toConnection &conn,QWidget *parent,const char *name=NULL)
  : toResultView(false,false,conn,parent,name)
{
  setReadAll(true);
  addColumn("Owner");
  addColumn("Object");
  addColumn("Constraint");
  addColumn("Condition");
  addColumn("Enabled");
  addColumn("Delete Rule");
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

  Query<<(const char *)conOwner;
  Query<<(const char *)conName;

  QString ret;
  while(!Query.eof()) {
    char buffer[31];
    Query>>buffer;
    if (!ret.isEmpty())
      ret.append(",");
    ret.append(buffer);
  }
  return ret;
}

static toSQL SQLConstraints("toResultReferences:References",
			    "SELECT Owner,
       Table_Name,
       Constraint_Name,
       R_Owner,
       R_Constraint_Name,
       Status,
       Delete_Rule
  FROM all_constraints a
 WHERE constraint_type = 'R'
   AND (r_owner,r_constraint_name) IN (SELECT b.owner,b.constraint_name
                                         FROM all_constraints b
                                        WHERE b.OWNER = :owner<char[31]>
                                          AND b.TABLE_NAME = :tab<char[31]>)
 ORDER BY Constraint_Name",
			    "List the references from foreign constraints to specified table, must return same columns");
static toSQL SQLDependencies("toResultReferences:Dependencies",
			     "SELECT owner,name,type||' '||dependency_type
  FROM dba_dependencies
 WHERE referenced_owner = :owner<char[31]>
   AND referenced_name = :tab<char[31]>
 ORDER BY owner,type,name",
			     "List the dependencies from other objects to this object, must return same number of columns");

QString toResultReferences::query(const QString &sql,const list<QString> &param)
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

    Query<<Owner;
    Query<<TableName;

    QListViewItem *item=NULL;
    while(!Query.eof()) {
      item=new QListViewItem(this,item,NULL);

      char buffer[MaxColSize+1];
      buffer[MaxColSize]=0;
      Query>>buffer;
      QString consOwner(buffer);
      Query>>buffer;
      item->setText(1,buffer);
      Query>>buffer;
      QString consName(buffer);
      QString colNames(constraintCols(Owner,buffer));
      item->setText(0,consOwner);
      item->setText(2,consName);
      Query>>buffer;
      QString rConsOwner(buffer);
      Query>>buffer;
      QString rConsName(buffer);
      Query>>buffer;
      item->setText(4,buffer);
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
      item->setText(5,buffer);
    }

    otl_stream Deps(1,
		    SQLDependencies(Connection),
		    Connection.connection());
    Deps<<Owner;
    Deps<<TableName;
    while(!Deps.eof()) {
      item=new QListViewItem(this,item,NULL);
      char buffer[MaxColSize+1];
      buffer[MaxColSize]=0;
      Deps>>buffer;
      item->setText(0,buffer);
      Deps>>buffer;
      item->setText(1,buffer);
      Deps>>buffer;
      item->setText(3,buffer);
      item->setText(4,"DEPENDENCY");
    }
  } catch (const QString &str) {
    toStatusMessage((const char *)str);
    updateContents();
    return str;
  } catch (const otl_exception &exc) {
    toStatusMessage((const char *)exc.msg);
    updateContents();
    return QString((const char *)exc.msg);
  }
  updateContents();
  return "";
}
