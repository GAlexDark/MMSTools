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
    ../Common/Core/stdafx.h \
    ../Common/Core/CReportBuilder.h \
    ../Common/Core/CSVLoader.h \
    ../Common/Core/CSVParser.h

SOURCES += \
    ../Common/Core/CBasicDBClass.cpp \
    ../Common/Core/CReportBuilder.cpp \
    ../Common/Core/CSVLoader.cpp \
    ../Common/Core/CSVParser.cpp \
    src/main.cpp

include(../deployment.pri)
