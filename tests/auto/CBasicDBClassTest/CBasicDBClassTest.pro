TEMPLATE = app

QT += testlib sql
QT -= gui

include(../../../MMSTools.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

INCLUDEPATH += $${PROJECT_ROOT_DIRECTORY}/Common/Core

HEADERS += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CBasicDBClass.h

SOURCES +=  \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CBasicDBClass.cpp \
    tst_cbasicdbclass.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
OTHER_FILES += data/*
