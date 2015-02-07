TEMPLATE = lib
CONFIG += plugin

TARGET = articletools
DEPENDPATH += ./
include(../plugins.pri)
include(../../libls3.pri)
include(../../libls3widgets.pri)


# Input
HEADERS += \
    ls3pluginarticletools.h \
    renamefilesdialog.h \
    renamefilesprogressdialog.h \
    pdfdropdockwidget.h \
    xmptools.h \
    pdftools.h \
    multiplerecordinsertdialog.h \
    wwwthread.h \
    doithread.h \
    pubmedthread.h \
    analyzepdfthread.h \
    pubmedsearchdialog.h

SOURCES += \
    ls3pluginarticletools.cpp \
    renamefilesdialog.cpp \
    renamefilesprogressdialog.cpp \
    pdfdropdockwidget.cpp \
    xmptools.cpp \
    pdftools.cpp \
    multiplerecordinsertdialog.cpp \
    wwwthread.cpp \
    doithread.cpp \
    pubmedthread.cpp \
    analyzepdfthread.cpp \
    pubmedsearchdialog.cpp

FORMS = renamefilesdialog.ui \
    renamefilesprogressdialog.ui \
    pdfdropdockwidget.ui \
    multiplerecordinsertdialog.ui \
    pubmedsearchdialog.ui

RESOURCES += \
    articletools.qrc \
    filetools.qrc

TRANSLATIONS= $$LS3OUTPUT/translations/de.articletools.ts


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
