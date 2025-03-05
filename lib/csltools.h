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

#ifndef CSLTOOLS_H
#define CSLTOOLS_H
#include <QDate>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QSet>
#include <QVariant>
#include "litsoz3tools_export.h"
#include <QtXml/QDomElement>
#include <QLocale>
#include <QRegularExpression>
#include <QDebug>

enum CSLOutputFormat {
    ofPlaintext,
    ofHTML
};

enum CSLFontStyle {
    fsNormal=0,
    fsItalic,
    fsOblique=fsItalic
};

enum CSLFontVariant {
    fvNormal=0,
    fvSmallCaps
};

enum CSLFontWeight {
    fwNormal=0,
    fwBold,
    fwLight=fwNormal
};

enum CSLTextDecoration {
    tdNone=0,
    tdUnderline
};

enum CSLVerticalAlign {
    vaBaseline=0,
    vaSuper,
    vaSub
};

enum CSLTextCase {
    tcNormal,
    tcLowercase,
    tcUppercase,
    tcCapitalizeFirst,
    tcCapitalizeAll,
    tcSentence,
    tcTitle
};

enum CSLTextDisplay {
    tdiBlock,
    tdiIndent,
    tdiRight=tdiIndent
};

LITSOZ3TOOLS_EXPORT QString applyTextCase(const QString& text, CSLTextCase tcase);

struct LITSOZ3TOOLS_EXPORT CSLFormatState {
    CSLFormatState() {
        fstyle=fsNormal;
        fvariant=fvNormal;
        fweight=fwNormal;
        tdecor=tdNone;
        valign=vaBaseline;
        tdisplay=tdiBlock;
    }

    QString startFormat(const CSLFormatState& lastFormat, CSLOutputFormat outf) const;
    QString endFormat(const CSLFormatState& newFormat, CSLOutputFormat outf) const ;

    CSLFontStyle fstyle;
    CSLFontVariant fvariant;
    CSLFontWeight fweight;
    CSLTextDecoration tdecor;
    CSLVerticalAlign valign;
    CSLTextDisplay tdisplay;
};

class CSLBasicProps {
    public:
        CSLBasicProps();
        virtual ~CSLBasicProps() {};
        void modifyStyle(CSLFormatState& style);
        QString applyTextCase(const QString& text);
        virtual void parseBasicProperties(const QDomElement& e);
    protected:

        QString suffix;
        QString prefix;
        QString delimiter;


        CSLFontStyle fstyle;
        CSLFontVariant fvariant;
        CSLFontWeight fweight;
        CSLTextDecoration tdecor;
        CSLVerticalAlign valign;
        CSLTextCase tcase;
        CSLTextDisplay tdisplay;



        bool set_fstyle;
        bool set_fvariant;
        bool set_fweight;
        bool set_tdecor;
        bool set_valign;
        bool set_tdisplay;
};

/** \brief read the metadata from a CSL file */
LITSOZ3TOOLS_EXPORT bool cslReadMetadata(const QString& filename, QString* name=NULL, QString* dependentFilename=NULL, bool* isDependent=NULL);
class LITSOZ3TOOLS_EXPORT CSLLocaleInterface {
    public:
        struct CSLLocaleValue {
            inline QString single() const { return m_single; }
            inline QString multiple() const { if (m_multiple.isEmpty()) return single(); else return m_multiple; }
            inline QString get(bool multipl=false) const {
                if (multipl) return multiple();
                return single();
            }

            QString m_single;
            QString m_multiple;
        };

        virtual CSLLocaleValue term(const QString& name, const QString& form=QString()) const=0;
        virtual QString term(const QString& name, bool single, const QString& form=QString()) const=0;
        virtual bool hasTerm(const QString& name) const=0;

        virtual ~CSLLocaleInterface() {}

};

class LITSOZ3TOOLS_EXPORT CSLLocale: public CSLLocaleInterface {
    public:

        class CSLDateFormat: public CSLBasicProps {
            public:
                virtual ~CSLDateFormat() {};
                CSLDateFormat(const QString& name=QString());
                QString name;
                QString prefix;
                bool hasPrefix;
                QString suffix;
                bool hasSuffix;
                QString range_delimiter;
                bool hasRange_delimiter;
                QString form;
                bool hasForm;
                void takeDataFrom(CSLDateFormat& other) {
                    if (other.hasForm) this->form=other.form;
                    if (other.hasSuffix) this->suffix=other.suffix;
                    if (other.hasPrefix) this->prefix=other.prefix;
                    if (other.hasRange_delimiter) this->range_delimiter=other.range_delimiter;
                }
        };



        explicit CSLLocale(const QString& fn);
        ~CSLLocale();
        inline QString name() const { return m_name; }
        static void parseLocale(QMap<QString, QMap<QString, CSLLocaleValue> >& m_terms, const QDomElement& e);
        static void parseDate(QList<CSLDateFormat>& m_terms, const QDomElement& e);
        virtual inline CSLLocaleValue term(const QString& name, const QString& form=QString()) const {
            //qDebug()<<"CSLLocale::form("<<name<<", "<<form<<") "<<m_terms.keys();
            if (m_terms.contains(name)) {
                //qDebug()<<"CSLLocale::form("<<name<<", "<<form<<"): "<<m_terms[name].keys();
                if (m_terms[name].contains(form)) return  m_terms[name].value(form);
                else return  m_terms[name].value("", CSLLocaleValue());
            }
            return CSLLocaleValue();
        }
        virtual inline QString term(const QString& name, bool single, const QString& form=QString()) const {
            if (single) return term(name, form).single();
            else return term(name, form).multiple();
        }
        virtual inline bool hasTerm(const QString& name) const {
            return m_terms.contains(name);
        }

        QString formatDate(QDate date, const QString& form=QString("text"));
        static QString formatDate(CSLLocaleInterface* localeIntf, const QLocale& locale, const QList<CSLDateFormat>& m_dates, QDate date, const QString& form=QString("text"), bool yearOnly=false);
        QString formatDate(CSLLocaleInterface* localeIntf, QDate date, const QString& form=QString("text"));
        inline QLocale locale() const { return m_locale; }
        inline QList<CSLDateFormat> dateParts(const QString& form=QString("text")) const {
            QList<CSLDateFormat> l;
            if (m_dates.contains(form)) l=m_dates[form];
            return l;
        }

    protected:
        QMap<QString, QMap<QString, CSLLocaleValue> > m_terms;
        QMap<QString, QList<CSLDateFormat> > m_dates;
        QLocale m_locale;
        QString m_name;

};

class LITSOZ3TOOLS_EXPORT CSLFile: public CSLLocaleInterface {
    public:


        explicit CSLFile(const QString& fn);

        ~CSLFile();

        inline bool isValid() const { return valid; }
        inline QString name() const { return m_name; }

        QString produce(const QMap<QString, QVariant> &data, bool citation=false, CSLOutputFormat outf=ofHTML);
        inline CSLLocale* cslLocale() const { return m_locale; }
        virtual inline CSLLocale::CSLLocaleValue term(const QString& name, const QString& form=QString()) const {
            CSLLocale::CSLLocaleValue res;
            if (m_terms.contains(name)) {
                if (m_terms[name].contains(form)) res= m_terms[name].value(form, CSLLocale::CSLLocaleValue());
                else res=  m_terms[name].value("", CSLLocale::CSLLocaleValue());
            } else {
                if (m_locale) {
                    if (form=="") res=m_locale->term(name, QString("long"));
                    else res=m_locale->term(name, form);
                }
            }
            //qDebug()<<"term("<<name<<form<<") = "<<res.m_single<<res.m_multiple;
            return res;
        }
        virtual inline QString term(const QString& name, bool single, const QString& form=QString()) const {
            if (single) return term(name, form).single();
            else return term(name, form).multiple();
        }

        inline virtual bool hasTerm(const QString& name) const {
            return m_terms.contains(name) || (m_locale && m_locale->hasTerm(name));
        }

        void setLocale(CSLLocale* locale);
        QString formatDate(QDate date, const QString& form=QString("text"));

    protected:


        class LITSOZ3TOOLS_EXPORT CSLNode: public CSLBasicProps {
            public:                
                explicit CSLNode(CSLNode* parent,CSLFile* file);
                virtual ~CSLNode();
                inline CSLNode* parent() const { return m_parent; }
                virtual QString produce(const QMap<QString, QVariant> &data, const CSLFormatState &currentFormat, CSLOutputFormat outf);
                static QVariant getCSLField(const QString& field, const QMap<QString, QVariant> &data, const QVariant &defaultVal=QVariant());
                virtual void parseProperties(const QDomElement& e);
                inline void setParent(CSLNode* p) { m_parent=p; }
                static QString escapeString(CSLOutputFormat outf, const QString& text);
            protected:
                CSLNode* m_parent;
                CSLFile* m_file;
        };

        class LITSOZ3TOOLS_EXPORT CSLListNode: public CSLNode {
            public:
                explicit CSLListNode(const QString& tagName, CSLNode* parent, CSLFile* file);
                virtual ~CSLListNode();
                virtual QString produce(const QMap<QString, QVariant> &data, const CSLFormatState& currentFormat, CSLOutputFormat outf);
                void addChild(CSLNode* c);
                void clearChildren();
            protected:
                QList<CSLNode*> m_children;
                QString tagName;
        };

        class LITSOZ3TOOLS_EXPORT CSLTextNode: public CSLNode {
            public:
                explicit CSLTextNode(CSLNode* parent, CSLFile* file);
                virtual ~CSLTextNode();
                virtual QString produce(const QMap<QString, QVariant> &data, const CSLFormatState& currentFormat, CSLOutputFormat outf);
                virtual void parseProperties(const QDomElement& e);
            protected:
                QString macro;
                QString variable;
                QString term;
                QString value;
                QString form;
                bool plural;
                bool quote;
                bool stripPeriods;
        };

        class LITSOZ3TOOLS_EXPORT CSLNumberNode: public CSLNode {
            public:
                explicit CSLNumberNode(CSLNode* parent, CSLFile* file);
                virtual ~CSLNumberNode();
                virtual QString produce(const QMap<QString, QVariant> &data, const CSLFormatState& currentFormat, CSLOutputFormat outf);
                virtual void parseProperties(const QDomElement& e);
            protected:
                QString variable;
                QString form;
        };

        class LITSOZ3TOOLS_EXPORT CSLLabelNode: public CSLNode {
            public:
                explicit CSLLabelNode(CSLNode* parent, CSLFile* file);
                virtual ~CSLLabelNode();
                virtual QString produce(const QMap<QString, QVariant> &data, const CSLFormatState& currentFormat, CSLOutputFormat outf);
                virtual void parseProperties(const QDomElement& e);
                inline void setVariable(const QString variable) {
                    this->variable=variable;
                }

            protected:
                QString variable;
                QString form;
                QString plural;
                bool stripPeriods;
        };

        class LITSOZ3TOOLS_EXPORT CSLDateNode: public CSLNode {
            public:
                explicit CSLDateNode(CSLNode* parent, CSLFile* file);
                virtual ~CSLDateNode();
                virtual QString produce(const QMap<QString, QVariant> &data, const CSLFormatState& currentFormat, CSLOutputFormat outf);
                virtual void parseProperties(const QDomElement& e);

            protected:
                QString variable;
                QString form;
                QList<CSLLocale::CSLDateFormat> dateParts;
        };

        class LITSOZ3TOOLS_EXPORT CSLNamesNode: public CSLNode {
            public:
                explicit CSLNamesNode(CSLNode* parent, CSLFile* file);
                virtual ~CSLNamesNode();
                virtual QString produce(const QMap<QString, QVariant> &data, const CSLFormatState& currentFormat, CSLOutputFormat outf);
                virtual void parseProperties(const QDomElement& e);
            protected:
                void parseNameProperties(const QDomElement& e);
                QStringList variables;
                enum NameAndSeparator {
                    nasAndTerm,
                    nasSymbol
                };
                NameAndSeparator andSeparator;
                enum DelimiterPrecedesEnum {
                    dpeaContextual,
                    dpeaAfterInverseName,
                    dpeaAlways,
                    dpeaNever
                };

                DelimiterPrecedesEnum delimiterPrecedesEtAl;
                DelimiterPrecedesEnum delimiterPrecedesLast;
                int etalMin;
                int etaluseFirst;
                bool etalUseLast;
                QString form;
                CSLBasicProps nameProps;
                CSLBasicProps familyNameProps;
                CSLBasicProps givenNameProps;
                CSLBasicProps etalProps;
                QString etal;
                bool initialize;
                QString initializeWith;
                QString nameAsSortOrder;
                QString sortSeparator;
                QList<CSLNode*> m_substitutes;
                CSLLabelNode* m_labelNode;

        };

        class LITSOZ3TOOLS_EXPORT CSLChooseNode: public CSLNode {
            public:
                explicit CSLChooseNode(CSLNode* parent, CSLFile* file);
                virtual ~CSLChooseNode();
                virtual QString produce(const QMap<QString, QVariant> &data, const CSLFormatState& currentFormat, CSLOutputFormat outf);
                virtual void parseProperties(const QDomElement& e);
            protected:
                enum IFType {
                    iftIsNumeric,
                    iftType,
                    iftVariable,
                    iftElse,
                    iftUncertainDate,
                    iftNever
                };

                struct ifData {
                    IFType ifType;
                    QString variable;
                    QString match;
                    QString type;
                    CSLListNode* node;
                };
                QList<ifData> ifs;
        };

        bool parse(const QString& filename);
        bool parseMacro(const QDomElement& e);
        CSLNode *parseElement(const QDomElement& e, CSLNode* parent=NULL);
        QString filename;
        QString m_name;
        bool valid;
        bool parsed;

        CSLNode* m_bibliography;
        CSLNode* m_citation;
        QMap<QString, CSLNode*> m_macros;
        QMap<QString, QMap<QString, CSLLocale::CSLLocaleValue> > m_terms;
        CSLLocale* m_locale;
};

#endif // CSLTOOLS_H
