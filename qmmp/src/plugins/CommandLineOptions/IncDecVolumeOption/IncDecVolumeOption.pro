include(../../plugins.pri)

CONFIG += release \
warn_on \
plugin  \
 lib

TARGET =$$PLUGINS_PREFIX/CommandLineOptions/incdecvolumeoption
QMAKE_CLEAN =$$PLUGINS_PREFIX/CommandLineOptions/libincdecvolumeoption.so

TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib

TRANSLATIONS = translations/incdecvolume_plugin_cs.ts \
               translations/incdevvolume_plugin_de.ts
               RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target

INCLUDEPATH += ../../../../src

LIBS += -lqmmpui

HEADERS += incdecvolumeoption.h

SOURCES += incdecvolumeoption.cpp
