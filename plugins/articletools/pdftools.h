#ifndef PDFTOOLS_H
#define PDFTOOLS_H

#include <QtCore>
#include <QApplication>

/*! \brief extract XMP metadata from the given file
 */
QMap<QString, QString> extractFromPDF(const QString& filename, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes, QString pluginDir);



#endif // PDFTOOLS_H
