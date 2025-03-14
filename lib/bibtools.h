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

#ifndef BIBTOOLS_H
#define BIBTOOLS_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QSet>
#include <QVariant>
#include "litsoz3tools_export.h"
/** \brief clean a string of whitesaces, special characters ... */
LITSOZ3TOOLS_EXPORT QString cleanStringForFilename(const QString& input, bool cleanStrongly=false, bool removeSlashBackslash=true);

/** \brief escape HTML character*/
LITSOZ3TOOLS_EXPORT QString escapeHTMLString(const QString& input);

/** \brief parse the given author string and return two list, one with the given and one with the family name */
LITSOZ3TOOLS_EXPORT void parseAuthors(const QString& authors, QStringList* givenNames, QStringList* familyNames);
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
LITSOZ3TOOLS_EXPORT QString formatEtalAuthors(const QString& authors, int maxAuthors=3, QString etal=QString(" et al."), bool shortGivenName=true, QString separator=QString("; "), QString lastSeparator=QString(" & "), QString nameSeparator=QString(", "), QString nameAddition=QString(""));
/*! \brief same as formatEtalAuthors(), but does not output given names */
LITSOZ3TOOLS_EXPORT QString formatEtalAuthorsFamilyNames(const QString& authors, int maxAuthors=3, QString etal=QString(" et al."),QString separator=QString("; "), QString lastSeparator=QString(" & "), QString nameAddition=QString(""));


/** \brief converts an integer into a letter code 0: "", 1: a, 2: b, ..., 26: z, 27: aa, 28: ab, ... */
LITSOZ3TOOLS_EXPORT QString intToLetterCode(unsigned int n);

/** \brief shorten names  (e.g. "Jan Wolfgang" to "J. W.") */
LITSOZ3TOOLS_EXPORT QString shortenGivenName(const QString& names, const QString& shortenWith=QString(". "));

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
LITSOZ3TOOLS_EXPORT QString reformatAuthors(const QString& auth, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions, const QList<QString>& ands, QString separators=QString(",;/\\\n\r"));

/*! \brief output the reference stored in database format in data, as summary with HTML markup

    \param data contains the current dataset as a QMap<QString, QVariant> The keys are the available database field names
                and the QVariant contains the value stored in the database
*/
LITSOZ3TOOLS_EXPORT QString formatReferenceSummary(const QMap<QString, QVariant>& data);

/*! \brief output the reference stored in database format in data, as summary with HTML markup in the form "Authors (Year): Title"

    \param data contains the current dataset as a QMap<QString, QVariant> The keys are the available database field names
                and the QVariant contains the value stored in the database
*/
LITSOZ3TOOLS_EXPORT QString formatShortReferenceSummary(const QMap<QString, QVariant>& data);

/*! \brief reformat a name without any separators (e.g. "Jan Wolfgang Krieger") to the standard format "Krieger, Jan Wolfgang"
 *
 *  \param auth the input string
 *  \param name_prefixes a prefix for the family name, like "van", "von", ...
 *  \param name_additions a set of string that contain common name additions in lowercase and without '.', ',', ...
 *
 *  An example name_additions could be: \code name_additions<<"jr"<<"sen"<<"i"<<"ii"<<"iii"<<"jun"; \endcode
 *  An example name_prefixes could be: \code name_prefixes<<"da"<<"de"<<"dal"<<"del"<<"der"<<"di"<<"do"<<"du"<<"dos"<<"la"<<"le"<<"van"<<"vande"<<"von"<<"den" \endcode
 */
LITSOZ3TOOLS_EXPORT QString reformatSingleName(const QString& auth, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions);

/** \brief return a path to an icon (PNG file) that represents the supplied filename */
LITSOZ3TOOLS_EXPORT QString filenameToIconFile(const QString& file);

/*! \brief search the supplied string for a value describing a year

    \return 0 if failed and the year else
*/
LITSOZ3TOOLS_EXPORT int extractYear(const QString& data);


/*! \brief clean stray characters from text

*/
LITSOZ3TOOLS_EXPORT QString cleanText(const QString& data, bool simplify=true, const QString& specialChars=QString::fromUtf8(",;-#+~*&%$\u00A7/\\"));

/*! \brief search the supplied string for the first integer number

    \return 0 if failed and the year else
*/
LITSOZ3TOOLS_EXPORT int extractNumber(const QString& data);
/*! \brief add a substring to a field with the given separator (if the field does not yet contain the substring!

*/
LITSOZ3TOOLS_EXPORT void addToFieldIfNotContained(QMap<QString, QVariant>& data, const QString& field, const QString& newdata, const QString& separator="\n");
/*! \brief returns the standardized language name

*/
LITSOZ3TOOLS_EXPORT QString decodeLanguage(const QString& lang);

/*! \brief counts the upper-case and the lower-case letters in a string

*/
LITSOZ3TOOLS_EXPORT void countLetterCase(const QString& string, int& upperCase, int& lowerCase);


#define LS3_SET_IF_NOT_EMPTY(set_var, value) {\
    QString v=value; \
    if (!v.trimmed().simplified().isEmpty()) { \
        set_var = v; \
    } \
}

inline uint qHash(const QStringList& v) {
  return qHash(v.join(""));
}

inline bool isEqual(QString s1, QString s2, bool ignorecase=false, bool trimm=false, bool simplify=false) {
    if (simplify) {
        s1=s1.simplified();
        s2=s2.simplified();
    }
    if (trimm) {
        s1=s1.trimmed();
        s2=s2.trimmed();
    }
    if (ignorecase) {
        return s1.toLower()==s2.toLower();
    } else {
        return s1==s2;
    }
}


inline bool startsWith(QString s1, QString start, bool ignorecase=false, bool trimm=false, bool simplify=false) {
    if (simplify) {
        s1=s1.simplified();
        start=start.simplified();
    }
    if (trimm) {
        s1=s1.trimmed();
        start=start.trimmed();
    }
    return s1.startsWith(start, ignorecase?Qt::CaseInsensitive:Qt::CaseSensitive);
}


inline QString replaceStart(QString s1, QString start, QString newstart, bool ignorecase=false, bool trimm=false, bool simplify=false) {
    if (simplify) {
        s1=s1.simplified();
        start=start.simplified();
    }
    if (trimm) {
        s1=s1.trimmed();
        start=start.trimmed();
    }
    if (s1.startsWith(start, ignorecase?Qt::CaseInsensitive:Qt::CaseSensitive)) {
        return newstart+s1.right(s1.size()-start.size());
    } else {
        return s1;
    }
}

void removeDuplicatesCaseInseitive(QStringList& sl);

#endif // BIBTOOLS_H
