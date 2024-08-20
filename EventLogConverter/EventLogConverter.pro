TEMPLATE = app

QT += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT_CVERSION = c++17
CONFIG += QT_CVERSION precompile_header

DEFINES += QT_USE_QSTRINGBUILDER

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

include(../commonmodules.pro)
HEADERS += \
    src/CElcGuiAppSettings.h \
    src/CLoadFilesOptionsDialog.h \
    src/CReportOptionsDialog.h \
    src/mainwindow.h

SOURCES += \
    src/CElcGuiAppSettings.cpp \
    src/CLoadFilesOptionsDialog.cpp \
    src/CReportOptionsDialog.cpp \
    src/main.cpp \
    src/mainwindow.cpp

FORMS += \
    src/CLoadFilesOptionsDialog.ui \
    src/CReportOptionsDialog.ui \
    src/mainwindow.ui

TRANSLATIONS += \
    i18n/$${TARGET}_ru_RU.ts \
    i18n/$${TARGET}_uk_UA.ts

CONFIG += lrelease

include(../deployment.pri)

RESOURCES += $${TARGET}.qrc

include(../MMSTools.pri)

RC_ICONS = img/elc.ico

MAJOR_VER = 2
MINOR_VER = 0
PATCH_VER = 5
BUILD_VER = 131
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
QMAKE_TARGET_COMMENTS = support@galexsoftware.info

DEFINES += TEST_SRCDIR=\\\"$${TEST_SRCDIR}\\\"
DEFINES += BUILD_VER=\\\"$${VERSTR}\\\"
DEFINES += BUILD_GIT=\\\"$${GIT_VERSION}\\\"
DEFINES += "BUILD_DATE=\"\\\"$${BUILD_DATE}\\\"\""
DEFINES += QT_VER=\\\"$${QT_VERSION}\\\"
DEFINES += CONTACT=\\\"support@galexsoftware.info\\\"
DEFINES += GUI
