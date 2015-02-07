#ifndef DOITHREAD_H
#define DOITHREAD_H

#include "ls3progressthread.h"
#include <QThread>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QPointer>
#include "ls3pluginservices.h"
#include <QSet>
#include <QString>
#include <QList>
#include "wwwthread.h"


#define CROSSREF_API_KEY "doi_litsoz3@jkrieger.de"

class DoiThread; // forward

class DoiThreadHelper: public QObject {
        Q_OBJECT
    public:
        explicit DoiThreadHelper(DoiThread* doi) {
            this->base=doi;
        };
    public slots:
        void DoiReplyComplete();

        void requestWork();

        void WWWReplyComplete();
        void UrlStoreReplyComplete();
        void fromWWWReplyComplete() ;
        void done();
        void doneDownloading();
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    private:
        DoiThread* base;
};

/*! \brief this class is a thread with the possibility to read data from a webpage


    It supports several request types:
      - DOI:
        -# ask crossref for metadata on the DOI
        -# try to resolve the DOI (also done if crossref does not return metadata) and read common
           metadata encoded in the resolved webpage
        .
      .
    This class follows (up to 50) HTTP redirects.
 */
class DoiThread : public WWWThread {
        Q_OBJECT
    public:
        friend class DoiThreadHelper;
        explicit DoiThread(QObject *parent = 0);

        void requestFromDoi(const QString& doi, LS3PluginServices *services);

        bool preRun();
    signals:
        
    protected slots:
        void DoiReplyComplete(QNetworkReply *reply_in=NULL);
    protected:
        DoiThreadHelper* doiHelper;
        enum {
            scanDOICrossref=11
        };

        /** \brief DOI we are currently retrieving data for */
        QString lastDoi;
        QNetworkRequest createDoiCrossrefRequest(QString DOI);
        QNetworkRequest createDoiResolveRequest(QString DOI);

};

#endif // DOITHREAD_H
