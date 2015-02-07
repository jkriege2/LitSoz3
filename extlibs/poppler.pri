include(png.pri)
include(jpeg.pri)
include(tiff.pri)
include(freetype.pri)
include(zlib.pri)
INCLUDEPATH += ../extlibs/poppler/include
INCLUDEPATH += ../extlibs/poppler/include/poppler/qt5
LIBS += -L../extlibs/poppler/lib/ -lpoppler-qt5
DEFINES += LS3_HAS_LIBPOPPLER
