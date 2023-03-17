#ifndef ERRORHIGHLIGHTER_H
#define ERRORHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class QTextDocument;
class ErrorHighlighter : public QSyntaxHighlighter
{
public:
    explicit ErrorHighlighter(QObject *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
     void addErrorFromat();
     void addSuccessFormat();
     void addWarningFormat();
     void addCommandFormat();

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
};

#endif // ERRORHIGHLIGHTER_H
