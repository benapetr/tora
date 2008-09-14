/* BEGIN_COMMON_COPYRIGHT_HEADER 
* END_COMMON_COPYRIGHT_HEADER */

#include "config.h"
#include "utils.h"
#include "toconf.h"
#include "toconfiguration.h"
#include "toresultdatasingle.h"
#include "toparamget.h"
#include "tomodeleditor.h"

#include <QScrollArea>
#include <QCheckBox>
#include <QLineEdit>
#include <QAction>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>


toResultDataSingle::toResultDataSingle(QWidget *parent)
        : QWidget(parent)
{

    Row       = -1;
    Container = NULL;
    Model     = NULL;

    setBackgroundRole(QPalette::Window);

    // fscking qscrollarea won't resize this is added
    QHBoxLayout *h = new QHBoxLayout;
    h->setContentsMargins(0, 0, 0, 0);
    h->setSpacing(0);
    setLayout(h);

    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}


void toResultDataSingle::changeSource(toResultModel *model, int _row)
{
    delete Container;
    Container = new QScrollArea(this);
    layout()->addWidget(Container);

    Model = model;

    QWidget *ext = new QWidget(Container);
    Container->setWidget(ext);
    Container->setWidgetResizable(true);

    QGridLayout *grid = new QGridLayout;
    ext->setLayout(grid);

    Value.clear();
    Null.clear();

    const toResultModel::HeaderList Headers = Model->headers();

    int row = 1;
    int col = 0;

    for (; row < Headers.size(); row++, col = 0)
    {
        QLabel *name = new QLabel(Headers[row].name, this);
        grid->addWidget(name, row, col++);

        QLineEdit *edit = new QLineEdit(this);
        edit->setObjectName(QString::number(row));
        edit->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                        QSizePolicy::Minimum));
        grid->addWidget(edit, row, col);
        // edit widget should get the most space
        grid->setColumnStretch(col++, 1);

        QCheckBox *box = new QCheckBox(("NULL"), this);
        connect(box, SIGNAL(toggled(bool)), edit, SLOT(setDisabled(bool)));
        grid->addWidget(box, row, col++);

        toParamGetButton *btn = new toParamGetButton(row, this);
        btn->setText(tr("Edit"));
        btn->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed));
        connect(btn, SIGNAL(clicked(int)), this, SLOT(showMemo(int)));
        connect(box, SIGNAL(toggled(bool)), btn, SLOT(setDisabled(bool)));
        connect(box, SIGNAL(toggled(bool)), this, SLOT(saveRow()));
        connect(edit, SIGNAL(editingFinished()), this, SLOT(saveRow()));
        grid->addWidget(btn, row, col++);

        Value.append(edit);
        Null.append(box);
    }

    // add widget at bottom of grid so it can resize
    grid->addWidget(new QLabel(this), row, 0);
    grid->setRowStretch(row, 1);

    changeRow(Row);

    connect(Model,
            SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
            this,
            SLOT(dataChanged(const QModelIndex &, const QModelIndex &)));
}


void toResultDataSingle::dataChanged(const QModelIndex &topLeft,
                                     const QModelIndex &bottomRight)
{
    if(topLeft.row() >= Row && bottomRight.row() <= Row)
        changeRow(Row);         // update
}


void toResultDataSingle::changeRow(int row)
{
    bool any = false;

    QList<QCheckBox *>::iterator chk = Null.begin();
    QList<QLineEdit *>::iterator val = Value.begin();

    for (int i = 1;
         i < Model->columnCount() && chk != Null.end() && val != Value.end();
         i++, chk++, val++)
    {
        QVariant str = Model->data(row, i);
        if (!str.isNull())
            any = true;
        (*chk)->setChecked(str.isNull());
        (*val)->setText(str.toString());
    }

    if (!any)
    {
        for (chk = Null.begin(); chk != Null.end(); chk++)
            (*chk)->setChecked(false);
    }

    Row = row;
}


void toResultDataSingle::saveRow()
{
    if (Row < 0)
        return;

    QList<QLineEdit *>::iterator val = Value.begin();
    for (int i = 1; i < Model->columnCount() && val != Value.end(); i++, val++)
    {
        QString v = QString::null;
        if ((*val)->isEnabled())
            v = (*val)->text();

        QModelIndex index = Model->createIndex(Row, i);
        QVariant orig = Model->data(Row, i).toString();

        if (!((v.isNull() && orig.isNull()) || v == orig))
            Model->setData(index, v);
    }
}


void toResultDataSingle::showMemo(int col)
{
    QModelIndex index = Model->index(Row, col);
    if(index.isValid()) {
        toModelEditor *memo = new toModelEditor(
            this,
            Model,
            index,
            false,
            true);

        memo->exec();
    }
}
