#ifndef LATEXTOOLS_H
#define LATEXTOOLS_H
#include <QStringList>
#include <QString>
#include <QVector>
#include <QMap>
#include <QObject>
#include <iostream>
#include "lib_imexport.h"


/** \brief tries to remove all LaTeX markup from a string, also replaces known special characters */
LS3LIB_EXPORT QString removeLaTeX(const QString& input);
/** \brief LaTeX-ifies the \a input and replaces all known special characters by LaTeX markup */
LS3LIB_EXPORT QString laTeXify(const QString& input, bool brackedDoubleUppercase=false);

/*! \brief a parser for BiBTeX files
  */
class LS3LIB_EXPORT BiBTeXParser {
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
