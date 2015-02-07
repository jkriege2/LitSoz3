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
