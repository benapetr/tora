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

#ifndef TOGLOBALSETTING_H
#define TOGLOBALSETTING_H

#include "todatabasesettingui.h"
#include "toglobalsettingui.h"
#include "tohelp.h"
#include "totool.h"
#include "totoolsettingui.h"

class toGlobalSetting : public toGlobalSettingUI, public toSettingTab {
public:
  toGlobalSetting(QWidget *parent=0,const char *name=0,WFlags fl=0);

  virtual void saveSetting(void);

  virtual void pluginBrowse(void);
  virtual void sqlBrowse(void);
  virtual void helpBrowse(void);
  virtual void sessionBrowse(void);
};

class toDatabaseSetting : public toDatabaseSettingUI, public toSettingTab {
public:
  toDatabaseSetting(QWidget *parent=0,const char *name=0,WFlags fl=0);

  virtual void saveSetting(void);
  virtual void numberFormatChange();
};

class toToolSetting : public toToolSettingUI, public toSettingTab {
public:
  toToolSetting(QWidget *parent=0,const char *name=0,WFlags fl=0);

  virtual void saveSetting(void);
  virtual void changeEnable(void);
};

#endif
