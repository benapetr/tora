/*****
*
* TOra - An Oracle Toolkit for DBA's and developers
* Copyright (C) 2003-2005 Quest Software, Inc
* Portions Copyright (C) 2005 Other Contributors
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
*      You may link this product with any GPL'd Qt library such as Qt/Free
*
* All trademarks belong to their respective owners.
*
*****/

#ifndef TORESULTDATASINGLE_H
#define TORESULTDATASINGLE_H

#include "config.h"
#include "toresultmodel.h"

#include <QWidget>
#include <QList>
#include <QPointer>

class QScrollArea;
class QCheckBox;
class QLineEdit;


/**
 * This widget is used for single record view in the content editor.
 *
 */
class toResultDataSingle : public QWidget
{
    Q_OBJECT;

    int Row;
    QScrollArea *Container;
    QList<QCheckBox *> Null;
    QList<QLineEdit *> Value;

    QPointer<toResultModel> Model;

public:
    toResultDataSingle(QWidget *parent);

    void changeSource(toResultModel *, int row);
    void changeRow(int row);

    inline int currentRow(void) const {
        return Row;
    }

private slots:
    virtual void showMemo(int row);
    void saveRow(void);
};

#endif
