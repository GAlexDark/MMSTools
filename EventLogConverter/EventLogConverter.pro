include(../MMSTools.pri)
include(../deployment.pri)

TEMPLATE = app
TARGET = EventLogConverter

QT += core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += precompile_header lrelease
CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT \
               QT_NO_INFO_OUTPUT \
               QT_NO_WARNING_OUTPUT
    CONFIG -= debug
    CONFIG += release
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# QXlsx code for Application Qt project
QXLSX_PARENTPATH = $${PROJECT_ROOT_DIRECTORY}/Common/3dparty/QXlsx/
QXLSX_HEADERPATH = $${PROJECT_ROOT_DIRECTORY}/Common/3dparty/QXlsx/header/
QXLSX_SOURCEPATH = $${PROJECT_ROOT_DIRECTORY}/Common/3dparty/QXlsx/source/
include($$QXLSX_PARENTPATH/QXlsx.pri)

INCLUDEPATH += $${PROJECT_ROOT_DIRECTORY}/Common/Core \
               $${PROJECT_ROOT_DIRECTORY}/src

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER  = $${PROJECT_ROOT_DIRECTORY}/Common/Core/stdafx.h

include($${PROJECT_ROOT_DIRECTORY}/commonmodules.pro)
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

RESOURCES += $${TARGET}.qrc

RC_ICONS = img/elc.ico

MAJOR_VER = 2
MINOR_VER = 0
PATCH_VER = 7
BUILD_VER = 138
BUILD_DATE = $$_DATE_

# VERSION_PE_HEADER = $${MAJOR_VER}.$${MINOR_VER}
win32 {
    VERSTR = $${MAJOR_VER}.$${MINOR_VER}.$${PATCH_VER}.$${BUILD_VER} # major.minor.patch.build

    CONFIG(release, debug|release) {
        WINDEPLOYQT = $$system_path($$[QT_INSTALL_BINS]/windeployqt.exe)
        TARGET_EXE  = $$system_path($$FINAL_DEST_DIR/$${TARGET}.exe)
        QMAKE_POST_LINK += \"$$WINDEPLOYQT\" --no-translations --no-opengl-sw --no-svg --compiler-runtime \"$$OUT_PWD/release/$$TARGET_EXE\"
    }
} else {
    VERSTR = $${MAJOR_VER}.$${MINOR_VER}.$${PATCH_VER} # major.minor.patch
}

VERSION = $${VERSTR}

QMAKE_TARGET_DESCRIPTION = "MMS Event Log Conversion GUI Utility"
QMAKE_TARGET_COPYRIGHT = "(C) 2023 Oleksii Gaienko, support@galexsoftware.info"
QMAKE_TARGET_PRODUCT = "MMS Event Log Conversion Utility"
QMAKE_TARGET_INTERNALNAME = "$${TARGET}"
QMAKE_TARGET_COMMENTS = support@galexsoftware.info

DEFINES += \
    QT_USE_QSTRINGBUILDER \
    GUI \
    CONTACT=\\\"support@galexsoftware.info\\\" \
    BUILD_VER=\\\"$${VERSTR}\\\" \
    BUILD_GIT=\\\"$${GIT_VERSION}\\\" \
    "BUILD_DATE=\"\\\"$${BUILD_DATE}\\\"\"" \
    QT_VER=\\\"$${QT_VERSION}\\\" \
    TEST_SRCDIR=\\\"$${TEST_SRCDIR}\\\"
