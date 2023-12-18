TEMPLATE = subdirs
include(MMSTools.pri)

SUBDIRS += \
    EventLogConverter \
    EventLogConverterc \
    tests

MOC_DIR = moc
OBJECTS_DIR = obj
UI_DIR = GeneratedFiles
UI_HEADERS_DIR = GeneratedFiles
UI_SOURCES_DIR = GeneratedFiles
#RCC_DIR = rcc
