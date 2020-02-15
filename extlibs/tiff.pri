include(zlib.pri)
INCLUDEPATH += $$PWD/libtiff/include/
LIBS += -L$$PWD/libtiff/lib/ -ltiff.dll
DEFINES += LS3_HAS_LIBTIFF
