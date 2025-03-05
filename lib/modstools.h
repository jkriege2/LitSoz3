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

#ifndef MODSTOOLS_H
#define MODSTOOLS_H


#include <QString>
#include <QMap>
#include <QVariant>
#include "litsoz3tools_export.h"

/*! \brief parse a MODS+XML record

    \see <a href="http://www.loc.gov/standards/mods/mods-outline-3-5.html">http://www.loc.gov/standards/mods/mods-outline-3-5.html</a>

    example query for isbn: <a href="http://copac.ac.uk/search?isn=9783527403103&format=XML+-+MODS">http://copac.ac.uk/search?isn=9783527403103&format=XML+-+MODS</a>
*/
LITSOZ3TOOLS_EXPORT QMap<QString, QVariant> extractMODSXMLMetadata(const QString& data, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions, const QList<QString>& ands);

#endif // MODSTOOLS_H
