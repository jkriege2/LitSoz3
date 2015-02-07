#ifndef PUBMEDTOOLS_H
#define PUBMEDTOOLS_H

#include <QString>
#include <QMap>
#include <QVariant>
#include "lib_imexport.h"

/*! \brief parse a pubmed record

    \see <a href="http://www.ncbi.nlm.nih.gov/books/NBK25499/#chapter4.EFetch">http://www.ncbi.nlm.nih.gov/books/NBK25499/#chapter4.EFetch</a>
*/
LS3LIB_EXPORT QMap<QString, QVariant> extractPubmedMetadata(const QString& data);

/** \brief extract the list of PMIDs contained i an esearch XML answer */
LS3LIB_EXPORT QList<QString> extractPMIDList(const QString& data);

/** \brief extract the ID of the Pubmed Book in the data */
LS3LIB_EXPORT QString extractPubmedBookIDList(const QString& data);

/** \brief extract the ID of the Pubmed Book in the data */
LS3LIB_EXPORT void extractPubmedIDConvertIDs(QMap<QString, QVariant>& record, const QString& data);


#endif // PUBMEDTOOLS_H
