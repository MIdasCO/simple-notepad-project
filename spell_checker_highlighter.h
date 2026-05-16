#ifndef SPELL_CHECKER_HIGHLIGHTER_H
#define SPELL_CHECKER_HIGHLIGHTER_H

#include "spell_checker.h"

#include <QSyntaxHighlighter>

class spell_checker_highlighter : public QSyntaxHighlighter {
public:
    spell_checker_highlighter(QTextDocument* parent, const spell_checker* checker);

protected:
    void highlightBlock(const QString& text) override;

private:
    const spell_checker* checker;
};

#endif // SPELL_CHECKER_HIGHLIGHTER_H
