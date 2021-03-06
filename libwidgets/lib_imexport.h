/*
    Copyright (c) 2013-2015 Jan W. Krieger (<jan@jkrieger.de>)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License (GPL) as published by
    the Free Software Foundation, either version 3.0 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include <QtCore/QtGlobal>

#if defined(LS3LIBWIDGETS_LIBRARY)
#  define LS3LIBWIDGETS_EXPORT Q_DECL_EXPORT
#  define LIB_EXPORT Q_DECL_EXPORT
#  define QFLIB_EXPORT Q_DECL_EXPORT
#else
#  define LS3LIBWIDGETS_EXPORT Q_DECL_IMPORT
#  define LIB_EXPORT Q_DECL_IMPORT
#  define QFLIB_EXPORT Q_DECL_IMPORT
#endif


