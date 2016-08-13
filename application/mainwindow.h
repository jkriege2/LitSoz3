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



#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QClipboard>
#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QTabWidget>
#include <QTableView>
#include <QLabel>
#include <QDataWidgetMapper>
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtCore>
#include <QtNetwork>

#include "fdf.h"
#include "programoptions.h"
#include "detailpane.h"
#include "extendedpane.h"
#include "contentspane.h"
#include "../lib/ls3pluginservices.h"
#include "ls3datastorexml.h"
#include "jkcharacterscreenkeyboard.h"
#include "selectionlist.h"
#include "searchresultlist.h"
#include "ls3importexport.h"
#include "ls3plugin.h"
#include "qenhancedtableview.h"
#include "ls3summarysortfilterproxymodel.h"
#include "ls3summaryproxymodel.h"
#include "searchdialog.h"
#include "qmodernprogresswidget.h"
#include "exportdialog.h"
#include "importdialog.h"
#include "referencetreedockwidget.h"
#include "../version.h"
#include "qrecentfilesmenu.h"
#include "threadoverview.h"
#include "fileviewpane.h"
#include <QWebView>



// TODO: Autorendatenbank mit Kontaktdaten
// TODO: Zeitschriftendatenbank: Links (aktuelle Ausgabe, Archiv, Authors-Guidelines), Abkürzung, Name, Adresse (?), Editoren, ...

/*! \brief main window


    Data from the datastore is sent through variouous proxy models:
      -# LS3SummaryProxyModel to cook it down to a summary displayed in the central table
      -# LS3SummarySortFilterProxyModel to sort and filter the summary table.
    .
 */
class MainWindow : public QMainWindow, public LS3PluginServices {
        Q_OBJECT
        Q_INTERFACES(LS3PluginServices)
    public:
        /** \brief class constructor */
        MainWindow(ProgramOptions* settings, QSplashScreen* splash);

        /** \brief class destructor */
        virtual ~MainWindow();

        void init();

    signals:
        /*! \brief this signal is emitted when the database is loaded or closed */
        void databaseLoaded(bool isLoaded);

        void showSplashMessage(const QString& message);

    protected slots:
        /** \brief export datasets */
        void exportDatabase();
        /** \brief import datasets */
        void importDatabase();
        /** \brief search the database */
        void search();

        void quickfind();

        void setCurrentFile(const QString &fileName);
        /** \brief create a new database and load it (called by File|New)*/
        void newFile();
        /** \brief open an existing database (called by File|Open)*/
        void open();
        /** \brief open a recently opened database (called by File|<Click on a recent file>)*/
        void openRecentFile(const QString& filename);

        bool save();
        bool saveAs();
        void closeDatabase();

        /** \brief display an about box */
        void about();

        /** \brief display an about Plugins box */
        void aboutPlugins();

        /*! \brief display an error message caused by a database error */
        void showDBError(QString errorMessage);

        /*! \brief connect data aware widgets with table model. Also displays te otherwise hidden widgets. */
        void connectWidgets();

        /*! \brief disconnect data aware widgets with table model. Hides the disconnected widgets. */
        void disconnectWidgets();

        /*! \brief remove the current record */
        void dbDelete();


        /*! \brief change the enabled state of the next, previous, last and first action, depending on the row \a row */
        void currentRecordChanged(int row);

        /*! \brief this is used to change the current row in the mapper when the current row in a QTableView changes */
        void tableRowChanged(const QModelIndex& current, const QModelIndex& previous);

        /*! \brief this is used to change the state of the database actions when data is edited in a QTableView changes */
        void tableDataChanged(int firstRow, int lastRow);

        /*! \brief this is used to change the current row in the mapper when the current row in a QTableView changes */
        void tableRowActivted(const QModelIndex & current);
        /*! \brief this is used to make the reference data dock widget visible */
        void tableRowDoubleClicked(const QModelIndex & current);

        /*! change sorting order when user selects a new sorting in cmbSorting */
        void changeSorting();

        void dbNext();
        void dbPrevious();

        void readSettings();
        void writeSettings();

        void previewAnchorClicked(const QUrl& link);

        void wasChanged(bool changed);

        void createMissingIDs();
        void cleanupTopics();
        void editTopics();

        QString copyCSL(CSLOutputFormat format, const QString linetemplate=QString("  <li><b>[%2]:</b>%1</li>\n"));
        void copyCSLHTMLTags();
        void copyCSLPlainText();
        void copyCSLFormatted();
    protected:
        bool maybeSave();
        void createActions();
        void createMenus();
        void createToolBars();
        void createStatusBar();
        void createWidgets();
        void createDockWidgets();



        /** \brief search for plugins */
        void searchPlugins(QString directory);

        /** \brief initialize all plugins */
        void initPlugins();
        /** \brief de-initialize all plugins */
        void deinitPlugins();
        /** \brief clear all plugins, also calls deinitPlugins() */
        void clearPlugins();

        /*! \brief connect tvMain events

            Together with disconnecttvMain() this may be used when you want to set the selection in tvMain, without
            events to be emitted.
        */
        void connecttvMain();
        /** \brief disconnect tvMain events
         *  \see connecttvMain()
         */
        void disconnecttvMain();

        QString strippedName(const QString &fullFileName);
        void closeEvent(QCloseEvent *event);
        void showEvent(QShowEvent *event);

        QString curFile;

        QMenu* fileMenu;
        QMenu* editMenu;
        QMenu* databaseMenu;
        QMenu* toolsMenu;
        QMenu* viewMenu;
        QMenu* viewToolbarsMenu;
        QMenu* viewDockMenu;
        QMenu* helpMenu;
        QRecentFilesMenu* recentFilesMenu;

        QToolBar* fileToolBar;
        QToolBar* editToolBar;
        QToolBar* toolsToolBar;
        QToolBar* databaseToolBar;
        QToolBar* quickfindToolBar;
        QToolBar* referencesToolBar;

        QAction* actQuickFindEnabled;
        QComboBox* cmbQuickFindFields;
        QEnhancedLineEdit* edtQuickFindTerm;
        QCheckBox* chkQuickFindRegExp;

        QAction* newAct;
        QAction* openAct;
        QAction* saveAct;
        QAction* saveAsAct;
        QAction* closeAct;
        QAction* exitAct;
        QAction* pasteAct;
        QAction* aboutAct;
        QAction* aboutPluginsAct;
        QAction* aboutQtAct;
        QAction* exportAct;
        QAction* importAct;
        QAction* searchAct;

        QAction* dbnextAct;
        QAction* dbpreviousAct;
        //QAction* dbfirstAct;
        //QAction* dblastAct;
        QAction* dbinsertAct;
        QAction* dbdeleteAct;
        //QAction* dbsubmitAct;
        //QAction* dbrejectAct;
        //QAction* dbrefreshAct;
        QAction* separatorAct;
        QAction* optionsAct;
        QAction* createMissingIDsAct;
        QAction* actCleanupTopics;
        QAction* actEditTopics;


        /** \brief this label is used to display database status information in the toolbar */
        QLabel* labDBStatusToolBar;

        /** \brief combobox to select sorting order */
        QComboBox* cmbSorting;

        /** \brief combobox to select CSL to use */
        QComboBox* cmbCSLs;
        QComboBox* cmbCSLLocales;

        QAction* actCopyFormatted;
        QAction* actCopyPlainText;
        QAction* actCopyHTMLtags;
        QComboBox* cmbCSLWhich;

        /** \brief combobox to select sorting direction */
        QComboBox* cmbSortingDirection;


        /** \brief tab widget for the reference data */
        QTabWidget* tabReferenceData;

        /** \brief main tab widget */
        QTabWidget* tabMain;

        /** \brief preview tab widget */
        QTabWidget* tabPreview;

        /** \brief table view to display database contents */
        QEnhancedTableView* tvMain;


        /** \brief the detail pane of the main window */
        DetailPane* details;

        /** \brief the extended pane of the main window */
        ExtendedPane* extended;

        /** \brief the contents pane of the main window */
        ContentsPane* contents;

        FileViewPane* fileView;

        /** \brief this object manages the program settings */
        ProgramOptions* settings;

        /** \brief QSplitter for the main window layout */
        QSplitter* splitter;

        /** \brief time index of last access to Amazon */
        clock_t amazonTimestamp;

        /** \brief datastore used to access the data of the currently loaded database */
        LS3DatastoreXML* datastore;

        /** \brief proxy model that transform the full database table to a summarized table which is more convenient for display */
        LS3SummaryProxyModel* summaryProxy;

        /**\brief proxy model switched in between datastore and tvMain which allows sorting and filtering */
        LS3SummarySortFilterProxyModel* tvMainSortProxy;

        /** \brief used to store the splash screen pixmap, as given to the constructor. This pixmap may be used for info dialogs. */
        QPixmap splashPix;

        /** \brief a progress bar */
        QProgressBar* progress;

        /** \brief screen keyboard for special characters */
        JKCharacterScreenKeyboard* kbSpecial;

        /** \brief selection list docking widget */
        SelectionList* lstSelection;

        /** \brief search result list docking widget */
        SearchResultList* lstSearchResults;

        /** \brief preview of current dataset */
        QWebView* txtPreview;

        /** \brief reference data dock widget */
        QDockWidget* dockReferenceData;

        /** \brief list of all LS3Exporter s */
        QList<LS3Exporter*> m_exporters;
        /** \brief list of all LS3Importer s */
        QList<LS3Importer*> m_importers;
        /** \brief list of all available plugins */
        QList<LS3Plugin*> m_plugins;
        QStringList m_pluginFilenames;
        SearchDialog* searchDlg;
        ExportDialog* exportDlg;
        ImportDialog* importDlg;
        ThreadOverview* dockThreadOverview;

        ReferenceTreeDockWidget* dockReferenceTree;

        /** \brief display style (given by settings->GetHowDisplayReferenceDetails() )*/
        int curDisplayStyle;

    public:
        /** \brief return a QWidget to use as parent for new widgets */
        virtual QWidget* GetParentWidget();

        /** \brief return the applications QSettings object */
        virtual QSettings* GetQSettings();

        /** \brief Access languageID (which language to use for display) */
        virtual QString GetLanguageID();

        /** \brief Access default currency */
        virtual QString GetDefaultCurrency();

        /** \brief Access default status */
        virtual QString GetDefaultStatus();

        /** \brief Access languageID (which language to use for display) */
        virtual QString GetIniFilename();

        /** \brief return the currently used stylesheet */
        virtual QString GetStylesheet();

        /** \brief return the currently used style */
        virtual QString GetStyle();

        /** \brief returns the current username */
        virtual QString GetUsername();

        /** \brief font name for the main table */
        virtual QString GetTableFontName();
        /** \brief font size for the main table */
        virtual int GetTableFontSize();

        void statusbarShowMessage(const QString & message, int timeout);
        void statusbarShowMessage(const QString & message);

        void progressSetRange(int min, int max);

        void progressSetRange(int max);

        void progressSetValue(int value);

        void progressIncValue(int inc);
        void progressIncValue();

        /** \brief register LS3Importer */
        virtual void registerImporter(LS3Importer* importer);

        /** \brief register LS3Importer */
        virtual void registerExporter(LS3Exporter* exporter);
        /** \brief register LS3Importer */
        virtual void deregisterImporter(LS3Importer* importer);

        /** \brief register LS3Importer */
        virtual void deregisterExporter(LS3Exporter* exporter);

        /** \brief return a pointer to a given menu, or NULL */
        virtual QMenu* getMenu(LS3PluginServices::AvailableMenus menu);

        /** \brief return a pointer to a given toolbar, or NULL */
        virtual QToolBar* getToolbar(LS3PluginServices::AvailableToolbars toolbar);

        virtual void addEditButton(LS3PluginServices::AvailableEditWidgetsForButtons edit, QAction* action);
        virtual QNetworkProxy getNetworkProxy();

        /** \brief insert a new menu on the right of \a before, or at the left border when \c before=NULL */
        virtual void insertMenu(QMenu* newMenu, QMenu* before=NULL);
        /** \brief insert a new menu on the right of \a before, or at the left border when \c before=NULL */
        virtual void insertToolBar(QToolBar* newToolbar);
        /** \brief add a new QDockWidget */
        virtual void addDockWidget(QDockWidget* dockWidget);
        virtual QList<QString> GetAndWords();
        /** \brief get name additions */
        virtual QSet<QString> GetNameAdditions();
        /** \brief get name prefixes */
        virtual QSet<QString> GetNamePrefixes();
        /** \brief get list if importers */
        virtual QList<LS3Importer*> getImporters();
        /** \brief get list if exporters */
        virtual QList<LS3Exporter*> getExporters();
        /** \brief return a list of currently visible records (list of correspondig uuids) */
        virtual QStringList getVisibleRecords();
        /** \brief return a list of FDF IDs */
        virtual QStringList getFDFIDs();
        /** \brief return a list of FDF names */
        virtual QStringList getFDFNames();
        /** \brief return an icon for the given FDF ID */
        virtual QIcon getFDFIcon(QString fdf);
        /** \brief return the name in the current language for the given FDF ID */
        virtual QString getFDFName(QString fdf);
        /** \brief the plugin directory */
        virtual QString getPluginDirectory();
        /** \brief return a pointer to the given plugin */
        virtual LS3Plugin* getPluginByID(QString ID);

        /** \brief return type of ID generation */
        virtual QString GetIDType() { return GetQSettings()->value("extended/idtype", "AuthorYear").toString(); };

        /** \brief save type of ID generation */
        virtual void SetIDType(QString type) { GetQSettings()->setValue("extended/idtype", type); };

        virtual void addProgressThread(LS3ProgressThread* thread);
    public slots:
        void loadStartup();
};

#endif
