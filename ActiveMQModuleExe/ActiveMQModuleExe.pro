#-------------------------------------------------
#
# Project created by QtCreator 2016-04-08T14:02:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ActiveMQModuleExe
TEMPLATE = app


SOURCES += main.cpp\
        ActiveMQModuleWidget.cpp

HEADERS  += ActiveMQModuleWidget.h \
    interface.h

FORMS    += ActiveMQModuleWidget.ui
DESTDIR += ../ExeTest

RESOURCES += \
    ActiveMQModuleexe.qrc


INCLUDEPATH +=D:/ActiveMQ/apr/include
INCLUDEPATH +=D:/ActiveMQ/activemq-cpp-library-3.8.3/src/main

LIBS += -L$$DESTDIR/ -lactivemq-cpp
LIBS += -L$$DESTDIR/ -llibapr-1
win32 {
    CONFIG(debug, debug|release) {
#        DEFINES += _DEBUG
        # vld 2.2 downloaded from http://vld.codeplex.com/
        VLD_PATH = C:\Program Files (x86)\Visual Leak Detector
        INCLUDEPATH += $VLD_PATH/include
        LIBS += -L$VLD_PATH/lib/Win32 -lvld
    }
}
