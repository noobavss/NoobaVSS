#-------------------------------------------------
#
# Project created by QtCreator 2013-04-01T12:18:25
#
#-------------------------------------------------

QT       -= gui

TARGET = NoobaPluginAPI
TEMPLATE = lib

CONFIG += staticlib

DEFINES += NOOBAPLUGINAPI_LIBRARY

SOURCES += \
    noobapluginbase.cpp \
    noobapluginbaseprivate.cpp \
    property.cpp \
    intproperty.cpp \
    port.cpp \
    noobapluginapi.cpp

HEADERS += noobapluginapi.h\
        NoobaPluginAPI_global.h \
    noobapluginbase.h \
    noobapluginbaseprivate.h \
    property.h \
    propertyprivate.h \
    intproperty.h \
    port.h \
    portprivate.h \
    apiglobal.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
