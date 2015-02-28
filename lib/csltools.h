#ifndef CSLTOOLS_H
#define CSLTOOLS_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QSet>
#include <QVariant>
#include "lib_imexport.h"
#include <QDomDocument>
#include <QDomElement>

/** \brief read the metadata from a CSL file */
LS3LIB_EXPORT bool cslReadMetadata(const QString& filename, QString* name=NULL);

class LS3LIB_EXPORT CSLFile {
    public:
        explicit CSLFile(const QString& fn);

        ~CSLFile();

        inline bool isValid() const { return valid; }
        inline QString name() const { return m_name; }

        QString produceHTML(const QMap<QString, QVariant> &data);

    protected:
        class LS3LIB_EXPORT CSLNode {
            public:
                enum FontStyle {
                    fsNormal,
                    fsItalic,
                    fsOblique
                };

                enum FontVariant {
                    fvNormal,
                    fvSmallCaps
                };

                enum FontWeight {
                    fwNormal,
                    fwBold,
                    fwLight=fwNormal
                };

                enum TextDecoration {
                    tdNone,
                    tdUnderline
                };

                enum VerticalAlign {
                    vaBaseline,
                    vaSuper,
                    vaSub
                };

                explicit CSLNode(CSLNode* parent,CSLFile* file);
                virtual ~CSLNode();
                inline CSLNode* parent() const { return m_parent; }
                virtual QString produceHTML(const QMap<QString, QVariant> &data);
                static QVariant getCSLField(const QString& field, const QMap<QString, QVariant> &data, const QVariant &defaultVal=QVariant());
                virtual void parseProperties(const QDomElement& e);
                inline void setParent(CSLNode* p) { m_parent=p; }
            protected:
                CSLNode* m_parent;
                CSLFile* m_file;

                QString suffix;
                QString prefix;
                QString delimiter;
                FontStyle fstyle;
                FontVariant fvariant;
                FontWeight fweight;
                TextDecoration tdecor;
                VerticalAlign valign;

                QString getHTMLFormatting(const QString& text, CSLNode* m_parent);
                QString getFontWeightTag(bool endtag=false);
                QString getFontStyleTag(bool endtag=false);
                QString getFontVariantTag(bool endtag=false);
                QString getTextDecorationTag(bool endtag=false);
                QString getVerticalAlignTag(bool endtag=false);


        };

        class LS3LIB_EXPORT CSLListNode: public CSLNode {
            public:
                explicit CSLListNode(CSLNode* parent, CSLFile* file);
                virtual ~CSLListNode();
                virtual QString produceHTML(const QMap<QString, QVariant> &data);
                void addChild(CSLNode* c);
                void clearChildren();
            protected:
                QList<CSLNode*> m_children;
        };

        class LS3LIB_EXPORT CSLTextNode: public CSLNode {
            public:
                explicit CSLTextNode(CSLNode* parent, CSLFile* file);
                virtual ~CSLTextNode();
                virtual QString produceHTML(const QMap<QString, QVariant> &data);
                virtual void parseProperties(const QDomElement& e);
            protected:
                QString macro;
                QString variable;
                QString term;
                QString value;
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
};

#endif // CSLTOOLS_H
