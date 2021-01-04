#-------------------------------------------------
#
# Project created by QtCreator 2014-10-07T11:24:08
#
#-------------------------------------------------

TEMPLATE = app
QT       += core gui widgets
TARGET   = UniSyncGui
CONFIG   -= qtquickcompile console

DEFINES += DCONSOLE_NO_SQL

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

win32:contains(QMAKE_HOST.arch, x86_64) {
  OBJECTS += unisyncgui64.res
} else {
  OBJECTS += unisyncgui32.res
}




