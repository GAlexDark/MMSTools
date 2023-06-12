QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += cmdline precompile_header
CONFIG(release, debug|release): QMAKE_CXXFLAGS_RELEASE += -Ofast
CONFIG(release, debug|release): QMAKE_CXXFLAGS += -Ofast
#CONFIG(debug, debug|release): QMAKE_CXXFLAGS_DEBUG += -pg -no-pie #-g3 -pg
#CONFIG(debug, debug|release): QMAKE_LFLAGS_DEBUG += -pg -no-pie #-pg -lgmon

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# QXlsx code for Application Qt project
QXLSX_PARENTPATH=../Common/3dparty/QXlsx/       # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=../Common/3dparty/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=../Common/3dparty/QXlsx/source/  # current QXlsx source path is ./source/
include(../Common/3dparty/QXlsx/QXlsx.pri)


INCLUDEPATH += ../Common/Settings
INCLUDEPATH += . src

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER  = src/stdafx.h

HEADERS += \
    src/stdafx.h \
    ../Common/Settings/QPayKioskSettings.h \
    src/mainwindow.h \
    src/ELCSettings.h \

SOURCES += \
    ../Common/Settings/QPayKioskSettings.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/ELCSettings.cpp

FORMS += \
    src/mainwindow.ui

LANGUAGES = ru en uk
TRANSLATIONS += \
    i18n/$${TARGET}_ru_RU.ts \
    i18n/$${TARGET}_uk_UA.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

MOC_DIR = moc
OBJECTS_DIR = obj
UI_DIR = GeneratedFiles
UI_HEADERS_DIR = GeneratedFiles
UI_SOURCES_DIR = GeneratedFiles
#RCC_DIR = rcc

RESOURCES += EventLogConverter.qrc
RC_FILE = $${TARGET}.rc

