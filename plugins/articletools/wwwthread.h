#ifndef WWWTHREAD_H
#define WWWTHREAD_H
#include "ls3progressthread.h"
#include <QThread>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QPointer>
#include "ls3pluginservices.h"
#include <QSet>
#include <QString>
#include <QList>
#include <QDebug>
#include <iostream>
#include <QtXml>
#include "bibtools.h"
#include "htmltools.h"
#include "crossreftools.h"
#include "pubmedtools.h"
#include "renamefilesdialog.h"
#include "renamefilesprogressdialog.h"
#include "filenametools.h"
#include "pdfdropdockwidget.h"
#include "xmptools.h"
#include "pdftools.h"


#define WWWTHREAD_MAX_REDIRECTS 50
#define WWWTHREAD_USER_AGENT "Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:25.0) Gecko/20100101 Firefox/25.0"
//"LitSoz/3.0 (Windows)"
#define WWWTHREAD_TIMEOUT_MS 30000
#define WWWTHREAD_TIMEOUT_DOWNLOAD_MS (30*60000)

class WWWThread; // forward

class WWWThreadHelper: public QObject {
        Q_OBJECT
    public:
        explicit WWWThreadHelper(WWWThread* www) {
            this->www=www;
        }

    public slots:
        void requestWork();

        void WWWReplyComplete();
        void UrlStoreReplyComplete();
        void fromWWWReplyComplete() ;
        void done();
        void doneDownloading();
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    private:
        WWWThread* www;
};


/*! \brief this class is a thread with the possibility to read data from a webpage



    This class follows (up to WWWTHREAD_MAX_REDIRECTS, currently 50) HTTP redirects.
 */
class WWWThread : public LS3ProgressThread {
        Q_OBJECT
    public:
        friend class WWWThreadHelper;

        explicit WWWThread(QObject *parent = 0);
        virtual ~WWWThread();

        void initDownloadRequest(const QString& url, const QString& target,  LS3PluginServices *services);
        void requestWebpageMetadata(const QString& url, LS3PluginServices *services);
        void requestMetadatasFromWebpage(const QString& url, LS3PluginServices *services);
        void initRecordData(const QMap<QString, QVariant>& newRecord);
        void addToRecordData(const QString& id, const QVariant& data);

        void setStoreInRecord(QString record);
        void setInsertnewRecord();
        void setNoInsertRecord();

    protected slots:
        virtual void startWork();

    protected:
        WWWThreadHelper* wwwHelper;

        virtual void run();


        virtual bool preRun();

        enum {
            scanNone=0,
            scanCommonMetadata=1,
            scanDownload=2

        };
        bool insertDirectlyAsRecord;
        int currentInstruction;
        /** \brief indicates whether a retrieve is running */
        bool running;

        QWidget* widMain;
        QNetworkAccessManager *manager;
        QNetworkProxy proxy;
        QList<QPointer<QNetworkReply> > replies;

        /** \brief coun the redirects during the current network retrival */
        int redirectCount;
        QString lastWWW;
        QString storeToFilename;
        /** \brief record we are currently assembling */
        QMap<QString, QVariant> newRecord;
        QString lastError;

        QSet<QString> namePrefixes;
        QSet<QString> nameAdditions;
        QList<QString> andWords;

        QList<QMap<QString, QVariant> > moreMetadata;
        QNetworkRequest createURLRequest(QString url);
        static QString networkErrorToString(QNetworkReply::NetworkError err);

        int insertInRecord;
        QString recordUUID;

    protected slots:
        void WWWReplyComplete(QNetworkReply *reply_in=NULL);
        void UrlStoreReplyComplete(QNetworkReply *reply_in=NULL);
        void fromWWWReplyComplete(QNetworkReply *reply_in=NULL);
        void done();
        void doneDownloading();
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    signals:
        void insertRecords(QMap<QString, QVariant> data);
        void storeInRecord(QString record, QMap<QString, QVariant> data);
        void haveMoreMetadataItems(QList<QMap<QString, QVariant> > data);
        void downloadComplete(QString record, QString filename);
        
    public slots:
        
};

#endif // WWWTHREAD_H
