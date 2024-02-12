TEMPLATE = app

QT += testlib
QT -= gui

include(../../../MMSTools.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

INCLUDEPATH += $${PROJECT_ROOT_DIRECTORY}/Common/Core

HEADERS += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CSingleApplication.h

SOURCES += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CSingleApplication.cpp \
    tst_csingleapplicationtest.cpp
