TEMPLATE = app
TARGET = litsoz3widgets_test
DEFINES += QT_DLL
DESTDIR = ../../output

LIBS += -L../../output -llitsoz3widgets

INCLUDEPATH += . \
               ../

HEADERS     = testmain.h

RESOURCES   =

SOURCES     = test.cpp \
              testmain.cpp

FORMS       =

TRANSLATIONS=

QT += gui core

message("Qt Version: $$QT_MAJOR_VERSION . $$QT_MINOR_VERSION")
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets printsupport
    message("Qt Major Version >5, using special Qt5.x include syntax for widgets")
#    win32{
#        !contains(LIBS, -lopengl32): LIBS += -lopengl32
#        !contains(LIBS, -lglu32): LIBS += -lglu32
#    }
}
CONFIG += console

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/
