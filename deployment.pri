# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

MOC_DIR = moc
OBJECTS_DIR = obj
UI_DIR = GeneratedFiles
UI_HEADERS_DIR = GeneratedFiles
UI_SOURCES_DIR = GeneratedFiles
PRECOMPILED_DIR = pch
