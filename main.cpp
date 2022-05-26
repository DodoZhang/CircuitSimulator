#include <QApplication>
#include <QDockWidget>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow *mainWindow = new MainWindow();
    mainWindow->resize(800, 600);
    mainWindow->show();
    return a.exec();
}
