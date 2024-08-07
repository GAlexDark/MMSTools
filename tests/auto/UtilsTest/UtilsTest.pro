TEMPLATE = app

QT += testlib
QT -= gui

include(../../../MMSTools.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

INCLUDEPATH += $${PROJECT_ROOT_DIRECTORY}/Common/Core

HEADERS += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/elcUtils.h \
    CTestClass.h

SOURCES +=  \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/elcUtils.cpp \
    CTestClass.cpp \
    tst_utilstest.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES += TEST_SRCDIR=\\\"$${TEST_SRCDIR}\\\"
OTHER_FILES += data/*
