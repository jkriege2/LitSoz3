include(zlib.pri)
INCLUDEPATH += ../extlibs/freetype/include/
LIBS += -L../extlibs/freetype/lib/ -lfreetype
DEFINES += LS3_HAS_LIBFREETYPE
