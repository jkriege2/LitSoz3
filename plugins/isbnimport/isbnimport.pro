TEMPLATE = lib
CONFIG += plugin


TARGET = isbnimport
DEPENDPATH += ./
include(../plugins.pri)
include(../../libls3.pri)
include(../../libls3widgets.pri)


# Input
HEADERS += ls3pluginisbnimport.h \
    isbnthread.h \
    multiplerecordinsertdialog.h

SOURCES += ls3pluginisbnimport.cpp \
    isbnthread.cpp \
    multiplerecordinsertdialog.cpp

FORMS = \
    multiplerecordinsertdialog.ui

RESOURCES += isbnimport.qrc

TRANSLATIONS= $$LS3OUTPUT/translations/de.isbnimport.ts


QT += gui core network xml

message("Qt Version: $$QT_MAJOR_VERSION . $$QT_MINOR_VERSION")
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets printsupport
    message("Qt Major Version >5, using special Qt5.x include syntax for widgets")
#    win32{
#        !contains(LIBS, -lopengl32): LIBS += -lopengl32
#        !contains(LIBS, -lglu32): LIBS += -lglu32
#    }
}
