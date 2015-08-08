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

#ifndef LS3BIBTEXEXPORTER_H
#define LS3BIBTEXEXPORTER_H

#include "ls3importexport.h"
#include <QObject>
#include <QFileDialog>
#include <QProgressDialog>
#include <QString>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QStringList>
#include <QMessageBox>
#include "bibtools.h"
#include "latextools.h"
#include "bibtexexportdialog.h"

class LS3BibTeXExporter : public LS3Exporter
{
    public:
        LS3BibTeXExporter();
        virtual ~LS3BibTeXExporter();

        /*! \brief execute the plugin. After this method returns all data stored in the returned vector
                   will be added to the current dataset.

            \return \c true on sucess and \c false else
            \param data the data to export
            \param pluginServices service object that allows access to LitSoz 3 services
         */
        virtual bool execute(const QVector<QMap<QString, QVariant> >& data, LS3PluginServices* pluginServices);


        /** \brief name for the exporter */
        virtual QString getName() { return QObject::tr("BiBTeX Export"); } ;

        /** \brief short description for the exporter */
        virtual QString getDescription() { return QObject::tr("export datasets to BiBTeX format"); } ;

        /** \brief short description for the exporter */
        virtual QIcon getIcon()  { return QIcon(":/latextools/bibtexexporter.png"); };

        /** \brief does the actual BiBtTeX export into the given QTextStream */
        static bool exportBibTeX(QTextStream &out, const QVector<QMap<QString, QVariant> > &data, LS3PluginServices *pluginServices);
};

#endif // LS3BIBTEXEXPORTER_H
