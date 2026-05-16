#include "spell_checker_highlighter.h"

#include <QRegularExpression>
#include <QTextCharFormat>

spell_checker_highlighter::spell_checker_highlighter(QTextDocument* parent, const spell_checker* checker)
    : QSyntaxHighlighter(parent)
    , checker(checker)
{
}

void spell_checker_highlighter::highlightBlock(const QString& text)
{
    if (checker == nullptr) {
        return;
    }

    static const QRegularExpression token_regex(R"([^\s]+)");
    auto it = token_regex.globalMatch(text);

    QTextCharFormat fmt;
    fmt.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    fmt.setUnderlineColor(Qt::red);

    while (it.hasNext()) {
        const auto match = it.next();
        const QString token = match.captured(0);
        if (!checker->is_correct_word(token)) {
            setFormat(match.capturedStart(), match.capturedLength(), fmt);
        }
    }
}
