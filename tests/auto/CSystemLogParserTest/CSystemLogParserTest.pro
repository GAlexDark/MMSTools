TEMPLATE = app

QT += testlib
QT -= gui

include(../../../MMSTools.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

INCLUDEPATH += $${PROJECT_ROOT_DIRECTORY}/Common/Core

HEADERS += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CBasicParser.h \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CSystemLogParser.h

SOURCES += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CBasicParser.cpp \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CSystemLogParser.cpp \
    tst_csystemlogparsertest.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
OTHER_FILES += data/*
