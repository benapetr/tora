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

#ifndef __TOSTORAGEDEFINITION_H
#define __TOSTORAGEDEFINITION_H

#include <qwidget.h>

class QGroupBox;
class QFrame;
class QCheckBox;
class QLabel;
class QSpinBox;
class toFilesize;
class QGroupBox;

class toStorageDefinition : public QWidget
{ 
  Q_OBJECT

  QGroupBox *GroupBox1;
  QFrame *Line1;
  QCheckBox *UnlimitedExtent;
  QLabel *TextLabel1_2;
  QSpinBox *MaximumExtent;
  QSpinBox *InitialExtent;
  QLabel *TextLabel1;
  toFilesize *NextSize;
  toFilesize *InitialSize;
  QGroupBox *Optimal;
  toFilesize *OptimalSize;
  QCheckBox *OptimalNull;
  QSpinBox *PCTIncrease;
  QLabel *TextLabel2;

public:
  toStorageDefinition(QWidget *parent=0,const char *name=0,WFlags fl=0);

  void forRollback(bool val);

  QString getSQL(void);

public slots:
  void optimalExtents(bool val);
  void unlimitedExtents(bool val);
};

#endif
