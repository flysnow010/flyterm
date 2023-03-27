/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
    : QTextEdit(parent)
    , commandParser(new SerialPortParser())
{
    document()->setMaximumBlockCount(1000);
    setContextMenuPolicy(Qt::CustomContextMenu);

    standardSizes = QFontDatabase::standardSizes();

    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::white);
    setPalette(p);

    textFormat.setFontFamily(fontName_);
    textFormat.setFontPointSize(fontSize_);
    connect(commandParser, SIGNAL(onBeep()), this, SLOT(onBeep()));
    connect(commandParser, SIGNAL(onGetCursorPos()), this, SLOT(onGetCursorPos()));
    connect(commandParser, SIGNAL(onForeColor(ColorRole)),
            this, SLOT(onForeColor(ColorRole)));
    connect(commandParser, SIGNAL(onColorClose()), this, SLOT(onColorClose()));
    connect(commandParser, SIGNAL(onBackspace(int)), this, SLOT(onBackspace(int)));
    connect(commandParser, SIGNAL(onLeft(int)), this, SLOT(onLeft(int)));
    connect(commandParser, SIGNAL(onRight(int)), this, SLOT(onRight(int)));
    connect(commandParser, SIGNAL(onText(QString)), this, SLOT(onText(QString)));
}

SerialPortConsole::~SerialPortConsole()
{
    delete commandParser;
    delete highlighter;
}

void SerialPortConsole::updateHightLighter(QString const& hightLighter)
{
    if(highlighter)
    {
        highlighter->setDocument(nullptr);
        delete highlighter;
        highlighter = nullptr;
    }
    highlighter = highLighterManager->create(hightLighter);
    if(highlighter)
    {
        highlighter->setDocument(document());
        highlighter->rehighlight();
    }
}

void SerialPortConsole::putData(const QByteArray &data)
{
    commandParser->parse(data);
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void SerialPortConsole::onBeep()
{
    QApplication::beep();
}

void SerialPortConsole::onGetCursorPos()
{
    emit onGotCursorPos(screen.row(), screen.col());
}

void SerialPortConsole::onBackspace(int count)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, count);
    cursor.deleteChar();
    setTextCursor(cursor);
}

void SerialPortConsole::onLeft(int count)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, count);
    setTextCursor(cursor);
}

void SerialPortConsole::onRight(int count)
{
    QTextCursor cursor = textCursor();
    if(count + screen.col() < screen.cols())
    {
        cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, count);
        screen.cursorRight(count);
    }
    else
    {
        screen.cursorRight(screen.cols() - screen.col());
        cursor.movePosition(QTextCursor::Right,
                            QTextCursor::MoveAnchor,
                            screen.cols() - screen.col());
    }
    setTextCursor(cursor);
}

void SerialPortConsole::onEnd()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
}

void SerialPortConsole::onText(QString const& text)
{
    if(logfile_)
        logfile_->write(text);
    if(text == "\r")
        return;

    QTextCursor tc = textCursor();
    if(text.startsWith("\r") && !text.startsWith("\r\n"))
    {
        removeCurrentRow();
        tc.insertText(text.right(text.size() - 1), isUseColor ? textFormat : normalFormat);
    }
    else
    {
        if(text.endsWith("\r"))// \r 和 \n分两次来
            tc.insertText(text.left(text.size() - 1), isUseColor ? textFormat : normalFormat);
        else
            tc.insertText(text, isUseColor ? textFormat : normalFormat);
    }

    if(isUseColor)
    {
        ColorRange colorRange;
        colorRange.role = currentForeRole;
        colorRange.end = tc.position();
        colorRange.start = colorRange.end - text.size();
        colorRanges << colorRange;
    }
}

void SerialPortConsole::onForeColor(ColorRole role)
{
    setForeColor(role);
}

void SerialPortConsole::onColorClose()
{
    setCloseColor();
}

void SerialPortConsole::setLocalEchoEnabled(bool set)
{
    m_localEchoEnabled = set;
}

void SerialPortConsole::copyAll()
{
    selectAll();
    copy();
    clearSelection();
}

void SerialPortConsole::clearall()
{
    clear();
    colorRanges.clear();
}

void SerialPortConsole::copyOne()
{
    if(selectStart != selectEnd)
    {
        int pos = selectText();
        setTextColor(palette().color(QPalette::Text));
        setTextBackgroundColor(palette().color(QPalette::Base));
        copy();

        QTextCursor cursor = textCursor();
        cursor.setPosition(pos);
        setTextCursor(cursor);
    }
}

void SerialPortConsole::setFontName(QString const& name)
{
    fontName_ = name;
    textFormat.setFontFamily(fontName_);
    normalFormat.setFontFamily(fontName_);
    selectAll();
    setFontFamily(fontName_);
    clearSelection();
}

void SerialPortConsole::setFontSize(int fontSize)
{
    fontSize_ = fontSize;
    textFormat.setFontPointSize(fontSize_);
    normalFormat.setFontPointSize(fontSize_);
    selectAll();
    setFontPointSize(fontSize_);
    clearSelection();
}

void SerialPortConsole::setConsoleColor(ConsoleColor const& color)
{
    QPalette p = palette();
    p.setColor(QPalette::Base, color.back);
    p.setColor(QPalette::Text, color.fore);
    setPalette(p);
}

void SerialPortConsole::setConsolePalette(ConsolePalette::Ptr palette)
{
    palette_ = palette;
    int oldPos = -1;
    for(int i = 0; i < colorRanges.size(); i++)
    {
        int pos = selectText(colorRanges[i].start, colorRanges[i].end);
        setTextColor(palette_->color(colorRanges[i].role).fore);
        if(oldPos == -1)
            oldPos = pos;
    }
    if(oldPos > 0)
    {
        QTextCursor cursor = textCursor();
        cursor.setPosition(oldPos);
        setTextCursor(cursor);
    }
}

bool SerialPortConsole::increaseFontSize()
{
    int fontIndex = standardSizes.indexOf(fontSize_) + 1;
    if(fontIndex < standardSizes.size())
    {
        setFontSize(standardSizes.at(fontIndex));
        return true;
    }
    return false;
}

bool SerialPortConsole::decreaseFontSize()
{
    int fontIndex = standardSizes.indexOf(fontSize_) - 1;
    if(fontIndex >= 0)
    {
        setFontSize(standardSizes.at(fontIndex));
        return true;
    }
    return false;
}

void SerialPortConsole::saveToFile()
{
    QFileDialog fileDialog(this, tr("Save as..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList mimeTypes;
    mimeTypes << "text/plain" << "text/html";
    fileDialog.setMimeTypeFilters(mimeTypes);
    fileDialog.setDefaultSuffix("txt");
    if (fileDialog.exec() != QDialog::Accepted)
        return;

   QStringList fileNames = fileDialog.selectedFiles();
   if(fileNames.isEmpty())
       return;
   QTextDocumentWriter writer(fileNames[0]);
   writer.write(document());
}

void SerialPortConsole::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Left:
        emit getData("\033[D");
        commandParser->setLeftKeyPress(true);
        break;
    case Qt::Key_Right:
        emit getData("\033[C");;
        commandParser->setRightKeyPress(true);
        break;
    case Qt::Key_Up:
        emit getData("\033[A");
        break;
    case Qt::Key_Down:
        emit getData("\033[B");
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        onEnd();
        emit getData(e->text().toLocal8Bit());
        commandParser->setEnterKeyPress(true);
        break;
    default:
        if (m_localEchoEnabled)
            QTextEdit::keyPressEvent(e);
        emit getData(e->text().toLocal8Bit());
        break;
    }
}

void SerialPortConsole::mousePressEvent(QMouseEvent * e)
{
    cursorPos = textCursor().position();
    if(e->buttons() & Qt::LeftButton)
        cancelSelection();


    QTextEdit::mousePressEvent(e);
}

void SerialPortConsole::mouseReleaseEvent(QMouseEvent *e)
{
    QTextEdit::mouseReleaseEvent(e);

    QTextCursor cursor = textCursor();
    setTextColor(palette().color(QPalette::HighlightedText));
    setTextBackgroundColor(palette().color(QPalette::Highlight));
    if(cursor.hasSelection())
    {
        selectStart = cursor.selectionStart();
        selectEnd = cursor.selectionEnd();
    }

    QScrollBar *bar = verticalScrollBar();
    int value = bar->value();
    cursor.setPosition(cursorPos);
    setTextCursor(cursor);
    bar->setValue(value);
}

void SerialPortConsole::mouseDoubleClickEvent(QMouseEvent *e)
{
    int pos = textCursor().position();
    QTextEdit::mouseDoubleClickEvent(e);
    setTextColor(palette().color(QPalette::HighlightedText));
    setTextBackgroundColor(palette().color(QPalette::Highlight));

    QTextCursor cursor = textCursor();
    selectStart = cursor.selectionStart();
    selectEnd = cursor.selectionEnd();
    cursor.setPosition(pos);
    setTextCursor(cursor);
}

void SerialPortConsole::insertFromMimeData(const QMimeData *source)
{
    if(source->hasText())
       emit getData(source->text().toUtf8());
    QTextEdit::insertFromMimeData(source);
}

void SerialPortConsole::removeCurrentRow()
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::LineUnderCursor);
    cursor.removeSelectedText();
    setTextCursor(cursor);
}

void SerialPortConsole::clearSelection()
{
    QTextCursor cursor = textCursor();
    cursor.clearSelection();
    setTextCursor(cursor);
}

void SerialPortConsole::cancelSelection()
{
    if(selectStart != selectEnd)
    {
        int pos = selectText();
        setTextColor(palette().color(QPalette::Text));
        setTextBackgroundColor(palette().color(QPalette::Base));
        QTextCursor cursor = textCursor();
        cursor.setPosition(pos);
        setTextCursor(cursor);
        updateColors();
        selectStart = 0;
        selectEnd = 0;
    }
}

int SerialPortConsole::selectText()
{
    QTextCursor cursor = textCursor();
    int pos = cursor.position();
    cursor.setPosition(selectStart);
    cursor.movePosition(QTextCursor::Right,
                        QTextCursor::KeepAnchor, selectEnd - selectStart);
    setTextCursor(cursor);
    return pos;
}

int SerialPortConsole::selectText(int start, int end)
{
    QTextCursor cursor = textCursor();
    int pos = cursor.position();
    cursor.setPosition(start);
    cursor.movePosition(QTextCursor::Right,
                        QTextCursor::KeepAnchor, end - start);
    setTextCursor(cursor);
    return pos;
}

void SerialPortConsole::updateColors()
{
    int oldPos = -1;
    for(int i = 0; i < colorRanges.size(); i++)
    {
        if(!(selectStart > colorRanges[i].end || selectEnd < colorRanges[i].start))
        {
            int pos = selectText(colorRanges[i].start, colorRanges[i].end);
            setTextColor(palette_->color(colorRanges[i].role).fore);
            if(oldPos == -1)
                oldPos = pos;
        }
    }
    if(oldPos > 0)
    {
        QTextCursor cursor = textCursor();
        cursor.setPosition(oldPos);
        setTextCursor(cursor);
    }
}

void SerialPortConsole::setForeColor(ColorRole role)
{
    isUseColor = true;
    currentForeRole = role;
    if(palette_)
        textFormat.setForeground(QBrush(palette_->color(role).fore));
}

void SerialPortConsole::setCloseColor()
{
    isUseColor = false;
    currentBackRole = ColorRole::NullRole;
    currentForeRole = ColorRole::NullRole;
    textFormat.setForeground(palette().color(QPalette::Text));
}
