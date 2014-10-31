#include "parsing/tolexeroracleapis.h"
#include "parsing/tolexeroracle.h"
#include "parsing/persistenttrie.h"
#include "editor/tosqltext.h"

toLexerOracleAPIs::toLexerOracleAPIs(QsciLexer *lexer) : QsciAbstractAPIs(lexer)
{
};

toLexerOracleAPIs::~toLexerOracleAPIs()
{
};

toLexerOracle* toLexerOracleAPIs::lexer() const
{
    return dynamic_cast<toLexerOracle*>(QsciAbstractAPIs::lexer());
};

void toLexerOracleAPIs::updateAutoCompletionList(const QStringList &context, QStringList &list)
{
    list << "AAA" << "BBB";
    toSqlText* editor = dynamic_cast<toSqlText*>(lexer()->editor());
    if (editor == NULL)
        return;

    QmlJS::PersistentTrie::Trie trie;

    QString text = editor->text();
    int len = editor->length();
    char* bufferText = new char[len+1];
    editor->SendScintilla(QsciScintillaBase::SCI_GETTEXT, len, bufferText);
    bufferText[len] = '\0';
    std::auto_ptr <SQLLexer::Lexer> lexer = LexerFactTwoParmSing::Instance().create("OracleGuiLexer", "", "toCustomLexer");
    lexer->setStatement(bufferText, len);
    SQLLexer::Lexer::token_const_iterator it = lexer->begin();
    while (it != lexer->end())
    {
        SQLLexer::Token::TokenType t = it->getTokenType();
        if (it->getTokenType() == SQLLexer::Token::L_IDENTIFIER)
            trie.insert(it->getText());
        it++;
    }
    list << trie.complete(context.last());
}

// Return the call tip for a function.
QStringList toLexerOracleAPIs::callTips(const QStringList &context, int commas,
                                        QsciScintilla::CallTipsStyle style,
                                        QList<int> &shifts)
{
    //QStringList wseps = lexer()->autoCompletionWordSeparators();
    QStringList cts;
    return cts;
}
