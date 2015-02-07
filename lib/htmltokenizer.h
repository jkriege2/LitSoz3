#ifndef HTMLTOKENIZER_H
#define HTMLTOKENIZER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QList>
#include <QPair>
#include "lib_imexport.h"



/*! \brief a simple TML tokenizer

    This class takes a string representing an HTML document or a part thereof and splits it up into tokens,
    stored as HtmlTokenizer::Token
*/
class LS3LIB_EXPORT HtmlTokenizer : public QObject {
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

        struct LS3LIB_EXPORT Token {
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

LS3LIB_EXPORT QDebug operator<<(QDebug dbg, const HtmlTokenizer::TokenType &c);

#endif // HTMLTOKENIZER_H
