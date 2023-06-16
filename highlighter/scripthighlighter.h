#ifndef SCRIPTHIGHLIGHTER_H
#define SCRIPTHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class ScriptHighlighter : public QSyntaxHighlighter
{
public:
    explicit ScriptHighlighter(QObject *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;
private:
     void addCommandFormat();
private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
};

#endif // SCRIPTHIGHLIGHTER_H
