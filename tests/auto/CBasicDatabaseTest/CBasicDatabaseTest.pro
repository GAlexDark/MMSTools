TEMPLATE = app

QT += testlib sql
QT -= gui

include(../../../MMSTools.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

INCLUDEPATH += $${PROJECT_ROOT_DIRECTORY}/Common/Core

HEADERS += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CBasicDatabase.h \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/DBStrings.h

SOURCES +=  \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CBasicDatabase.cpp \
    tst_cbasicdatabase.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
OTHER_FILES += data/*
