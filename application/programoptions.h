#ifndef PROGRAMOPTIONS_H
#define PROGRAMOPTIONS_H

#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "optionsdialog.h"
#include "previewstylemanager.h"
#include <QNetworkProxy>
// forward
class fdfManager;

/** \brief this class manages the overall program options (and may also display an options Dialog */
class ProgramOptions: public QObject {
    Q_OBJECT
public:
    /** \brief Default constructor
     *
     * \param ini filename of the INI file of the application
     * \param parent pointer to the parent object
     * \param app the application this object manages the properties for
     */
    ProgramOptions( QString ini=QString(""), QObject * parent = 0, QApplication* app=NULL);

    /** \brief Default destructor */
    virtual ~ProgramOptions();


    /** \brief Access lastOpenDBDirectory (the last directory accessed in the OpenDatabase file dialog) */
    inline QString GetLastOpenDBDirectory() { return lastOpenDBDirectory; }
    /** \brief Set lastOpenDBDirectory (the last directory accessed in the OpenDatabase file dialog)
     * \param val New value to set
     */
    inline void SetLastOpenDBDirectory(QString val) {
        if (lastOpenDBDirectory != val) {
            lastOpenDBDirectory = val;
            //settings.setValue("mainwindow/last_opendb_directory", lastOpenDBDirectory);
            writeSettings();
            emit configDataChanged();
        }
    }


    /** \brief Access lastNewDBDirectory (the last directory accessed in the NewDatabase file dialog) */
    inline QString GetLastNewDBDirectory() { return lastNewDBDirectory; }
    /** \brief Set lastNewDBDirectory (the last directory accessed in the NewDatabase file dialog)
     * \param val New value to set
     */
    inline void SetLastNewDBDirectory(QString val) {
        if (lastNewDBDirectory != val) {
            lastNewDBDirectory = val;
            //settings.setValue("mainwindow/last_newdb_directory", lastNewDBDirectory);
            writeSettings();
            emit configDataChanged();
        }
    }


    /** \brief Access configDirectory (the directory which contains the user config files */
    QString GetConfigDirectory();
    /** \brief Access configDirectory (the directory which contains the assets files (*.fdf ...)) */
    QString GetAssetsDirectory();


    /** \brief Access currentFile ( the currently opened file) */
    inline QString GetCurrentFile() { return currentFile; }
    /** \brief Set currentFile ( the currently opened file)
     * \param val New value to set
     */
    inline void SetCurrentFile(QString val) {
        if (currentFile != val) {
            currentFile = val;
            //settings.setValue("mainwindow/last_opened_file", currentFile);
            writeSettings();
            emit currentFileChanged(currentFile);
            emit configDataChanged();
        }

    }

    void setProxyHost(const QString& host);
    void setProxyPort(quint16 port);
    quint16 getProxyPort() const;
    QString getProxyHost() const;
    int getProxyType() const;
    void setProxyType(int type);

    void setQNetworkProxy(QNetworkProxy& proxy);


    /** \brief Access languageID (which language to use for display) */
    inline QString GetLanguageID() { return languageID; }


    /** \brief Access default currency */
    inline QString GetDefaultCurrency() { return defaultCurrency; }

    /** \brief Access default status */
    inline QString GetDefaultStatus() { return defaultStatus; }


    /** \brief Access languageID (which language to use for display) */
    inline QString GetIniFilename() { return iniFilename; }

    /** \brief return the currently used stylesheet */
    inline QString GetStylesheet() { return stylesheet; }

    /** \brief return the currently used style */
    inline QString GetStyle() { return style; }

    /** \brief returns the current username */
    inline QString GetUsername() { return username; }

    /** \brief returns the special characters for en screen keyboard */
    inline QString GetSpecialCharacters() { return specialCharacters; }


    /** \brief returns a pointer to the fdfManager */
    inline fdfManager* GetFDFManager() { return fdf; };

    inline PreviewStyleManager* GetPreviewStyleManager() { return previewStyles; };

    /** \brief return a pointer to a QSettings object that may be used to save config data throughout the application */
    inline QSettings* GetQSettings() { return settings; }

    /** \brief font name for the table */
    inline QString GetTableFontName() { return tableFontName; }
    /** \brief font size for the table */
    inline int GetTableFontSize() { return tableFontSize; }

    /** \brief width of shorter widgets on extended tab */
    inline int GetExtendedLongWidth() { return extendedLongWidth; };
    /** \brief width of longer widgets on extended tab */
    inline int GetExtendedShortWidth() { return extendedShortWidth; };

    /** \brief return the startup file */
    inline QString GetStartupFile() { return startupFile; };

    /** \brief get "and" words */
    inline QList<QString> GetAndWords() { return andWords; };
    /** \brief get name additions */
    inline QSet<QString> GetNameAdditions() { return nameAdditions; };
    /** \brief get name prefixes */
    inline QSet<QString> GetNamePrefixes() { return namePrefixes; };

    /** \brief the currently used preview style */
    inline int GetCurrentPreviewStyle() { return currentPreviewStyle; };

    /** \brief how to display "Reference Details" (0: as dockable Window, 1: as Tab behind "Reference Overview) */
    inline int GetHowDisplayReferenceDetails() { return howDisplayReferenceDetails; };

public slots:
    /** \brief open options dialog
     *
     * When this modal dialog is closed by "OK", the settings are updated, the config directory is possibly
     * reread and the configDataChanged() signal is emitted (if any data changed)
     */
    void openSettingsDialog();


signals:
    /** \brief emitted when any of the data is changed by the user */
    void configDataChanged();

    /** \brief emitted when settings should be saved */
    void storeSettings();

    /** \brief emitted when the language has been changed */
    void languageChanged(QString languageID);

    /** \brief emitted when the config directory has been reread */
    void configDirReread();

    /** \brief emitted when the current file changed */
    void currentFileChanged(QString currentFile);

    /** \brief emitted when the stylesheet changed */
    void stylesheetChanged(QString stylesheet);

    /** \brief emitted when the style changed */
    void styleChanged(QString style);

    /** \brief emitted when the table font changes */
    void tableFontChanged(QString tableFontName, int tableFontSize);

protected:
    /** \brief this function reads the current settings from the internal QSettings object settings */
    void readSettings();
    /** \brief this function writes the current settings to the internal QSettings object settings */
    void writeSettings();
    /*! \brief read in the config directory tree

        This function does:
          - search for FDF files and parse them
        .
     */
    void initConfig();

    /*! \brief load tool config files */
    void loadConfigFiles();

    /*! \brief save tool config files */
    void saveConfigFiles();

    /** \brief QSettings object used for config options I/O */
    QSettings* settings;
    /** \brief if this is true, the config directory is reread in any case. This is set to false by initConfig() */
    bool readConfig;
    /** \brief the application this object manages the properties for */
    QApplication* app;
    /** \brief this object manages the FDF files found by the applicaion */
    fdfManager* fdf;

    PreviewStyleManager* previewStyles;

    quint16 proxyPort;
    QString proxyHost;
    int proxyType;

    /** \brief style to use in the application */
    QString style;
    /** \brief stylesheet to use in the application */
    QString stylesheet;
    /** \brief filename of INI file */
    QString iniFilename;
    /** \brief language of the user interface */
    QString languageID;
    /** \brief currently opened file */
    QString currentFile;
    /** \brief the last directory in the Open Database dialog */
    QString lastOpenDBDirectory;
    /** \brief the last directory in the New Database dialog */
    QString lastNewDBDirectory;
    /** \brief root of the "config" directory tree */
    QString configDirectory;
    /** \brief root of the "assets" directory tree */
    QString assetsDirectory;
    /** \brief name of the user */
    QString username;
    /** \brief font name for the table */
    QString tableFontName;
    /** \brief file to load on startup */
    QString startupFile;
    /** \brief font size for the table */
    int tableFontSize;
    /** \brief width of shorter widgets on extended tab */
    int extendedShortWidth;
    /** \brief width of longer widgets on extended tab */
    int extendedLongWidth;
    /** \brief default currency */
    QString defaultCurrency;
    /** \brief default status */
    QString defaultStatus;
    /** \brief special characters for en screen keyboard */
    QString specialCharacters;
    /** \brief "and" words */
    QList<QString> andWords;
    /** \brief name additions */
    QSet<QString> nameAdditions;
    /** \brief name prefixes */
    QSet<QString> namePrefixes;
    /** \brief the currently used preview style */
    int currentPreviewStyle;
    /** \brief how to display "Reference Details" (0: as dockable Window, 1: as Tab behind "Reference Overview) */
    int howDisplayReferenceDetails;

};

#endif // PROGRAMOPTIONS_H
