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

#include "utils.h"

#include "tobarchart.h"
#include "toconf.h"
#include "tolinechart.h"
#include "topiechart.h"
#include "toresulttableview.h"
#include "toresultmodel.h"
#include "tovisualize.h"

#include <qcombobox.h>
#include <qlabel.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>

#include <QVBoxLayout>
#include <QPixmap>

#include "icons/axis.xpm"
#include "icons/execute.xpm"
#include "icons/grid.xpm"
#include "icons/legend.xpm"

toVisualize::toVisualize(toResultTableView *source, QWidget *parent, const char *name)
    : QWidget(parent), Source(source) {

    if(name)
        setObjectName(name);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setSpacing(0);
    vbox->setContentsMargins(0, 0, 0, 0);
    setLayout(vbox);

    QToolBar *toolbar = toAllocBar(this, tr("Data visualization"));
    vbox->addWidget(toolbar);

    Type = new QComboBox(toolbar, TO_TOOLBAR_WIDGET_NAME);
    Type->insertItem(tr("Bar chart"), 0);
    Type->insertItem(tr("Line chart"), 1);
    Type->insertItem(tr("Pie chart"), 2);
    toolbar->addWidget(Type);

    toolbar->addSeparator();

    Legend = new QToolButton(toolbar);
    Legend->setToggleButton(true);
    Legend->setIconSet(QIcon(QPixmap(const_cast<const char**>(legend_xpm))));
    Legend->setToolTip(tr("Display legend"));
    Legend->setOn(true);
    toolbar->addWidget(Legend);

    Grid = new QToolButton(toolbar);
    Grid->setToggleButton(true);
    Grid->setIconSet(QIcon(QPixmap(const_cast<const char**>(grid_xpm))));
    Grid->setToolTip(tr("Display grid"));
    Grid->setOn(true);
    toolbar->addWidget(Grid);

    Axis = new QToolButton(toolbar);
    Axis->setToggleButton(true);
    Axis->setIconSet(QIcon(QPixmap(const_cast<const char**>(axis_xpm))));
    Axis->setToolTip(tr("Display axis legend"));
    Axis->setOn(true);
    toolbar->addWidget(Axis);

    QLabel *title = new QLabel(tr("Title columns"));
    title->setToolTip(tr("Number of columns to use as title"));
    toolbar->addWidget(title);

    Title = new QComboBox(toolbar, TO_TOOLBAR_WIDGET_NAME);
    Title->insertItem(QString::fromLatin1("1"));
    Title->insertItem(QString::fromLatin1("2"));
    Title->insertItem(QString::fromLatin1("3"));
    Title->insertItem(QString::fromLatin1("4"));
    Title->insertItem(QString::fromLatin1("5"));
    Title->insertItem(QString::fromLatin1("6"));
    toolbar->addWidget(Title);

    toolbar->addSeparator();

    toolbar->addAction(QIcon(QPixmap(const_cast<const char**>(execute_xpm))),
                       tr("Update chart"),
                       this,
                       SLOT(display(void)));

    QLabel *s = new QLabel(toolbar);
    s->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                 QSizePolicy::Minimum));
    toolbar->addWidget(s);

    Result = new QWidget(this);
    Result->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Expanding));
    vbox->addWidget(Result);
}

void toVisualize::display(void) {
    QWidget *last = Result;
    toResultModel *model = Source->model();

    switch (Type->currentItem()) {

    case 0: {
        toBarChart *chart = new toBarChart(this);
        layout()->addWidget(chart);
        chart->showLegend(Legend->isOn());
        int tit = std::max(1, Title->currentText().toInt()) +
            Source->numberColumn();
        chart->setTitle(
            model->headerData(Source->numberColumn(),
                              Qt::Horizontal, Qt::DisplayRole).toString());
        chart->showGrid(Grid->isOn() ? 5 : 0);
        chart->showAxisLegend(Axis->isOn());
        chart->setSamples();

        std::list<QString> lst;
        for(int i = tit; i < model->columnCount(); i++) {
            toPush(lst,
                   model->headerData(i, Qt::Horizontal, Qt::DisplayRole)
                   .toString());
        }
        chart->setLabels(lst);

        for(int row = 0; row < model->rowCount(); row++) {
            QModelIndex index = model->createIndex(row, Source->numberColumn());
            QString label = model->data(index, Qt::EditRole).toString();
            for(int j = 1; j < tit; j++) {
                QModelIndex index = model->createIndex(row, j);
                label += QString::fromLatin1(", ");
                label += model->data(index, Qt::EditRole).toString();
            }

            std::list<double> val;

            for(int i = tit; i < model->columnCount(); i++) {
                QModelIndex index = model->createIndex(row, i);
                toPush(val, model->data(index, Qt::EditRole).toDouble());
            }

            chart->addValues(val, label);
        }

        Result = chart;
    }
    break;

    case 1: {
        toLineChart *chart = new toLineChart(this);
        layout()->addWidget(chart);
        chart->showLegend(Legend->isOn());
        int tit = std::max(1, Title->currentText().toInt()) +
            Source->numberColumn();
        chart->setTitle(
            model->headerData(Source->numberColumn(),
                              Qt::Horizontal, Qt::DisplayRole).toString());
        chart->showGrid(Grid->isOn() ? 5 : 0);
        chart->showAxisLegend(Axis->isOn());
        chart->setSamples();

        std::list<QString> lst;
        for(int i = tit; i < model->columnCount(); i++) {
            toPush(lst, model->headerData(
                       i,
                       Qt::Horizontal,
                       Qt::DisplayRole).toString());
        }
        chart->setLabels(lst);

        for(int row = 0; row < model->rowCount(); row++) {
            QModelIndex index = model->createIndex(row, Source->numberColumn());
            QString label = model->data(index, Qt::EditRole).toString();
            for(int j = 1; j < tit; j++) {
                QModelIndex index = model->createIndex(row, j);
                label += QString::fromLatin1(", ");
                label += model->data(index, Qt::EditRole).toString();
            }

            std::list<double> val;

            for(int i = tit; i < model->columnCount(); i++) {
                QModelIndex index = model->createIndex(row, i);
                toPush(val, model->data(index, Qt::EditRole).toDouble());
            }

            chart->addValues(val, label);
        }

        Result = chart;
    }
    break;

    case 2: {
        toPieChart *chart = new toPieChart(this);
        layout()->addWidget(chart);
        chart->showLegend(Legend->isOn());
        int tit = std::max(1, Title->currentText().toInt()) +
            Source->numberColumn();
        chart->setTitle(
            model->headerData(Source->numberColumn(),
                              Qt::Horizontal, Qt::DisplayRole).toString());

        for(int row = 0; row < model->rowCount(); row++) {
            QModelIndex index = model->createIndex(row, Source->numberColumn());
            QString label = model->data(index, Qt::EditRole).toString();
            for(int j = 1; j < tit; j++) {
                QModelIndex index = model->createIndex(row, j);
                label += QString::fromLatin1(", ");
                label += model->data(index, Qt::EditRole).toString();
            }

            index = model->createIndex(row, tit);
            chart->addValue(model->data(index, Qt::EditRole).toDouble(), label);
        }

        Result = chart;
    }
    break;

    default:
        Result = new QWidget(this);
        layout()->addWidget(Result);
    }

    Result->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Expanding));
    Result->show();
    delete last;
}
