#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setApplicationName("FlyTerm");
    QApplication::setApplicationVersion("V1.0.0");
    MainWindow::LoadSettings();
    MainWindow::InstallTranstoirs();
    MainWindow w;
    w.show();
    return a.exec();
}
