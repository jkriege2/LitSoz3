TEMPLATE = lib
TARGET = litsoz3tools
DEPENDPATH += .

include(../litsoz3.pri)
include(../autoversioning.pri)

release {
    message("building $$TARGET $$TEMPLATE in RELEASE mode, output is in $$LS3OUTPUT")
} else {
    message("building $$TARGET $$TEMPLATE in DEBUG mode, output is in $$LS3OUTPUT")
}

QT += xml gui network

message("Qt Version: $$QT_MAJOR_VERSION . $$QT_MINOR_VERSION")
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets printsupport
    message("Qt Major Version >5, using special Qt5.x include syntax for widgets")
#    win32{
#        !contains(LIBS, -lopengl32): LIBS += -lopengl32
#        !contains(LIBS, -lglu32): LIBS += -lglu32
#    }
}

DESTDIR = $$LS3OUTPUT
DEFINES += LS3LIB_LIBRARY
CONFIG += dll lib_bundle exceptions rtti stl create_prl

# Input
HEADERS += bibtools.h \
           ls3plugin.h \
           ls3pluginservices.h \
           ls3datastore.h \
           ls3selectiondisplaymodel.h \
           ls3importexport.h \
           ls3pluginbase.h \
           filenametools.h \
           latextools.h \
           htmltools.h \
           crossreftools.h \
           ls3extractmetadataplugin.h \
           pubmedtools.h \
           ls3referencetreeitem.h \
           ls3referencetreemodel.h \
    htmltokenizer.h \
    lib_imexport.h \
    ls3progressthread.h \
    filetools.h \
    modstools.h

SOURCES += bibtools.cpp \
           ls3datastore.cpp \
           ls3selectiondisplaymodel.cpp \
           filenametools.cpp \
           latextools.cpp \
           htmltools.cpp \
           crossreftools.cpp \
           pubmedtools.cpp \
           ls3referencetreeitem.cpp \
           ls3referencetreemodel.cpp \
    htmltokenizer.cpp \
    ls3progressthread.cpp \
    filetools.cpp \
    modstools.cpp

FORMS +=

RESOURCES += \
    lib.qrc

TRANSLATIONS= ../output/translations/de.lib.ts



