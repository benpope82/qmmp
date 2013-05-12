include(../../plugins.pri)

HEADERS += decodercdaudiofactory.h \
           decoder_cdaudio.h \
           settingsdialog.h
SOURCES += decoder_cdaudio.cpp \
           decodercdaudiofactory.cpp \
           settingsdialog.cpp

TARGET =$$PLUGINS_PREFIX/Input/cdaudio
QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libcdaudio.so


INCLUDEPATH += ../../../
CONFIG += warn_on \
plugin \
link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -L/usr/lib  -I/usr/include
PKGCONFIG += libcdio libcdio_cdda libcdio_paranoia libcddb
TRANSLATIONS = translations/cdaudio_plugin_ru.ts \
               translations/cdaudio_plugin_uk_UA.ts \
               translations/cdaudio_plugin_zh_CN.ts \
               translations/cdaudio_plugin_zh_TW.ts \
               translations/cdaudio_plugin_cs.ts \
               translations/cdaudio_plugin_pl.ts \
               translations/cdaudio_plugin_de.ts \
               translations/cdaudio_plugin_it.ts \
               translations/cdaudio_plugin_tr.ts \
               translations/cdaudio_plugin_lt.ts \
               translations/cdaudio_plugin_nl.ts \
               translations/cdaudio_plugin_ja.ts \
               translations/cdaudio_plugin_es.ts
 
RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}

target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
FORMS += settingsdialog.ui
