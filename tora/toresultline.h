//***************************************************************************
/* $Id$
**
** Copyright (C) 2000-2001 GlobeCom AB.  All rights reserved.
**
** This file is part of the Toolkit for Oracle.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.globecom.net/tora/ for more information.
**
** Contact tora@globecom.se if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef __TORESULTLINE_H
#define __TORESULTLINE_H

#include <time.h>

#include "toresult.h"
#include "tolinechart.h"

class toSQL;

/** Display the result of a query in a piechart. The first column of the query should
 * contain the x value and the rest of the columns should be values of the diagram. The
 * legend is the column name. Connects to the tool timer for updates automatically.
 */

class toResultLine : public toLineChart, public toResult {
  Q_OBJECT
  /** Display flow in change per second instead of actual values.
   */
  bool Flow;
  /** Timestamp of last fetch.
   */
  time_t LastStamp;
  /** Last read values.
   */
  std::list<double> LastValues;
  /** Query to run.
   */
  QString SQL;
  /** Parameters to query
   */
  toQList Param;
  void query(const QString &sql,const toQList &param,bool first);
public:
  /** Create widget.
   * @param parent Parent of list.
   * @param name Name of widget.
   */
  toResultLine(QWidget *parent,const char *name=NULL);

  /** Display actual values or flow/s.
   * @param on Display flow or absolute values.
   */
  void setFlow(bool on)
  { Flow=on; }
  /** Return if flow is displayed.
   * @return If flow is used.
   */
  bool flow(void)
  { return Flow; }

  /** Set SQL to query.
   * @param sql Query to run.
   */
  void setSQL(const QString &sql)
  { SQL=sql; }
  /** Set the SQL statement of this list.
   * @param sql SQL containing statement.
   */
  void setSQL(toSQL &sql);

  /** Reimplemented for internal reasons.
   */
  virtual void query(const QString &sql,const toQList &param)
  { query(sql,param,true); }
  /** Perform the specified query.
   * @param sql SQL containing statement.
   */
  void query(toSQL &sql);
  /** Reimplemented for internal reasons.
   */
  void query(const QString &sql)
  { toQList p; query(sql,p); }
  /** Reimplemented for internal reasons.
   */
  virtual void clear(void)
  { LastStamp=0; LastValues.clear(); toLineChart::clear(); }
  /** Transform valueset. Make it possible to perform more complex transformation.
   * called directly before adding the valueset to the chart. After flow transformation.
   * Default is passthrough.
   * @param input The untransformed valueset.
   * @return The valueset actually added to the chart.
   */
  virtual std::list<double> transform(std::list<double> &input);
public slots:
  /** Read another value to the chart.
   */
  virtual void refresh(void)
  { query(SQL,Param,false); }
  /** Reimplemented for internal reasons.
   */
  virtual void changeParams(const QString &Param1)
  { toQList p; p.insert(p.end(),Param1); query(SQL,p); }
  /** Reimplemented for internal reasons.
   */
  virtual void changeParams(const QString &Param1,const QString &Param2)
  { toQList p; p.insert(p.end(),Param1); p.insert(p.end(),Param2); query(SQL,p); }
  /** Reimplemented for internal reasons.
   */
  virtual void changeParams(const QString &Param1,const QString &Param2,const QString &Param3)
  { toQList p; p.insert(p.end(),Param1); p.insert(p.end(),Param2); p.insert(p.end(),Param3); query(SQL,p); }
};

#endif
