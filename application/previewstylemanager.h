#ifndef PREVIEWSTYLEMANAGER_H
#define PREVIEWSTYLEMANAGER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QPair>
#include <QtAlgorithms>
#include <QMap>
#include <QVariant>
#include "csltools.h"


class CSLFile; // forward
class CSLLocale; // forward

/*! \brief manages and executes preview styles

*/
class PreviewStyleManager : public QObject {
        Q_OBJECT
    public:
        PreviewStyleManager(QObject* parent);
        virtual ~PreviewStyleManager();

        void searchCSLLocales(const QString& dir);
        void searchCSL(const QString& dir);

        /** \brief return a list of the names of the preview styles */
        QStringList styles() const;

        /** \brief return a list of the names of the preview styles */
        QStringList locales() const;

        /** \brief return the evaluated HTML for the i-th style */
        QString createPreview(int i, const QMap<QString, QVariant>& data, QString locale=QString("en-US"), CSLOutputFormat outf=ofHTML) const;

        /** \brief return the name for the i-th style */
        QString getName(int i) const;
    protected:
        QList<QPair<QString, CSLFile*> > csls;
        //QList<CSLFile*> cslfiles;
        QMap<QString, CSLLocale*> csllocales;
};

#endif // PREVIEWSTYLEMANAGER_H
