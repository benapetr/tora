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

#ifndef TORESULTSCHEMA_H
#define TORESULTSCHEMA_H

#include "config.h"

#include "toresultcombo.h"

class toConnection;


/**
 * This widget displays a list of schemas
 *
 */
class toResultSchema : public toResultCombo
{
    Q_OBJECT;

    QString ConnectionKey;

public:

    /**
     * Create the widget.
     *
     * @param parent Parent widget.
     * @param name Name of widget.
     */
    toResultSchema(toConnection &conn,
                   QWidget *parent,
                   const char *name = NULL);

private slots:

    // stores last schema selected in qsettings
    void updateLastSchema(const QString &schema);

public slots:

    /**
     * Updates connections to use schema
     *
     */
    void update(const QString &schema);


    /**
     * Update to currently selected schema
     *
     */
    void update(void);
};

#endif
