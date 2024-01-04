TEMPLATE = app

QT += testlib
QT -= gui

include(../../../MMSTools.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

INCLUDEPATH += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core \
    $${PROJECT_ROOT_DIRECTORY}/EventLogConverterc/src

HEADERS += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CBasicSettings.h \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CELCSettings.h \
    $${PROJECT_ROOT_DIRECTORY}/EventLogConverterc/src/CELCCSettings.h

SOURCES += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CBasicSettings.cpp \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CELCSettings.cpp \
    $${PROJECT_ROOT_DIRECTORY}/EventLogConverterc/src/CELCCSettings.cpp \
    tst_celccsettings.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES += TEST_SRCDIR=\\\"$${TEST_SRCDIR}\\\"
OTHER_FILES += data/*
