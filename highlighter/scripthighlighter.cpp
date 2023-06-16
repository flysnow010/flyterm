#include "scripthighlighter.h"

ScriptHighlighter::ScriptHighlighter(QObject *parent) : QSyntaxHighlighter(parent)
{
    addCommandFormat();
}

void ScriptHighlighter::addCommandFormat()
{
    HighlightingRule rule;
    QTextCharFormat format;

    format.setFontWeight(QFont::Bold);
    format.setForeground(Qt::darkGreen);
    rule.format = format;

    QStringList keywords = QString("start;end;bsave;esave;"
                                   "sleep;msleep;ksend;xsend;ysend").split(";");
    for(auto const& keyword : keywords)
    {
        rule.pattern = QRegularExpression(QString("\\b%1\\b").arg(keyword));
        highlightingRules.append(rule);
    }
}

void ScriptHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
