include(zlib.pri)
INCLUDEPATH += ../extlibs/libtiff/include/
LIBS += -L../extlibs/libtiff/lib/ -ltiff
DEFINES += LS3_HAS_LIBTIFF
