3rd PARTY LIBS FOR LITSOZ
~~~~~~~~~~~~~~~~~~~~~~~~~

This directory contains source distributions of 3rd party libs needed to build LitSoz 3.0.
You can use these, but do not have to (if you have these libs somewhere else on you system.

LitSoz 3.0 needs these external libs:
  - poppler (for PDF display&parsing)
  
Unfortunately to build these libs, you also need to build several dependent libraries,
which are also part of this directory.

For windows and Linux (and possibly also MacOS) a sheel-script (bash) is provided that
unpacks the source distributions and builds the libs. this script is

  build_dependencies.sh
  
In order to run it on Windows, you need a MinGW compiler and an MSys1 build environment, which
you can install by executing ./msys/MSYS-1.0.11.exe . Ensure to register YOUR MinGW installation
with this system so the builds work. 

The build_dependencies.sh script will guide you through the compile steps. Ensure to set the
requested environment variables appropriately that are listed at the start of the script.