TEMPLATE = app

QT += testlib
QT -= gui

include(../../../MMSTools.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

INCLUDEPATH += $${PROJECT_ROOT_DIRECTORY}/EventLogConverterc/src

HEADERS += \
    $${PROJECT_ROOT_DIRECTORY}/EventLogConverterc/src/CDataSourceList.h

SOURCES += \
    $${PROJECT_ROOT_DIRECTORY}/EventLogConverterc/src/CDataSourceList.cpp \
    tst_cdatasourcelisttest.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES += TEST_SRCDIR=\\\"$${TEST_SRCDIR}\\\"
OTHER_FILES += data/*
