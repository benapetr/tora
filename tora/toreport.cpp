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
#include "toextract.h"
#include "toreport.h"

#ifndef WIN32
#include <unistd.h>
#else
#include <winsock.h>
#endif

#include <qdatetime.h>

static bool SameContext(const QString &str,const QString &str2)
{
  if (str.startsWith(str2)) {
    if (str.length()==str2.length())
      return true;
    if (str.length()>str2.length()&&
	str[str2.length()]==QChar('\001'))
      return true;
  }
  return false;
}

#if 0
static std::list<QString>::iterator FindItem(std::list<QString> &desc,
					     std::list<QString>::iterator i,
					     int level,
					     const QString &context,
					     const QString &search)
{
  while(i!=desc.end()) {
    if (!SameContext(*i,context))
      return desc.end();
    if (toExtract::partDescribe(*i,level)==search)
      return i;
    i++;
  }
  return desc.end();
}
#endif

static QString DescribePart(std::list<QString> &desc,
			    std::list<QString>::iterator i,
			    int level,
			    const QString &parentContext)
{
  do {
    if (!SameContext(*i,parentContext))
      return QString::null;
    QString part=toExtract::partDescribe(*i,level);
    QString next=toExtract::partDescribe(*i,level+1);
    if (next.isNull())
      return toExtract::partDescribe(*i,level);
    else
      return QString("<H%1>%2</H%1>\n").
	arg(level).
	arg(part);
  } while(i!=desc.end());
}

QString toGenerateReport(toConnection &conn,std::list<QString> &desc)
{
  char host[1024];
  gethostname(host,1024);

  QString db=conn.host();
  if (db.length()&&db!="*")
    db+=":";
  else
    db=QString::null;
  db+=conn.database();

  QString ret("<HTML><HEAD><TITLE>Report on database %3</TITLE></HEAD>\n"
	      "<BODY><H1>Report on database %3</H1>\n"
	      "<TABLE BORDER=0>\n"
	      "<TR><TD>Generated by:</TD><TD>TOra, Version %1</TD></TR>\n"
	      "<TR><TD>At:</TD><TD>%2</TD></TR>\n"
	      "<TR><TD>From:</TD><TD>%3, an %4 %5 database</TD></TR>\n"
	      "<TR><TD>On:</TD><TD>%6</TD></TR>\n"
	      "</TABLE>\n");
  ret.
    arg(TOVERSION).
    arg(host).
    arg(db).
    arg(conn.provider()).
    arg(conn.version()).
    arg(QDateTime::currentDateTime().toString());

  std::list<QString>::iterator i=desc.begin();
  while(i!=desc.end()) {
    QString context=toExtract::partDescribe(*i,0);
    ret+=DescribePart(desc,i,1,context);
  }

  return ret;

}
