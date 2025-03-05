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

#ifndef REGEXTOOLS_H
#define REGEXTOOLS_H

#include "litsoz3tools_export.h"
#include <QString>
#include <QDebug>
#include <QElapsedTimer>
#include <QRegularExpression>

/** \brief retrieves a regular expression from an internal cache, thread-safe */
LITSOZ3TOOLS_EXPORT std::shared_ptr<QRegularExpression> ls3_getRegExFromCache(const QString& pattern, QRegularExpression::PatternOptions options=QRegularExpression::NoPatternOption, bool wildcard=false);

/** \brief returns \c true, if \a text contains a match to the given regular expression \a regex,
 *         starts from \a offset and optionally returns the match in \a caps \c =[fullmatch, cap1,cap2,...]
 *  \ingroup jkqtptools_string
 *
 *  \note this function uses an internal cache, so the \a regex does not have
 *        to be compiled every time this is called (with the same \a regex ).
 *
 *  \see ls3_rxExactlyMatches(), ls3_rxIndexIn(), ls3_rxContains(), ls3_rxPartiallyMatchesAt()
 */
LITSOZ3TOOLS_EXPORT bool ls3_rxContains(const QString& text, const QString &regex, size_t offset=0, QStringList* caps=nullptr);

/** \brief returns the next match (i.e. its index) of the given regular expression \a regex within \a text,
 *         starts from \a offset and optionally returns the match in \a caps \c =[fullmatch, cap1,cap2,...]
 *  \ingroup jkqtptools_string
 *
 *  \note this function uses an internal cache, so the \a regex does not have
 *        to be compiled every time this is called (with the same \a regex ).
 *
 *  \see ls3_rxExactlyMatches(), ls3_rxIndexIn(), ls3_rxContains(), ls3_rxPartiallyMatchesAt()
 */
LITSOZ3TOOLS_EXPORT qsizetype ls3_rxIndexIn(const QString& text, const QString &regex, size_t offset=0, QStringList* caps=nullptr);

/** \brief returns \c true, if \a text exactly matches the given regular expression \a regex,
 *         starts from \a offset and optionally returns the match in \a caps \c =[fullmatch, cap1,cap2,...]
 *  \ingroup jkqtptools_string
 *
 *  \note this function uses an internal cache, so the \a regex does not have
 *        to be compiled every time this is called (with the same \a regex ).
 *
 *  \see ls3_rxExactlyMatches(), ls3_rxIndexIn(), ls3_rxContains(), ls3_rxPartiallyMatchesAt()
 */
LITSOZ3TOOLS_EXPORT bool ls3_rxExactlyMatches(const QString& text, const QString &regex, QStringList* caps=nullptr);


/** \brief returns \c true, if \a text partially matches the given regular expression \a regex,
 *         starting from \a offset (and the match starts at \a offset !!!)
 *         and optionally returns the match in \a caps \c =[fullmatch, cap1,cap2,...]
 *  \ingroup jkqtptools_string
 *
 *  \note this function uses an internal cache, so the \a regex does not have
 *        to be compiled every time this is called (with the same \a regex ).
 *
 *  \see ls3_rxExactlyMatches(), ls3_rxIndexIn(), ls3_rxContains(), ls3_rxPartiallyMatchesAt()
 */
LITSOZ3TOOLS_EXPORT bool ls3_rxPartiallyMatchesAt(const QString& text, const QString &regex, size_t offset=0, QStringList* caps=nullptr);

#endif // REGEXTOOLS_H
