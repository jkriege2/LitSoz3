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

#ifndef PUBMEDTOOLS_H
#define PUBMEDTOOLS_H

#include <QString>
#include <QMap>
#include <QVariant>
#include "litsoz3tools_export.h"

/*! \brief parse a pubmed record

    \see <a href="http://www.ncbi.nlm.nih.gov/books/NBK25499/#chapter4.EFetch">http://www.ncbi.nlm.nih.gov/books/NBK25499/#chapter4.EFetch</a>
*/
LITSOZ3TOOLS_EXPORT QMap<QString, QVariant> extractPubmedMetadata(const QString& data);

/** \brief extract the list of PMIDs contained i an esearch XML answer */
LITSOZ3TOOLS_EXPORT QList<QString> extractPMIDList(const QString& data);

/** \brief extract the ID of the Pubmed Book in the data */
LITSOZ3TOOLS_EXPORT QString extractPubmedBookIDList(const QString& data);

/** \brief extract the ID of the Pubmed Book in the data */
LITSOZ3TOOLS_EXPORT void extractPubmedIDConvertIDs(QMap<QString, QVariant>& record, const QString& data);


#endif // PUBMEDTOOLS_H
