#-------------------------------------------------
#
# Project created by QtCreator 2014-02-02T00:42:09
#
#-------------------------------------------------

VERSION = 0.1.0

QT       += core gui widgets

TARGET = yasem-desktop-gui
TEMPLATE = lib

CONFIG += c++11

DEFINES += DESKTOPGUI_LIBRARY

INCLUDEPATH += ../../yasem-core

SOURCES += desktopgui.cpp \
    mainwindow.cpp \
    guiconfigprofile.cpp \
    guistbobject.cpp \
    settingsdialog.cpp

HEADERS += desktopgui.h\
    desktopgui_global.h \
    mainwindow.h \
    guiconfigprofile.h \
    guistbobject.h \
    settingsdialog.h  \
    ../../yasem-core/profilemanager.h

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
    res/js/profile_config.js \
    LICENSE \
    README.md

FORMS    += \
    settingsdialog.ui

include(../../common.pri)
DESTDIR = $$DEFAULT_PLUGIN_DIR

RESOURCES += \
    resources.qrc


