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

#ifndef LS3PLUGIN_H
#define LS3PLUGIN_H

#include <QtPlugin>
#include <QString>
#include <QMap>
#include <QVector>
#include <QVariant>

#include "ls3pluginservices.h"
#include "ls3datastore.h"
#include "litsoz3tools_export.h"





/*! \brief LitSoz 3 Plugin interface class

*/
class LS3Plugin {
    public:
        /** Default destructor */
        virtual ~LS3Plugin() {};

        /** \brief name for the plugin */
        virtual QString getName()=0;

        /** \brief short description for the plugin */
        virtual QString getDescription()=0;

        /** \brief author the plugin */
        virtual QString getAuthor()=0;

        /** \brief copyright information the plugin */
        virtual QString getCopyright()=0;

        /** \brief weblink for the plugin */
        virtual QString getWeblink()=0;

        /** \brief icon for the plugin */
        virtual QString getIconFilename()=0;

        /** \brief initialize the plugin (create buttons, menu entries, register import/export objects ...) */
        virtual void init(LS3PluginServices* pluginServices)=0;

        /** \brief de-initialize the plugin (clean up when program is closed) */
        virtual void deinit(LS3PluginServices* pluginServices) =0;

        /** \brief this function is called when a datastore is closed */
        virtual void openData(LS3Datastore* datastore)=0;

        /** \brief this function is called when a datastore is closed */
        virtual void closeData(LS3Datastore* datastore)=0;
};













Q_DECLARE_INTERFACE(LS3Plugin,
                     "jkrieger.de.litsoz3.plugins.LS3Plugin/1.0")
#endif // LS3PLUGIN_H
