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
 *      software in the executable aside from Oracle client libraries. You
 *      are also allowed to link this program with the Qt Non Commercial for
 *      Windows.
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

#ifndef __TOBARCHART_H
#define __TOBARCHART_H

#include <list>
#include <qwidget.h>

/** A widget that displays a barchart. Each line is added onto the next one.
 */

class toBarChart : public QWidget {
  Q_OBJECT

protected:
  list<list<double> > Values;
  list<QString> XValues;
  list<QString> Labels;
  bool Legend;
  int Grid;
  bool AxisText;
  double MaxValue;
  bool MaxAuto;
  QString YPostfix;
  int Samples;
  bool AutoSamples;
  QString Title;

  static double round(double round,bool up);
public:
  /** Create a new barchart.
   * @param parent Parent widget.
   * @param name Name of widget.
   * @param f Widget flags.
   */
  toBarChart(QWidget *parent=NULL,const char *name=NULL,WFlags f=0);

  /** Specify if legend should be displayed to the right of the graph, default is on.
   * @param on Whether to display legend or not.
   */
  void showLegend(bool on)
  { Legend=on; update(); }
  /** Check if legend is displayed or not.
   * @return If legend is displayed or not.
   */
  bool legend(void) const
  { return Legend; }

  /** Set title of the chart. Set to empty string to not display title.
   * @param title Title of chart.
   */
  void setTitle(const QString &title=QString::null)
  { Title=title; update(); }
  /** Get title of chart.
   * @return Title of chart.
   */
  const QString &title(void)
  { return Title; }

  /** Specify if a grid should be displayed in the graph, default is on.
   * @param div Number of parts to divide grid into.
   */
  void showGrid(int div=0)
  { Grid=div; update(); }
  /** Check if grid is displayed or not.
   * @return Number of parts to divide grid into.
   */
  int grid(void) const
  { return Grid; }

  /** Specify if a y-axis legend should be displayed in the graph, default is on.
   * @param on Whether to display legend or not.
   */
  void showAxisLegend(bool on)
  { AxisText=on; update(); }
  /** Check if y-axis legend is displayed or not.
   * @return If legend is displayed or not.
   */
  bool axisLegend(void) const
  { return AxisText; }

  /** Set y postfix value. This will be appended to the values in the axis legend.
   * @param postfix The string to append.
   */
  void setYPostfix(const QString &postfix)
  { YPostfix=postfix; update(); }
  /** Set max value on y-axis to auto.
   */
  void setMaxValueAuto(void)
  { MaxAuto=true; update(); }
  /** Set max value on y-axis to auto.
   */
  void setMaxValue(double maxVal)
  { MaxAuto=false; MaxValue=maxVal; update(); }
  /** Get maximum value on y-axis. Will not return the automatically determinned maximum value.
   * @return Maximum value on y-axis.
   */
  double maxValue(void) const
  { return MaxValue; }

  /** Set the number of samples on the x-axis. Setting samples to -1 will keep enough samples
   * to display one for each pixel of the screen. More samples than that will never be used
   * regardless of this setting.
   * @param samples Number of samples.
   */
  void setSamples(int samples=-1);
  /** Get the maximum number of samples on the x-axis.
   * @return Max number of samples.
   */
  int samples(void) const
  { return Samples; }

  /** Set the labels on the chart lines.
   * @param labels Labels of the lines. Empty labels will not show up in the legend.
   */
  void setLabels(const list<QString> &labels)
  { Labels=labels; update(); }
  /** Get the labels of the chart lines.
   * @return List of labels.
   */
  list<QString> &labels(void)
  { return Labels; }

  /** Add a new value set to the chart.
   * @param value New values for charts (One for each line).
   * @param label X-value on these values.
   */
  virtual void addValues(list<double> &value,const QString &xValues);

  /** Clear the values from the chart.
   */
  void clear(void)
  { Values.clear(); XValues.clear(); }

  /** Get list of labels
   * @return List of labels
   */
  list<QString> &xValues(void)
  { return XValues; }

  /** Get list of values.
   * @return Values in piechart.
   */
  list<list<double> > &values(void)
  { return Values; }

protected:
  /** Reimplemented for internal reasons.
   */
  virtual void paintEvent(QPaintEvent *e);
};

#endif
