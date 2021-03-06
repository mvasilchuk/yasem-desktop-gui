#-------------------------------------------------
#
# Project created by QtCreator 2014-02-02T00:42:09
#
#-------------------------------------------------

VERSION = 0.1.0
TARGET = yasem-desktop-gui
TEMPLATE = lib

include($${top_srcdir}/common.pri)

QT       += core gui widgets qml quick quickwidgets opengl

DEFINES += DESKTOPGUI_LIBRARY

SOURCES += desktopgui.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    pluginsdialog.cpp \
    aboutappdialog.cpp \
    desktopguiobject.cpp \
    openglwidgetcontainer.cpp

HEADERS +=\
    desktopgui_global.h \
    mainwindow.h \
    settingsdialog.h  \
    pluginsdialog.h \
    aboutappdialog.h \
    desktopguiplugin.h \
    desktopguiobject.h \
    openglwidgetcontainer.h

win32: {
HEADERS += \
    $${SDK_DIR}\browser.h \
    $${SDK_DIR}\mediaplayer.h
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

OTHER_FILES += \
    metadata.json \
    LICENSE \
    README.md

RESOURCES += \
    resources.qrc


