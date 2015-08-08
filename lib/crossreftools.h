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

#ifndef CROSSREFTOOLS_H
#define CROSSREFTOOLS_H

#include <QString>
#include <QMap>
#include <QVariant>
#include "lib_imexport.h"

/*! \brief parse a unixref metadata string

    \see <a href="http://labs.crossref.org/site/quick_and_dirty_api_guide.html">http://labs.crossref.org/site/quick_and_dirty_api_guide.html</a>
    \see <a href="http://www.crossref.org/schema/documentation/unixref1.1/unixref1.1.html">http://www.crossref.org/schema/documentation/unixref1.1/unixref1.1.html</a>
*/
LS3LIB_EXPORT QMap<QString, QVariant> extractCrossrefUnixrefMetadata(const QString& data);

#endif // CROSSREFTOOLS_H
