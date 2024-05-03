TEMPLATE = app

QT += testlib sql
QT -= gui

include(../../../MMSTools.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

INCLUDEPATH += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core \
    $${PROJECT_ROOT_DIRECTORY}/EventLogConverter/src

HEADERS += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/elcUtils.h \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CBasicDatabase.h \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CBasicSettings.h \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CElcCommonSettings.h \
    $${PROJECT_ROOT_DIRECTORY}/EventLogConverter/src/CElcGuiAppSettings.h

SOURCES += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/elcUtils.cpp \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CBasicDatabase.cpp \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CBasicSettings.cpp \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CElcCommonSettings.cpp \
    $${PROJECT_ROOT_DIRECTORY}/EventLogConverter/src/CElcGuiAppSettings.cpp \
    tst_celcguiappsettingstest.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES += TEST_SRCDIR=\\\"$${TEST_SRCDIR}\\\"
OTHER_FILES += data/*
