include(zlib.pri)
INCLUDEPATH += ../extlibs/libjpeg/include/
LIBS += -L../extlibs/libjpeg/lib/ -ljpeg
DEFINES += LS3_HAS_LIBJPEG
