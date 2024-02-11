TEMPLATE = app

QT += testlib sql
QT -= gui

include(../../../MMSTools.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

INCLUDEPATH += $${PROJECT_ROOT_DIRECTORY}/Common/Core

HEADERS += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CBasicDatabase.h \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/elcUtils.h

SOURCES +=  \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CBasicDatabase.cpp \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/elcUtils.cpp \
    tst_utilstest.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES += TEST_SRCDIR=\\\"$${TEST_SRCDIR}\\\"
OTHER_FILES += data/*
