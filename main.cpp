#include <QApplication>
#include <QDockWidget>

#include "MainWindow.h"
#include "Simulation/Circuit.h"
#include "Simulation/Elements/VoltageSource.h"
#include "Simulation/Elements/Resistance.h"
#include "Graphic/OscilloscopeWidget.h"
#include "Graphic/ParametersInputWidget.h"

using namespace CirSim;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow *mainWindow = new MainWindow();
    mainWindow->resize(800, 600);
    mainWindow->show();
    return a.exec();
}
