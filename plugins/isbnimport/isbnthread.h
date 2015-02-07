#ifndef ISBNTHREAD_H
#define ISBNTHREAD_H
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
#include "htmltools.h"
#include "bibtools.h"

#define ISBNThread_MAX_REDIRECTS 50
//#define ISBNThread_USER_AGENT "LitSoz/3.0 (Windows)"
#define ISBNThread_USER_AGENT "Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:25.0) Gecko/20100101 Firefox/25.0"
#define ISBNThread_TIMEOUT_MS 60000
#define ISBNThread_TIMEOUT_DOWNLOAD_MS (30*60000)

class ISBNThread; // forward
class ISBNThreadHelper: public QObject {
        Q_OBJECT
    public:
        explicit ISBNThreadHelper(ISBNThread* base) {
            this->base=base;
        }
    public slots:
        void requestWork();
        void ISBNdbReplyComplete();
        void GoogleBooksReplyComplete();
        void CopacReplyComplete();

    protected:
        ISBNThread* base;

};

/*! \brief this class is a thread with the possibility to read data from a webpage



    This class follows (up to ISBNThread_MAX_REDIRECTS, currently 50) HTTP redirects.
 */
class ISBNThread : public LS3ProgressThread {
        Q_OBJECT
    public:
        friend class ISBNThreadHelper;
        explicit ISBNThread(QObject *parent = 0);
        virtual ~ISBNThread();

        void requestGoogleBooks(const QString &isbn, LS3PluginServices *services);
        void requestISBNdb(const QString& isbn, LS3PluginServices *services);
        void requestCOPAC(const QString& isbn, LS3PluginServices *services);
        void initRecordData(const QMap<QString, QVariant>& newRecord);

        void setStoreInRecord(QString record);
        void setInsertnewRecord();
        void setNoInsertRecord();
    protected slots:
        virtual void startWork();

    protected:
        ISBNThreadHelper* isbnHelper;

        virtual void run();

        virtual bool preRun();

        enum {
            scanNone=0,
            scanISBNDB=1,
            scanGoogleBooks=2,
            scanCopac=3,
        };
        int currentInstruction;
        /** \brief indicates whether a retrieve is running */
        bool running;

        QWidget* widMain;
        QNetworkAccessManager *manager;
        QNetworkProxy proxy;
        QList<QPointer<QNetworkReply> > replies;

        /** \brief coun the redirects during the current network retrival */
        int redirectCount;
        QString lastISBN;
        /** \brief record we are currently assembling */
        QMap<QString, QVariant> newRecord;
        QString lastError;

        QSet<QString> namePrefixes;
        QSet<QString> nameAdditions;
        QList<QString> andWords;

        QList<QMap<QString, QVariant> > moreMetadata;
        QNetworkRequest createISBNdbRequest(const QString& isbn);
        QNetworkRequest createGoogleBooksRequest(const QString &isbn);
        QNetworkRequest createCopacRequest(const QString &isbn);
        QString networkErrorToString(QNetworkReply::NetworkError err);

        int insertInRecord;
        QString recordUUID;

    protected slots:
        void ISBNdbReplyComplete(QNetworkReply *reply_in=NULL);
        void GoogleBooksReplyComplete(QNetworkReply *reply_in=NULL);
        void CopacReplyComplete(QNetworkReply *reply_in=NULL);
        void done();

    signals:
        void insertRecords(QMap<QString, QVariant> data);
        void storeInRecord(QString record, QMap<QString, QVariant> data);
        void haveMoreMetadataItems(QList<QMap<QString, QVariant> > data);
        
    public slots:
        
};

#endif // ISBNTHREAD_H
