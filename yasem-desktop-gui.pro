#-------------------------------------------------
#
# Project created by QtCreator 2014-02-02T00:42:09
#
#-------------------------------------------------

QT       += core gui widgets

#VER_MAJ = 1
#VER_MIN = 0
#VER_PAT = 0

TARGET = yasem-desktop-gui
TEMPLATE = lib
CONFIG += plugin

CONFIG += c++11

DEFINES += DESKTOPGUI_LIBRARY

INCLUDEPATH += ../yasem-core

SOURCES += desktopgui.cpp \
    mainwindow.cpp \
    guiconfigprofile.cpp \
    guistbobject.cpp

HEADERS += desktopgui.h\
    desktopgui_global.h \
    mainwindow.h \
    guiconfigprofile.h \
    guistbobject.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

OTHER_FILES += \
    metadata.json \
    res/html/profiles.html \
    res/css/main.css \
    res/img/white-highlight.png \
    res/js/main.js \
    res/html/profile_config.html \
    res/js/profile_config.js

FORMS    +=

include(../common.pri)
DESTDIR = $$DEFAULT_PLUGIN_DIR

RESOURCES += \
    resources.qrc


