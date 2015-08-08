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

#ifndef LS3BIBTEXIMPORTER_H
#define LS3BIBTEXIMPORTER_H

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

/*! \brief Class to import BiBTeX files/strings
*/
class LS3BibTeXImporter : public LS3Importer {
    public:
        /** Default constructor */
        LS3BibTeXImporter(): LS3Importer() {};
        /** Default destructor */
        virtual ~LS3BibTeXImporter() {};



        /** \copydoc LS3Importer::execute() */
        virtual QVector<QMap<QString, QVariant> > execute(LS3PluginServices* pluginServices);

        /** \copydoc LS3Importer::importsText() */
        virtual bool importsText() const { return true; }

        /** \copydoc LS3Importer::importText() */
        virtual QVector<QMap<QString, QVariant> > importText(const QString& text, LS3PluginServices* pluginServices);



        /** \copydoc LS3Importer::getName() */
        virtual QString getName() { return QObject::tr("BiBTeX Import"); };

        /** \copydoc LS3Importer::getDescription() */
        virtual QString getDescription() { return QObject::tr("import BiBTeX files"); };

        /** \copydoc LS3Importer::getIcon() */
        virtual QIcon getIcon() { return QIcon(":/latextools/bibteximporter.png"); };



};

#endif // LS3BIBTEXIMPORTER_H
