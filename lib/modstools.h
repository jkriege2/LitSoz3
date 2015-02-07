#ifndef MODSTOOLS_H
#define MODSTOOLS_H


#include <QString>
#include <QMap>
#include <QVariant>
#include "lib_imexport.h"

/*! \brief parse a MODS+XML record

    \see <a href="http://www.loc.gov/standards/mods/mods-outline-3-5.html">http://www.loc.gov/standards/mods/mods-outline-3-5.html</a>

    example query for isbn: <a href="http://copac.ac.uk/search?isn=9783527403103&format=XML+-+MODS">http://copac.ac.uk/search?isn=9783527403103&format=XML+-+MODS</a>
*/
LS3LIB_EXPORT QMap<QString, QVariant> extractMODSXMLMetadata(const QString& data, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions, const QList<QString>& ands);

#endif // MODSTOOLS_H
