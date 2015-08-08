macx {
    !contains( LIBS, litsoz3tools ) {
        QMAKE_LFLAGS += -F$$LS3OUTPUT
        LIBS += -framework litsoz3tools
    }
} else {
    !contains( LIBS, -llitsoz3tools ) {
        LIBS += -L$$LS3OUTPUT  -llitsoz3tools
    }
}

INCLUDEPATH += $$PWD/lib/

