TEMPLATE = app

QT = core

QT_CVERSION = c++17
CONFIG += QT_CVERSION cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# The path to the OpenSSL folder
HOST_DEPENDENT_PATH = D:/QtSDK/Tools/OpenSSLv3
*-msvc* {
  message(Microsoft VS)
  LIBS += -L$${HOST_DEPENDENT_PATH}/Win_x64/lib -llibssl -llibcrypto
  INCLUDEPATH += $${HOST_DEPENDENT_PATH}/Win_x64/include

  # ref: https://stackoverflow.com/questions/16688079/how-to-change-the-c-runtime-library-setting-in-qtcreator
  QMAKE_CXXFLAGS_DEBUG += /MTd
  QMAKE_CFLAGS_DEBUG += /MTd
  QMAKE_CXXFLAGS_RELEASE += /MT
  QMAKE_CFLAGS_RELEASE += /MT
  QMAKE_CXXFLAGS_RELEASE += /Zi
  QMAKE_LFLAGS_RELEASE += /DEBUG
}

#For the MinGW uses a custom build of the OpenSSL version 3.3.1
*-g++* {
  message(MinGW)
  CONFIG(debug, debug|release) {
    message(config_debug)
    LIBS += -L$${HOST_DEPENDENT_PATH}/MinGW/lib/debug/lib64 -llibssl -llibcrypto
  }
  CONFIG(release, debug|release) {
    message(config_release)
    LIBS += -L$${HOST_DEPENDENT_PATH}/MinGW/lib/release/lib64 -llibssl -llibcrypto
  }
  INCLUDEPATH += $${HOST_DEPENDENT_PATH}/MinGW/lib/src/include
  LIBS += -lws2_32 -lGDI32 -lADVAPI32 -lCRYPT32 -lUSER32
}

INCLUDEPATH += ../Common/Core
INCLUDEPATH += . src

HEADERS += \
  ../Common/Core/CConsoleOutput.h \
  ../Common/Core/CBasicCmdLineParser.h \
  ../Common/Core/elcUtils.h \
  ../Common/Core/MMSTypes.h \
  src/CP7bMakerCmdLineParser.h \
  src/CPkcs7.h \
  src/BioByteArray.h

SOURCES += \
  ../Common/Core/CBasicCmdLineParser.cpp \
  ../Common/Core/elcUtils.cpp \
  src/CP7bMakerCmdLineParser.cpp \
  src/CPkcs7.cpp \
  src/BioByteArray.cpp \
  src/main.cpp

include(../deployment.pri)
include(../MMSTools.pri)

RC_ICONS = img/elcc.ico

MAJOR_VER = 1
MINOR_VER = 0
PATCH_VER = 0
BUILD_VER = 4
BUILD_DATE = $$_DATE_

VER_PE = $${MAJOR_VER}.$${MINOR_VER}
win32:VERSTR = $${MAJOR_VER}.$${MINOR_VER}.$${PATCH_VER}.$${BUILD_VER} # major.minor.patch.build
else: VERSTR = $${MAJOR_VER}.$${MINOR_VER}.$${PATCH_VER} # major.minor.patch

VERSION_PE_HEADER = $${VER_PE}
VERSION = $${VERSTR}
QMAKE_TARGET_DESCRIPTION = p7b file maker Console Utility
QMAKE_TARGET_COPYRIGHT = (C) 2024 Oleksii Gaienko
QMAKE_TARGET_PRODUCT = p7b file maker Utility
QMAKE_TARGET_INTERNALNAME = $${TARGET}
QMAKE_TARGET_COMMENTS = support@galexsoftware.info

DEFINES += TEST_SRCDIR=\\\"$${TEST_SRCDIR}\\\"
DEFINES += BUILD_VER=\\\"$${VERSTR}\\\"
DEFINES += BUILD_GIT=\\\"$${GIT_VERSION}\\\"
DEFINES += "BUILD_DATE=\"\\\"$${BUILD_DATE}\\\"\""
DEFINES += QT_VER=\\\"$${QT_VERSION}\\\"
DEFINES += CONTACT=\\\"support@galexsoftware.info\\\"
DEFINES += CONSOLE
