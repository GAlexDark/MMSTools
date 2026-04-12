include(../MMSTools.pri)
include(../deployment.pri)

TEMPLATE = app
TARGET = mmsxlsx2json

QT += core

CONFIG += cmdline precompile_header
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
QXLSX_PARENTPATH=$${PROJECT_ROOT_DIRECTORY}/Common/3dparty/QXlsx/       # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=$${PROJECT_ROOT_DIRECTORY}/Common/3dparty/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=$${PROJECT_ROOT_DIRECTORY}/Common/3dparty/QXlsx/source/  # current QXlsx source path is ./source/
include($$QXLSX_PARENTPATH/QXlsx.pri)

INCLUDEPATH += $${PROJECT_ROOT_DIRECTORY}/Common/Core \
               $${PROJECT_ROOT_DIRECTORY}/src

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER  = $${PROJECT_ROOT_DIRECTORY}/Common/Core/stdafx.h

HEADERS += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CConsoleOutput.h \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CBasicCmdLineParser.h \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/elcUtils.h \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/MMSTypes.h \
    src/CX2jConvCmdLineParser.h

SOURCES += \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/CBasicCmdLineParser.cpp \
    $${PROJECT_ROOT_DIRECTORY}/Common/Core/elcUtils.cpp \
    src/CX2jConvCmdLineParser.cpp \
    src/main.cpp

RC_ICONS = img/elcc.ico

MAJOR_VER = 1
MINOR_VER = 0
PATCH_VER = 1
BUILD_VER = 8
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

QMAKE_TARGET_DESCRIPTION = "MMS XLSX to JSON Conversion Console Utility"
QMAKE_TARGET_COPYRIGHT = "(C) 2024 Oleksii Gaienko, support@galexsoftware.info"
QMAKE_TARGET_PRODUCT = "MMS XLSX to JSON Conversion Utility"
QMAKE_TARGET_INTERNALNAME = "$${TARGET}"
QMAKE_TARGET_COMMENTS = support@galexsoftware.info

DEFINES += \
    CONSOLE \
    CONTACT=\\\"support@galexsoftware.info\\\" \
    BUILD_VER=\\\"$${VERSTR}\\\" \
    BUILD_GIT=\\\"$${GIT_VERSION}\\\" \
    "BUILD_DATE=\"\\\"$${BUILD_DATE}\\\"\"" \
    QT_VER=\\\"$${QT_VERSION}\\\" \
    TEST_SRCDIR=\\\"$${TEST_SRCDIR}\\\"
