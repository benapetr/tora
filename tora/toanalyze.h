//***************************************************************************
/*
 * TOra - An Oracle Toolkit for DBA's and developers
 * Copyright (C) 2000-2001,2001 GlobeCom AB
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

#ifndef __TOANALYZE_H
#define __TOANALYZE_H

#include <list>

#include "totool.h"
#include "tonoblockquery.h"

class toResultView;
class QPopupMenu;
class QComboBox;
class QSpinBox;
class QTimer;

class toAnalyze : public toToolWidget {
  Q_OBJECT

  toResultView *Statistics;
  QComboBox *Schema;
  QComboBox *Operation;
  QComboBox *For;
  QSpinBox *Sample;
  QSpinBox *Parallel;
  QLabel *Current;
  QToolButton *Stop;
  QTimer *Poll;

  std::list<toNoBlockQuery *> Running;
  std::list<QString> Pending;

  QPopupMenu *ToolMenu;
public:
  toAnalyze(QWidget *parent,toConnection &connection);

public slots:
  virtual void changeOperation(int);
  virtual void execute(void);
  virtual void poll(void);
  virtual void stop(void);
  virtual void refresh(void); 
  virtual void windowActivated(QWidget *widget);
};

#endif
