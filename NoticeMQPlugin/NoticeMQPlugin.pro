#-------------------------------------------------
#
# Project created by QtCreator 2016-04-12T11:18:14
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = NoticeMQPlugin
TEMPLATE = lib

DESTDIR += ../ExeTest/Plugin/NoticeMQ

SOURCES += \
    NoticeMQPlugin.cpp \
    consumer.cpp \
    producter.cpp \
    ActiveMQTransportListener.cpp

HEADERS += \
    NoticeMQPlugin.h \
    interface.h \
    consumer.h \
    producter.h \
    ActiveMQTransportListener.h
DISTFILES += NoticeMQPlugin.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}
INCLUDEPATH +=D:/ActiveMQ/apr/include
INCLUDEPATH +=D:/ActiveMQ/activemq-cpp-library-3.8.3/src/main

LIBS += ws2_32.lib
LIBS += -L$$DESTDIR/../../ -lactivemq-cpp
LIBS += -L$$DESTDIR/../../ -llibapr-1
win32 {
    CONFIG(debug, debug|release) {
#        DEFINES += _DEBUG
        # vld 2.2 downloaded from http://vld.codeplex.com/
        VLD_PATH = C:\Program Files (x86)\Visual Leak Detector
        INCLUDEPATH += $VLD_PATH/include
        LIBS += -L$VLD_PATH/lib/Win32 -lvld
    }
}
