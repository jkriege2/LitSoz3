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

#include "doithread.h"

DoiThread::DoiThread(QObject *parent) :
    WWWThread(parent)
{
}


void DoiThread::requestFromDoi(const QString &doi, LS3PluginServices* services) {
    setName(tr("requesting metadata from DOI %1 ...").arg(doi));

    namePrefixes=services->GetNamePrefixes();
    nameAdditions=services->GetNameAdditions();
    andWords=services->GetAndWords();
    proxy=services->getNetworkProxy();

    lastDoi=doi.trimmed().simplified();
    if (!lastDoi.startsWith("10.")) {
        int idx=lastDoi.indexOf("10.");
        if (idx>=0) lastDoi=lastDoi.mid(idx);
    }


    running=true;
    lastError="";

    redirectCount=0;

    newRecord.clear();
    newRecord["doi"]=lastDoi;
    newRecord["type"]="article";

    currentInstruction=scanNone;
    if (!lastDoi.isEmpty()) currentInstruction=scanDOICrossref;

}

bool DoiThread::preRun() {
    doiHelper=new DoiThreadHelper(this);
    //qDebug()<<"DoiThread::preRun():"<<currentThread();
    bool ok=WWWThread::preRun();
    if (currentInstruction==scanDOICrossref) {
        //qDebug()<<"  CrossRef: creating network request ... ";
        QNetworkRequest request=createDoiCrossrefRequest(lastDoi);
        //qDebug()<<"  CrossRef: calling get() ... ";
        QNetworkReply* reply = manager->get(request);
        connect(reply, SIGNAL(finished()), doiHelper, SLOT(DoiReplyComplete()));
        //qDebug()<<"  CrossRef: calling exec() ... ";
        ok=true;
    }
    //qDebug()<<"DoiThread::preRun(): exit";
    return ok;
}


QNetworkRequest DoiThread::createDoiResolveRequest(QString DOI) {
    QString urlstr="http://dx.doi.org/%1";
    QUrl DOIUrl=QUrl(urlstr.arg(DOI));

    QNetworkRequest request;
    request.setUrl(DOIUrl);
    request.setRawHeader("User-Agent", WWWTHREAD_USER_AGENT);

    return request;
}

QNetworkRequest DoiThread::createDoiCrossrefRequest(QString DOI) {
    QString urlstr="http://www.crossref.org/openurl/?id=doi:%1&noredirect=true&pid=%2&format=unixref";
    QUrl DOIUrl=QUrl(urlstr.arg(DOI).arg(CROSSREF_API_KEY));

    QNetworkRequest request;
    request.setUrl(DOIUrl);
    request.setRawHeader("User-Agent", WWWTHREAD_USER_AGENT);

    return request;
}


void DoiThread::DoiReplyComplete (QNetworkReply* reply_in) {
    //qDebug()<<"DoiReplyComplete():   ..."<<currentThread();
    QNetworkReply* resp=reply_in;
    if (!resp) resp=qobject_cast<QNetworkReply*>(sender());
    if (!resp) return;

    if (m_wasCanceled) {
        resp->abort();
        resp->deleteLater();
        done();
        return;
    }

    //qDebug()<<"DoiReplyComplete():   errorcode="<<resp->error()<<"   url="<<resp->url();
    if (!resp->attribute(QNetworkRequest::RedirectionTargetAttribute).toString().isEmpty()) {
        if (redirectCount<WWWTHREAD_MAX_REDIRECTS) {
            // FOLLOWING A NEW URL (URL redirectrion)
            QUrl newUrl=resp->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            if (newUrl.isRelative()) newUrl=resp->url().resolved(newUrl);
            QNetworkRequest request;
            request.setUrl(newUrl);
            request.setRawHeader("User-Agent", WWWTHREAD_USER_AGENT);
            QNetworkReply *reply = manager->get(request);
            connect(reply, SIGNAL(finished()), doiHelper, SLOT(DoiReplyComplete()));
            resp->abort();
            resp->deleteLater();
            redirectCount++;
            //qDebug()<<newUrl;
            setMessage(tr("following new URL: %1 ...").arg(newUrl.toString()));
        } else {
            if (currentInstruction==scanDOICrossref) {
                // OK, we didn't find info for the paper on CrossRef, so we
                // try to simply resolve the DOI and extract the metadata from
                // the webpage found in the resolving step.

                currentInstruction=scanCommonMetadata;

                QNetworkRequest request=createDoiResolveRequest(lastDoi);
                QNetworkReply *reply = manager->get(request);
                connect(reply, SIGNAL(finished()), doiHelper, SLOT(DoiReplyComplete()));
                resp->abort();
                resp->deleteLater();
                redirectCount=0;
                setMessage(tr("resolving DOI: %1 ...").arg(request.url().toString()));
            } else {
                setError(tr("too many redirects"));
                done();
            }
        }
    } else {
        if (resp->isFinished()) {
            if (resp->error()==QNetworkReply::NoError) {
                // SO WE FOUND A VALID WEBPAGE ... LET's READ THE CONTENTS

                //qDebug()<<resp->rawHeaderList();
                //qDebug()<<resp->rawHeader("Content-Encoding");
                //qDebug()<<resp->rawHeader("Transfer-Encoding");
                QByteArray databa=resp->readAll();
                QByteArray encoding=guessEncoding(resp->rawHeader("Content-Encoding"), databa);
                //qDebug()<<encoding;
                //qDebug()<<databa;
                QString data=QTextCodec::codecForName(encoding)->toUnicode(databa);
                //qDebug()<<data;
                bool append=false;


                if (currentInstruction==scanCommonMetadata) {
                    // WE SHOULD EXTRACT METADATA FROM THE HTML PAGE
                    setMessage(tr("retrieving metadata from HTML ..."));
                    QMap<QString, QVariant> dc=extractMetadataFromHTML(data,namePrefixes, nameAdditions, andWords);
                    //qDebug()<<"\nscanCommonMetadata:  "<<dc;

                    if (!newRecord.contains("url")) newRecord["url"]=resp->url().toString();

                    if (dc.size()>0) {
                        QMapIterator<QString, QVariant> it(dc);
                        while (it.hasNext()) {
                            it.next();
                            if ((it.key()=="authors") || (it.key()=="editors")) {
                                if (!newRecord.contains(it.key())) newRecord[it.key()]=reformatAuthors(it.value().toString(), namePrefixes, nameAdditions, andWords);
                            } else {
                                if (!newRecord.contains(it.key()) || it.key()=="type") newRecord[it.key()]=it.value();
                            }
                        }
                    }
                    append=true;
                } else if (currentInstruction==scanDOICrossref) {
                    // retrieve data from crossref result
                    setMessage(tr("retrieving crossref data ..."));
                    QMap<QString, QVariant> dc=extractCrossrefUnixrefMetadata(data);

                    if (dc.size()>0) {
                        QMapIterator<QString, QVariant> it(dc);
                        while (it.hasNext()) {
                            it.next();
                            if ((it.key()=="authors") || (it.key()=="editors")) {
                                if (!newRecord.contains(it.key())) newRecord[it.key()]=reformatAuthors(it.value().toString(), namePrefixes, nameAdditions, andWords);
                            } else {
                                if (!newRecord.contains(it.key()) || it.key()=="type") newRecord[it.key()]=it.value();
                            }
                        }
                    }

                    // THERE MIGHT BE MORE METADATA ON THE WEBPAGE OF THE JOURNAL ... SO WE START A commonMetadata scan
                    currentInstruction=scanCommonMetadata;
                    QNetworkRequest request=createDoiResolveRequest(lastDoi);
                    QNetworkReply *reply = manager->get(request);
                    connect(reply, SIGNAL(finished()), doiHelper, SLOT(DoiReplyComplete()));
                    resp->abort();
                    resp->deleteLater();
                    redirectCount=0;
                    setMessage(tr("resolving DOI: %1 ...").arg(request.url().toString()));

                    append=false;
                }
                if (append) {
                    // WE SET append, SO WE FINISHED EXTRACTING DATA FOR THIS RECORD ... LET'S add the record and done the request/thread
                    //qDebug()<<"FINISHED: insert records";
                    done();
                }
            } else {
                // WE CAME ACROSS AN ERROR; SO WE done THE RAME WITH AN ERROR MESSAGE
                //qDebug()<<"FINISHED";
                if (resp->error()!=QNetworkReply::OperationCanceledError) {
                    setError(tr("network error. %1").arg(networkErrorToString(resp->error())));
                    done();
                }
            }
            resp->deleteLater();
        }
    }

    //qDebug()<<"DoiReplyComplete():   DONE";
}


void DoiThreadHelper::DoiReplyComplete()
{
    base->DoiReplyComplete(qobject_cast<QNetworkReply*>(sender()));
}

void DoiThreadHelper::requestWork() {
    base->startWork();
}

void DoiThreadHelper::WWWReplyComplete() {
    base->WWWReplyComplete(qobject_cast<QNetworkReply*>(sender()));
};
void DoiThreadHelper::UrlStoreReplyComplete() {
    base->UrlStoreReplyComplete(qobject_cast<QNetworkReply*>(sender()));
};
void DoiThreadHelper::fromWWWReplyComplete() {
    base->fromWWWReplyComplete(qobject_cast<QNetworkReply*>(sender()));
};
void DoiThreadHelper::done() {
    base->done();
};
void DoiThreadHelper::doneDownloading() {
    base->doneDownloading();
};
void DoiThreadHelper::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    base->downloadProgress(bytesReceived, bytesTotal);
};
