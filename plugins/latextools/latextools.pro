TEMPLATE = lib
CONFIG += plugin


TARGET = latextools
DEPENDPATH += ./

include(../plugins.pri)
include(../../libls3.pri)
include(../../libls3widgets.pri)


# Input
HEADERS += ls3pluginlatextools.h \
           ls3bibteximporter.h \
           ls3bibtexexporter.h \
    bibtexexportdialog.h

SOURCES += ls3pluginlatextools.cpp \
           ls3bibteximporter.cpp \
           ls3bibtexexporter.cpp \
    bibtexexportdialog.cpp

FORMS = \
    bibtexexportdialog.ui

RESOURCES += latextools.qrc

TRANSLATIONS= $$LS3OUTPUT/translations/de.latextools.ts


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
