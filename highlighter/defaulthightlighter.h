#ifndef DEFAULTHIGHTLIGHTER_H
#define DEFAULTHIGHTLIGHTER_H
#include <QSyntaxHighlighter>

class DefaultHightLighter : public QSyntaxHighlighter
{
public:
    DefaultHightLighter(QObject *parent = nullptr);
protected:
    void highlightBlock(const QString &text) override;
};

#endif // DEFAULTHIGHTLIGHTER_H
