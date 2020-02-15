#! /bin/bash

function print_result {
	if [ $2 == -1 ]; then
		echo -e "building '$1': \033[0;33m NOT SELECTED \033[0m" 
	fi
	if [ $2 == -2 ]; then
		echo -e "building '$1': \033[0;31m ERROR IN CONFIGURE \033[0m" 
	fi
	if [ $2 == -3 ]; then
		echo -e "building '$1': \033[0;31m ERROR IN MAKE STEP \033[0m" 
	fi
	if [ $2 == -4 ]; then
		echo -e "building '$1': \033[0;31m ERROR IN INSTALL STEP \033[0m" 
	fi
	if [ $2 == -5 ]; then
		echo -e "building '$1': \033[0;33m INSTALL SCRIPT DOES NOT SUPPORT THIS ON YOUR PLATFORM \033[0m" 
	fi
	if [ $2 == 0 ]; then
		echo -e "building '$1': \033[1;32m OK \033[0m" 
	fi
}


function copy_sharedlibs {
	cp $1/*.so ../output
	cp $1/*.so.* ../output
	cp $1/*.dll ../output
	cp $1/*.dylib ../output
}

function copy_exe {
	cp $1/*.exe ../output
}

echo -e "========================================================================\n"\
"= LitSoz 3 dependency build script                                     =\n"\
"========================================================================\n\n"\
"This script will try to build all the dependency libraries \n"\
"needed for LitSoz. All libraries will be built in dynamic linking mode\n"\
"and stay inside their subdirectory of this extlibs directory, so they\n"\
"don't interfere with your system-libraries. The .pri-files in this di-\n"\
"rectory include the libraries in a way that preferences these libraries \n"\
"here!\n\nBefore running, please make sure that QTDIR points to Qt!!! using\n"\
"  export QTDIR=/c/development/Qt/5.13.2/mingw73_64/\n"\
"  export PATH=/c/development/Qt/Tools/mingw730_64/bin/:/C/development/CMake/bin/:$PATH \n"\
"\n\nFinally this script will copy all generated dynamic link libraries into\n"\
"the LitSoz output folder, so they are found by the program at runtime."\
"\n\nFirst we need to set some basics for the builds:"\

read -p "Do you want to build keep the build directories (y/n)? " -n 1 KEEP_BUILD_DIR
echo -e  "\n"
read -p "how many parallel builds do you want to use in make (1/2/3/.../9)? " -n 1  MAKE_PARALLEL_BUILDS
echo -e  "\n"

CURRENTDIR=${PWD}
PATH="${PATH}:${CURRENTDIR}/pkg_config/bin/:${CURRENTDIR}/../output/"
export PATH
QT_INFO_LIBS=`${QTDIR}/bin/qmake -query QT_INSTALL_LIBS`
QT_INFO_BIN=`${QTDIR}/bin/qmake -query QT_INSTALL_BINS`
QT_INFO_PLUGINS=`${QTDIR}/bin/qmake -query QT_INSTALL_PLUGINS`
QT_INFO_INSTALLDIR=`${QTDIR}/bin/qmake -query QT_INSTALL_PREFIX`
QT_INFO_VERSION=`${QTDIR}/bin/qmake -query QT_VERSION`
echo -e "\n\nbuilding for\n    Qt version ${QT_INFO_VERSION}\n       in ${QT_INFO_INSTALLDIR}\n       libs ${QT_INFO_LIBS}\n\n"

PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${QT_INFO_LIBS}/pkgconfig/"
export PKG_CONFIG_PATH 

zlibOK=-1
read -p "Do you want to build 'zlib' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "------------------------------------------------------------------------\n"\
	"-- BUILDING: zlib                                                     --\n"\
	"------------------------------------------------------------------------\n\n"\

	cd zlib
	mkdir build
	mkdir bin
	tar xvf zlib-1.2.11.tar.gz -C ./build/
	cd build/zlib-1.2.11
	ISMSYS=`uname -o`
	echo $ISMSYS
	if [ "$ISMSYS" != "${string/Msys/}" ] ; then
		BINARY_PATH='../../bin'
		INCLUDE_PATH='../../include'
	    LIBRARY_PATH='../../lib'
		
		echo -e 'BINARY_PATH='$BINARY_PATH'\nINCLUDE_PATH='$INCLUDE_PATH'\nLIBRARY_PATH='$LIBRARY_PATH'\nLOC='${PICFLAGS} $MORECFLAGS|cat - ./win32/Makefile.gcc > ./Makefile.gcc
		
		MAKEFILE="Makefile.gcc"
	else
		./configure --prefix=${CURRENTDIR}/zlib  
		MAKEFILE="Makefile"
	fi
	libOK=$?
	if [ $libOK -eq 0 ] ; then
		make -j${MAKE_PARALLEL_BUILDS} -f $MAKEFILE
		
		libOK=$?
		if [ $libOK -eq 0 ] ; then		
			make -j${MAKE_PARALLEL_BUILDS} install -f $MAKEFILE  SHARED_MODE=1
			libOK=$?
			if [ $libOK -ne 0 ] ; then		
				libOK=-4
			fi
		else
			libOK=-3
		fi
	else
	    libOK=-2
	fi

	cd ../../
	if [ $KEEP_BUILD_DIR == "n" ] ; then
		rm -rf build
	fi
	cd ${CURRENTDIR}
	
	zlibOK=$libOK

fi

copy_sharedlibs ./zlib/bin/

C_INCLUDE_PATH="${C_INCLUDE_PATH}:${CURRENTDIR}/zlib/include/"
CPLUS_INCLUDE_PATH="${CPLUS_INCLUDE_PATH}:${CURRENTDIR}/zlib/include/"
LIBRARY_PATH="${LIBRARY_PATH}:${CURRENTDIR}/zlib/lib/"
LIBS="${LIBS} -lz"
ZLIB_LIB="${CURRENTDIR}/zlib/lib/libz.a"
ZLIB_INC="${CURRENTDIR}/zlib/include/"
export C_INCLUDE_PATH
export CPLUS_INCLUDE_PATH
export LIBRARY_PATH
PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${CURRENTDIR}/zlib/lib/pkgconfig"
export




libfreetypeOK=-1
read -p "Do you want to build 'freetype' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "------------------------------------------------------------------------\n"\
	"-- BUILDING: freetype                                                 --\n"\
	"------------------------------------------------------------------------\n\n"\

	cd freetype
	mkdir build
	mkdir include
	mkdir include/freetype2
	mkdir include/freetype2/freetype
	mkdir include/freetype2/freetype/config
	tar xvf freetype-2.10.1.tar.gz -C ./build/
	cd build/freetype-2.10.1
	
	./configure --enable-shared --disable-static  --prefix=${CURRENTDIR}/freetype
	libOK=$?
	if [ $libOK -eq 0 ] ; then
		make -j${MAKE_PARALLEL_BUILDS}
		
		libOK=$?
		if [ $libOK -eq 0 ] ; then		
			make -j${MAKE_PARALLEL_BUILDS} install
			libOK=$?
			if [ $libOK -ne 0 ] ; then		
				libOK=-4
			fi
		else
			libOK=-3
		fi
	else
	    libOK=-2
	fi

	cd ../../
	if [ $KEEP_BUILD_DIR == "n" ] ; then
		rm -rf build
	fi
	cd ${CURRENTDIR}
	
	libfreetypeOK=$libOK

fi

copy_sharedlibs ./freetype/bin/


FREETYPE_CFLAGS="-I${CURRENTDIR}/freetype/include/ -I${CURRENTDIR}/freetype/include/freetype2/"
FREETYPE_LIBS="-L${CURRENTDIR}/freetype/lib/ -lfreetype"
FREETYPE_DIR="${CURRENTDIR}/freetype"

C_INCLUDE_PATH="${C_INCLUDE_PATH}:${CURRENTDIR}/freetype/include/:${CURRENTDIR}/freetype/include/freetype2/"
CPLUS_INCLUDE_PATH="${CPLUS_INCLUDE_PATH}:${CURRENTDIR}/freetype/include/:${CURRENTDIR}/freetype/include/freetype2/"
LIBRARY_PATH="${LIBRARY_PATH}:${CURRENTDIR}/freetype/lib/"
FREETYPE_LIB="${CURRENTDIR}/freetype/lib/libfreetype.dll.a"
FREETYPE_INC="${CURRENTDIR}/freetype/include/"
export C_INCLUDE_PATH
export CPLUS_INCLUDE_PATH
export LIBRARY_PATH
export FREETYPE_CFLAGS
export FREETYPE_LIBS
PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${CURRENTDIR}/freetype/lib/pkgconfig"
export PKG_CONFIG_PATH 





libpngOK=-1
read -p "Do you want to build 'libpng' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "\n------------------------------------------------------------------------\n"\
	"-- BUILDING: libpng                                                    --\n"\
	"------------------------------------------------------------------------\n\n"\

	cd libpng
	mkdir build
	tar xvf libpng-1.5.4.tar.gz -C ./build/
	cd build/libpng-1.5.4
	./configure --enable-shared --disable-static --prefix=${CURRENTDIR}/libpng LDFLAGS=-L${CURRENTDIR}/zlib/lib CFLAGS=-I${CURRENTDIR}/zlib/include CXXFLAGS=-I${CURRENTDIR}/zlib/include/
	libOK=$?
	if [ $libOK -eq 0 ] ; then
		make -j${MAKE_PARALLEL_BUILDS}
		
		libOK=$?
		if [ $libOK -eq 0 ] ; then		
			make -j${MAKE_PARALLEL_BUILDS} install
			libOK=$?
			if [ $libOK -ne 0 ] ; then		
				libOK=-4
			fi
		else
			libOK=-3
		fi
	else
	    libOK=-2
	fi
	

	cd ../../
	if [ $KEEP_BUILD_DIR == "n" ] ; then
		rm -rf build
	fi
	cd ${CURRENTDIR}
	
	libpngOK=$libOK

fi

copy_sharedlibs ./libpng/bin/



C_INCLUDE_PATH="${C_INCLUDE_PATH}:${CURRENTDIR}/libpng/include/"
CPLUS_INCLUDE_PATH="${CPLUS_INCLUDE_PATH}:${CURRENTDIR}/libpng/include/"
LIBRARY_PATH="${LIBRARY_PATH}:${CURRENTDIR}/libpng/lib/"
LIBPNG_CFLAGS="-I${CURRENTDIR}/libpng/include/"
LIBPNG_LIBS="-L${CURRENTDIR}/libpng/lib/ -lpng"
PNG_LIB="${CURRENTDIR}/libpng/lib/libpng.dll.a"
PNG_INC="${CURRENTDIR}/libpng/include/"
export C_INCLUDE_PATH
export CPLUS_INCLUDE_PATH
export LIBRARY_PATH
export LIBPNG_CFLAGS
export LIBPNG_LIBS
PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${CURRENTDIR}/libpng/lib/pkgconfig"
export PKG_CONFIG_PATH 



libJPEGOK=-1
read -p "Do you want to build 'libJPEG' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "\n------------------------------------------------------------------------\n"\
	"-- BUILDING: libJPEG                                                  --\n"\
	"------------------------------------------------------------------------\n\n"\

	cd libjpeg
	mkdir build
	tar xvf jpegsrc.v9a.tar.gz -C ./build/
	cd build/jpeg-9a
	./configure --enable-shared --disable-static --disable-dependency-tracking --prefix=${CURRENTDIR}/libjpeg LDFLAGS=-L${CURRENTDIR}/zlib/lib CFLAGS=-I${CURRENTDIR}/zlib/include CXXFLAGS=-I${CURRENTDIR}/zlib/include/
	libOK=$?
	if [ $libOK -eq 0 ] ; then
		make -j${MAKE_PARALLEL_BUILDS}
		
		libOK=$?
		if [ $libOK -eq 0 ] ; then		
			make -j${MAKE_PARALLEL_BUILDS} install
			libOK=$?
			if [ $libOK -ne 0 ] ; then		
				libOK=-4
			fi
		else
			libOK=-3
		fi
	else
	    libOK=-2
	fi
	

	cd ../../
	if [ $KEEP_BUILD_DIR == "n" ] ; then
		rm -rf build
	fi
	cd ${CURRENTDIR}
	
	libJPEGOK=$libOK

fi

copy_sharedlibs ./libjpeg/bin/

C_INCLUDE_PATH="${C_INCLUDE_PATH}:${CURRENTDIR}/libjpeg/include/"
CPLUS_INCLUDE_PATH="${CPLUS_INCLUDE_PATH}:${CURRENTDIR}/libjpeg/include/"
LIBRARY_PATH="${LIBRARY_PATH}:${CURRENTDIR}/libjpeg/lib/"
JPEG_LIB="${CURRENTDIR}/libjpeg/lib/libjpeg.dll.a"
JPEG_INC="${CURRENTDIR}/libjpeg/include/"
export C_INCLUDE_PATH
export CPLUS_INCLUDE_PATH
export LIBRARY_PATH
PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${CURRENTDIR}/libjpeg/lib/pkgconfig"
export PKG_CONFIG_PATH 

LIBJPEG_CONFIGFLAGS=
if [ -e ${CURRENTDIR}/libjpeg/lib/libjpeg.a ] ; then
	LIBJPEG_CONFIGFLAGS="--with-jpeg-include-dir=${CURRENTDIR}/libjpeg/include --with-jpeg-lib-dir=${CURRENTDIR}/libjpeg/lib"
fi


libtiffOK=-1
read -p "Do you want to build 'libtiff' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "\n------------------------------------------------------------------------\n"\
	"-- BUILDING: libtiff                                                   --\n"\
	"------------------------------------------------------------------------\n\n"\

	cd libtiff
	mkdir build
	tar xvf tiff-4.0.3.tar.gz -C ./build/
	cd build/tiff-4.0.3
	./configure --enable-shared --disable-static  --disable-jbig ${zlib_CONFIG}  ${LIBJPEG_CONFIGFLAGS} --prefix=${CURRENTDIR}/libtiff
	
	libOK=$?
	if [ $libOK -eq 0 ] ; then
		make 
		
		libOK=$?
		if [ $libOK -eq 0 ] ; then		
			make  install
			libOK=$?
			if [ $libOK -ne 0 ] ; then		
				libOK=-4
			fi
		else
			libOK=-3
		fi
	else
	    libOK=-2
	fi
	

	cd ../../
	if [ $KEEP_BUILD_DIR == "n" ] ; then
		rm -rf build
	fi
	cd ${CURRENTDIR}
	
	libtiffOK=$libOK

fi

copy_sharedlibs ./libtiff/bin/

C_INCLUDE_PATH="${C_INCLUDE_PATH}:${CURRENTDIR}/libtiff/include/"
CPLUS_INCLUDE_PATH="${CPLUS_INCLUDE_PATH}:${CURRENTDIR}/libtiff/include/"
LIBRARY_PATH="${LIBRARY_PATH}:${CURRENTDIR}/libtiff/lib/"
TIFF_LIB="${CURRENTDIR}/libtiff/lib/libtiff.dll.a"
TIFF_INC="${CURRENTDIR}/libtiff/include/"
export C_INCLUDE_PATH
export CPLUS_INCLUDE_PATH
export LIBRARY_PATH
PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${CURRENTDIR}/libtiff/lib/pkgconfig"
export PKG_CONFIG_PATH 






libopenjpegOK=-1
read -p "Do you want to build 'openjpeg' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "\n------------------------------------------------------------------------\n"\
	"-- BUILDING: openjpeg                                                  --\n"\
	"------------------------------------------------------------------------\n\n"\

	cd libopenjpeg
	mkdir build

	
	tar xvf openjpeg-2.3.1.tar.gz -C ./build/
	cd build/openjpeg-2.3.1
	
	mkdir build
	cd build
	cmake -G "MSYS Makefiles" -DCMAKE_PREFIX_PATH="${QTDIR}" -DCMAKE_INSTALL_PREFIX="${CURRENTDIR}/libopenjpeg" -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ..

	libOK=$?
	if [ $libOK -eq 0 ] ; then
		cmake --build . -j${MAKE_PARALLEL_BUILDS}
		
		libOK=$?
		if [ $libOK -eq 0 ] ; then		
			cmake --install .
			libOK=$?
			if [ $libOK -ne 0 ] ; then		
				libOK=-4
			fi
		else
			libOK=-3
		fi
	else
	    libOK=-2
	fi
	

	cd ../../
	if [ $KEEP_BUILD_DIR == "n" ] ; then
		rm -rf build
	fi
	cd ${CURRENTDIR}
	
	libopenjpegOK=$libOK

fi


copy_sharedlibs ./libopenjpeg/bin/

C_INCLUDE_PATH="${C_INCLUDE_PATH}:${CURRENTDIR}/libopenjpeg/include/"
CPLUS_INCLUDE_PATH="${CPLUS_INCLUDE_PATH}:${CURRENTDIR}/libopenjpeg/include/"
LIBRARY_PATH="${LIBRARY_PATH}:${CURRENTDIR}/libopenjpeg/lib/"
OPENJPEG_LIB="${CURRENTDIR}/libopenjpeg/lib/libopenjp2.dll.a"
OPENJPEG_INC="${CURRENTDIR}/libopenjpeg/include/"
OPENJPEG_CMAKE="${CURRENTDIR}/libopenjpeg/lib/openjpeg-2.3/"
export C_INCLUDE_PATH
export CPLUS_INCLUDE_PATH
export LIBRARY_PATH
PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${CURRENTDIR}/libopenjpeg/lib/pkgconfig"
export PKG_CONFIG_PATH 





libpopplerOK=-1
read -p "Do you want to build 'poppler' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "\n------------------------------------------------------------------------\n"\
	"-- BUILDING: poppler                                                  --\n"\
	"------------------------------------------------------------------------\n\n"\

	cd poppler
	mkdir build3
	
	PKG_CONFIG_PATH=""
	export PKG_CONFIG_PATH 
	
	tar xvf poppler-0.85.0.tar.gz -C ./build3/
	cd build3/poppler-poppler-0.85.0
	
	cp -rf ../../FindNSS3.cmake.patch ./cmake/modules/FindNSS3.cmake
	
	mkdir build
	cd build
	cmake -G "MSYS Makefiles" -DCMAKE_PREFIX_PATH="${QTDIR}" -DOpenJPEG_DIR="${OPENJPEG_CMAKE}" -DFREETYPE_LIBRARY="${FREETYPE_LIB}" -DFREETYPE_INCLUDE_DIRS="${ZLIB_INC}" -DJPEG_LIBRARY="${JPEG_LIB}" -DJPEG_INCLUDE_DIR="${JPEG_INC}" -DZLIB_LIBRARY="${ZLIB_LIB}" -DZLIB_INCLUDE_DIR="${ZLIB_INC}" -DPNG_LIBRARY="${PNG_LIB}" -DPNG_PNG_INCLUDE_DIR="${PNG_INC}" -DTIFF_LIBRARY="${TIFF_LIB}" -DTIFF_INCLUDE_DIR="${TIFF_INC}" -DENABLE_CMS="none" -DENABLE_NSS3=OFF -DENABLE_GOBJECT_INTROSPECTION=OFF -DDISABLE_NSS3=ON -DENABLE_CPP=OFF -DENABLE_GLIB=OFF -DENABLE_SPLASH=ON -DBUILD_CPP_TESTS=OFF -DBUILD_QT5_TESTS=OFF -DBUILD_GTK_TESTS=OFF -DENABLE_LIBCURL=OFF -DCMAKE_PREFIX_PATH="${QTDIR}" -DCMAKE_INSTALL_PREFIX="${CURRENTDIR}/poppler" -DENABLE_LIBOPENJPEG=openjpeg2 -DENABLE_QT5=ON ..
	# -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON

	libOK=$?
	if [ $libOK -eq 0 ] ; then
		cmake --build . -j${MAKE_PARALLEL_BUILDS}
		
		libOK=$?
		if [ $libOK -eq 0 ] ; then		
			cmake --install .
			libOK=$?
			if [ $libOK -ne 0 ] ; then		
				libOK=-4
			fi
		else
			libOK=-3
		fi
	else
	    libOK=-2
	fi
	

	cd ../../
	if [ $KEEP_BUILD_DIR == "n" ] ; then
		rm -rf build3
	fi
	cd ${CURRENTDIR}
	
	libpopplerOK=$libOK

fi


copy_sharedlibs ./poppler/bin/
copy_exe ./poppler/bin/





echo -e  "\n------------------------------------------------------------------------\n"\
"-- BUILD RESULTS                                                       --\n"\
"------------------------------------------------------------------------\n\n"\

print_result "zlib" $zlibOK
print_result "freetype" $libfreetypeOK
print_result "libpng" $libpngOK
print_result "libjpeg" $libJPEGOK
print_result "libtiff" $libtiffOK
print_result "libopenjpeg" $libopenjpegOK
print_result "poppler" $libpopplerOK


