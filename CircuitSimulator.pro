QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Graphic/EditorElement.cpp \
    Graphic/EditorWidget.cpp \
    Graphic/EditorWire.cpp \
    Graphic/Elements/EditorCapacity.cpp \
    Graphic/Elements/EditorCurrentSource.cpp \
    Graphic/Elements/EditorDiode.cpp \
    Graphic/Elements/EditorFunctionGenerator.cpp \
    Graphic/Elements/EditorGround.cpp \
    Graphic/Elements/EditorInductor.cpp \
    Graphic/Elements/EditorMOSFET.cpp \
    Graphic/Elements/EditorResistor.cpp \
    Graphic/Elements/EditorVCC.cpp \
    Graphic/Elements/EditorVoltageSource.cpp \
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
    Simulation/Elements/MOSFET.cpp \
    Simulation/Elements/Resistor.cpp \
    Simulation/Elements/VoltageSource.cpp \
    Simulation/Net.cpp \
    Simulation/Pin.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    Graphic/EditorElement.h \
    Graphic/EditorWidget.h \
    Graphic/EditorWire.h \
    Graphic/Elements/EditorCapacity.h \
    Graphic/Elements/EditorCurrentSource.h \
    Graphic/Elements/EditorDiode.h \
    Graphic/Elements/EditorFunctionGenerator.h \
    Graphic/Elements/EditorGround.h \
    Graphic/Elements/EditorInductor.h \
    Graphic/Elements/EditorMOSFET.h \
    Graphic/Elements/EditorResistor.h \
    Graphic/Elements/EditorVCC.h \
    Graphic/Elements/EditorVoltageSource.h \
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
    Simulation/Elements/MOSFET.h \
    Simulation/Elements/Resistor.h \
    Simulation/Elements/VoltageSource.h \
    Simulation/Net.h \
    Simulation/Pin.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
