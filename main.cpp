#include <QApplication>
#include <QDockWidget>
#include <QTranslator>
#include <QDir>
#include <QDebug>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();
    return a.exec();
}
