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
