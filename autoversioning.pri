# only write new version files in release mode
# in debug mode: only make sure that the files exist, but do not update them





message(AUTOVERSIONING IN $$PWD)
#try to read compiler name and version
COMPILERVERSION=$$system($$QMAKE_CXX --version)
COMPILERVERSION_MACHINE=$$system($$QMAKE_CXX -dumpmachine)
message($$QMAKE_CXX  .$${DIR_SEPARATOR}tools$${DIR_SEPARATOR}ls3infotool$${DIR_SEPARATOR}main.cpp -o .$${DIR_SEPARATOR}output$${DIR_SEPARATOR}ls3infotool$${EXE_SUFFIX})
system($$QMAKE_CXX  .$${DIR_SEPARATOR}tools$${DIR_SEPARATOR}ls3infotool$${DIR_SEPARATOR}main.cpp -o .$${DIR_SEPARATOR}output$${DIR_SEPARATOR}ls3infotool$${EXE_SUFFIX})

# try to read the SVN version
GITCOMMITCOUNT = $$system(git rev-list HEAD --count)
contains(GITCOMMITCOUNT,exported):GITCOMMITCOUNT=
contains(GITCOMMITCOUNT,exported):GITCOMMITCOUNT=
isEmpty(GITCOMMITCOUNT) {
    GITCOMMITCOUNT = ---
}

DATESTR = $$system(.$${DIR_SEPARATOR}output$${DIR_SEPARATOR}ls3infotool$${EXE_SUFFIX} --date)
DATESTRYEAR = $$system(.$${DIR_SEPARATOR}output$${DIR_SEPARATOR}ls3infotool$${EXE_SUFFIX} --year)

message("COMPILE MODE: GITCOMMITCOUNT is: $$GITCOMMITCOUNT")
message("COMPILE MODE: build date is: $$DATESTR")
message("COMPILE MODE: compiler: $$COMPILERVERSION")
message("COMPILE MODE: compiler.machine: $$COMPILERVERSION_MACHINE")

system($$PWD$${DIR_SEPARATOR}output$${DIR_SEPARATOR}ls3infotool$${EXE_SUFFIX} --writedate $$PWD$${DIR_SEPARATOR}compiledate.h)

win32 {
    system(echo $$LITERAL_HASH define GITCOMMITCOUNT \"$$GITCOMMITCOUNT\"  > gitversion.h )
    system(echo $$LITERAL_HASH define COMPILER \"$$COMPILERVERSION (MACHINE: $$COMPILERVERSION_MACHINE)\"  > compiler.h )
} else {
    system(echo \'$$LITERAL_HASH define GITCOMMITCOUNT \"$$GITCOMMITCOUNT\"\'  > gitversion.h )
    system(echo \'$$LITERAL_HASH define COMPILER \"$$COMPILERVERSION (MACHINE: $$COMPILERVERSION_MACHINE)\"\'  > compiler.h )
}


