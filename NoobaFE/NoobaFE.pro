######################################################################
# Automatically generated by qmake (2.01a) Tue Apr 2 00:20:58 2013
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += . Forms Resources
INCLUDEPATH += .

# Input
HEADERS += FrameProcessor.h \
           MainWindow.h \
           NoobaEye.h \
           OutputWindow.h \
           PluginLoader.h \
           PluginsConfigUI.h
FORMS += Forms/MainWindow.ui Forms/OutputWindow.ui Forms/PluginsConfigUI.ui
SOURCES += FrameProcessor.cpp \
           main.cpp \
           MainWindow.cpp \
           OutputWindow.cpp \
           PluginLoader.cpp \
           PluginsConfigUI.cpp
RESOURCES += Resources/mainwind.qrc

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../NoobaPluginAPI/ -lNoobaPluginAPI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../NoobaPluginAPI/ -lNoobaPluginAPId
else:unix: LIBS += -L$$OUT_PWD/../NoobaPluginAPI/ -lNoobaPluginAPI

INCLUDEPATH += $$PWD/../NoobaPluginAPI
DEPENDPATH += $$PWD/../NoobaPluginAPI
