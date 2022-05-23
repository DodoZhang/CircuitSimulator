QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Graphic/OscilloscopeWidget.cpp \
    Graphic/ParametersInputWidget.cpp \
    InfixExpression.cpp \
    Simulation/Circuit.cpp \
    Simulation/Element.cpp \
    Simulation/Elements/Capacity.cpp \
    Simulation/Elements/CurrentSource.cpp \
    Simulation/Elements/Diode.cpp \
    Simulation/Elements/FunctionGenerator.cpp \
    Simulation/Elements/Inductor.cpp \
    Simulation/Elements/Resistance.cpp \
    Simulation/Elements/VoltageSource.cpp \
    Simulation/Net.cpp \
    Simulation/Pin.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    Graphic/OscilloscopeWidget.h \
    Graphic/ParametersInputWidget.h \
    InfixExpression.h \
    MainWindow.h \
    Simulation/Circuit.h \
    Simulation/Element.h \
    Simulation/Elements/Capacity.h \
    Simulation/Elements/CurrentSource.h \
    Simulation/Elements/Diode.h \
    Simulation/Elements/FunctionGenerator.h \
    Simulation/Elements/Inductor.h \
    Simulation/Elements/Resistance.h \
    Simulation/Elements/VoltageSource.h \
    Simulation/Net.h \
    Simulation/Pin.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
