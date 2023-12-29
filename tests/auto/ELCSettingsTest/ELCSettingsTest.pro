TEMPLATE = app

QT += testlib
QT -= gui

include(../../../MMSTools.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

INCLUDEPATH += $${PROJECT_ROOT_DIRECTORY}/Common/Core

HEADERS += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/QPayKioskSettings.h \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/ELCSettings.h

SOURCES += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/QPayKioskSettings.cpp \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/ELCSettings.cpp \
    tst_elcsettings.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES += TEST_SRCDIR=\\\"$${TEST_SRCDIR}\\\"
OTHER_FILES += data/*
