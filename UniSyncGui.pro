#-------------------------------------------------
#
# Project created by QtCreator 2014-10-07T11:24:08
#
#-------------------------------------------------

QT       += core gui

TARGET = UniSyncGui
TEMPLATE = app
DEFINES += DCONSOLE_NO_SQL COMPILED_WITH_QT4X

SOURCES +=\
        unisyncwindow.cpp \
    ../gSAFE/dconsole.cpp

HEADERS  += unisyncwindow.h \
    ../gSAFE/dconsole.h

FORMS    += unisyncwindow.ui
INCLUDEPATH += . \
            ../gSAFE/

RESOURCES += \
    unisync.qrc

OBJECTS += unisyncgui.res