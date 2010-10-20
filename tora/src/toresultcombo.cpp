
/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * TOra - An Oracle Toolkit for DBA's and developers
 * 
 * Shared/mixed copyright is held throughout files in this product
 * 
 * Portions Copyright (C) 2000-2001 Underscore AB
 * Portions Copyright (C) 2003-2005 Quest Software, Inc.
 * Portions Copyright (C) 2004-2009 Numerous Other Contributors
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
 *      these libraries. 
 * 
 *      You may link this product with any GPL'd Qt library.
 * 
 * All trademarks belong to their respective owners.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "utils.h"

#include "toconf.h"
#include "toconnection.h"
#include "toeventquery.h"
#include "toresultcombo.h"
#include "tosql.h"
#include "totool.h"


toResultCombo::toResultCombo(QWidget *parent, const char *name)
  : QComboBox(parent), Query(0), SelectionPolicy(None)
{
    setObjectName(name);
    connect(this, SIGNAL(activated(int)),
            this, SLOT(changeSelected(void)));
    setSizeAdjustPolicy(QComboBox::AdjustToContents);
}

toResultCombo::~toResultCombo()
{
    if (Query)
        delete Query;
}

void toResultCombo::query(const QString &sql, const toQList &param)
{
    if (!setSQLParams(sql, param))
        return ;

    try
    {
        clear();
        addItems(Additional);
        for (int i = 0;i < Additional.count();i++)
            if (Additional[i] == Selected)
                setCurrentIndex(i);

        Query = new toEventQuery(connection(), toQuery::Background, sql, param);
        connect(Query, SIGNAL(dataAvailable()), this, SLOT(poll()));
        connect(Query, SIGNAL(done()), this, SLOT(queryDone()));
        Query->readAll(); // indicate that all records should be fetched
        Query->start();
    }
    TOCATCH
}

void toResultCombo::changeSelected(void)
{
    Selected = currentText();
}

void toResultCombo::poll(void)
{
    try
    {
        if (!toCheckModal(this))
            return ;

        if (Query)
        {
            while (Query->hasMore())
            {
                QString t = Query->readValue();
                QStringList l;
                for (unsigned i = 1; i < Query->describe().size(); ++i)
                {
                    QString v = Query->readValue();
                    l.append(v);
                }
                addItem(t, QVariant(l));
                if (t == Selected)
                    setCurrentIndex(count() - 1);
            }
        }
    }
    catch (const QString &exc)
    {
        emit done();
        toStatusMessage(exc);
    }
}

void toResultCombo::queryDone(void)
{
    switch(SelectionPolicy)
    {
        case First:
            if (currentIndex() != 0)
                setCurrentIndex(0);
            break;
        case Last:
            if (currentIndex() != count() - 1)
                setCurrentIndex(count() - 1);
            break;
        case LastButOne:
            if (currentIndex() != count() - 2)
                setCurrentIndex(count() - 2);
            break;
        case None:
            ;
    };
    setFont(font()); // Small hack to invalidate size hint of combobox which should resize to needed size.
    updateGeometry();
    emit done();
} // queryDone
