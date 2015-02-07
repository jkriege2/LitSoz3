TEMPLATE = subdirs

# LitSoz 3 tools library
SUBDIRS += lib

# Test application for lib
SUBDIRS += libtest
libtest.depends = lib
libtest.subdir = ./lib/test
Debug_TestLIB.target = Debug_TestLIB
Debug_TestLIB.depends =  debug
QMAKE_EXTRA_TARGETS += Debug_TestLIB

# additional widgets library
SUBDIRS += libwidgets
libwidgets.depends = lib

# Test application for libwidgets
SUBDIRS += libwidgetstest
libwidgetstest.depends = libwidgets lib
libwidgetstest.subdir = ./libwidgets/test
Debug_TestWidgetsLIB.target = Debug_TestWidgetsLIB
Debug_TestWidgetsLIB.depends = debug
QMAKE_EXTRA_TARGETS += Debug_TestWidgetsLIB



# Main application LitSoz 3.0
SUBDIRS += application
application.depends = lib libwidgets


# also build the plugins
SUBDIRS += isbnimport
isbnimport.subdir = ./plugins/isbnimport
isbnimport.depends = lib libwidgets

SUBDIRS += latextools
latextools.subdir = ./plugins/latextools
latextools.depends = lib libwidgets


SUBDIRS += articletools
articletools.subdir = ./plugins/articletools
articletools.depends = lib libwidgets

