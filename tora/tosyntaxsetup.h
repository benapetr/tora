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

#ifndef __TOSYNTAXSETUP_H
#define __TOSYNTAXSETUP_H

#include <map>
#include "totool.h"
#include "tosyntaxsetupui.h"
#include "tohighlightedtext.h"
#include "tohelp.h"

class QFont;

class toSyntaxSetup : public toSyntaxSetupUI, public toSettingTab { 
  QString List;
  QListBoxItem *Current;
  std::map<QString,QColor> Colors;
  toSyntaxAnalyzer Analyzer;
public:
  toSyntaxSetup(QWidget *parent=0,const char *name=0,WFlags fl=0);
  virtual void saveSetting(void);
  void checkFixedWidth(const QFont &fnt);
public slots:
  virtual void changeLine(QListBoxItem *);
  virtual void selectColor(void);
  virtual void selectFont(void);
  virtual void selectText(void);
  virtual void selectResultFont(void);
  virtual void changeUpper(bool val)
  { Example->setKeywordUpper(val); Example->update(); }
  virtual void changeHighlight(bool val)
  { Example->setHighlight(val); Example->update(); }
};

#endif
