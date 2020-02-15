include(zlib.pri)
INCLUDEPATH += $$PWD/freetype/include/
LIBS += -L$$PWD/freetype/lib/ -lfreetype.dll
DEFINES += LS3_HAS_LIBFREETYPE
