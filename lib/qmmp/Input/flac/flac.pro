# ???? ?????? ? KDevelop ?????????? qmake.
# ------------------------------------------- 
# ?????????? ???????????? ???????? ???????? ???????: ./Plugins/Input/flac
# ???? - ??????????:  

FORMS += detailsdialog.ui 
HEADERS += tag.h \
	   decoderflacfactory.h \
           decoder_flac.h \
           detailsdialog.h 
SOURCES += decoder_flac.cpp \
           decoderflacfactory.cpp \
           detailsdialog.cpp \
           tag.cpp 
DESTDIR = ../
QMAKE_CLEAN += ../libflac.so
INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin \
link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../
LIBS += -lqmmp -L/usr/lib -I/usr/include
PKGCONFIG += taglib flac
TRANSLATIONS = translations/flac_plugin_ru.ts
RESOURCES = translations/translations.qrc
target.path = /lib/qmmp/Input
INSTALLS += target

