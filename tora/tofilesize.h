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

#ifndef __TOFILESIZE_H
#define __TOFILESIZE_H

#include <qbuttongroup.h>
#include <qwidget.h>

class QRadioButton;
class QSpinBox;

class toFilesize : public QButtonGroup {
  Q_OBJECT

  QSpinBox *Value;
  QRadioButton *MBSize;
  QRadioButton *KBSize;
  void setup(void);
protected slots:
  void changeType(bool);
  void changedSize(void)
  { emit valueChanged(); }
signals:
  void valueChanged(void); 
public:
  toFilesize(QWidget* parent=0,const char* name=0);
  toFilesize(const QString &title,QWidget* parent=0,const char* name=0);
  void setValue(int sizeInKB);
  int value(void);
  QString sizeString(void);
};

#endif
