#include "defaulthightlighter.h"
#include "hightlightermanager.h"

DefaultHightLighter::DefaultHightLighter(QObject *parent)
    : QSyntaxHighlighter(parent)
{
}

void DefaultHightLighter::highlightBlock(const QString &)
{
}

