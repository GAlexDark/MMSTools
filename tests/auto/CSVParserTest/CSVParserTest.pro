TEMPLATE = app

QT += testlib
QT -= gui

include(../../../MMSTools.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

INCLUDEPATH += $${PROJECT_ROOT_DIRECTORY}/Common/Core

HEADERS += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CSVParser.h

SOURCES +=  \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CSVParser.cpp \
    tst_csvparser.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
OTHER_FILES += data/*
