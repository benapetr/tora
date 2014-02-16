
/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * TOra - An Oracle Toolkit for DBA's and developers
 * 
 * Shared/mixed copyright is held throughout files in this product
 * 
 * Portions Copyright (C) 2000-2001 Underscore AB
 * Portions Copyright (C) 2003-2005 Quest Software, Inc.
 * Portions Copyright (C) 2004-2013 Numerous Other Contributors
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
 * along with this program as the file COPYING.txt; if not, please see
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
 * 
 *      As a special exception, you have permission to link this program
 *      with the Oracle Client libraries and distribute executables, as long
 *      as you follow the requirements of the GNU GPL in regard to all of the
 *      software in the executable aside from Oracle client libraries.
 * 
 * All trademarks belong to their respective owners.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef TOGLOBALSETTING_H
#define TOGLOBALSETTING_H

#include "core/totool.h"

#include "ui_todatabasesettingui.h"
#include "ui_toglobalsettingui.h"
#include "ui_totoolsettingui.h"
#include "ui_connectioncolorsdialogui.h"


class ConnectionColorsDialog : public QDialog, public Ui::ConnectionColorsDialog
{
    Q_OBJECT

public:
    ConnectionColorsDialog(QWidget * parent);

private:
    void newItem(const QString & color, const QString & desc);
private slots:
    void addItem();
    void deleteItem();
    void accept();
};


class toGlobalSetting : public QWidget
    , public Ui::toGlobalSettingUI
    , public toSettingTab
{
    Q_OBJECT;

public:
    toGlobalSetting(QWidget *parent = 0, const char *name = 0, Qt::WFlags fl = 0);

public slots:
    virtual void saveSetting(void);

    virtual void sqlBrowse(void);
    virtual void helpBrowse(void);
    virtual void sessionBrowse(void);
    virtual void cacheBrowse(void);

    void oracleBrowse(void);
    void mysqlBrowse(void);
    void pqsqlBrowse(void);
    void graphvizBrowse(void);

    void ColorizedConnectionsConfigure_clicked();
};

class toDatabaseSetting : public QWidget
    , public Ui::toDatabaseSettingUI
    , public toSettingTab
{
    Q_OBJECT;

public:
    toDatabaseSetting(QWidget *parent = 0, const char *name = 0, Qt::WFlags fl = 0);

public slots:
    virtual void saveSetting(void);
    virtual void numberFormatChange();
    virtual void IndicateEmptyColor_clicked();
};

class toToolSetting : public QWidget
    , public Ui::toToolSettingUI
    , public toSettingTab
{
    Q_OBJECT;

public:
    toToolSetting(QWidget *parent = 0, const char *name = 0, Qt::WFlags fl = 0);

public slots:
    virtual void saveSetting(void);
    virtual void changeEnable(void);
};

#endif
