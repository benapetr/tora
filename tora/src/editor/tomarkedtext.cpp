
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

#include "editor/tomarkedtext.h"
#include "core/toconf.h"
#include "core/toconfiguration.h"
#include "core/toglobalevent.h"
#include "core/tologger.h"

#include <QtGui/QClipboard>
#include <QtGui/QPrintDialog>
#include <QtXml/QDomDocument>
#include <QtGui/QShortcut>
#include <QtCore/QtDebug>
#include <QtGui/QMenu>
#include <QtGui/QClipboard>

#include <Qsci/qsciprinter.h>
#include <Qsci/qscilexersql.h>

#include "icons/undo.xpm"
#include "icons/redo.xpm"
#include "icons/copy.xpm"
#include "icons/cut.xpm"
#include "icons/paste.xpm"


#define ACCEL_KEY(k) "\t" + QString("Ctrl+" #k)

void QSciMessage::notify()
{
	Utils::toStatusMessage(text, true);
}

toMarkedText::toMarkedText(QWidget *parent, const char *name)
	: QsciScintilla(parent)
	, DragStart()
	, m_searchText()
	, m_flags()
	, m_searchIndicator(9) // see QsciScintilla docs
	, m_origContent()
{
    if (name)
        setObjectName(name);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    super::setFont(Utils::toStringToFont(toConfigurationSingle::Instance().textFontName()));
    super::setMarginLineNumbers(0, true);
    super::setCallTipsStyle(CallTipsNone);

    // WARNING: it looks like this hack is mandatory for macosx. Oracle simly
    // doesn't understand mac's eols, so force to UNIX is a functional workaround
#ifdef Q_WS_MAC
    super::setEolMode(QsciScintilla::EolUnix);
#endif

    // it allows to use S&R in eg. error message output
    //QShortcut *searchReplaceShortcut = new QShortcut(QKeySequence::Find, parent, SLOT(searchReplace()), SLOT(searchReplace()));

    // search all occurrences
    // allow indicator painting *under* the text (but it makes editor slower a bit...)
    // It paints a colored box under the text for all occurrences of m_searchText.
    super::indicatorDefine(QsciScintilla::RoundBoxIndicator, m_searchIndicator);
    // TODO/FIXME: make it configurable
    super::setIndicatorForegroundColor(Qt::red, m_searchIndicator);
    super::setIndicatorOutlineColor(Qt::black, m_searchIndicator);
    super::setIndicatorDrawUnder(true, m_searchIndicator);
    // end of search all occurrences

    connect(this, SIGNAL(linesChanged()), this, SLOT(linesChanged()));

    // sets default tab width
    super::setTabWidth(toConfigurationSingle::Instance().tabStop());
    super::setIndentationsUseTabs(!toConfigurationSingle::Instance().useSpacesForIndent());

    super::setUtf8(true);
    setAcceptDrops(true); // QWidget::setAcceptDrops

    super::setMarginWidth(0, QString::fromAscii("00"));
}

toMarkedText::~toMarkedText()
{
//	toEditWidget::lostFocus();
}

QString toMarkedText::wordAtPosition(int position, bool onlyWordCharacters /* = true */) const
{
    if (position < 0)
        return QString();

    long start_pos = SendScintilla(SCI_WORDSTARTPOSITION, position, onlyWordCharacters);
    long end_pos = SendScintilla(SCI_WORDENDPOSITION, position, onlyWordCharacters);

	int style1 = SendScintilla(QsciScintilla::SCI_GETSTYLEAT, start_pos) & 0x1f;
    int style2 = SendScintilla(QsciScintilla::SCI_GETSTYLEAT, end_pos) & 0x1f;

    // QScintilla returned single word within quotes
    if( style1 == QsciLexerSQL::DoubleQuotedString || style1 == QsciLexerSQL::SingleQuotedString)
    {
    	start_pos = SendScintilla(QsciScintilla::SCI_POSITIONBEFORE, start_pos);
    	int style = SendScintilla(QsciScintilla::SCI_GETSTYLEAT, start_pos) & 0x1f;
    	while( style == style1)
    	{
    		start_pos = SendScintilla(QsciScintilla::SCI_POSITIONBEFORE, start_pos);
        	style = SendScintilla(QsciScintilla::SCI_GETSTYLEAT, start_pos) & 0x1f;
    	}
    	start_pos = SendScintilla(QsciScintilla::SCI_POSITIONAFTER, start_pos);

    	end_pos = SendScintilla(QsciScintilla::SCI_POSITIONAFTER, end_pos);
    	style = SendScintilla(QsciScintilla::SCI_GETSTYLEAT, end_pos) & 0x1f;
    	while( style == style1)
    	{
    		end_pos = SendScintilla(QsciScintilla::SCI_POSITIONAFTER, end_pos);
        	style = SendScintilla(QsciScintilla::SCI_GETSTYLEAT, end_pos) & 0x1f;
    	}
    }

    int word_len = end_pos - start_pos;
    if (word_len <= 0)
        return QString();

    char *buf = new char[word_len + 1];
    SendScintilla(SCI_GETTEXTRANGE, start_pos, end_pos, buf);
    QString word = convertTextS2Q(buf);
    delete[] buf;

    return word;
}

// Return the word at the given coordinates.
QString toMarkedText::wordAtLineIndex(int line, int index) const
{
    return wordAtPosition(positionFromLineIndex(line, index));
}

// Convert a Scintilla string to a Qt Unicode string.
QString toMarkedText::convertTextS2Q(const char *s) const
{
    if (isUtf8())
        return QString::fromUtf8(s);

    return QString::fromLatin1(s);
}

void toMarkedText::linesChanged()
{
    int x = QString::number(lines()).length() + 1;
    setMarginWidth(0, QString().fill('0', x));

}

void toMarkedText::setWordWrap(bool enable)
{
    if (enable)
    {
        setWrapMode(QsciScintilla::WrapWord);
        setWrapVisualFlags(QsciScintilla::WrapFlagByBorder,
                           QsciScintilla::WrapFlagByBorder);
    }
    else
    {
        setWrapMode(QsciScintilla::WrapNone);
        setWrapVisualFlags(QsciScintilla::WrapFlagNone,
                           QsciScintilla::WrapFlagNone);
    }
}

void toMarkedText::setXMLWrap(bool wrap)
{
    if (wrap)
    {
        QDomDocument d;
        if (m_origContent.isEmpty())
            m_origContent = text();
        if (d.setContent(m_origContent))
            setText(d.toString(2));
        else
            m_origContent.clear();
    }
    else
    {
        if (!m_origContent.isEmpty())
        {
            setText(m_origContent);
            m_origContent.clear();
        }
    }
}

void toMarkedText::print(const QString  &fname)
{
    QsciPrinter printer;

    QPrintDialog dialog(&printer, this);
    dialog.setMinMax(1, 1000);
    dialog.setFromTo(1, 1000);

    if (!fname.isEmpty())
    {
        QFileInfo info(fname);
        dialog.setWindowTitle(tr("Print %1").arg(info.fileName()));
        printer.setOutputFileName(info.path() +
                                  QString("/") +
                                  info.baseName() +
                                  ".pdf");
    }
    else
        dialog.setWindowTitle(tr("Print Document"));

    // printRange() not handling this and not sure what to do about it
//     if(hasSelectedText())
//         dialog.addEnabledOption(QAbstractPrintDialog::PrintSelection);

    if (!dialog.exec())
        return;

    printer.setCreator(tr(TOAPPNAME));

    // they show up in the print
    setMarginLineNumbers(0, false);
    printer.printRange(this);
    setMarginLineNumbers(0, true);
}

void toMarkedText::copy()
{
	QsciScintilla::copy();
#ifdef QT_DEBUG
	QMimeData *md = new QMimeData();
	QString txt = QApplication::clipboard()->mimeData()->text();
	QString html = getHTML();
	QString rtf = getRTF();
#ifdef Q_OS_MAC
    md->setData(QLatin1String("text/html"), html.toUtf8());
#else
    md->setHtml(html);
#endif
    md->setData(QLatin1String("text/rtf"), rtf.toUtf8());
    QApplication::clipboard()->setMimeData(md, QClipboard::Clipboard);
	TLOG(0, toDecorator, __HERE__) << "html:" << html << std::endl;
	TLOG(0, toDecorator, __HERE__) << "rtf:" << rtf << std::endl;
#endif
}

void toMarkedText::paste()
{
#ifdef QT_DEBUG
	QMimeData const *md = QApplication::clipboard()->mimeData();
	TLOG(0, toDecorator, __HERE__) << md->formats().join("'") << std::endl;
	Q_FOREACH(QString format, md->formats())
	{
		QByteArray ba = md->data(format);
		TLOG(0, toNoDecorator, __HERE__) << format << std::endl
				<< ba.data() << std::endl;
	}
#endif
	QsciScintilla::paste();
}

void toMarkedText::newLine(void)
{
    // new line
    switch (eolMode())
    {
    case EolWindows:
        insert ("\n\r");
        break;

    case EolMac:
        insert ("\n");
        break;

    default:
        // Unix is default one
        insert ("\r");
        break;
    }

    if (toConfigurationSingle::Instance().autoIndent())
    {
        int curline, curcol;
        getCursorPosition (&curline, &curcol);
        if (curline > 0)
        {
            QString str = text(curline - 1);
            QString ind;
            for (int i = 0; i < str.length() && str.at(i).isSpace(); i++)
                ind += str.at(i);
            if (ind.length())
                insert(ind, false);
        }
    }
}

void toMarkedText::dropEvent(QDropEvent *e)
{
    if(e->source() == this || e->source() == viewport())
    {
        QPoint point = e->pos() - DragStart;
        // forces a reasonable drag distance
        if (point.manhattanLength() < QApplication::startDragDistance())
        {
            e->ignore();
            return;
        }

        e->acceptProposedAction();
        beginUndoAction();

        QString selection = selectedText();
        removeSelectedText();

        long position = SendScintilla(SCI_POSITIONFROMPOINT,
                                      e->pos().x(),
                                      e->pos().y());
        SendScintilla(SCI_SETCURRENTPOS, position);

        int line, index;
        getCursorPosition(&line, &index);
        insertAt(selection, line, index);
        setSelection(line, index, line, index + selection.length());

        endUndoAction();
    }
    else
        QsciScintilla::dropEvent(e);

    setFocus();
}

void toMarkedText::mousePressEvent(QMouseEvent *e)
{
    DragStart = QPoint();
    if(e->button() == Qt::LeftButton && geometry().contains(e->pos()))
    {
        if(!toConfigurationSingle::Instance().editDragDrop())
        {
            // would normally be a drag request. clear selection.
            if(QsciScintilla::hasSelectedText())
                QsciScintilla::selectAll(false);
        }
        else if(QsciScintilla::hasSelectedText())
            DragStart = e->pos();
    }

    QsciScintilla::mousePressEvent(e);
}

void toMarkedText::keyPressEvent(QKeyEvent *e)
{
//    if (Search)
//    {
//        bool ok = false;
//        if (e->modifiers() == Qt::NoModifier && e->key() == Qt::Key_Backspace)
//        {
//            int len = SearchString.length();
//            if (len > 0)
//                SearchString.truncate(len - 1);
//            ok = true;
//        }
//        else if (e->key() != Qt::Key_Escape)
//        {
//            QString t = e->text();
//            if (t.length())
//            {
//                SearchString += t;
//                ok = true;
//            }
//            else if (e->key() == Qt::Key_Shift ||
//                     e->key() == Qt::Key_Control ||
//                     e->key() == Qt::Key_Meta ||
//                     e->key() == Qt::Key_Alt)
//            {
//                ok = true;
//            }
//        }
//
//        if (ok)
//        {
//            incrementalSearch(m_searchDirection, false);
//            e->accept();
//            return ;
//        }
//        else
//        {
//            Search = false;
//            LastSearch = SearchString;
//            Utils::toStatusMessage(QString::null);
//        }
//    }
    QsciScintilla::keyPressEvent(e);
}

#ifdef TORA3_GRAPH
void toMarkedText::exportData(std::map<QString, QString> &data, const QString &prefix)
{
    data[prefix + ":Filename"] = Filename;
    data[prefix + ":Text"] = text();
    int curline, curcol;
    getCursorPosition (&curline, &curcol);
    data[prefix + ":Column"] = QString::number(curcol);
    data[prefix + ":Line"] = QString::number(curline);
    if (isModified())
        data[prefix + ":Edited"] = "Yes";
}

void toMarkedText::importData(std::map<QString, QString> &data, const QString &prefix)
{
    QString txt = data[prefix + ":Text"];
    if (txt != text())
        setText(txt);
    Filename = data[prefix + ":Filename"];
    setCursorPosition(data[prefix + ":Line"].toInt(), data[prefix + ":Column"].toInt());
    if (data[prefix + ":Edited"].isEmpty())
        setModified(false);
}
#endif

/*
static int FindIndex(const QString &str, int line, int col)
{
    int pos = 0;
    for (int i = 0;i < line;i++)
    {
        pos = str.indexOf('\n', pos);
        if (pos < 0)
            return pos;
        pos++;
    }
    return pos + col;
}*/

void toMarkedText::findPosition(int index, int &line, int &col)
{
    int pos = 0;
    for (int i = 0; i < lines(); i++)
    {
        QString str = text(i);
        if (str.length() + pos >= index)
        {
            line = i;
            col = index - pos;
            return ;
        }
        pos += str.length();
    }
    col = -1;
    line = -1;
    return ;
}


bool toMarkedText::findText(const QString &searchText, const QString &replaceText, Search::SearchFlags flags)
{
    int line, index;
    bool found = false;

    getCursorPosition(&line, &index);

  	found = findFirst(searchText,
                      (bool)(flags & Search::Regexp),
                      (bool)(flags & Search::CaseSensitive),
                      (bool)(flags & Search::WholeWords),
                      true, //bool   wrap,
                      (bool)(flags & Search::Forward),
                      line,
                      index,
                      true //bool   show = true
                     );

    if (m_searchText != searchText || m_flags != flags)
    {
    	m_searchText = searchText;
    	m_flags = flags;

        // find and highlight all occurrences of m_searchText
        // from the beginning to the end
        int from = 0;
        int to = text().length();

        // clear previously used marked text
        clearIndicatorRange(0, 0, lines(), lineLength(lines()-1), m_searchIndicator);

        if (!found)
            return found;

        // set searching flags
        int searchFlags = 0;
        //QsciScintilla::SCFIND_WORDSTART;
        if (flags & Search::Regexp)
            searchFlags |= QsciScintilla::SCFIND_REGEXP;
        if (flags & Search::CaseSensitive)
            searchFlags |= QsciScintilla::SCFIND_MATCHCASE;
        if (flags & Search::WholeWords)
            searchFlags |= QsciScintilla::SCFIND_WHOLEWORD;

        SendScintilla(QsciScintilla::SCI_SETINDICATORCURRENT, m_searchIndicator);

        while (from < to)
        {
            // set searching range
            SendScintilla(QsciScintilla::SCI_SETTARGETSTART, from);
            SendScintilla(QsciScintilla::SCI_SETTARGETEND, to);

            SendScintilla(QsciScintilla::SCI_SETSEARCHFLAGS, searchFlags);
            from = SendScintilla(QsciScintilla::SCI_SEARCHINTARGET,
                                 m_searchText.length(), m_searchText.toUtf8().data());

            // SCI_SEARCHINTARGET returns -1 when it doesn't find anything
            if (from == -1)
            {
                break;
            }

            int end = SendScintilla(QsciScintilla::SCI_GETTARGETEND);
            // mark current occurrence of searchText
            SendScintilla(QsciScintilla::SCI_INDICATORFILLRANGE, from, end - from);
            from = end;
        } // while
    }

    if (!isReadOnly() && found && searchText != replaceText)
    {
    	if (m_flags & Search::Replace)
    	{
    		this->replace(replaceText);
    	}
    	else if (m_flags & Search::ReplaceAll)
    	{
    		QsciScintilla::beginUndoAction();
    		while (findNext())
    		    this->replace(replaceText);
    		QsciScintilla::endUndoAction();
    	}
    }

    return found; // TODO/FIXME: what to do with a retval?
}

void toMarkedText::findStop()
{
	clearIndicatorRange(0, 0, lines(), lineLength(lines()-1), m_searchIndicator);
}

#if 0
// TODO: this part is waiting for QScintilla backend feature (yet unimplemented).
void toMarkedText::setSelectionType(int aType)
{
    TLOG(2, toDecorator, __HERE__) << "setSelectionType" << SendScintilla(SCI_GETSELECTIONMODE) << aType;
    TLOG(2, toDecorator, __HERE__) << SendScintilla(SCI_SETSELECTIONMODE, aType);
    TLOG(2, toDecorator, __HERE__) << "setSelectionType" << SendScintilla(SCI_GETSELECTIONMODE) << aType;
}
#endif

void toMarkedText::focusInEvent (QFocusEvent *e)
{
    TLOG(9, toDecorator, __HERE__) << this << std::endl;
    QsciScintilla::focusInEvent(e);
    int curline, curcol;
    getCursorPosition (&curline, &curcol);
    toGlobalEventSingle::Instance().setCoordinates(curline + 1, curcol + 1);

    emit gotFocus();
}

void toMarkedText::focusOutEvent (QFocusEvent *e)
{
    qDebug() << ">>> toMarkedText::focusOutEvent" << this;
    QsciScintilla::focusOutEvent(e);
    emit lostFocus();
}

void toMarkedText::contextMenuEvent(QContextMenuEvent *e)
{
    QPointer<toMarkedText> that = this;
    QPointer<QMenu> popup = createPopupMenu( e->pos() );
    if (!popup)
        return;

    e->accept();

    // NOTE: this emit exist for compatibility with older
    //       version of TOra. It will have to be removed
    //       once the way around it will be figured out
    emit displayMenu(popup);

    popup->exec(e->globalPos());
    delete popup;
}

/**
 * This function is called to create a right mouse button popup menu
 * at the specified position. If you want to create a custom popup menu,
 * reimplement this function and return the created popup menu. Ownership
 * of the popup menu is transferred to the caller.
 */
QMenu *toMarkedText::createPopupMenu(const QPoint& pos)
{
    Q_UNUSED(pos);

    const bool isEmptyDocument = (lines() == 0);

    // create menu
    QMenu   *popup = new QMenu(this);
    QAction *action;

    if (!isReadOnly())
    {
        action = popup->addAction(QIcon(undo_xpm), tr("&Undo"), this, SLOT(undo()));
        action->setShortcut(QKeySequence::Undo);
        action->setEnabled(isUndoAvailable());

        action = popup->addAction(QIcon(redo_xpm), tr("&Redo"), this, SLOT(redo()));
        action->setShortcut(QKeySequence::Redo);
        action->setEnabled(isRedoAvailable());

        popup->addSeparator();

        action = popup->addAction(QIcon(cut_xpm), tr("Cu&t"), this, SLOT(cut()));
        action->setShortcut(QKeySequence::Cut);
        action->setToolTip(tr("Cut to clipboard"));
        action->setEnabled(hasSelectedText());

        action = popup->addAction(QIcon(copy_xpm),
                                  tr("&Copy"),
                                  this,
                                  SLOT(copy()));
        action->setShortcut(QKeySequence::Copy);
        action->setToolTip(tr("Copy to clipboard"));
        action->setEnabled(hasSelectedText());

        action = popup->addAction(QIcon(paste_xpm),
                                  tr("&Paste"),
                                  this,
                                  SLOT(paste()));
        action->setShortcut(QKeySequence::Paste);
        action->setToolTip(tr("Paste from clipboard"));
        action->setEnabled(!QApplication::clipboard()->text(
                               QClipboard::Clipboard).isEmpty());

        action = popup->addAction(tr("Clear"),
                                  parent(),
                                  SLOT(clear()));
        action->setToolTip(tr("Clear editor"));
        action->setEnabled(!isEmptyDocument);

        popup->addSeparator();
    }

    action = popup->addAction(tr("Select &All"),
                              this,
                              SLOT(selectAll()));
    action->setShortcut(QKeySequence::SelectAll);
    action->setEnabled(!isEmptyDocument);

    return popup;
}

QString toMarkedText::getHTML()
{
	// TODO check this:
	// <span style="color: rgb(192,192,192); background: rgb(0,0,0); font-weight: normal; font-style: normal; font-decoration: normal"> Achaea's IP address is 69.65.42.66<br /></span>
	//static const QString SPAN_CLASS = QString::fromAscii("<span class=\"S%1\">");
	static const QString SPAN_CLASS = QString::fromAscii("<span style=\"color: rgb(192,192,192); background: rgb(0,0,0); font-weight: normal; font-style: normal; font-decoration: normal\">");

	clearIndicatorRange(0, 0, lines(), lineLength(lines()-1), m_searchIndicator);
	recolor();
	int tabSize = 4;
	int wysiwyg = 1;
	int tabs = 0;
	int onlyStylesUsed = 1;
	int titleFullPath = 0;

	int startPos = SendScintilla(SCI_GETSELECTIONSTART);
	int endPos = SendScintilla(SCI_GETSELECTIONEND);

	bool styleIsUsed[STYLE_MAX + 1];
	if (onlyStylesUsed) {
		int i;
		for (i = 0; i <= STYLE_MAX; i++) {
			styleIsUsed[i] = false;
		}
		// check the used styles
		for (i = startPos; i < endPos; i++) {
			styleIsUsed[getStyleAt(i) & 0x7F] = true;
		}
	} else {
		for (int i = 0; i <= STYLE_MAX; i++) {
			styleIsUsed[i] = true;
		}
	}
	styleIsUsed[STYLE_DEFAULT] = true;

	QString retval;
	retval += "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
	retval += "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
	retval += "<head>\n";
	//retval += "<title>%s</title>\n"; //static_cast<const char *>(filePath.Name().AsUTF8().c_str()));

	// Probably not used by robots, but making a little advertisement for those looking
	// at the source code doesn't hurt...
	retval += "<meta name=\"Generator\" content=\"SciTE - www.Scintilla.org\" />\n";
	retval += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n";
	retval += "<style type=\"text/css\">\n";

	QString bgColour;
//#if 0
	QFont sdmono;
#if defined(Q_OS_WIN)
	sdmono = QFont("Verdana",9);
#elif defined(Q_OS_MAC)
	sdmono = QFont("Verdana", 12);
#else
	sdmono = QFont("Bitstream Vera Sans",8);
#endif

	QMap<int, QString> styles;
	for (int istyle = 0; istyle <= STYLE_MAX; istyle++) {
		if ((istyle > STYLE_DEFAULT) && (istyle <= STYLE_LASTPREDEFINED))
			continue;
		if (styleIsUsed[istyle]) {
			QFont font = lexer()->font(istyle);
			int size = font.pointSize();
			bool italics = font.italic();
			int weight = font.weight();
			bool bold = font.bold();
			QString family = font.family();
			QColor fore = lexer()->color(istyle);
			QColor back = lexer()->paper(istyle);
			QString styledSpan = "<span style=\"";
			//			if (CurrentBuffer()->useMonoFont && sd.font.length() && sdmono.font.length()) {
			//				sd.font = sdmono.font;
			//				sd.size = sdmono.size;
			//				sd.italics = sdmono.italics;
			//				sd.weight = sdmono.weight;
			//			}

			if (istyle == STYLE_DEFAULT) {
				retval += "span {\n";
			} else {
				retval += QString(".S%1 {\n").arg(istyle);
			}

			if (italics) {
				retval += "\tfont-style: italic;\n";
				styledSpan += "font-style: italic; ";
			}

			if (bold) {
				retval += "\tfont-weight: bold;\n";
				styledSpan += "font-weight: bold; ";
			}
			if (wysiwyg && !family.isEmpty()) {
				retval += QString("\tfont-family: '%1';\n").arg(family);
				styledSpan += QString("font-family: '%1'; ").arg(family);
			}
			if (fore.isValid()) {
				retval += QString("\tcolor: %1;\n").arg(fore.name());
				styledSpan += QString("color: rgb(%1,%2,%3); ").arg(fore.red()).arg(fore.green()).arg(fore.blue());
			} else if (istyle == STYLE_DEFAULT) {
				retval += "\tcolor: #000000;\n";
				styledSpan += QString("color: rgb(0,0,0); ");
			}
			if (back.isValid() &&  istyle != STYLE_DEFAULT) {
				retval += QString("\tbackground: %1;\n").arg(back.name());
				retval += QString("\ttext-decoration: inherit;\n");
				styledSpan += QString("background: rgb(%1,%2,%3); ").arg(back.red()).arg(back.green()).arg(back.blue());
			}
			if (wysiwyg && size) {
				retval += QString("\tfont-size: %1pt;\n").arg(size);
				styledSpan += QString("font-size: %1pt").arg(size);
			}
			retval += "}\n";
			styledSpan += "\">";
			styles.insert(istyle, styledSpan);
		}
	}

	retval += "</style>\n";
	retval += "</head>\n";

	if (!bgColour.isEmpty())
		retval += QString("<body bgcolor=\"%s\">\n").arg(bgColour);
	else
		retval += "<body>\n";

	//int line = acc.GetLine(0);
	int level = (getLevelAt(0) & SC_FOLDLEVELNUMBERMASK) - SC_FOLDLEVELBASE;
	int newLevel;
	int styleCurrent = getStyleAt(0);
	bool inStyleSpan = false;
	bool inFoldSpan = false;
	// Global span for default attributes
	if (wysiwyg) {
		retval += "<span>";
	} else {
		retval += "<pre>";
	}

	if (styleIsUsed[styleCurrent]) {
		if (styles.contains(styleCurrent))
			retval += styles.value(styleCurrent);
		else
			retval += SPAN_CLASS.arg(styleCurrent);
		inStyleSpan = true;
	}
	// Else, this style has no definition (beside default one):
	// no span for it, except the global one

	int column = 0;
	for (int i = startPos; i < endPos; i++)
	{
		char ch = getByteAt(i);
		int style = getStyleAt(i);

		if (style != styleCurrent) {
			if (inStyleSpan) {
				retval += "</span>";
				inStyleSpan = false;
			}
			if (ch != '\r' && ch != '\n') {	// No need of a span for the EOL
				if (styleIsUsed[style]) {
					if (styles.contains(style))
						retval += styles.value(style);
					else
						retval += SPAN_CLASS.arg(style);
					inStyleSpan = true;
				}
				styleCurrent = style;
			}
		}
		switch (ch)
		{
		case ' ':
		{
			if (wysiwyg) {
				char prevCh = '\0';
				if (column == 0) {	// At start of line, must put a &nbsp; because regular space will be collapsed
					prevCh = ' ';
				}
				while (i < endPos && getByteAt(i) == ' ') {
					if (prevCh != ' ') {
						retval += ' ';
					} else {
						retval += "&nbsp;";
					}
					prevCh = getByteAt(i);
					i++;
					column++;
				}
				i--; // the last incrementation will be done by the for loop
			} else {
				retval += ' ';
				column++;
			}
		}
		break;
		case '\t':
		{
			int ts = tabSize - (column % tabSize);
			if (wysiwyg) {
				for (int itab = 0; itab < ts; itab++) {
					if (itab % 2) {
						retval += ' ';
					} else {
						retval += "&nbsp;";
					}
				}
				column += ts;
			} else {
				if (tabs) {
					retval += ch;
					column++;
				} else {
					for (int itab = 0; itab < ts; itab++) {
						retval += ' ';
					}
					column += ts;
				}
			}
		}
		break;
		case '\r':
		case '\n':
		{
			if (inStyleSpan) {
				retval += "</span>";
				inStyleSpan = false;
			}
			if (inFoldSpan) {
				retval += "</span>";
				inFoldSpan = false;
			}
			if (ch == '\r' && getByteAt(i + 1) == '\n') {
				i++;	// CR+LF line ending, skip the "extra" EOL char
			}
			column = 0;
			if (wysiwyg) {
				retval += "<br />";
			}

			styleCurrent = getStyleAt(i + 1);
			retval += '\n';

			if (styleIsUsed[styleCurrent] && getByteAt(i + 1) != '\r' && getByteAt(i + 1) != '\n') {
				// We know it's the correct next style,
				// but no (empty) span for an empty line
				if (styles.contains(styleCurrent))
					retval += styles.value(styleCurrent);
				else
					retval += SPAN_CLASS.arg(styleCurrent);
				inStyleSpan = true;
			}
		}
		break;
		case '<':
			retval += "&lt;";
			column++;
			break;
		case '>':
			retval += "&gt;";
			column++;
			break;
		case '&':
			retval += "&amp;";
			column++;
			break;
		default:
			retval += ch;
			column++;
		}
	}

	if (inStyleSpan) {
		retval += "</span>";
	}

	if (!wysiwyg) {
		retval += "</pre>";
	} else {
		retval += "</span>";
	}

	retval += "\n</body>\n</html>\n";
	return retval;
}

// extracts control words that are different between two styles
static QString  GetRTFStyleChange(QString const& last, QString const& current)
{   // \f0\fs20\cf0\highlight0\b0\i0
	QString delta;
	QStringList lastL = last.split('\\');
	QStringList currentL = current.split('\\');

	// font face, size, color, background, bold, italic
	for (int i = 0; i < 6; i++) {
		if ( lastL.at(i) != currentL.at(i)) { // changed
			delta += '\\';
			delta += currentL.at(i);
		}
	}
	if (delta.isEmpty())
		delta += " ";
	return delta;
}

QString toMarkedText::getRTF()
{
	static const QString RTF_HEADEROPEN = "{\\rtf1\\ansi\\deff0\\deftab720";
	static const QString RTF_FONTDEFOPEN = "{\\fonttbl";
	static const QString RTF_FONTDEF = "{\\f%1\\fnil\\fcharset%2 %3;}";
	static const QString RTF_FONTDEFCLOSE = "}";
	static const QString RTF_COLORDEFOPEN = "{\\colortbl";
	static const QString RTF_COLORDEF = "\\red%1\\green%2\\blue%3;";
	static const QString RTF_COLORDEFCLOSE = "}";
	static const QString RTF_HEADERCLOSE = "\n";
	static const QString RTF_BODYOPEN = "";
	static const QString RTF_BODYCLOSE = "}";

	static const QString RTF_SETFONTFACE = "\\f";
	static const QString RTF_SETFONTSIZE = "\\fs%1";
	static const QString RTF_SETCOLOR = "\\cf%1";
	static const QString RTF_SETBACKGROUND = "\\highlight%1";
	static const QString RTF_BOLD_ON = "\\b";
	static const QString RTF_BOLD_OFF = "\\b0";
	static const QString RTF_ITALIC_ON = "\\i";
	static const QString RTF_ITALIC_OFF = "\\i0";
	static const QString RTF_UNDERLINE_ON = "\\ul";
	static const QString RTF_UNDERLINE_OFF = "\\ulnone";
	static const QString RTF_STRIKE_ON = "\\i";
	static const QString RTF_STRIKE_OFF = "\\strike0";

	static const QString RTF_EOLN = "\\par\n";
	static const QString RTF_TAB = "\\tab ";

	static const int MAX_STYLEDEF = 128;
	static const int MAX_FONTDEF  = 64;
	static const int MAX_COLORDEF = 8;
	static const QString RTF_FONTFACE = "Courier New";
	static const QString RTF_COLOR = "#000000";

	QString fp;
	int startPos = SendScintilla(SCI_GETSELECTIONSTART);
	int endPos = SendScintilla(SCI_GETSELECTIONEND);

	clearIndicatorRange(0, 0, lines(), lineLength(lines()-1), m_searchIndicator);
	recolor();

	// Read the default settings
	StyleDefinition defaultStyle;
	defaultStyle.font = lexer()->font(STYLE_DEFAULT);
	defaultStyle.size = lexer()->font(STYLE_DEFAULT).pointSize();
	defaultStyle.fore = lexer()->color(STYLE_DEFAULT);
	defaultStyle.back = lexer()->paper(STYLE_DEFAULT);
	defaultStyle.weight = lexer()->font(STYLE_DEFAULT).weight();
	defaultStyle.italics = lexer()->font(STYLE_DEFAULT).italic();
	defaultStyle.bold= lexer()->font(STYLE_DEFAULT).bold();

	bool tabs = true;
	int tabSize = 4;
	int wysiwyg = 1;

	QFont sdmono;
#if defined(Q_OS_WIN)
	sdmono = QFont("Verdana",9);
#elif defined(Q_OS_MAC)
	sdmono = QFont("Verdana", 12);
#else
	sdmono = QFont("Bitstream Vera Sans",8);
#endif

	QString fontFace = sdmono.family();
	if (fontFace.length()) {
		defaultStyle.font = fontFace;
	} else if (defaultStyle.font.family().isEmpty()) {
		defaultStyle.font = RTF_FONTFACE;
	}
	int fontSize = sdmono.pointSize();
	if (fontSize > 0) {
		defaultStyle.size = fontSize << 1;
	} else if (defaultStyle.size == 0) {
		defaultStyle.size = 10 << 1;
	} else {
		defaultStyle.size <<= 1;
	}

	QString styles[STYLE_DEFAULT + 1];
	QFont fonts[STYLE_DEFAULT + 1];
	QColor colors[STYLE_DEFAULT + 1];
	QString lastStyle, deltaStyle;
	int fontCount = 1, colorCount = 2, i;

	fp += RTF_HEADEROPEN;
	{ // fonts definitions
		fp += RTF_FONTDEFOPEN;

		fonts[0] = defaultStyle.font;

		unsigned int characterset = 1; //props.GetInt("character.set", SC_CHARSET_DEFAULT);
		fp += RTF_FONTDEF.arg(0).arg(characterset).arg(defaultStyle.font.family());

		colors[0] = defaultStyle.fore;
		colors[1] = defaultStyle.back;

		for (int istyle = 0; istyle < STYLE_DEFAULT; istyle++) {
			StyleDefinition sd;
			sd.font = lexer()->font(istyle);
			sd.size = lexer()->font(istyle).pointSize();
			sd.fore = lexer()->color(istyle);
			sd.back = lexer()->paper(istyle);
			sd.weight = lexer()->font(istyle).weight();
			sd.italics = lexer()->font(istyle).italic();
			sd.bold= lexer()->font(istyle).bold();

			for (i = 0; i < fontCount; i++)
				if (sd.font == fonts[i])
					break;
			if (i >= fontCount) {
				fonts[fontCount++] = sd.font;
				fp += RTF_FONTDEF.arg(i).arg(characterset).arg(sd.font.family());
			}
			lastStyle + RTF_SETFONTFACE + i;
			//lastStyle += RTF_SETFONTFACE + "0";

			lastStyle += RTF_SETFONTSIZE.arg( sd.size ? sd.size << 1 : defaultStyle.size);

			for (i = 0; i < colorCount; i++)
				if (sd.fore == colors[i])
					break;
			if (i >= colorCount)
				colors[colorCount++] = sd.fore;
			lastStyle += RTF_SETCOLOR.arg(i);
			//lastStyle += RTF_SETCOLOR "0";	// Default fore

			for (i = 0; i < colorCount; i++)
				if (sd.back == colors[i])
					break;
			if (i >= colorCount)
				colors[colorCount++] = sd.back;
			lastStyle += RTF_SETBACKGROUND.arg(i);
			//lastStyle += RTF_SETBACKGROUND + "1";	// Default back (use only one default background)


			lastStyle += sd.bold ? RTF_BOLD_ON : RTF_BOLD_OFF;
			lastStyle += sd.italics ? RTF_ITALIC_ON : RTF_ITALIC_OFF;

			styles[istyle] = lastStyle;
		}
		styles[STYLE_DEFAULT] = RTF_SETFONTFACE + "0"
				+ RTF_SETFONTSIZE.arg(defaultStyle.size)
				+ RTF_SETCOLOR.arg("0")
				+ RTF_SETBACKGROUND.arg("1")
				+ RTF_BOLD_OFF
				+ RTF_ITALIC_OFF;
		fp += RTF_FONTDEFCLOSE;
	}

	fp += RTF_COLORDEFOPEN;
	for (i = 0; i < colorCount; i++)
	{
		fp += RTF_COLORDEF.arg(colors[i].red()).arg(colors[i].green()).arg(colors[i].blue());
	}
	fp += RTF_COLORDEFCLOSE;

	fp += RTF_HEADERCLOSE;

	fp += RTF_BODYOPEN
			+ RTF_SETFONTFACE + "0"
			+ RTF_SETFONTSIZE.arg(defaultStyle.size)
			+ RTF_SETCOLOR.arg("0 ");
	lastStyle = styles[STYLE_DEFAULT];
	bool prevCR = false;
	int styleCurrent = -1;
	int column = 0;
	for (i = startPos; i < endPos; i++) {
		char ch = getByteAt(i);
		int style = getStyleAt(i);
		if (style > STYLE_DEFAULT)
			style = 0;
		if (style != styleCurrent) {
			deltaStyle = GetRTFStyleChange(lastStyle, styles[style]);
			fp += deltaStyle;
			lastStyle = styles[style];
			styleCurrent = style;
		}
		switch (ch)
		{
		case '{':
			fp +="\\{"; break;
		case '}':
			fp +="\\}"; break;
		case '\\':
			fp +="\\\\"; break;
		case '\t':
			if (tabs) {
				fp +=RTF_TAB;
			} else {
				int ts = tabSize - (column % tabSize);
				for (int itab = 0; itab < ts; itab++) {
					fp += ' ';
				}
				column += ts - 1;
			}
			break;
		case '\n':
			if (!prevCR) {
				fp +=RTF_EOLN;
				column = -1;
			}
			break;
		case '\r':
			fp +=RTF_EOLN;
			column = -1;
			break;
		default:
			fp += ch;
		}
		column++;
		prevCR = ch == '\r';
	}
	fp += RTF_BODYCLOSE;

	return fp;
}

void toMarkedText::insert(const QString &str, bool select)
{
    int lineFrom;
    int indexFrom;

    // Make this work as one undo so user doesn't see his code disappear first.
    QsciScintilla::beginUndoAction();

    QsciScintilla::removeSelectedText();

    if (select)
        getCursorPosition(&lineFrom, &indexFrom);

    // insert() doesn't work as advertised.
    // docs say: "The new current position if after the inserted text."

    // I thought it would mean that the cursor would be at the end of
    // the inserted text. Now I'm not really sure what the heck that
    // means.

    QsciScintilla::insert(str);

    // get new position and select if requested
    if (select)
        setSelection(lineFrom, indexFrom, lineFrom, indexFrom + str.length());

    QsciScintilla::endUndoAction();
}

/**
 * Find the start of the next word in either a forward (delta >= 0) or backwards direction
 * (delta < 0).
 * This is looking for a transition between character classes although there is also some
 * additional movement to transit white space.
 * Used by cursor movement by word commands.
 */
int toMarkedText::NextWordStart(int pos, int delta) {
	int length = text().length(); // get length in chars. (while pure length() return number of bytes)
	if (delta < 0) {
		while (pos > 0 && (m_charClasifier.GetClass(getByteAt(pos - 1)) == CharClassify::ccSpace))
			pos--;
		if (pos > 0) {
			CharClassify::cc ccStart = m_charClasifier.GetClass(getByteAt(pos-1));
			while (pos > 0 && (m_charClasifier.GetClass(getByteAt(pos - 1)) == ccStart)) {
				pos--;
			}
		}
	} else {
		CharClassify::cc ccStart = m_charClasifier.GetClass(getByteAt(pos));
		while (pos < (length) && (m_charClasifier.GetClass(getByteAt(pos)) == ccStart))
			pos++;
		while (pos < (length) && (m_charClasifier.GetClass(getByteAt(pos)) == CharClassify::ccSpace))
			pos++;
	}
	return pos;
}

/**
 * Find the end of the next word in either a forward (delta >= 0) or backwards direction
 * (delta < 0).
 * This is looking for a transition between character classes although there is also some
 * additional movement to transit white space.
 * Used by cursor movement by word commands.
 */
int toMarkedText::NextWordEnd(int pos, int delta) {
	int length = text().length(); // get length in chars. (while pure length() return number of bytes)
	if (delta < 0) {
		if (pos > 0) {
			CharClassify::cc ccStart = m_charClasifier.GetClass(getByteAt(pos-1));
			if (ccStart != CharClassify::ccSpace) {
				while (pos > 0 && m_charClasifier.GetClass(getByteAt(pos - 1)) == ccStart) {
					pos--;
				}
			}
			while (pos > 0 && m_charClasifier.GetClass(getByteAt(pos - 1)) == CharClassify::ccSpace) {
				pos--;
			}
		}
	} else {
		while (pos < length && m_charClasifier.GetClass(getByteAt(pos)) == CharClassify::ccSpace) {
			pos++;
		}
		if (pos < length) {
			CharClassify::cc ccStart = m_charClasifier.GetClass(getByteAt(pos));
			while (pos < length && m_charClasifier.GetClass(getByteAt(pos)) == ccStart) {
				pos++;
			}
		}
	}
	return pos;
}

char toMarkedText::getByteAt(int pos)
{
	char ch = SendScintilla(SCI_GETCHARAT, pos);
	return ch;
}

int toMarkedText::getStyleAt(int pos)
{
	int style = SendScintilla(QsciScintilla::SCI_GETSTYLEAT, pos);
	return style;
}

int toMarkedText::getLevelAt(int line)
{
	int level = SendScintilla(QsciScintilla::SCI_GETFOLDLEVEL, line);
	return level;
}

wchar_t toMarkedText::getWCharAt(int pos) {
	//http://vacuproj.googlecode.com/svn/trunk/npscimoz/npscimoz/oldsrc/trunk.nsSciMoz.cxx
	char _retval[4];
    /*
     * This assumes that Scintilla is using an utf-8 byte-addressed buffer.
     *
     * Return the character that is represented by the utf-8 sequence at
     * the requested position (we could be in the middle of the sequence).
     */
    int byte, byte2, byte3;

    /*
     * Unroll 1 to 3 byte UTF-8 sequences.  See reference data at:
     * http://www.cl.cam.ac.uk/~mgk25/unicode.html
     * http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt
     *
     * SendEditor must always be cast to return an unsigned char.
     */

    byte = (unsigned char) SendScintilla(SCI_GETCHARAT, pos);
    if (byte < 0x80) {
    	/*
    	 * Characters in the ASCII charset.
    	 * Also treats \0 as a valid characters representing itself.
    	 */
    	return byte;
    }

    while ((byte < 0xC0) && (byte >= 0x80) && (pos > 0)) {
    	/*
    	 * Naked trail byte.  We asked for an index in the middle of
    	 * a UTF-8 char sequence.  Back up to the beginning.  We should
    	 * end up with a start byte >= 0xC0 and <= 0xFD, but check against
    	 * 0x80 still in case we have a screwy buffer.
    	 *
    	 * We could store bytes as we walk backwards, but this shouldn't
    	 * be the common case.
    	 */
    	byte = (unsigned char) SendScintilla(SCI_GETCHARAT, --pos);
    }

    if (byte < 0xC0) {
    	/*
    	 * Handles properly formed UTF-8 characters between 0x01 and 0x7F.
    	 * Also treats \0 and naked trail bytes 0x80 to 0xBF as valid
    	 * characters representing themselves.
    	 */
    } else if (byte < 0xE0) {
    	byte2 = (unsigned char) SendScintilla(SCI_GETCHARAT, pos+1);
    	if ((byte2 & 0xC0) == 0x80) {
    		/*
    		 * Two-byte-character lead-byte followed by a trail-byte.
    		 */
    		byte = (((byte & 0x1F) << 6) | (byte2 & 0x3F));
    	}
    	/*
    	 * A two-byte-character lead-byte not followed by trail-byte
    	 * represents itself.
    	 */
    } else if (byte < 0xF0) {
    	byte2 = (unsigned char) SendScintilla(SCI_GETCHARAT, pos+1);
    	byte3 = (unsigned char) SendScintilla(SCI_GETCHARAT, pos+2);
    	if (((byte2 & 0xC0) == 0x80) && ((byte3 & 0xC0) == 0x80)) {
    		/*
    		 * Three-byte-character lead byte followed by two trail bytes.
    		 */

    		byte = (((byte & 0x0F) << 12)
    				| ((byte2 & 0x3F) << 6) | (byte3 & 0x3F));
    	}
    	/*
    	 * A three-byte-character lead-byte not followed by two trail-bytes
    	 * represents itself.
    	 */
    }
#if 0
    /*
     * Byte represents a 4-6 byte sequence.  The rest of Komodo currently
     * won't support this, which makes this code very hard to test.
     * Leave it commented out until we have better 4-6 byte UTF-8 support.
     */
    else {
	/*
	 * This is the general loop construct for building up Unicode
	 * from UTF-8, and could be used for 1-6 byte len sequences.
	 *
	 * The following structure is used for mapping current UTF-8 byte
	 * to number of bytes trail bytes.  It doesn't backtrack from
	 * the middle of a UTF-8 sequence.
	 */
	static const unsigned char totalBytes[256] = {
	    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,6,6
	};
	int ch, trail;

	trail = totalBytes[byte] - 1; // expected number of trail bytes
	if (trail > 0) {
	    ch = byte & (0x3F >> trail);
	    do {
		byte2 = (unsigned char) SendEditor(SCI_GETCHARAT, ++pos, 0);
		if ((byte2 & 0xC0) != 0x80) {
		    *_retval = (PRUnichar) byte;
		    return NS_OK;
		}
		ch <<= 6;
		ch |= (byte2 & 0x3F);
		trail--;
	    } while (trail > 0);
	    *_retval = (PRUnichar) ch;
	    return NS_OK;
	}
    }
#endif

    return byte;
}

toMarkedText::CharClassify toMarkedText::m_charClasifier;

toMarkedText::CharClassify::CharClassify() {
	SetDefaultCharClasses(true);
}

void toMarkedText::CharClassify::SetDefaultCharClasses(bool includeWordClass) {
	// Initialize all char classes to default values
	for (int ch = 0; ch < 256; ch++) {
		if (ch == '\r' || ch == '\n')
			charClass[ch] = ccNewLine;
		else if (ch < 0x20 || ch == ' ')
			charClass[ch] = ccSpace;
		else if (includeWordClass && (ch >= 0x80 || isalnum(ch) || ch == '_' || ch == '$' || ch == '#' || ch == ':' || ch == '@' ))
			charClass[ch] = ccWord;
		else
			charClass[ch] = ccPunctuation;
	}
}

void toMarkedText::CharClassify::SetCharClasses(const unsigned char *chars, cc newCharClass) {
	// Apply the newCharClass to the specifed chars
	if (chars) {
		while (*chars) {
			charClass[*chars] = static_cast<unsigned char>(newCharClass);
			chars++;
		}
	}
}

int toMarkedText::CharClassify::GetCharsOfClass(cc characterClass, unsigned char *buffer) {
	// Get characters belonging to the given char class; return the number
	// of characters (if the buffer is NULL, don't write to it).
	int count = 0;
	for (int ch = maxChar - 1; ch >= 0; --ch) {
		if (charClass[ch] == characterClass) {
			++count;
			if (buffer) {
				*buffer = static_cast<unsigned char>(ch);
				buffer++;
			}
		}
	}
	return count;
}
