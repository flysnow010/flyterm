QT       += core gui network serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
INCLUDEPATH += ./include
DEFINES += CRYPTOPP_IMPORTS
CONFIG(debug, debug|release){
DEFINES += DEBUG
    LIBS += -L$$(PWD)\lib -lssh.dll -lrsa -lQTelnetd
}
else
{
    LIBS += -L$$(PWD)\lib -lssh.dll -lrsa -lQTelnet
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
    console/console.cpp \
    console/consolescreen.cpp \
    console/historyconsole.cpp \
    console/serialportconsole.cpp \
    console/sshconsole.cpp \
    console/telnetconsole.cpp \
    core/basesession.cpp \
    core/commandmanager.cpp \
    core/commandthread.cpp \
    core/consolecodec.cpp \
    core/consoleparser.cpp \
    core/languagemanager.cpp \
    core/localshell.cpp \
    core/logfile.cpp \
    core/serialportparser.cpp \
    core/serialsession.cpp \
    core/shellthread.cpp \
    core/sshchannel.cpp \
    core/sshparser.cpp \
    core/sshsession.cpp \
    core/sshshell.cpp \
    core/telnetparser.cpp \
    core/telnetsession.cpp \
    core/userauth.cpp \
    dialog/aboutdialog.cpp \
    dialog/commanddialog.cpp \
    dialog/connectdialog.cpp \
    dialog/fileprogressdialog.cpp \
    dialog/passworddialog.cpp \
    dock/buttonsdockwidget.cpp \
    dock/commanddockwidget.cpp \
    dock/sessiondockwidget.cpp \
    highlighter/defaulthightlighter.cpp \
    highlighter/errorhighlighter.cpp \
    highlighter/hightlightermanager.cpp \
    highlighter/scripthighlighter.cpp \
    main.cpp \
    mainwindow.cpp \
    model/sessionmodel.cpp \
    model/treeitem.cpp \
    model/treemodel.cpp \
    service/tftp/baseudp.cpp \
    service/tftp/serversocket.cpp \
    service/tftp/tftp.cpp \
    service/tftp/tftpserver.cpp \
    service/tftp/tftpserverfile.cpp \
    ssh/channel.cpp \
    ssh/dir.cpp \
    ssh/file.cpp \
    ssh/fileinfo.cpp \
    ssh/scp.cpp \
    ssh/session.cpp \
    ssh/sftp.cpp \
    transfer/kermit/kermit.cpp \
    transfer/kermit/kermitfilerecver.cpp \
    transfer/kermit/kermitfilesender.cpp \
    transfer/kermit/kermitrecvfile.cpp \
    transfer/kermit/kermitsendfile.cpp \
    transfer/sftp/sftpsession.cpp \
    transfer/sftp/sftptransfer.cpp \
    transfer/sftp/sshconnect.cpp \
    transfer/xyzmodem/modem.cpp \
    transfer/xyzmodem/xmodem.cpp \
    transfer/xyzmodem/xymodemfilerecver.cpp \
    transfer/xyzmodem/xymodemfilesender.cpp \
    transfer/xyzmodem/xymodemrecvfile.cpp \
    transfer/xyzmodem/xymodemsendfile.cpp \
    transfer/xyzmodem/ymodem.cpp \
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
    console/console.h \
    console/consolechar.h \
    console/consolescreen.h \
    console/historyconsole.h \
    console/serialportconsole.h \
    console/sshconsole.h \
    console/telnetconsole.h \
    core/basesession.h \
    core/commandmanager.h \
    core/commandthread.h \
    core/connecttype.h \
    core/consolecodec.h \
    core/consoleparser.h \
    core/languagemanager.h \
    core/localshell.h \
    core/logfile.h \
    core/prikey.h \
    core/pubkey.h \
    core/serialportparser.h \
    core/serialsession.h \
    core/serialsettings.h \
    core/shellthread.h \
    core/sshchannel.h \
    core/sshparser.h \
    core/sshsession.h \
    core/sshsettings.h \
    core/sshshell.h \
    core/telnetparser.h \
    core/telnetsession.h \
    core/telnetsettings.h \
    core/userauth.h \
    dialog/aboutdialog.h \
    dialog/commanddialog.h \
    dialog/connectdialog.h \
    dialog/fileprogressdialog.h \
    dialog/passworddialog.h \
    dock/buttonsdockwidget.h \
    dock/commanddockwidget.h \
    dock/sessiondockwidget.h \
    highlighter/defaulthightlighter.h \
    highlighter/errorhighlighter.h \
    highlighter/hightlightermanager.h \
    highlighter/scripthighlighter.h \
    mainwindow.h \
    model/sessionmodel.h \
    model/treeitem.h \
    model/treemodel.h \
    service/tftp/baseudp.h \
    service/tftp/serversocket.h \
    service/tftp/tftp.h \
    service/tftp/tftpserver.h \
    service/tftp/tftpserverfile.h \
    ssh/channel.h \
    ssh/dir.h \
    ssh/file.h \
    ssh/fileinfo.h \
    ssh/scp.h \
    ssh/session.h \
    ssh/sftp.h \
    ssh/sshprivate.h \
    transfer/kermit/kermit.h \
    transfer/kermit/kermitfilerecver.h \
    transfer/kermit/kermitfilesender.h \
    transfer/kermit/kermitrecvfile.h \
    transfer/kermit/kermitsendfile.h \
    transfer/sftp/sftpsession.h \
    transfer/sftp/sftptransfer.h \
    transfer/sftp/sshconnect.h \
    transfer/xyzmodem/modem.h \
    transfer/xyzmodem/xmodem.h \
    transfer/xyzmodem/xymodemfilerecver.h \
    transfer/xyzmodem/xymodemfilesender.h \
    transfer/xyzmodem/xymodemrecvfile.h \
    transfer/xyzmodem/xymodemsendfile.h \
    transfer/xyzmodem/ymodem.h \
    transfer/xyzmodem/zmodem.h \
    util/util.h

FORMS += \
    dialog/aboutdialog.ui \
    dialog/commanddialog.ui \
    dialog/connectdialog.ui \
    dialog/fileprogressdialog.ui \
    dialog/passworddialog.ui \
    dock/buttonsdockwidget.ui \
    dock/commanddockwidget.ui \
    dock/sessiondockwidget.ui \
    mainwindow.ui

RC_ICONS = FlyTerm.ico
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
TRANSLATIONS += en.ts zh_CN.ts
RESOURCES += \
    FlyTerm.qrc
