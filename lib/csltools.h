#ifndef CSLTOOLS_H
#define CSLTOOLS_H
#include <QDate>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QSet>
#include <QVariant>
#include "lib_imexport.h"
#include <QDomDocument>
#include <QDomElement>
#include <QLocale>
#include <QRegExp>

/** \brief read the metadata from a CSL file */
LS3LIB_EXPORT bool cslReadMetadata(const QString& filename, QString* name=NULL);
class LS3LIB_EXPORT CSLLocaleInterface {
    public:
        struct CSLLocaleValue {
            inline QString single() { return m_single; }
            inline QString multiple() { if (m_multiple.isEmpty()) return single(); else return m_multiple; }
            inline QString get(bool multipl=false) {
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

class LS3LIB_EXPORT CSLLocale: public CSLLocaleInterface {
    public:

        struct CSLDateFormat {
            QString name;
            QString prefix;
            QString suffix;
            QString range_delimiter;
            QString form;
        };

        explicit CSLLocale(const QString& fn);
        ~CSLLocale();
        inline QString name() const { return m_name; }
        static void parseLocale(QMap<QString, QMap<QString, CSLLocaleValue> >& m_terms, const QDomElement& e);
        static void parseDate(QList<CSLDateFormat>& m_terms, const QDomElement& e);
        virtual inline CSLLocaleValue term(const QString& name, const QString& form=QString()) const {
            if (m_terms.contains(name)) {
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
        static QString formatDate(CSLLocaleInterface* localeIntf, const QLocale& locale, const QList<CSLDateFormat>& m_dates, QDate date, const QString& form=QString("text"));
        QString formatDate(CSLLocaleInterface* localeIntf, QDate date, const QString& form=QString("text"));
        inline QLocale locale() const { return m_locale; }
    protected:
        QMap<QString, QMap<QString, CSLLocaleValue> > m_terms;
        QMap<QString, QList<CSLDateFormat> > m_dates;
        QLocale m_locale;
        QString m_name;

};

class LS3LIB_EXPORT CSLFile: public CSLLocaleInterface {
    public:
        explicit CSLFile(const QString& fn);

        ~CSLFile();

        inline bool isValid() const { return valid; }
        inline QString name() const { return m_name; }

        QString produceHTML(const QMap<QString, QVariant> &data, bool citation=false);

        virtual inline CSLLocale::CSLLocaleValue term(const QString& name, const QString& form=QString()) const {
            if (m_terms.contains(name)) {
                if (m_terms[name].contains(form)) return  m_terms[name].value(form, CSLLocale::CSLLocaleValue());
                else return  m_terms[name].value("", CSLLocale::CSLLocaleValue());
            } else {
                if (m_locale) return m_locale->term(name, form);
            }
            return CSLLocale::CSLLocaleValue();
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
        enum FontStyle {
            fsNormal=0,
            fsItalic,
            fsOblique=fsItalic
        };

        enum FontVariant {
            fvNormal=0,
            fvSmallCaps
        };

        enum FontWeight {
            fwNormal=0,
            fwBold,
            fwLight=fwNormal
        };

        enum TextDecoration {
            tdNone=0,
            tdUnderline
        };

        enum VerticalAlign {
            vaBaseline=0,
            vaSuper,
            vaSub
        };

        enum TextCase {
            tcNormal,
            tcLowercase,
            tcUppercase,
            tcCapitalizeFirst,
            tcCapitalizeAll,
            tcSentence,
            tcTitle
        };

        static QString applyTextCase(const QString& text, TextCase tcase);

        struct CSLFormatState {
            CSLFormatState() {
                fstyle=fsNormal;
                fvariant=fvNormal;
                fweight=fwNormal;
                tdecor=tdNone;
                valign=vaBaseline;
            }

            QString startFormatHTML(const CSLFormatState& lastFormat) const;
            QString endFormatHTML(const CSLFormatState& newFormat) const ;

            FontStyle fstyle;
            FontVariant fvariant;
            FontWeight fweight;
            TextDecoration tdecor;
            VerticalAlign valign;
        };

        class LS3LIB_EXPORT CSLNode {
            public:
                explicit CSLNode(CSLNode* parent,CSLFile* file);
                virtual ~CSLNode();
                inline CSLNode* parent() const { return m_parent; }
                virtual QString produceHTML(const QMap<QString, QVariant> &data, const CSLFormatState &currentFormat);
                static QVariant getCSLField(const QString& field, const QMap<QString, QVariant> &data, const QVariant &defaultVal=QVariant());
                virtual void parseProperties(const QDomElement& e);
                inline void setParent(CSLNode* p) { m_parent=p; }
            protected:
                CSLNode* m_parent;
                CSLFile* m_file;

                QString suffix;
                QString prefix;
                QString delimiter;

                void modifyStyle(CSLFormatState& style);
                QString applyTextCase(const QString& text);

                FontStyle fstyle;
                FontVariant fvariant;
                FontWeight fweight;
                TextDecoration tdecor;
                VerticalAlign valign;
                TextCase tcase;



                bool set_fstyle;
                bool set_fvariant;
                bool set_fweight;
                bool set_tdecor;
                bool set_valign;

                /*QString getHTMLFormatting(const QString& text, CSLNode* m_parent);
                QString getFontWeightTag(bool endtag=false);
                QString getFontStyleTag(bool endtag=false);
                QString getFontVariantTag(bool endtag=false);
                QString getTextDecorationTag(bool endtag=false);
                QString getVerticalAlignTag(bool endtag=false);*/


        };

        class LS3LIB_EXPORT CSLListNode: public CSLNode {
            public:
                explicit CSLListNode(CSLNode* parent, CSLFile* file);
                virtual ~CSLListNode();
                virtual QString produceHTML(const QMap<QString, QVariant> &data, const CSLFormatState& currentFormat);
                void addChild(CSLNode* c);
                void clearChildren();
            protected:
                QList<CSLNode*> m_children;
        };

        class LS3LIB_EXPORT CSLTextNode: public CSLNode {
            public:
                explicit CSLTextNode(CSLNode* parent, CSLFile* file);
                virtual ~CSLTextNode();
                virtual QString produceHTML(const QMap<QString, QVariant> &data, const CSLFormatState& currentFormat);
                virtual void parseProperties(const QDomElement& e);
            protected:
                QString macro;
                QString variable;
                QString term;
                QString value;
                QString form;
                bool plural;
        };

        class LS3LIB_EXPORT CSLNumberNode: public CSLNode {
            public:
                explicit CSLNumberNode(CSLNode* parent, CSLFile* file);
                virtual ~CSLNumberNode();
                virtual QString produceHTML(const QMap<QString, QVariant> &data, const CSLFormatState& currentFormat);
                virtual void parseProperties(const QDomElement& e);
            protected:
                QString variable;
                QString form;
        };

        class LS3LIB_EXPORT CSLChooseNode: public CSLNode {
            public:
                explicit CSLChooseNode(CSLNode* parent, CSLFile* file);
                virtual ~CSLChooseNode();
                virtual QString produceHTML(const QMap<QString, QVariant> &data, const CSLFormatState& currentFormat);
                virtual void parseProperties(const QDomElement& e);
            protected:
                enum IFType {
                    iftIsNumeric,
                    iftType,
                    iftVariable,
                    iftElse,
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

        CSLNode* m_bibliography;
        CSLNode* m_citation;
        QMap<QString, CSLNode*> m_macros;
        QMap<QString, QMap<QString, CSLLocale::CSLLocaleValue> > m_terms;
        CSLLocale* m_locale;
};

#endif // CSLTOOLS_H
