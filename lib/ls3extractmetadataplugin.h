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

#ifndef LS3EXTRACTMETADATAPLUGIN_H
#define LS3EXTRACTMETADATAPLUGIN_H

#include <QString>
#include <QMap>
#include <QVariant>
#include "lib_imexport.h"

class LS3LIB_EXPORT LS3ExtractMetadataPlugin {
    public:
        /** \brief retrieve metadata from a given DOI (waits until requests are complete */
        virtual QMap<QString, QVariant> getMetadataFromDoi(QString DOI)=0;
};



Q_DECLARE_INTERFACE(LS3ExtractMetadataPlugin,
                     "jkrieger.de.litsoz3.plugins.LS3ExtractMetadataPlugin/1.0")

#endif // LS3EXTRACTMETADATAPLUGIN_H
