#ifndef CSLTOOLS_H
#define CSLTOOLS_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QSet>
#include <QVariant>
#include "lib_imexport.h"

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
                explicit CSLNode(CSLNode* parent);
                virtual ~CSLNode();
                inline CSLNode* parent() const { rteurn m_parent; }
                virtual QString produceHTML(const QMap<QString, QVariant> &data);
                static QVariant getCSLField(const QString& field, const QMap<QString, QVariant> &data, const QVariant &defaultVal=QVariant());
            protected:
                CSLNode* m_parent;
        };

        bool parse(const QString& filename);
        QString filename;
        QString m_name;
        bool valid;

        CSLNode* m_bibliography;
        QMap<QString, CSLNode*> m_macros;
};

#endif // CSLTOOLS_H
