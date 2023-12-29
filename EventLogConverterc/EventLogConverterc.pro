TEMPLATE = app

QT = core sql

QT_CVERSION = c++11
CONFIG += QT_CVERSION cmdline precompile_header
CONFIG(release, debug|release): QMAKE_CXXFLAGS_RELEASE += -Ofast
CONFIG(release, debug|release): QMAKE_CXXFLAGS += -Ofast

lessThan(QT_MAJOR_VERSION, 6) {
    CONFIG(debug, debug|release): message("gprof mode")
    CONFIG(debug, debug|release): QMAKE_CFLAGS_DEBUG += -g -pg  #-no-pie
    CONFIG(debug, debug|release): QMAKE_CXXFLAGS_DEBUG += -g -pg  #-no-pie
    CONFIG(debug, debug|release): QMAKE_LFLAGS_DEBUG += -g -pg -lgmon #-no-pie
}

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
    ../Common/Core/CBasicDBClass.h \
    ../Common/Core/Debug.h \
    ../Common/Core/QStorageInfoHelper.h \
    ../Common/Core/stdafx.h \
    ../Common/Core/CReportBuilder.h \
    ../Common/Core/CSVLoader.h \
    ../Common/Core/CSVParser.h \
    ../Common/Core/CSingleApplication.h \
    ../Common/Core/QPayKioskSettings.h \
    ../Common/Core/ELCSettings.h \
    src/CConsoleOutput.h \
    src/CDataSourceList.h \
    src/QCommandLineParserHelper.h

SOURCES += \
    ../Common/Core/CBasicDBClass.cpp \
    ../Common/Core/CReportBuilder.cpp \
    ../Common/Core/CSVLoader.cpp \
    ../Common/Core/CSVParser.cpp \
    ../Common/Core/CSingleApplication.cpp \
    ../Common/Core/QPayKioskSettings.cpp \
    ../Common/Core/ELCSettings.cpp \
    src/CDataSourceList.cpp \
    src/QCommandLineParserHelper.cpp \
    src/main.cpp

include(../deployment.pri)
include(../MMSTools.pri)

RC_ICONS = img/elcc.ico

MAJOR_VER = 1
MINOR_VER = 0
PATCH_VER = 0
BUILD_VER = 0
BUILD_DATE = $$_DATE_

VER_PE = $${MAJOR_VER}.$${MINOR_VER}
win32:VERSTR = $${MAJOR_VER}.$${MINOR_VER}.$${PATCH_VER}.$${BUILD_VER} # major.minor.patch.build
else: VERSTR = $${MAJOR_VER}.$${MINOR_VER}.$${PATCH_VER} # major.minor.patch

VERSION_PE_HEADER = $${VER_PE}
VERSION = $${VERSTR}
QMAKE_TARGET_DESCRIPTION = MMS Event Log Conversion Console Utility
QMAKE_TARGET_COPYRIGHT = (C) 2023 Oleksii Gaienko
QMAKE_TARGET_PRODUCT = MMS Event Log Conversion Utility
QMAKE_TARGET_INTERNALNAME = $${TARGET}

DEFINES += TEST_SRCDIR=\\\"$${TEST_SRCDIR}\\\"
DEFINES += BUILD_VER=\\\"$${VERSTR}\\\"
DEFINES += BUILD_GIT=\\\"$${GIT_VERSION}\\\"
DEFINES += "BUILD_DATE=\"\\\"$${BUILD_DATE}\\\"\""
DEFINES += QT_VER=\\\"$${QT_VERSION}\\\"
