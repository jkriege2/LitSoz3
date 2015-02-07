#ifndef XMPTOOLS_H
#define XMPTOOLS_H

#include <QString>
#include <QMap>
#include <QList>
#include <QSet>
#include "bibtools.h"


/*! \brief extract XMP metadata from the given file
 */
QMap<QString, QString> extractXMP(const QString& filename, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes);

#endif // XMPTOOLS_H
