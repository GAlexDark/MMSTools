TEMPLATE = app

QT += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT_CVERSION = c++11
CONFIG += QT_CVERSION precompile_header
CONFIG(release, debug|release): QMAKE_CXXFLAGS_RELEASE += -Ofast
CONFIG(release, debug|release): QMAKE_CXXFLAGS += -Ofast

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# QXlsx code for Application Qt project
QXLSX_PARENTPATH=../Common/3dparty/QXlsx/       # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=../Common/3dparty/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=../Common/3dparty/QXlsx/source/  # current QXlsx source path is ./source/
include(../Common/3dparty/QXlsx/QXlsx.pri)

INCLUDEPATH += ../Common/Core
INCLUDEPATH += . src

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER  = ../Common/Core/stdafx.h

HEADERS += \
    ../Common/Core/CBasicDatabase.h \
    ../Common/Core/CBasicSettings.h \
    ../Common/Core/CELCSettings.h \
    ../Common/Core/CSingleApplication.h \
    ../Common/Core/Debug.h \
    ../Common/Core/stdafx.h \
    src/CELCWSettings.h \
    src/mainwindow.h

SOURCES += \
    ../Common/Core/CBasicDatabase.cpp \
    ../Common/Core/CBasicSettings.cpp \
    ../Common/Core/CELCSettings.cpp \
    ../Common/Core/CSingleApplication.cpp \
    src/CELCWSettings.cpp \
    src/main.cpp \
    src/mainwindow.cpp

FORMS += \
    src/mainwindow.ui

TRANSLATIONS += \
    i18n/$${TARGET}_ru_RU.ts \
    i18n/$${TARGET}_uk_UA.ts

CONFIG += lrelease

include(../deployment.pri)

RESOURCES += $${TARGET}.qrc
RC_FILE = $${TARGET}.rc

include(../MMSTools.pri)


MAJOR_VER = 2
MINOR_VER = 0
PATCH_VER = 0
BUILD_VER = 0
BUILD_DATE = $$_DATE_

VER_PE = $${MAJOR_VER}.$${MINOR_VER}
win32:VERSTR = $${MAJOR_VER}.$${MINOR_VER}.$${PATCH_VER}.$${BUILD_VER} # major.minor.patch.build
else: VERSTR = $${MAJOR_VER}.$${MINOR_VER}.$${PATCH_VER} # major.minor.patch

VERSION_PE_HEADER = $${VER_PE}
VERSION = $${VERSTR}
QMAKE_TARGET_DESCRIPTION = MMS Event Log Conversion GUI Utility
QMAKE_TARGET_COPYRIGHT = (C) 2023 Oleksii Gaienko
QMAKE_TARGET_PRODUCT = MMS Event Log Conversion Utility
QMAKE_TARGET_INTERNALNAME = $${TARGET}

DEFINES += TEST_SRCDIR=\\\"$${TEST_SRCDIR}\\\"
DEFINES += BUILD_VER=\\\"$${VERSTR}\\\"
DEFINES += BUILD_GIT=\\\"$${GIT_VERSION}\\\"
DEFINES += "BUILD_DATE=\"\\\"$${BUILD_DATE}\\\"\""
