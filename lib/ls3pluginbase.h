#ifndef LS3PLUGINBASE_H
#define LS3PLUGINBASE_H



#include "ls3plugin.h"
#include "lib_imexport.h"



/*! \brief LitSoz 3 Plugin base class which implements some features over LS3Plugin

*/
class LS3LIB_EXPORT LS3PluginBase: public QObject, public LS3Plugin {
        Q_OBJECT
        Q_INTERFACES(LS3Plugin)
    public:
        /** \brief default constructor */
        LS3PluginBase() : QObject(NULL) { m_currentDatastore=NULL; };

        /** \brief default destructor */
        virtual ~LS3PluginBase() {};

        /** \copydoc LS3Plugin::openData() */
        virtual void openData(LS3Datastore* datastore) { m_currentDatastore=datastore; };

        /** \copydoc LS3Plugin::closeData() */
        virtual void closeData(LS3Datastore* datastore) { m_currentDatastore=NULL; };

        /** \brief initialize the plugin (create buttons, menu entries, register import/export objects ...) */
        virtual void init(LS3PluginServices* pluginServices) { m_pluginServices=pluginServices; };

        /** \brief de-initialize the plugin (clean up when program is closed) */
        virtual void deinit(LS3PluginServices* pluginServices)  {};

        /** \brief return the datastore last opened with openData() */
        LS3Datastore* datastore() const { return m_currentDatastore; }

        /** \brief plugin services */
        LS3PluginServices* pluginServices() const { return m_pluginServices; }
    protected:
        LS3Datastore* m_currentDatastore;
        LS3PluginServices* m_pluginServices;

};

#endif // LS3PLUGINBASE_H
