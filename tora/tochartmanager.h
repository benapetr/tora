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

#ifndef TOCHARTMANAGER_H
#define TOCHARTMANAGER_H

#include "tohelp.h"

#include <qvbox.h>
#include <qfile.h>

#include <list>
#include <map>

class toChartManager;
class toLineChart;
class toListView;
class toResult;
class toChartSetup;
class toChartAlarm;

class toChartReceiver : public QObject {
  Q_OBJECT

  QString LastName;
  toChartManager *Parent;
  toLineChart *Chart;
  toResult *Result;
public:
  toChartReceiver(toChartManager *parent,toLineChart *chart);

  toLineChart *chart(void)
  { return Chart; }
  toResult *result(void);
  QString name(void);
public slots:
  void valueAdded(std::list<double> &value,const QString &xValues);
};

class toChartManager : public QVBox, public toHelpContext {
  Q_OBJECT

  enum action {
    StatusMessage,
    Ignore
  };

  struct alarmSignal {
    action Action;
    QString xValue;
    QString Chart;
    QString Alarm;
    alarmSignal();
    alarmSignal(action act,const QString &xvalue,const QString &chart,const QString &alarm)
      : Action(act),xValue(xvalue),Chart(chart),Alarm(alarm)
    { }
  };

  struct chartTrack {
    QFile File;
    bool Persistent;

    chartTrack()
    { Persistent=true; }
    chartTrack(const QString &file,bool pers)
      : File(file)
    { Persistent=pers; }
    chartTrack(const chartTrack &fil)
      : File(fil.File.name())
    { Persistent=fil.Persistent; }
    const chartTrack &operator =(const chartTrack &fil)
    {
      if (fil.File.name()!=File.name()) {
	File.close();
	File.setName(fil.File.name());
      }
      Persistent=fil.Persistent;
      return *this;
    }
  };

  struct chartAlarm {
    enum operation {
      Any,
      All,
      Sum,
      Average,
      Max,
      Min
    };
    enum comparison {
      Equal,
      NotEqual,
      Less,
      Greater,
      LessEqual,
      GreaterEqual
    };

    operation Operation;
    comparison Comparison;
    action Action;
    double Value;
    std::list<int> Columns;
    bool Persistent;
    bool Signal;

    chartAlarm();
    chartAlarm(const QString &,bool persistent);
    chartAlarm(operation oper,comparison comp,action act,
	       double value,std::list<int> &cols,
	       bool persistent);
    QString toString(void);

    bool checkValue(double value);

    void valueAdded(toChartManager *manager,const QString &str,
		    std::list<double> &value,const QString &xValue);
  };

  QTimer Timer;
  QTimer Refresh;
  toListView *List;
  QPopupMenu *ToolMenu;

  std::map<QString,std::list<chartAlarm> > Alarms;
  std::map<QString,chartTrack> Files;

  std::list<toChartReceiver *> Charts;
  std::list<alarmSignal> SignalAlarms;

  friend struct chartAlarm;
  friend struct alarmSignal;

  toChartReceiver *selectedChart(void);
  void loadSettings(void);
  void saveSettings(void);
public:
  toChartManager(QWidget *parent);
  ~toChartManager();

  void valueAdded(toLineChart *chart,const QString &chartName,
		  std::list<double> &vale,const QString &xValue);

  friend class toChartSetup;
  friend class toChartAlarm;
public slots:
  void refresh(void);
  void addChart(toLineChart *chart);
  void setupChart(toLineChart *chart);
  void removeChart(toLineChart *chart);
  void windowActivated(QWidget *widget);
  void alarm(void);
  void setupChart(void);
  void openChart(void);
};

#endif
