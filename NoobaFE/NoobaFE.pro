######################################################################
# Automatically generated by qmake (2.01a) Tue Apr 2 00:20:58 2013
######################################################################

TEMPLATE = app

CONFIG(debug, debug|release): message(Debug build!)
CONFIG(release, debug|release): message(Release build!)

CONFIG(debug, debug|release): TARGET =  NoobaVSS_Debug
CONFIG(release, debug|release): TARGET =  NoobaVSS_Release

CONFIG(debug, debug|release): DESTDIR = ../../NoobaVSS_build/NoobaFE/Debug/
CONFIG(release, debug|release): DESTDIR = ../../NoobaVSS_build/NoobaFE/Release/

win32:TEMPLATE = vcapp

# Input
HEADERS += \
           MainWindow.h \
           NoobaEye.h \
           PluginLoader.h \
           PluginsConfigUI.h \
    NoobaPlugin.h \
    OutputWind.h \
    ParamConfigWind.h \
    VidOutputSubWind.h \
    ParamDelegate.h

FORMS += Forms/MainWindow.ui Forms/PluginsConfigUI.ui \
    Forms/OutputWind.ui \
    Forms/ParamConfigWind.ui \
    Forms/VidOutputSubWind.ui

SOURCES += \
           main.cpp \
           MainWindow.cpp \
           PluginLoader.cpp \
           PluginsConfigUI.cpp \
    NoobaPlugin.cpp \
    OutputWind.cpp \
    ParamConfigWind.cpp \
    VidOutputSubWind.cpp \
    ParamDelegate.cpp

RESOURCES += Resources/mainwind.qrc

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}

CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../NoobaVSS_build/NoobaPluginAPI/Debug/ -lNoobaPluginAPId
CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../NoobaVSS_build/NoobaPluginAPI/Release/ -lNoobaPluginAPI

win32{
    # need to put win32 opencv lib paths
}

INCLUDEPATH += $$PWD/../NoobaPluginAPI
DEPENDPATH += $$PWD/../NoobaPluginAPI
