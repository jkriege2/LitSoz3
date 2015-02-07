

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QtSql>
#include <QTabWidget>
#include <QTableView>
#include <QLabel>
#include <QDataWidgetMapper>
#include <QtGui>
#include <QtSql>
#include <QtCore>

#include "version.h"
#include "fdf.h"
#include "programoptions.h"

// TODO: Autorendatenbank mit Kontaktdaten
// TODO: Zeitschriftendatenbank: Links (aktuelle Ausgabe, Archiv, Authors-Guidelines), Abkürzung, Name, Adresse (?), Editoren, ...

/*! \brief



 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /** \brief class constructor */
    MainWindow(ProgramOptions* settings);

    /** \brief class destructor */
    ~MainWindow();

signals:
    /*! \brief this signal is emitted when the database is loaded or closed */
    void databaseLoaded(bool isLoaded);

    /*! \brief this signal is emitted when a new record is added or a record is beeing edited
               and is used to enable/disable the submit/reject buttons */
    void databaseEditing(bool editing);

protected slots:
    /** \brief create a new database and load it (called by File|New)*/
    void newFile();
    /** \brief open an existing database (called by File|Open)*/
    void open();
    /** \brief open a recently opened database (called by File|<Click on a recent file>)*/
    void openRecentFile();
    /** \brief  */
    void updateRecentFileActions();

    /** \brief display an about box */
    void about();

    /*! \brief load an existing database and bind it to the data-aware widget

        \return \c true on success
     */
    bool loadDB(const QString &fileName);

    /*! \brief create a new database with all required fields, but does NOT load it

        \return \c true on success
     */
    bool newDB(const QString &fileName);

    /*! \brief close the currently opened database (if one is opened).

        Does nothing if no database is opened.
    */
    void closeDB();

    /*! \brief display an error message caused by a SQL database error */
    void showDBError(const QSqlError &err);

    /*! \brief connect data aware widgets with table model. Also displays te otherwise hidden widgets. */
    void connectWidgets();

    /*! \brief disconnect data aware widgets with table model. Hides the disconnected widgets. */
    void disconnectWidgets();

    /*! \brief insert a new record */
    void dbInsert();

    /*! \brief remove the current record */
    void dbDelete();

    /*! \brief call select() to refresh the viewed data */
    void dbRefresh();

    /*! \brief undo all current changes/editing */
    void dbReject();

    /*! \brief submit all current changes/editing */
    void dbSubmit();

    /*! \brief change the enabled state of the next, previous, last and first action, depending on the row \a row */
    void updateDBActions(int row);

    /*! \brief this is used to change the current row in the mapper when the current row in a QTableView changes */
    void tableRowChanged(const QModelIndex& current, const QModelIndex& previous);

    /*! \brief this is used to change the state of the database actions when data is edited in a QTableView changes */
    void tableDataChanged(const QModelIndex& current, const QModelIndex& previous);

    /*! \brief read in the config directory tree

        This function does:
          - search for FDF files and parse them
        .
     */
    void initConfig(QString configDir);

protected:
    void init();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createWidgets();
    void readSettings();
    void writeSettings();

    /** \brief returns \c true if a database is currently opened */
    bool isDBOpened();


    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    void closeEvent(QCloseEvent *event);

    QMenu * fileMenu;
    QMenu * editMenu;
    QMenu* databaseMenu;
    QMenu * helpMenu;
    QMenu *recentFilesMenu;

    QToolBar* fileToolBar;
    QToolBar* editToolBar;
    QToolBar* databaseToolBar;

    QAction* newAct;
    QAction* openAct;
    QAction* closeAct;
    QAction* exitAct;
    QAction* pasteAct;
    QAction* aboutAct;
    QAction* aboutQtAct;
    QAction* dbnextAct;
    QAction* dbpreviousAct;
    QAction* dbfirstAct;
    QAction* dblastAct;
    QAction* dbinsertAct;
    QAction* dbdeleteAct;
    QAction* dbsubmitAct;
    QAction* dbrejectAct;
    QAction* dbrefreshAct;
    QAction* separatorAct;
    QAction* optionsAct;

    /** \brief length of the list of recently opened files */
    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];

    /** \brief this label is used to display database status information in the toolbar */
    QLabel* labDBStatusToolBar;


    /** \brief main tab widget */
    QTabWidget* tabMain;

    /** \brief table view to display database contents */
    QTableView* tvMain;

    /** \brief main data model which holds the literature table for viewing and editing */
    QSqlRelationalTableModel* data;

    /** \brief name of the connection used by data */
    QString dataConnectionName;

    /** \brief mapper that maps between widgets and the model <data> */
    QDataWidgetMapper* mapper;


    /** \brief this object manages the program settings */
    ProgramOptions* settings;
};

#endif
