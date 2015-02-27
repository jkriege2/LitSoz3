#ifndef LS3PLUGINSERVICES_H
#define LS3PLUGINSERVICES_H

#include <QWidget>
#include <QString>
#include <QSettings>
#include <QVector>
#include <QMap>
#include <QVariant>
#include <QMenu>
#include <QToolBar>
#include <QDockWidget>
#include "lib_imexport.h"
#include <QNetworkProxy>

// FORWARD DECLARATIONS:
class LS3Importer;
class LS3Exporter;
class LS3Plugin;
class LS3ProgressThread;

/*! \brief this class provides some services for plugins that allow them to access certain features of the main application


*/
class LS3PluginServices {
    public:
        /** Default destructor */
        virtual ~LS3PluginServices() {};

        /** \brief return a QWidget to use as parent for new widgets */
        virtual QWidget* GetParentWidget()=0;

        /** \brief return the applications QSettings object */
        virtual QSettings* GetQSettings()=0;

        /** \brief Access languageID (which language to use for display) */
        virtual QString GetLanguageID()=0;

        /** \brief Access default currency */
        virtual QString GetDefaultCurrency()=0;

        /** \brief Access default status */
        virtual QString GetDefaultStatus()=0;

        /** \brief Access languageID (which language to use for display) */
        virtual QString GetIniFilename()=0;

        /** \brief return the currently used stylesheet */
        virtual QString GetStylesheet()=0;

        /** \brief return the currently used style */
        virtual QString GetStyle()=0;

        /** \brief returns the current username */
        virtual QString GetUsername()=0;

        /** \brief font name for the main table */
        virtual QString GetTableFontName()=0;

        /** \brief font size for the main table */
        virtual int GetTableFontSize()=0;

        /** \brief get "and" words */
        virtual QList<QString> GetAndWords()=0;
        /** \brief get name additions */
        virtual QSet<QString> GetNameAdditions()=0;
        /** \brief get name prefixes */
        virtual QSet<QString> GetNamePrefixes()=0;

        /** \brief return type of ID generation */
        virtual QString GetIDType()=0;

        /** \brief save type of ID generation */
        virtual void SetIDType(QString type)=0;

        /** \brief display message in statusbar */
        virtual void statusbarShowMessage(const QString & message, int timeout)=0;

        /** \brief display message in statusbar */
        virtual void statusbarShowMessage(const QString & message)=0;

        /** \brief set progress bar range to min..max */
        virtual void progressSetRange(int min, int max)=0;

        /** \brief set progress bar range to 0..max */
        virtual void progressSetRange(int max)=0;

        /** \brief set progress bar position */
        virtual void progressSetValue(int value)=0;

        /** \brief set progress increment position by inc */
        virtual void progressIncValue(int inc)=0;

        /** \brief set progress increment position by 1*/
        virtual void progressIncValue()=0;

        /** \brief register LS3Importer */
        virtual void registerImporter(LS3Importer* importer)=0;

        /** \brief register LS3Exporter */
        virtual void registerExporter(LS3Exporter* exporter)=0;

        /** \brief deregister LS3Importer */
        virtual void deregisterImporter(LS3Importer* importer)=0;

        /** \brief deregister LS3Exporter */
        virtual void deregisterExporter(LS3Exporter* exporter)=0;
        /** \brief get list if importers */
        virtual QList<LS3Importer*> getImporters()=0;

        /** \brief get list if exporters */
        virtual QList<LS3Exporter*> getExporters()=0;


        enum AvailableMenus {
            FileMenu,
            EditMenu,
            DatabaseMenu,
            ToolsMenu,
            ViewMenu,
            HelpMenu
        };

        enum AvailableToolbars {
            FileToolbar,
            EditToolbar,
            DatabaseToolbar,
            ToolsToolbar
        };

        /** \brief return a pointer to a given menu, or NULL */
        virtual QMenu* getMenu(AvailableMenus menu)=0;

        /** \brief return a pointer to a given toolbar, or NULL */
        virtual QToolBar* getToolbar(AvailableToolbars toolbar)=0;

        /** \brief insert a new menu on the right of \a before, or at the left border when \c before=NULL */
        virtual void insertMenu(QMenu* newMenu, QMenu* before=NULL)=0;
        /** \brief insert a new menu on the right of \a before, or at the left border when \c before=NULL */
        virtual void insertToolBar(QToolBar* newToolbar)=0;
        /** \brief add a new QDockWidget */
        virtual void addDockWidget(QDockWidget* dockWidget)=0;

        /** \brief return a list of currently visible records (list of correspondig uuids) */
        virtual QStringList getVisibleRecords()=0;


        /** \brief return a list of FDF IDs */
        virtual QStringList getFDFIDs()=0;
        /** \brief return a list of FDF names */
        virtual QStringList getFDFNames()=0;
        /** \brief return an icon for the given FDF ID */
        virtual QIcon getFDFIcon(QString fdf)=0;
        /** \brief return the name in the current language for the given FDF ID */
        virtual QString getFDFName(QString fdf)=0;
        /** \brief the plugin directory */
        virtual QString getPluginDirectory()=0;

        /** \brief return a pointer to the given plugin */
        virtual LS3Plugin* getPluginByID(QString ID)=0;


        enum AvailableEditWidgetsForButtons {
            EditDOI,
            EditCiteSeer,
            EditPubmed,
            EditPMCID,
            EditArXiv,
            EditURL,
            EditFile
        };
        /** \brief add an action to the specified widget (usually as a button) */
        virtual void addEditButton(AvailableEditWidgetsForButtons edit, QAction* action)=0;

        /** \brief add a display widget for the given thread */
        virtual void addProgressThread(LS3ProgressThread* thread)=0;

        virtual QNetworkProxy getNetworkProxy()=0;

};


Q_DECLARE_INTERFACE(LS3PluginServices,
                     "jkrieger.de.litsoz3.plugins.LS3PluginServices/1.0")

#endif // LS3PLUGINSERVICES_H
