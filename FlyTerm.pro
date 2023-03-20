QT       += core gui network serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
INCLUDEPATH += ./include
DEFINES += CRYPTOPP_IMPORTS
CONFIG(debug, debug|release){
    LIBS += -L$$(PWD)\lib -lQSshd -lQTelnetd -lCrypto
}
else
{
    LIBS += -L$$(PWD)\lib -lQSsh -lQTelnet -lCrypto
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    child/serialportwidget.cpp \
    child/sshwidget.cpp \
    child/telnetwidget.cpp \
    color/consolecolor.cpp \
    color/consolepalette.cpp \
    console/alternateconsole.cpp \
    console/commandconsole.cpp \
    console/consolescreen.cpp \
    console/historyconsole.cpp \
    console/serialportconsole.cpp \
    console/sshconsole.cpp \
    console/telnetconsole.cpp \
    core/commandmanager.cpp \
    core/commandthread.cpp \
    core/consoleparser.cpp \
    core/logfile.cpp \
    core/passwordserver.cpp \
    core/serialportparser.cpp \
    core/serialsession.cpp \
    core/session.cpp \
    core/sshparser.cpp \
    core/sshsession.cpp \
    core/telnetparser.cpp \
    core/telnetsession.cpp \
    dialog/aboutdialog.cpp \
    dialog/commanddialog.cpp \
    dialog/connectdialog.cpp \
    dialog/passworddialog.cpp \
    dialog/sendfileprogressdialog.cpp \
    dock/buttonsdockwidget.cpp \
    dock/commanddockwidget.cpp \
    dock/sessiondockwidget.cpp \
    highlighter/defaulthightlighter.cpp \
    highlighter/errorhighlighter.cpp \
    highlighter/hightlightermanager.cpp \
    main.cpp \
    mainwindow.cpp \
    model/sessionmodel.cpp \
    model/treeitem.cpp \
    model/treemodel.cpp \
    service/tftp/baseudp.cpp \
    service/tftp/tftp.cpp \
    service/tftp/tftpfile.cpp \
    service/tftp/tftpserver.cpp \
    service/tftp/udpsocket.cpp \
    transfer/xyzmodem/modem.cpp \
    transfer/xyzmodem/xmodem.cpp \
    transfer/xyzmodem/xymodemfilesender.cpp \
    transfer/xyzmodem/xymodemsendfile.cpp \
    transfer/xyzmodem/ymoderm.cpp \
    transfer/xyzmodem/zmodem.cpp \
    util/util.cpp

HEADERS += \
    child/serialportwidget.h \
    child/sshwidget.h \
    child/telnetwidget.h \
    color/consolecolor.h \
    color/consolepalette.h \
    color/consolerole.h \
    console/alternateconsole.h \
    console/commandconsole.h \
    console/consolechar.h \
    console/consolescreen.h \
    console/historyconsole.h \
    console/serialportconsole.h \
    console/sshconsole.h \
    console/telnetconsole.h \
    core/commandmanager.h \
    core/commandthread.h \
    core/connecttype.h \
    core/consoleparser.h \
    core/logfile.h \
    core/passwordserver.h \
    core/serialportparser.h \
    core/serialsession.h \
    core/serialsettings.h \
    core/session.h \
    core/sshparser.h \
    core/sshsession.h \
    core/sshsettings.h \
    core/telnetparser.h \
    core/telnetsession.h \
    core/telnetsettings.h \
    dialog/aboutdialog.h \
    dialog/commanddialog.h \
    dialog/connectdialog.h \
    dialog/passworddialog.h \
    dialog/sendfileprogressdialog.h \
    dock/buttonsdockwidget.h \
    dock/commanddockwidget.h \
    dock/sessiondockwidget.h \
    highlighter/defaulthightlighter.h \
    highlighter/errorhighlighter.h \
    highlighter/hightlightermanager.h \
    mainwindow.h \
    model/sessionmodel.h \
    model/treeitem.h \
    model/treemodel.h \
    service/tftp/baseudp.h \
    service/tftp/tftp.h \
    service/tftp/tftpfile.h \
    service/tftp/tftpserver.h \
    service/tftp/udpsocket.h \
    transfer/xyzmodem/modem.h \
    transfer/xyzmodem/xmodem.h \
    transfer/xyzmodem/xymodemfilesender.h \
    transfer/xyzmodem/xymodemsendfile.h \
    transfer/xyzmodem/ymoderm.h \
    transfer/xyzmodem/zmodem.h \
    util/util.h

FORMS += \
    dialog/aboutdialog.ui \
    dialog/commanddialog.ui \
    dialog/connectdialog.ui \
    dialog/passworddialog.ui \
    dialog/sendfileprogressdialog.ui \
    dock/buttonsdockwidget.ui \
    dock/commanddockwidget.ui \
    dock/sessiondockwidget.ui \
    mainwindow.ui

RC_ICONS = FlyTerm.ico
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    FlyTerm.qrc
