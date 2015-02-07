TEMPLATE = lib
TARGET = litsoz3widgets
DEPENDPATH += .


include(../litsoz3.pri)

release {
    message("building $$TARGET $$TEMPLATE in RELEASE mode, output is in $$LS3OUTPUT")
} else {
    message("building $$TARGET $$TEMPLATE in DEBUG mode, output is in $$LS3OUTPUT")
}

QT += xml gui

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
DEFINES += LS3LIBWIDGETS_LIBRARY
CONFIG += dll lib_bundle exceptions rtti stl create_prl


HEADERS += qcompleterfromfile.h \
           qcompleterplaintextedit.h \
           qenhancedlineedit.h \
           qmappablecombobox.h \
           qspecialtoolbutton.h \
           jkstarratingwidget.h \
           jkstyledbutton.h \
           jkcharacterscreenkeyboard.h \
           flowlayout.h \
           htmldelegate.h \
           qenhancedtableview.h \
           qmodernprogresswidget.h \
           qcheckablestringlistmodel.h \
    lib_imexport.h \
    qrecentfilesmenu.h \
    qfilenameslistedit.h \
    jkverticalscrollarea.h


SOURCES += qcompleterfromfile.cpp \
           qcompleterplaintextedit.cpp \
           qenhancedlineedit.cpp \
           qmappablecombobox.cpp \
           qspecialtoolbutton.cpp\
           jkstarratingwidget.cpp \
           jkstyledbutton.cpp \
           jkcharacterscreenkeyboard.cpp \
           flowlayout.cpp \
           htmldelegate.cpp \
           qenhancedtableview.cpp \
           qmodernprogresswidget.cpp \
           qcheckablestringlistmodel.cpp \
    qrecentfilesmenu.cpp \
    qfilenameslistedit.cpp \
    jkverticalscrollarea.cpp

FORMS +=

RESOURCES += libwidgets.qrc

TRANSLATIONS= ../output/translations/de.libwidgets.ts


