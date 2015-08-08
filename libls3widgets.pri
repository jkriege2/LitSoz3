include(libls3.pri)

macx {
    !contains( LIBS, litsoz3widgets ) {
        QMAKE_LFLAGS += -F$$LS3OUTPUT
        LIBS += -framework litsoz3widgets
    }
} else {
    !contains( LIBS, -llitsoz3widgets ) {
        LIBS += -L$$LS3OUTPUT  -llitsoz3widgets
    }
}

INCLUDEPATH += $$PWD/libwidgets/
