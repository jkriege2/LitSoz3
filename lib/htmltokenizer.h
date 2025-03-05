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

#ifndef HTMLTOKENIZER_H
#define HTMLTOKENIZER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QList>
#include <QPair>
#include "litsoz3tools_export.h"



/*! \brief a simple TML tokenizer

    This class takes a string representing an HTML document or a part thereof and splits it up into tokens,
    stored as HtmlTokenizer::Token
*/
class LITSOZ3TOOLS_EXPORT HtmlTokenizer : public QObject {
        Q_OBJECT
    public:

        enum TokenType {
            ENDOFFILE=0,
            TEXT=1,
            //WHITESPACE=2,
            TAG=3,
            ENDTAG=4,
            SINGLETAG=5,
            COMMENT=6,
            PROCESSINGINSTRUCTION=7,
            DOCTYPE=8,
            UNKNOWN=9
        };

        struct LITSOZ3TOOLS_EXPORT Token {
            TokenType type;
            /** \brief in all kinds of tags: everything without the surrounding delimiters */
            QString contents;
            /** \brief the complete contents of the element */
            QString completeContents;
            /** \biref the name of the tag */
            QString tagName;
            /** \brief indicates whether parse() has been called */
            bool isParsed;

            /** \brief contains a list of all properties in the tag */
            QList<QPair<QString, QString> > properties;

            /** \briefparse the tag contents and fill properties */
            void parse();

            /** \brief returns the value of the given property (ignores case and returns \c "" if property does not exist!) */
            QString getProperty(QString pName);

            Token();
        };

        explicit HtmlTokenizer(const QString& html, QObject *parent = 0);


        Token getNextToken();
    protected:
        QString html;
        int pos;
};

LITSOZ3TOOLS_EXPORT QDebug operator<<(QDebug dbg, const HtmlTokenizer::TokenType &c);

#endif // HTMLTOKENIZER_H
