include(zlib.pri)
INCLUDEPATH += $$PWD/libtiff/include/
LIBS += -L$$PWD/libtiff/lib/ -ltiff
DEFINES += LS3_HAS_LIBTIFF
