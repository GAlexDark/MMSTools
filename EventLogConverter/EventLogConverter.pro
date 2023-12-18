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
    ../Common/Core/QPayKioskSettings.h \
    ../Common/Core/ELCSettings.h \
    ../Common/Core/stdafx.h \
    src/mainwindow.h

SOURCES += \
    ../Common/Core/QPayKioskSettings.cpp \
    ../Common/Core/ELCSettings.cpp \
    src/main.cpp \
    src/mainwindow.cpp

FORMS += \
    src/mainwindow.ui

LANGUAGES = ru en uk
TRANSLATIONS += \
    i18n/$${TARGET}_ru_RU.ts \
    i18n/$${TARGET}_uk_UA.ts

include(../deployment.pri)

RESOURCES += $${TARGET}.qrc
RC_FILE = $${TARGET}.rc

include(../MMSTools.pri)
DEFINES += TEST_SRCDIR=\\\"$${TEST_SRCDIR}\\\"
