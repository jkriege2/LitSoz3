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

#ifndef LANGUAGETOOLS_H
#define LANGUAGETOOLS_H


#include <QString>
#include <QStringList>
#include <QMap>
#include <QSet>
#include <QVariant>
#include "litsoz3tools_export.h"
#include <QPair>
#include <QHash>

class LITSOZ3TOOLS_EXPORT LanguageRecognizer {
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
LITSOZ3TOOLS_EXPORT QString recognizeLanguage(const QString& text, double* distance=NULL);

#endif // LANGUAGETOOLS_H
