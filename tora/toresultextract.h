//***************************************************************************
/*
 * TOra - An Oracle Toolkit for DBA's and developers
 * Copyright (C) 2003 Quest Software, Inc
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
 *      these libraries without written consent from Quest Software, Inc.
 *      Observe that this does not disallow linking to the Qt Free Edition.
 *
 * All trademarks belong to their respective owners.
 *
 ****************************************************************************/

#ifndef TORESULTEXTRACT_H
#define TORESULTEXTRACT_H

#include "toresult.h"
#include "toworksheet.h"

class toSQL;

/** This widget displays the SQL used to recreate an object. It uses the
 * @ref toExtract class to do this.
 */

class toResultExtract : public toWorksheet, public toResult {
  Q_OBJECT

  bool Prompt;

public:
  /** Create the widget.
   * @param parent Parent widget.
   * @param name Name of widget.
   */
  toResultExtract(bool prompt,QWidget *parent,const char *name=NULL);

  /** Ignores sql and extect object name and owner as parameters.
   */
  virtual void query(const QString &sql,const toQList &param);

  /** Support Oracle
   */
  virtual bool canHandle(toConnection &conn);
public slots:
  /** Reimplemented for internal reasons.
   */
  virtual void refresh(void)
  { toResult::refresh(); }
  /** Reimplemented for internal reasons.
   */
  virtual void changeParams(const QString &Param1)
  { toResult::changeParams(Param1); }
  /** Reimplemented For internal reasons.
   */
  virtual void changeParams(const QString &Param1,const QString &Param2)
  { toResult::changeParams(Param1,Param2); }
  /** Reimplemented for internal reasons.
   */
  virtual void changeParams(const QString &Param1,const QString &Param2,const QString &Param3)
  { toResult::changeParams(Param1,Param2,Param3); }
};

#endif
