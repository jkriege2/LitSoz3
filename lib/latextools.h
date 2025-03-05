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

#ifndef LATEXTOOLS_H
#define LATEXTOOLS_H
#include <QStringList>
#include <QString>
#include <QVector>
#include <QMap>
#include <QObject>
#include <iostream>
#include "litsoz3tools_export.h"


/** \brief tries to remove all LaTeX markup from a string, also replaces known special characters */
LITSOZ3TOOLS_EXPORT QString removeLaTeX(const QString& input);
/** \brief LaTeX-ifies the \a input and replaces all known special characters by LaTeX markup */
LITSOZ3TOOLS_EXPORT QString laTeXify(const QString& input, bool brackedDoubleUppercase=false);

/*! \brief a parser for BiBTeX files
  */
class LITSOZ3TOOLS_EXPORT BiBTeXParser {
    public:
        struct Record {
            QString type;
            QString ID;
            QMap<QString, QString> fields;
        };


        BiBTeXParser(const QString& input=QString(""));

        bool parse(const QString& input, QString* errorMessage=NULL);

        int getRecordCount() { return records.size(); }
        Record getRecord(int i) { return records[i]; }


    protected:
        QString input;
        qint32 pos;
        qint32 line;
        QString lastError;
        bool errorOccured;

        QMap<QString, QString> strings;

        QVector<Record> records;

        bool findRecord(Record* record);
        void parseProperty(Record* record, bool getFull, QString name=QString(""));

        QString replaceStrings(const QString& data);

        enum TokenType {
            ttAt,
            ttEOF,
            ttUnknown,
            ttName,
            ttOpenBrace,
            ttCloseBrace,
            ttEquals,
            ttString,
            ttComma
        };

        struct Token {
            TokenType type;
            QString value;
        };

        TokenType getNextToken();

        Token currentToken;
};

#endif // LATEXTOOLS_H
