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

#ifndef __TORESULT_H
#define __TORESULT_H

#include <list>

#include "toqvalue.h"

class toTimer;
class toConnection;

/** Abstract baseclass of widgets that can perform queries. Usefull because you can execute
 * the query without knowing how it's UI is presented.
 */

class toResult {
protected:
  /** Get the current connection from the closest tool.
   * @return Reference to connection.
   */
  toConnection &connection(void);
  /** Get the timer associated with the closest tool.
   * @return Pointer to tool timer.
   */
  toTimer *timer(void);
public:
  toResult(void)
  { }
  virtual ~toResult()
  { }

  /** Re execute last query
   */
  virtual void refresh(void) = 0;
  /** Perform a query.
   * @param sql Execute an SQL statement.
   * @param params Parameters needed as input to execute statement.
   */
  virtual void query(const QString &sql,const toQList &params) = 0;
  /** Reexecute with changed parameters.
   * @param Param1 First parameter.
   */
  virtual void changeParams(const QString &Param1) = 0;
  /** Reexecute with changed parameters.
   * @param Param1 First parameter.
   * @param Param1 Second parameter.
   */
  virtual void changeParams(const QString &Param1,const QString &Param2) = 0;
  /** Reexecute with changed parameters.
   * @param Param1 First parameter.
   * @param Param2 Second parameter.
   * @param Param3 Third parameter.
   */
  virtual void changeParams(const QString &Param1,const QString &Param2,const QString &Param3) = 0;
};

#endif
