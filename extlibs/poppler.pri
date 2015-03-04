include(png.pri)
include(jpeg.pri)
include(tiff.pri)
include(freetype.pri)
include(zlib.pri)
INCLUDEPATH += $$PWD/poppler/include
INCLUDEPATH += $$PWD/poppler/include/poppler
INCLUDEPATH += $$PWD/poppler/include/poppler/qt5
LIBS += -L$$PWD/poppler/lib/ -lpoppler-qt5
DEFINES += LS3_HAS_LIBPOPPLER
