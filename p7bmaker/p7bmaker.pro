include(../MMSTools.pri)
include(../deployment.pri)

TEMPLATE = app
TARGET = p7bmaker

QT = core

CONFIG += cmdline
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

# The path to the OpenSSL folder
HOST_DEPENDENT_PATH = D:/QtSDK/Tools/OpenSSLv3
OPENSSL_PATH = $${OPENSSL_DIR}
isEmpty(OPENSSL_PATH): OPENSSL_PATH = $${HOST_DEPENDENT_PATH}

win32-msvc* {
  message("Building with MSVC")
  INCLUDEPATH += $${OPENSSL_PATH}/Win_x64/include
  LIBS += -L$${OPENSSL_PATH}/Win_x64/lib -llibssl -llibcrypto
}

#For the MinGW uses a custom build of the OpenSSL version 3.3.1
win32-g++ {
  message("Building with MinGW")
  INCLUDEPATH += $${OPENSSL_PATH}/MinGW/lib/src/include

  CONFIG(debug, debug|release) {
    LIBS += -L$${OPENSSL_PATH}/MinGW/lib/debug/lib64
  } else {
    LIBS += -L$${OPENSSL_PATH}/MinGW/lib/release/lib64
  }
  LIBS += -llibssl -llibcrypto -lws2_32 -lGDI32 -lADVAPI32 -lCRYPT32 -lUSER32
}

INCLUDEPATH += $${PROJECT_ROOT_DIRECTORY}/Common/Core \
               $${PROJECT_ROOT_DIRECTORY}/src

HEADERS += \
  $${PROJECT_ROOT_DIRECTORY}/Common/Core/CConsoleOutput.h \
  $${PROJECT_ROOT_DIRECTORY}/Common/Core/CBasicCmdLineParser.h \
  $${PROJECT_ROOT_DIRECTORY}/Common/Core/elcUtils.h \
  $${PROJECT_ROOT_DIRECTORY}/Common/Core/MMSTypes.h \
  src/CP7bMakerCmdLineParser.h \
  src/CPkcs7.h

SOURCES += \
  $${PROJECT_ROOT_DIRECTORY}/Common/Core/CBasicCmdLineParser.cpp \
  $${PROJECT_ROOT_DIRECTORY}/Common/Core/elcUtils.cpp \
  src/CP7bMakerCmdLineParser.cpp \
  src/CPkcs7.cpp \
  src/main.cpp

RC_ICONS = img/elcc.ico

MAJOR_VER = 1
MINOR_VER = 0
PATCH_VER = 0
BUILD_VER = 11
BUILD_DATE = $$_DATE_

# VERSION_PE_HEADER = $${MAJOR_VER}.$${MINOR_VER}
win32 {
    VERSTR = $${MAJOR_VER}.$${MINOR_VER}.$${PATCH_VER}.$${BUILD_VER} # major.minor.patch.build

    CONFIG(release, debug|release) {
        WINDEPLOYQT = $$system_path($$[QT_INSTALL_BINS]/windeployqt.exe)
        TARGET_EXE  = $$system_path($$FINAL_DEST_DIR/$${TARGET}.exe)
        # This postbuild process dont copy the OpenSSL dlls.
        QMAKE_POST_LINK += \"$$WINDEPLOYQT\" --no-translations --no-opengl-sw --no-svg --compiler-runtime \"$$OUT_PWD/release/$$TARGET_EXE\"
    }
} else {
    VERSTR = $${MAJOR_VER}.$${MINOR_VER}.$${PATCH_VER} # major.minor.patch
}

VERSION = $${VERSTR}

QMAKE_TARGET_DESCRIPTION = "p7b file maker Console Utility"
QMAKE_TARGET_COPYRIGHT = "(C) 2024 Oleksii Gaienko, support@galexsoftware.info"
QMAKE_TARGET_PRODUCT = "p7b file maker Utility"
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
