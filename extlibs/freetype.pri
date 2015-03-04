include(zlib.pri)
INCLUDEPATH += $$PWD/freetype/include/
LIBS += -L$$PWD/freetype/lib/ -lfreetype
DEFINES += LS3_HAS_LIBFREETYPE
