#-------------------------------------------------
#
# Project created by QtCreator 2016-04-08T13:25:51
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = ActiveMQModulePlugin
TEMPLATE = lib

SOURCES += ActiveMQModulePlugin.cpp

HEADERS += ActiveMQModulePlugin.h \
    interface.h
DISTFILES += ActiveMQModulePlugin.json

DESTDIR += ../ExeTest/Plugin

unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH +=D:/ActiveMQ/apr/include
INCLUDEPATH +=D:/ActiveMQ/activemq-cpp-library-3.8.3/src/main

LIBS += -L$$DESTDIR/../ -lactivemq-cpp
LIBS += -L$$DESTDIR/../ -llibapr-1
win32 {
    CONFIG(debug, debug|release) {
#        DEFINES += _DEBUG
        # vld 2.2 downloaded from http://vld.codeplex.com/
        VLD_PATH = C:\Program Files (x86)\Visual Leak Detector
        INCLUDEPATH += $VLD_PATH/include
        LIBS += -L$VLD_PATH/lib/Win32 -lvld
    }
}
