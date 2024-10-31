TEMPLATE = app

QT += testlib
QT -= gui

include(../../../MMSTools.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

# The path to the OpenSSL folder
HOST_DEPENDENT_PATH = D:/QtSDK/Tools/OpenSSLv3

#For the MinGW uses a custom build of the OpenSSL version 3.3.1
*-g++* {
  LIBS += -L$${HOST_DEPENDENT_PATH}/MinGW/lib/debug/lib64 -llibssl -llibcrypto
  INCLUDEPATH += $${HOST_DEPENDENT_PATH}/MinGW/lib/src/include
  LIBS += -lws2_32 -lGDI32 -lADVAPI32 -lCRYPT32 -lUSER32
}

INCLUDEPATH += $${PROJECT_ROOT_DIRECTORY}/p7bmaker/src

HEADERS += \
    $${PROJECT_ROOT_DIRECTORY}/p7bmaker/src/CPkcs7.h

SOURCES += \
    $${PROJECT_ROOT_DIRECTORY}/p7bmaker/src/CPkcs7.cpp \
    tst_cpkcs7test.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
OTHER_FILES += data/*
