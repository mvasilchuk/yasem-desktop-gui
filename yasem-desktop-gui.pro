#-------------------------------------------------
#
# Project created by QtCreator 2014-02-02T00:42:09
#
#-------------------------------------------------

VERSION = 0.1.0

QT       += core gui widgets qml quick

TARGET = yasem-desktop-gui
TEMPLATE = lib

CONFIG += c++11

DEFINES += DESKTOPGUI_LIBRARY

INCLUDEPATH += ../../yasem-core/
DEPENDPATH += ../../yasem-core/

SOURCES += desktopgui.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    pluginsdialog.cpp \
    aboutappdialog.cpp \
    ../../yasem-core/plugin.cpp \
    desktopguiobject.cpp \
    ../../yasem-core/mediaplayerpluginobject.cpp

HEADERS +=\
    desktopgui_global.h \
    mainwindow.h \
    settingsdialog.h  \
    ../../yasem-core/profilemanager.h \
    pluginsdialog.h \
    aboutappdialog.h \
    ../../yasem-core/plugin.h \
    ../../yasem-core/abstractpluginobject.h \
    ../../yasem-core/mediaplayerpluginobject.h \
    ../../yasem-core/browserpluginobject.h \
    desktopguiplugin.h \
    desktopguiobject.h

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


