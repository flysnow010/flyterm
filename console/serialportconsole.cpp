
#include "serialportconsole.h"
#include "core/serialportparser.h"
#include "highlighter/hightlightermanager.h"
#include <QScrollBar>
#include <QTextBlock>
#include <QFileDialog>
#include <QTextDocumentWriter>
#include <QApplication>
#include <QMimeData>
#include <QDebug>

SerialPortConsole::SerialPortConsole(QWidget *parent)
    : Console(parent)
{
}

ConsoleParser * SerialPortConsole::createParser()
{
    return new SerialPortParser();
}


