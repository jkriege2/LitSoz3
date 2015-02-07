#ifndef LS3PLUGINARTICLETOOLS_H
#define LS3PLUGINARTICLETOOLS_H

#include "ls3pluginbase.h"
#include <QWidget>
#include <QComboBox>
#include <QNetworkReply>
#include <QProgressDialog>
#include <QLineEdit>
#include <QNetworkReply>
#include "jkstyledbutton.h"
#include "qenhancedlineedit.h"
#include "qmodernprogresswidget.h"
#include "wwwthread.h"
#include "doithread.h"
#include "pubmedthread.h"
#include "filetools.h"
#include "analyzepdfthread.h"

/*! \brief Import books by Article plugin
*/
class LS3PluginArticleTools : public LS3PluginBase {
        Q_OBJECT

    public:
        LS3PluginArticleTools();
        virtual ~LS3PluginArticleTools();


        /** \brief name for the plugin */
        virtual QString getName() { return tr("article tools"); }

        /** \brief short description for the plugin */
        virtual QString getDescription() { return tr("import reference data from Internet for a given Article, importPDF files and extract MetaData ..."); }

        /** \brief author the plugin */
        virtual QString getAuthor() { return tr("J. Krieger"); }

        /** \brief copyright information the plugin */
        virtual QString getCopyright() { return tr("(c) 2011-2012 by J. W. Krieger"); }

        /** \brief weblink for the plugin */
        virtual QString getWeblink() { return tr("http://www.jkrieger.de/"); }

        /** \brief icon for the plugin */
        virtual QString getIconFilename() { return QString(":/articletools/logo.png"); }

        /** \brief initialize the plugin (create buttons, menu entries, register import/export objects ...) */
        virtual void init(LS3PluginServices* pluginServices);

        /** \brief de-initialize the plugin (clean up when program is closed) */
        virtual void deinit(LS3PluginServices* pluginServices) ;

        /** \brief this function is called when a datastore is closed */
        virtual void openData(LS3Datastore* datastore);

        /** \brief this function is called when a datastore is closed */
        virtual void closeData(LS3Datastore* datastore);

    protected:
        QAction* actFillFromDoi;
        QAction* actFillFromPMID;
        QAction* actFillFromPMCID;
        QAction* actDownloadPDFFromPMCID;
        QAction* actDownloadPDFFromArxiv;
        QAction* actDownloadPDFFromDoi;
        QAction* actPasteDoi;
        QAction* actPasteWebpage;
        QAction* actPasteFromWebpage;
        QAction* actPastePDFFromURL;
        QAction* actInsertePDFFromURL;
        QAction* actPastePMID;
        QAction* actShowArticleToolWindow;
        QAction* actRenameFiles;
        QAction* actRenameFilesCurrentrecord;
        QAction* actInsertFilesArticle;
        QAction* actDockDisplay;
        QAction* actPubmedSearch;
        QComboBox* cmbDropType;
        /** \brief dock widget holding the article search tools window */
        QDockWidget* docArticleTools;
        /** \brief submenu for the article tools */
        QMenu* menuArticleTools;
        QToolBar* tbTools;

        QWidget* widMain;
        QComboBox* cmbWhat;
        QEnhancedLineEdit* edtArticle;
        JKStyledButton* btnExecute;
        QModernProgressDialog* dlgProgress;
    public slots:
        void searchPubmed();

        void clipboardDataChanged();
        void pasteDoi();
        void pastePMID();
        void articleInsert();
        void fillFromDoi();
        void fillFromPMID();
        void pasteWebpage();
        void pasteFromWebpage();
        void pastePDFFromURL();
        void insertPDFFromURL();

        void insertFilesArticle();
        void typeChanged(QString item);
        void actionChanged(int item);
        void filesDropped(QStringList files, QString type, int action);
        void renameFiles();
        void renameFilesCurrentRecord();
        void insertFiles(const QStringList& files, const QString& type=QString("article"), int action=0);
        void insertFile(const QString& filename, const QString& type=QString("article"), int action=0);


        void insertDOI(const QString& DOI, const QString& recordUUID=QString(""));
        void insertPubmed(const QString& PMID, const QString& recordUUID=QString(""));
        void insertPubmedAndDoi(const QString& PMID, const QString& DOI, const QString& recordUUID=QString(""));
        void insertWebpage(const QString& WWW, const QString& recordUUID=QString(""), const QMap<QString, QVariant>& newRecord=QMap<QString, QVariant>());
        void insertFromWebpage(const QString& WWW);
        void downloadFile(QString url, QString filename);
        //void searchPubmed(const QString& searchphrase);

        void downloadAndAnalyzeFile(QString url, QString filename);
        void clearAndRestoreFocus();
        void scanPDFAndInsert(QString record, QString filename);
        void insertMoreRecords(QList<QMap<QString, QVariant> > moreMetadata);
        void insertRecord(QMap<QString, QVariant> data, bool scanPubmedDoi=false);
        void storeInRecord(QString record, QMap<QString, QVariant> data, bool scanPubmedDoi=false);

        void fillFromPMCID();
        void downloadPDFFromPMCID();
        void downloadPDFFromArxiv();
        void downloadPDFFromDoi();
};


#endif // LS3PLUGINARTICLETOOLS_H
