include(zlib.pri)
INCLUDEPATH += $$PWD/libjpeg/include/
LIBS += -L$$PWD/libjpeg/lib/ -ljpeg
DEFINES += LS3_HAS_LIBJPEG
