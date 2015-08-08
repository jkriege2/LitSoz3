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

#ifndef LS3IMPORTEXPORT_H
#define LS3IMPORTEXPORT_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QString>
#include <QVariant>
#include "ls3datastore.h"
#include "ls3pluginservices.h"
#include "lib_imexport.h"


/*! \brief LitSoz 3 importer class

    The main functionality is implemented in the execute() method. This method returns a list of records. Each dataset
    is represented by a QMap<QString, QVariant> which maps from a field name (QString) to a value (QVariant). The main
    application (LitSoz 3.0) will take care of storing the imported data to the current table. Certain fields are filled
    automatically, if no value is provided by the plugin. For valid field names see \link ls3_database LitSoz 3.0 Databases \endlink

    \see ls3_import

*/
class LS3Importer {
    public:

        /** \brief Default destructor */
        virtual ~LS3Importer() {};

        /*! \brief execute the plugin. After this method returns all data stored in the returned vector
                   will be added to the current dataset.

            \param pluginServices service object that allows access to LitSoz 3 services
            \return a list of dataset to add to the database

            Do not set the contents of the uuid field, as this will be done by the main application.
         */
        virtual QVector<QMap<QString, QVariant> > execute(LS3PluginServices* pluginServices)=0;

        /** \brief returns \c true if this may be used to import a given text */
        virtual bool importsText() const { return false; }

        /*! \brief import data from a given text.

            Only implement this if importsText() returns \c true

            \param text the text to import
            \param pluginServices service object that allows access to LitSoz 3 services
            \return a list of dataset to add to the database

            Do not set the contents of the uuid field, as this will be done by the main application.
         */
        virtual QVector<QMap<QString, QVariant> > importText(const QString& text, LS3PluginServices* pluginServices)=0;



        /** \brief name for the importer */
        virtual QString getName()=0;

        /** \brief short description for the importer */
        virtual QString getDescription()=0;

        /** \brief short description for the importer */
        virtual QIcon getIcon()=0;

};



Q_DECLARE_INTERFACE(LS3Importer,
                     "jkrieger.de.litsoz3.plugins.LS3Importer/1.0")





/*! \brief LitSoz 3 exporter class

    The main functionality is implemented in the execute() method. This method receives a list of records that should be exported.
    As for LS3PluginImport this list is stored as QVector<QMap<QString, QVariant> > where each item in the QVector is a record
    and each QString, QVariant pair is a field in the record.

    For valid field names see \link ls3_database LitSoz 3.0 Databases \endlink

    \see ls3_export
*/
class LS3Exporter {
    public:

        /** \brief Default destructor */
        virtual ~LS3Exporter() {};

        /*! \brief execute the plugin. After this method returns all data stored in the returned vector
                   will be added to the current dataset.

            \return \c true on sucess and \c false else
            \param data the data to export
            \param pluginServices service object that allows access to LitSoz 3 services
         */
        virtual bool execute(const QVector<QMap<QString, QVariant> >& data, LS3PluginServices* pluginServices)=0;


        /** \brief name for the exporter */
        virtual QString getName()=0;

        /** \brief short description for the exporter */
        virtual QString getDescription()=0;

        /** \brief short description for the exporter */
        virtual QIcon getIcon()=0;
};

Q_DECLARE_INTERFACE(LS3Exporter,
                     "jkrieger.de.litsoz3.plugins.LS3Exporter/1.0")


#endif // LS3IMPORTEXPORT_H
