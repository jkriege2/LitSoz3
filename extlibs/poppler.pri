include(png.pri)
include(jpeg.pri)
include(tiff.pri)
include(freetype.pri)
include(zlib.pri)
include(openjpeg.pri)
INCLUDEPATH += $$PWD/poppler/include
INCLUDEPATH += $$PWD/poppler/include/poppler
LIBS += -L$$PWD/poppler/lib/ -lpoppler.dll
greaterThan(QT_MAJOR_VERSION, 4) {
    INCLUDEPATH += $$PWD/poppler/include/poppler/qt5
    LIBS += -L$$PWD/poppler/lib/ -lpoppler-qt5.dll
} else {
    INCLUDEPATH += $$PWD/poppler/include/poppler/qt4
    LIBS += -L$$PWD/poppler/lib/ -lpoppler-qt4
}
DEFINES += LS3_HAS_LIBPOPPLER
