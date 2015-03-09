#ifndef LANGUAGETOOLS_H
#define LANGUAGETOOLS_H


#include <QString>
#include <QStringList>
#include <QMap>
#include <QSet>
#include <QVariant>
#include "lib_imexport.h"
#include <QPair>
#include <QHash>

class LS3LIB_EXPORT LanguageRecognizer {
    public:
        LanguageRecognizer();
        void init(const QStringList& datafiles);
        void initFromDirs(const QStringList& datadirs);
        bool isInitialize() const { return m_initialized; }
        QString recognizeLanguage(const QString& text, double *distance=NULL);
        void clearInit();
        static QString cleanText(const QString& text);
        static void splitNGrams(QHash<QString, int>& hash, const QString& text, int length=3);
        static QList<QPair<QString, int> > hashToList(const QHash<QString, int>& hash, int minOccurence=-1);
        static LanguageRecognizer* globalInstance();
        static double getMaxUseDist(int firstNNgrams=500);
    protected:
        bool m_initialized;
        struct LangDB {
            QList<QPair<QString, int> > ngramcount;
            QHash<QString, int> ngramposmap;
            double distance(const QList<QPair<QString, int> >& other, int firstNNgrams=500) const;
            double getMaxUseDist(int firstNNgrams=500) const;
        };

        QMap<QString, LangDB> m_dbs;
};


/** \brief tries to recognize the language of the given text and returns its ID, or an empty string if the language could not be recognized */
LS3LIB_EXPORT QString recognizeLanguage(const QString& text, double* distance=NULL);

#endif // LANGUAGETOOLS_H
