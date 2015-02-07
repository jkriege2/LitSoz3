#ifndef FILENAMETOOLS_H
#define FILENAMETOOLS_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QVariant>
#include "lib_imexport.h"

/*! \brief create a new filename, based on given record and old filename, as well as a scheme

    These templates in the scheme are replaced:
      * \c %original_name% - original filename
      * \c %extension%/%ext% - original file extension
      * \c %topic% - topic
      * \c %authors% - authors/editors (max 3 names + possibly etal)
      * \c %title% - title of the publication
      * \c %year% - year of publication
      * \c %publisher% - publisher
      * \c %places% - places
      * \c %journal% - journal
      * \c %number% - issue/number
      * \c %volume% - volume
      * \c %booktitle% - book title
      * \c %series% - series
      * \c %isbn% - ISBN
      * \c %pmid% - PubMedID
      * \c %doi% - DOI
      * \c %type% - type
      * \c %topic% - topic
      * \c %language% - language
      * \c %uuid% - uuid
    .
 */
LS3LIB_EXPORT QString createFileName(const QString& scheme, const QMap<QString, QVariant>& data, const QString& filename, int maxLength=200);

/*! \brief create a new file directory, based on given record and old directory name, as well as a scheme

    These templates in the scheme are replaced:
      * \c %original_name% - original filename
      * \c %topic% - topic
      * \c %authors% - authors/editors (max 3 names + possibly etal)
      * \c %title% - title of the publication
      * \c %year% - year of publication
      * \c %publisher% - publisher
      * \c %places% - places
      * \c %journal% - journal
      * \c %number% - issue/number
      * \c %volume% - volume
      * \c %booktitle% - book title
      * \c %series% - series
      * \c %isbn% - ISBN
      * \c %pmid% - PubMedID
      * \c %doi% - DOI
      * \c %type% - type
      * \c %topic% - topic
      * \c %language% - language
      * \c %uuid% - uuid
    .
 */
LS3LIB_EXPORT QString createFileDir(const QString& scheme, const QMap<QString, QVariant>& data, const QString& dirname, int maxLength=200);

/** \brief convert the given string to a string suitable for use in filenames */
LS3LIB_EXPORT QString cleanFilename(const QString& text, int maxLen=127, bool removeDot=true, bool removeSlash=true);

#endif // FILENAMETOOLS_H
