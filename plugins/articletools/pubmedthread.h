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

#ifndef PUBMEDTHREAD_H
#define PUBMEDTHREAD_H

#include "doithread.h"
class PubmedThread; // forward

class PubmedThreadHelper: public QObject {
        Q_OBJECT
    public:
        explicit PubmedThreadHelper(PubmedThread* doi) {
            this->base=doi;
        };
    public slots:
        void PubmedReplyComplete();
        void DoiReplyComplete();

        void requestWork();

        void WWWReplyComplete();
        void UrlStoreReplyComplete();
        void fromWWWReplyComplete() ;
        void done();
        void doneDownloading();
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    private:
        PubmedThread* base;
};

class PubmedThread : public DoiThread
{
        Q_OBJECT
    public:
        friend class PubmedThreadHelper;
        explicit PubmedThread(QObject *parent = 0);
        
        void setCatalog(const QString& catalog);

        void requestFromPMID(const QString& pmid, LS3PluginServices *services);
        void requestFromPMIDBooks(const QString& pmid, LS3PluginServices *services);
        void requestFromPMIDandDOI(const QString& pmid, const QString& DOI, LS3PluginServices *services);

        bool preRun();
    signals:

    protected slots:
        void PubmedReplyComplete(QNetworkReply* reply_in=NULL);
    protected:
        PubmedThreadHelper* pubmedHelper;
        enum {
            scanPubmed=21,
            scanPubmedAndDoi=22,
            scanPubmedBooks=23,
            scanPubmedResolveIDs=24
        };

        /** \brief DOI we are currently retrieving data for */
        QString lastPMID;
        QString lastSearch;
        static int max_result_count;
        QString catalog;

    public:
        static QNetworkRequest createPMIDRequest(QString PMID, const QString &catalog=QString("pubmed"));
        static QNetworkRequest createPubmedSearchRequest(QString phrase, int maxresults=100, const QString &catalog=QString("pubmed"));
        static QNetworkRequest createPMIDBooksRequest(QString PMID);
        static QNetworkRequest createPubmedResolveRequest(QString PMID);
};

#endif // PUBMEDTHREAD_H
