#-------------------------------------------------
#
# Project created by QtCreator 2014-02-02T00:42:09
#
#-------------------------------------------------

VERSION = 0.1.0
TARGET = yasem-desktop-gui
TEMPLATE = lib

include($${top_srcdir}/common.pri)

QT       += core gui widgets qml quick

DEFINES += DESKTOPGUI_LIBRARY

SOURCES += desktopgui.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    pluginsdialog.cpp \
    aboutappdialog.cpp \
    desktopguiobject.cpp \
    $${CORE_ROOT_DIR}/mediaplayerpluginobject.cpp

HEADERS +=\
    desktopgui_global.h \
    mainwindow.h \
    settingsdialog.h  \
    $${CORE_ROOT_DIR}/profilemanager.h \
    pluginsdialog.h \
    aboutappdialog.h \
    $${CORE_ROOT_DIR}/mediaplayerpluginobject.h \
    $${CORE_ROOT_DIR}/browserpluginobject.h \
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

RESOURCES += \
    resources.qrc


