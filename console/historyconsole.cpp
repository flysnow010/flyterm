#include "historyconsole.h"
#include <QTextDocumentWriter>
#include <QTextCodec>
#include <QFile>

HistoryConsole::HistoryConsole(QWidget *parent)
    : QTextBrowser(parent)
{
    textFormat.setFontFamily(fontName_);
    textFormat.setFontPointSize(fontSize_);
    mergeCurrentCharFormat(textFormat);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setFrameShape(QFrame::Shape::Box);
}

void HistoryConsole::saveToFile(QString const& fileName)
{
    QTextDocumentWriter writer(fileName);
    writer.write(document());
}

void HistoryConsole::loadFromFile(QString const& fileName)
{
     QFile file(fileName);
     if (!file.open(QFile::ReadOnly))
         return;

     QByteArray data = file.readAll();
     QTextCodec *codec = Qt::codecForHtml(data);
     QString str = codec->toUnicode(data);
     if (Qt::mightBeRichText(str)) {
         setHtml(str);
     } else {
         str = QString::fromLocal8Bit(data);
         setPlainText(str);
     }
}
