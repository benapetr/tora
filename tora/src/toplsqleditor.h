/*****
*
* TOra - An Oracle Toolkit for DBA's and developers
* Copyright (C) 2003-2008 Quest Software, Inc
* Portions Copyright (C) 2008 Other Contributors
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

#ifndef TOPLSQLEDITOR_H
#define TOPLSQLEDITOR_H


#include "totool.h"
#include "tohighlightedtext.h"
#include "tosqlparse.h"

class QMdiSubWindow;
class QTreeView;
class toCodeModel;
class toConnection;
class toPLSQLWidget;
class toTreeWidget;
class toTreeWidgetItem;


/*! \brief Advanced PL/SQL Editor. It's based on
Debugger code. But it can compile PL/SQL unit in all cases.
Even if is the user without debug grants/roles.

It can compile for "production" and for "warnings" as well.
There is implemented navigation for bugs etc.
*/
class toPLSQLEditor : public toToolWidget
{
    Q_OBJECT;

    // Toolbar
    void createActions(void);

    QComboBox *Schema;
    QAction   *refreshAct;
    QAction   *newSheetAct;
    QAction   *compileAct;
    QAction   *compileWarnAct;
    QAction   *closeAct;
    QAction   *closeAllAct;
    QAction   *nextErrorAct;
    QAction   *previousErrorAct;

    // Extra menu
    QMenu *ToolMenu;

    // Content pane
    QTreeView   *Objects;
    toCodeModel *CodeModel;
    QSplitter   *splitter;
    // Editors
    QTabWidget *Editors;

    void updateCurrent(void);
    QString editorName(const QString &schema,
                       const QString &object,
                       const QString &type);
    QString editorName(toPLSQLWidget *text);


    bool viewSource(const QString &schema,
                    const QString &name,
                    const QString &type,
                    int line,
                    bool current = false);

    void closeEditor(toPLSQLWidget* &editor);

protected:
    virtual void closeEvent(QCloseEvent *);

public:
    toPLSQLEditor(QWidget *parent, toConnection &connection);

    virtual ~toPLSQLEditor();

    toPLSQLWidget *currentEditor(void);
    QString currentSchema(void);

public slots:
    void compile(void);
    void compileWarn(void);
    void changeSchema(int);
    void refresh(void);
    void changePackage(const QModelIndex &, const QModelIndex &);
    void prevError(void);
    void nextError(void);
    void showSource(toTreeWidgetItem *);
    void newSheet(void);
    void windowActivated(QMdiSubWindow *w);
    void closeEditor(void);
    void closeAllEditor(void);
};

#endif
