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

#ifndef SERIALPORTCONSOLE_H
#define SERIALPORTCONSOLE_H

#include <QTextEdit>
#include "core/logfile.h"
#include "color/consolecolor.h"
#include "console/consolescreen.h"

class ConsoleParser;
class QSyntaxHighlighter;
class QMimeData;
class SerialPortConsole : public QTextEdit
{
    Q_OBJECT
public:
    explicit SerialPortConsole(QWidget *parent = nullptr);
    ~SerialPortConsole();

    void putData(const QByteArray &data);
    void setLocalEchoEnabled(bool set);
    void setLogFile(LogFile::Ptr const& logfile) { logfile_ = logfile; }
    void setFontName(QString const& name);
    void setFontSize(int fontSize);
    void setConsoleColor(ConsoleColor const& color);
    void setConsolePalette(ConsolePalette::Ptr palette);

    int fontSize() const { return fontSize_; }
    void updateHightLighter(QString const& hightLighter);
    void cancelSelection();
    struct ColorRange
    {
        ColorRole role;
        int start;
        int end;
    };
signals:
    void getData(const QByteArray &data);
    void onGotCursorPos(int row, int col);
public slots:
    void copyAll();
    void copyOne();
    bool increaseFontSize();
    bool decreaseFontSize();
    void saveToFile();
protected:
    void keyPressEvent(QKeyEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void insertFromMimeData(const QMimeData *source) override;
private slots:
    void onBeep();
    void onGetCursorPos();
    void onBackspace(int count);
    void onLeft(int count);
    void onRight(int count);
    void onEnd();
    void onText(QString const& text);
    void onForeColor(ColorRole role);
    void onColorClose();
private:
    void removeCurrentRow();
    void clearSelection();
    int selectText();
    int selectText(int start, int end);
    void setForeColor(ColorRole role);
    void setCloseColor();
private:
    ConsoleParser *commandParser;
    ConsolePalette::Ptr palette_;
    QSyntaxHighlighter *highlighter = nullptr;
    LogFile::Ptr logfile_;
    ColorRole currentForeRole;
    ColorRole currentBackRole;
    QString fontName_ = "Courier New";
    int     fontSize_ = 12;
    QList<ColorRange> colorRanges;
    QList<int> standardSizes;
    bool isUseColor = false;
    QTextCharFormat textFormat;
    QTextCharFormat normalFormat;
    bool m_localEchoEnabled = false;
    int cursorPos = 0;
    int selectStart = 0;
    int selectEnd = 0;
    ConsoleScreen screen;
};

#endif // SERIALPORTCONSOLE_H
