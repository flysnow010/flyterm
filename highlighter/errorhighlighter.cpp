#include "errorhighlighter.h"
#include "hightlightermanager.h"

ErrorHighlighter::ErrorHighlighter(QObject *parent)
    : QSyntaxHighlighter(parent)
{
    addErrorFromat();
    addSuccessFormat();
    addWarningFormat();
    addCommandFormat();
}

void ErrorHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}

void ErrorHighlighter::addErrorFromat()
{
    HighlightingRule rule;
    QTextCharFormat format;

    format.setFontWeight(QFont::Bold);
    format.setForeground(Qt::red);
    rule.format = format;

    QStringList keywords = QString("[Ee]rror;[Ee]rrors;ERROR;[Ff]ailing;[Ff]ailed").split(";");
    for(auto const& keyword : keywords)
    {
        rule.pattern = QRegularExpression(QString("\\b%1\\b").arg(keyword));
        highlightingRules.append(rule);
    }
}

void ErrorHighlighter::addSuccessFormat()
{
    HighlightingRule rule;
    QTextCharFormat format;

    format.setFontWeight(QFont::Bold);
    format.setForeground(Qt::green);
    rule.format = format;

    QStringList keywords = QString("[Oo][Kk];[Ss][uU][cC][cC][eE][sS][sS];[Ff]inished;").split(";");
    for(auto const& keyword : keywords)
    {
        rule.pattern = QRegularExpression(QString("\\b%1\\b").arg(keyword));
        highlightingRules.append(rule);
    }
}

void ErrorHighlighter::addWarningFormat()
{
    HighlightingRule rule;
    QTextCharFormat format;

    format.setFontWeight(QFont::Bold);
    format.setForeground(Qt::yellow);
    rule.format = format;

    QStringList keywords = QString("[Ww]arning;[Nn]ot;[Nn]o;[Uu]nkown").split(";");
    for(auto const& keyword : keywords)
    {
        rule.pattern = QRegularExpression(QString("\\b%1\\b").arg(keyword));
        highlightingRules.append(rule);
    }
}

void ErrorHighlighter::addCommandFormat()
{
    HighlightingRule rule;
    QTextCharFormat format;

    format.setFontItalic(true);
    format.setFontWeight(QFont::Bold);
    rule.format = format;

    QStringList keywords = QString("sudo;cd;arch;ln;pidof;sh;ash;echo;login;"
                                   "ping;sleep;egrep;ls;ping6;stty;"
                                   "cat;false;lsattr;su;chattr;fdflush;"
                                   "mkdir;printenv;sync;chgrp;fgrep;mknod;"
                                   "ps;tar;chmod;getopt;mktemp;pwd;touch;"
                                   "chown;grep;more;resume;true;cp;gunzip;"
                                   "mount;rm;umount;cpio;gzip;mountpoint;"
                                   "rmdir;uname;date;hostname;mt;usleep;"
                                   "dd;kill;mv;sed;vi;df;link;netstat;"
                                   "setarch;watch;dmesg;nice;setpriv;zcat").split(";");
    for(auto const& keyword : keywords)
    {
        rule.pattern = QRegularExpression(QString("\\b%1\\b").arg(keyword));
        highlightingRules.append(rule);
    }
}

