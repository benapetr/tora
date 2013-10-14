
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

#ifndef __TOSYNTAXSETUP_H__
#define __TOSYNTAXSETUP_H__


#include "ui_tosyntaxsetupui.h"
#include "core/totool.h"

#include <map>

#include <QtCore/QString>
#include <QtCore/QMetaEnum>

class QFont;
class QListWidgetItem;
class toSyntaxAnalyzer;

class toSyntaxSetup : public QWidget
    , public Ui::toSyntaxSetupUI
    , public toSettingTab
{

    Q_OBJECT;

    QString ListFontName;
    QListWidgetItem *Current;
    QMetaEnum ColorsEnum;
    QMap<int, QColor> FGColors, BGColors; // enum toSyntaxAnalyzer::wordClass => QColor
    QMap<int, QFont> Fonts;               // enum toSyntaxAnalyzer::wordClass => QFont
    toSyntaxAnalyzer* Analyzer;
public:
    toSyntaxSetup(QWidget *parent = 0, const char *name = 0, Qt::WFlags fl = 0);
    virtual void saveSetting(void);

public slots:
    virtual void changeLine(QListWidgetItem *);
    virtual void selectColor(void);
    virtual void selectFont(void);
    virtual void selectText(void);
    virtual void selectResultFont(void);
    virtual void changeUpper(bool val)
    {
        //TODO Example->setKeywordUpper(val);
        Example->update();
    }
    virtual void changeHighlight(bool val)
    {
        //TODO Example->setSyntaxColoring(val);
        Example->update();
    }
    void openEditorShortcutsDialog();
private:
    int wordClass() const;         // returns enum toSyntaxAnalyzer::wordClass
    void checkFixedWidth(const QFont &fnt);
};

#endif
