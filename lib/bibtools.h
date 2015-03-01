#ifndef BIBTOOLS_H
#define BIBTOOLS_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QSet>
#include <QVariant>
#include "lib_imexport.h"
/** \brief clean a string of whitesaces, special characters ... */
LS3LIB_EXPORT QString cleanString(const QString& input, bool cleanStrongly=false);

/** \brief escape HTML character*/
LS3LIB_EXPORT QString escapeHTMLString(const QString& input);

/** \brief parse the given author string and return two list, one with the given and one with the family name */
LS3LIB_EXPORT void parseAuthors(const QString& authors, QStringList* givenNames, QStringList* familyNames);
/*! \brief parse a given authors firld (format Family, Given; Family, Given; ...) and output at max. \a maxAuthors names.
           if there are more, output \a etal

    \param authors the input
    \param maxAuthors maximum number of authors to output (-1 to output all)
    \param etal "et al." string to output if authors are more than maxAuthors
    \param shortGivenName shorten given names "Jan Wolfgang" -> "J. W."
    \param separator separator string between author names
    \param lastSeparator separator before last author
    \param nameSeparator Separator between family and given name. If empty then the order is <GivenName> <FamilyName>
    \param nameAddition add this string to the name (e.g. "(ed.)" )
    \return the reformated authors

 */
LS3LIB_EXPORT QString formatEtalAuthors(const QString& authors, int maxAuthors=3, QString etal=QString(" et al."), bool shortGivenName=true, QString separator=QString("; "), QString lastSeparator=QString(" & "), QString nameSeparator=QString(", "), QString nameAddition=QString(""));
/*! \brief same as formatEtalAuthors(), but does not output given names */
LS3LIB_EXPORT QString formatEtalAuthorsFamilyNames(const QString& authors, int maxAuthors=3, QString etal=QString(" et al."),QString separator=QString("; "), QString lastSeparator=QString(" & "), QString nameAddition=QString(""));


/** \brief converts an integer into a letter code 0: "", 1: a, 2: b, ..., 26: z, 27: aa, 28: ab, ... */
LS3LIB_EXPORT QString intToLetterCode(unsigned int n);

/** \brief shorten names  (e.g. "Jan Wolfgang" to "J. W.") */
LS3LIB_EXPORT QString shortenGivenName(const QString& names);

/** \brief reformats lists of authors to the default LitSoz format
 *
 *  \param auth the input string
 *  \param name_prefixes a prefix for the family name, like "van", "von", ...
 *  \param name_additions a set of string that contain common name additions in lowercase and without '.', ',', ...
 *  \param ands this set contains words that are recognized as "and"
 *  \param separators these separators are beeing identified
 *
 *  An example name_additions could be: \code name_additions<<"jr"<<"sen"<<"i"<<"ii"<<"iii"<<"jun"; \endcode
 *  An example name_prefixes could be: \code name_prefixes<<"da"<<"de"<<"dal"<<"del"<<"der"<<"di"<<"do"<<"du"<<"dos"<<"la"<<"le"<<"van"<<"vande"<<"von"<<"den" \endcode
 *  An example ands could be: \code ands<<"and"<<"und"<<"&"<<"et" \endcode
 */
LS3LIB_EXPORT QString reformatAuthors(const QString& auth, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions, const QList<QString>& ands, QString separators=QString(",;/\\\n\r"));

/*! \brief output the reference stored in database format in data, as summary with HTML markup

    \param data contains the current dataset as a QMap<QString, QVariant> The keys are the available database field names
                and the QVariant contains the value stored in the database
*/
LS3LIB_EXPORT QString formatReferenceSummary(const QMap<QString, QVariant>& data);

/*! \brief output the reference stored in database format in data, as summary with HTML markup in the form "Authors (Year): Title"

    \param data contains the current dataset as a QMap<QString, QVariant> The keys are the available database field names
                and the QVariant contains the value stored in the database
*/
LS3LIB_EXPORT QString formatShortReferenceSummary(const QMap<QString, QVariant>& data);

/*! \brief reformat a name without any separators (e.g. "Jan Wolfgang Krieger") to the standard format "Krieger, Jan Wolfgang"
 *
 *  \param auth the input string
 *  \param name_prefixes a prefix for the family name, like "van", "von", ...
 *  \param name_additions a set of string that contain common name additions in lowercase and without '.', ',', ...
 *
 *  An example name_additions could be: \code name_additions<<"jr"<<"sen"<<"i"<<"ii"<<"iii"<<"jun"; \endcode
 *  An example name_prefixes could be: \code name_prefixes<<"da"<<"de"<<"dal"<<"del"<<"der"<<"di"<<"do"<<"du"<<"dos"<<"la"<<"le"<<"van"<<"vande"<<"von"<<"den" \endcode
 */
LS3LIB_EXPORT QString reformatSingleName(const QString& auth, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions);

/** \brief return a path to an icon (PNG file) that represents the supplied filename */
LS3LIB_EXPORT QString filenameToIconFile(const QString& file);

/*! \brief search the supplied string for a value describing a year

    \return 0 if failed and the year else
*/
LS3LIB_EXPORT int extractYear(const QString& data);
/*! \brief search the supplied string for the first integer number

    \return 0 if failed and the year else
*/
LS3LIB_EXPORT int extractNumber(const QString& data);
/*! \brief add a substring to a field with the given separator (if the field does not yet contain the substring!

*/
LS3LIB_EXPORT void addToFieldIfNotContained(QMap<QString, QVariant>& data, const QString& field, const QString& newdata, const QString& separator="\n");
/*! \brief returns the standardized language name

*/
LS3LIB_EXPORT QString decodeLanguage(const QString& lang);

#define LS3_SET_IF_NOT_EMPTY(set_var, value) {\
    QString v=value; \
    if (!v.trimmed().simplified().isEmpty()) { \
        set_var = v; \
    } \
}

#endif // BIBTOOLS_H
