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

#ifndef TOBARCHART_H
#define TOBARCHART_H

#include "tolinechart.h"

#include <list>

/** A widget that displays a barchart. Each line is added onto the next one.
 */

class toBarChart : public toLineChart {
  Q_OBJECT

protected:
  virtual void paintChart(QPainter *p,QRect &rect);
public:
  /** Create a new barchart.
   * @param parent Parent widget.
   * @param name Name of widget.
   * @param f Widget flags.
   */
  toBarChart(QWidget *parent=NULL,const char *name=NULL,WFlags f=0);

  /** Create a new barchart by copying all the data from another barchart.
   * @param chart Chart to copy data from.
   * @param parent Parent widget.
   * @param name Name of widget.
   * @param f Widget flags.
   */
  toBarChart(toBarChart *chart,QWidget *parent=NULL,const char *name=NULL,WFlags f=0);

  /** Open chart in new window.
   */
  virtual toLineChart *openCopy(QWidget *parent);
};

#endif
