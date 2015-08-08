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

#include "wwwthread.h"

WWWThread::WWWThread(QObject *parent) :
    LS3ProgressThread(parent)
{
    insertInRecord=-1;
    manager=NULL;
    insertDirectlyAsRecord=true;
}

WWWThread::~WWWThread()
{
    if (manager) delete manager;
}

void WWWThread::initDownloadRequest(const QString &url, const QString &target, LS3PluginServices *services) {
    setName(tr("download URL '%1'' ...").arg(url));

    namePrefixes=services->GetNamePrefixes();
    nameAdditions=services->GetNameAdditions();
    andWords=services->GetAndWords();
    proxy=services->getNetworkProxy();

    lastWWW=url;
    storeToFilename=target;

    running=true;
    lastError="";

    redirectCount=0;
    setNoInsertRecord();

    newRecord.clear();

    currentInstruction=scanDownload;
}

void WWWThread::requestWebpageMetadata(const QString &url, LS3PluginServices *services) {
    setName(tr("request metadata for URL '%1'' ...").arg(url));

    namePrefixes=services->GetNamePrefixes();
    nameAdditions=services->GetNameAdditions();
    andWords=services->GetAndWords();
    proxy=services->getNetworkProxy();

    lastWWW=url;

    running=true;
    lastError="";

    redirectCount=0;
    newRecord.clear();

    currentInstruction=scanCommonMetadata;
}

void WWWThread::requestMetadatasFromWebpage(const QString &url, LS3PluginServices *services)
{
    setName(tr("request metadatas from URL '%1'' ...").arg(url));

    namePrefixes=services->GetNamePrefixes();
    nameAdditions=services->GetNameAdditions();
    andWords=services->GetAndWords();
    proxy=services->getNetworkProxy();

    lastWWW=url;

    running=true;
    lastError="";

    redirectCount=0;
    newRecord.clear();

    insertDirectlyAsRecord=false;

    currentInstruction=scanCommonMetadata;
}

void WWWThread::initRecordData(const QMap<QString, QVariant> &newRecord) {
    this->newRecord=newRecord;
}

void WWWThread::addToRecordData(const QString &id, const QVariant &data)
{
    this->newRecord[id]=data;
}

void WWWThread::setStoreInRecord(QString record) {
    insertInRecord=0;
    recordUUID=record;
}

void WWWThread::setInsertnewRecord() {
    insertInRecord=-1;
}

void WWWThread::setNoInsertRecord() {
    insertInRecord=-2;
}

void WWWThread::startWork()
{
    if (!preRun()) quit();
}


void WWWThread::run() {
    //qDebug()<<"WWWThread::run()";
    manager = new QNetworkAccessManager(NULL);
    //qDebug()<<"WWWThread::run(): new AccessManager";
    manager->setProxy(proxy);
    //qDebug()<<"WWWThread::run(): setProxy";
    manager->moveToThread(this);
    //qDebug()<<"WWWThread::run(): moveToThread";

    // injects a call to startWork() running in the WWWThread, not the main thread ...
    // note: QThread LIVES in the main thread, although it REFERENCES a new thread!!!
    wwwHelper=new WWWThreadHelper(this);
    wwwHelper->moveToThread(this);
    QTimer::singleShot(10, wwwHelper, SLOT(requestWork()));

    //if (preRun()) {
        //qDebug()<<"WWWThread::run(): preRun()";
        exec();
    //}
}

bool WWWThread::preRun() {
    //qDebug()<<"WWWThread::preRun():"<<currentThread();
    bool ok=false;
    if (currentInstruction==scanDownload) {
        QNetworkRequest request=createURLRequest(lastWWW);
        QNetworkReply* reply = manager->get(request);
        connect(reply, SIGNAL(finished()), wwwHelper, SLOT(UrlStoreReplyComplete()));
        ok=true;
    }
    if (currentInstruction==scanCommonMetadata) {
        QNetworkRequest request=createURLRequest(lastWWW);
        QNetworkReply* reply = manager->get(request);
        connect(reply, SIGNAL(finished()), wwwHelper, SLOT(fromWWWReplyComplete()));
        ok=true;
    }
    //qDebug()<<"WWWThread::preRun(): exit";
    return ok ;
}











QNetworkRequest WWWThread::createURLRequest(QString url) {
    QUrl PMIDUrl=QUrl(url);

    QNetworkRequest request;
    request.setUrl(PMIDUrl);
    request.setRawHeader("User-Agent", WWWTHREAD_USER_AGENT);

    return request;
}



QString WWWThread::networkErrorToString(QNetworkReply::NetworkError err) {
    switch(err) {
        case QNetworkReply::ConnectionRefusedError: return tr("connection refused"); break;
        case QNetworkReply::RemoteHostClosedError: return tr("remote host closed connection"); break;
        case QNetworkReply::HostNotFoundError: return tr("host not found"); break;
        case QNetworkReply::TimeoutError: return tr("timeout"); break;
        case QNetworkReply::OperationCanceledError: return tr("operation canceled"); break;
        case QNetworkReply::SslHandshakeFailedError: return tr("SSL handshake failed"); break;
        case QNetworkReply::TemporaryNetworkFailureError: return tr("temporary network failure"); break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    case QNetworkReply::NetworkSessionFailedError: return tr("network session failes"); break;
    case QNetworkReply::BackgroundRequestNotAllowedError: return tr("background request not allowede"); break;
    case QNetworkReply::ContentConflictError: return tr("content conflict"); break;
    case QNetworkReply::ContentGoneError: return tr("content gone"); break;

    case QNetworkReply::InternalServerError: return tr("internal server error"); break;
    case QNetworkReply::OperationNotImplementedError: return tr("operation not implemented"); break;
    case QNetworkReply::ServiceUnavailableError: return tr("servie unavailable"); break;
#endif

        case QNetworkReply::ProxyConnectionRefusedError: return tr("proxy connection refused"); break;
        case QNetworkReply::ProxyConnectionClosedError: return tr("proxy closed connection"); break;
        case QNetworkReply::ProxyNotFoundError: return tr("proxy not found"); break;
        case QNetworkReply::ProxyTimeoutError: return tr("proxy timeout"); break;
        case QNetworkReply::ProxyAuthenticationRequiredError: return tr("proxy authentication required"); break;
        case QNetworkReply::ContentAccessDenied: return tr("access denied"); break;
        case QNetworkReply::ContentOperationNotPermittedError: return tr("content operation not permitted"); break;
        case QNetworkReply::ContentNotFoundError: return tr("content not found"); break;
        case QNetworkReply::AuthenticationRequiredError: return tr("authentication required"); break;
        case QNetworkReply::ContentReSendError: return tr("content resend"); break;
        case QNetworkReply::ProtocolUnknownError: return tr("protocol unknown"); break;
        case QNetworkReply::ProtocolInvalidOperationError: return tr("invalid protocol operation"); break;
        case QNetworkReply::UnknownNetworkError: return tr("unknown network"); break;
        case QNetworkReply::UnknownProxyError: return tr("unknown proxy"); break;
        case QNetworkReply::UnknownContentError: return tr("unknown content"); break;
        case QNetworkReply::ProtocolFailure: return tr("protocol failure"); break;
        default : return tr("unknown error"); break;
    }
    return QString("");

}



void WWWThread::WWWReplyComplete (QNetworkReply *reply_in) {
    QNetworkReply* resp=reply_in;
    if (!resp) resp=qobject_cast<QNetworkReply*>(sender());
    if (!resp) return;

    if (m_wasCanceled) {
        resp->abort();
        resp->deleteLater();
        done();
        return;
    }
    //qDebug()<<resp->error()<<resp->readAll();
    if (!resp->attribute(QNetworkRequest::RedirectionTargetAttribute).toString().isEmpty()) {
        if (redirectCount<WWWTHREAD_MAX_REDIRECTS) {
            QUrl newUrl=resp->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            if (newUrl.isRelative()) newUrl=resp->url().resolved(newUrl);
            QNetworkRequest request;
            request.setUrl(newUrl);
            request.setRawHeader("User-Agent", WWWTHREAD_USER_AGENT);
            QNetworkReply *reply = manager->get(request);
            connect(reply, SIGNAL(finished()), wwwHelper, SLOT(WWWReplyComplete()));

            resp->abort();
            resp->deleteLater();
            redirectCount++;
            //qDebug()<<newUrl;
            setMessage(tr("following new URL: %1 ...").arg(lastWWW).arg(newUrl.toString()));
        } else {
            setError("too many redirects");
            done();
        }
    } else {
        if (resp->isFinished()) {
            if (resp->error()==QNetworkReply::NoError) {


                QByteArray databa=resp->readAll();
                QByteArray encoding=guessEncoding(resp->rawHeader("Content-Encoding"), databa);
                //qDebug()<<encoding;
                //qDebug()<<databa;
                QString data=QTextCodec::codecForName(encoding)->toUnicode(databa);

                setMessage(tr("retrieving metadata from HTML ...").arg(lastWWW));
                QMap<QString, QVariant> dc=extractHTMLMetadata(data, namePrefixes, nameAdditions, andWords);

                if (!newRecord.contains("url")) newRecord["url"]=resp->url().toString();

                if (dc.size()>0) {
                    QMapIterator<QString, QVariant> it(dc);
                    while (it.hasNext()) {
                        it.next();
                        if ((it.key()=="authors") || (it.key()=="editors")) {
                            if (!newRecord.contains(it.key())) newRecord[it.key()]=reformatAuthors(it.value().toString(), namePrefixes, nameAdditions, andWords);
                        } else {
                            if (!newRecord.contains(it.key())) newRecord[it.key()]=it.value();
                        }
                    }
                }
                done();
            } else {
                setError(resp->errorString());
                done();
            }

            resp->deleteLater();
        }
    }
}



void WWWThread::UrlStoreReplyComplete (QNetworkReply* reply_in) {
    QNetworkReply* resp=reply_in;
    if (!resp) resp=qobject_cast<QNetworkReply*>(sender());
    if (!resp) return;

    if (m_wasCanceled) {
        resp->abort();
        resp->deleteLater();
        running=false;
        quit();
        return;
    }
    //qDebug()<<resp->error()<<resp->readAll();
    if (!resp->attribute(QNetworkRequest::RedirectionTargetAttribute).toString().isEmpty()) {
        if (redirectCount<WWWTHREAD_MAX_REDIRECTS) {
            QUrl newUrl=resp->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            if (newUrl.isRelative()) newUrl=resp->url().resolved(newUrl);
            QNetworkRequest request;
            request.setUrl(newUrl);
            request.setRawHeader("User-Agent", WWWTHREAD_USER_AGENT);
            QNetworkReply *reply = manager->get(request);
            connect(reply, SIGNAL(finished()), wwwHelper, SLOT(UrlStoreReplyComplete()));
            connect(reply, SIGNAL(downloadProgress(qint64, qint64)), wwwHelper, SLOT(downloadProgress(qint64, qint64)));
            replies.append(reply);
            resp->abort();
            resp->deleteLater();
            redirectCount++;
            //qDebug()<<newUrl;
            setMessage(tr("following new URL: %1 ...").arg(newUrl.toString()));
        } else {

            setError(tr("too many redirects"));
            doneDownloading();
        }
    } else {
        if (resp->isFinished()) {
            if (resp->error()==QNetworkReply::NoError) {

                setMessage(tr("saving URL to '%1' ...").arg(storeToFilename));


                QFile f(storeToFilename);
                if (f.open(QIODevice::WriteOnly)) {
                    f.write(resp->readAll());
                    f.close();
                } else {
                    setError(tr("error saving file '%2': %1").arg(f.errorString()).arg(storeToFilename));
                }

            } else {
                setError(resp->errorString());
            }
            doneDownloading();

            resp->deleteLater();
            replies.clear();
        }
    }
}




void WWWThread::fromWWWReplyComplete (QNetworkReply *reply_in) {
    QNetworkReply* resp=reply_in;
    if (!resp) resp=qobject_cast<QNetworkReply*>(sender());
    if (!resp) return;

    if (m_wasCanceled) {
        resp->abort();
        resp->deleteLater();
        done();
        return;
    }

    //qDebug()<<resp->error()<<resp->readAll();
    if (!resp->attribute(QNetworkRequest::RedirectionTargetAttribute).toString().isEmpty()) {
        if (redirectCount<WWWTHREAD_MAX_REDIRECTS) {
            QUrl newUrl=resp->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            if (newUrl.isRelative()) newUrl=resp->url().resolved(newUrl);
            QNetworkRequest request;
            request.setUrl(newUrl);
            request.setRawHeader("User-Agent", WWWTHREAD_USER_AGENT);
            QNetworkReply *reply = manager->get(request);
            connect(reply, SIGNAL(finished()), wwwHelper, SLOT(fromWWWReplyComplete()));

            replies.append(reply);
            resp->abort();
            resp->deleteLater();
            redirectCount++;
            //qDebug()<<newUrl;
            setMessage(tr("following new URL: %1 ...").arg(newUrl.toString()));
        } else {

            setError(tr("too many redirects"));
            done();
        }
    } else {
        if (resp->isFinished()) {
            if (resp->error()==QNetworkReply::NoError) {


                QByteArray databa=resp->readAll();
                QByteArray encoding=guessEncoding(resp->rawHeader("Content-Encoding"), databa);
                //qDebug()<<encoding;
                //qDebug()<<databa;
                QString data=QTextCodec::codecForName(encoding)->toUnicode(databa);

                setMessage(tr("retrieving metadata from HTML ..."));

                if (resp->error()==QNetworkReply::NoError) {


                    QMap<QString, QVariant> dc=extractHTMLMetadata(data, namePrefixes, nameAdditions, andWords);
                    QList<QMap<QString, QVariant> >coins=extractCoins(data, namePrefixes, nameAdditions, andWords);
                    //qDebug()<<dc;

                    if (!newRecord.contains("url")) newRecord["url"]=resp->url().toString();

                    if (dc.size()>0) {
                        QMapIterator<QString, QVariant> it(dc);
                        while (it.hasNext()) {
                            it.next();
                            if ((it.key()=="authors") || (it.key()=="editors")) {
                                if (!newRecord.contains(it.key())) newRecord[it.key()]=reformatAuthors(it.value().toString(), namePrefixes, nameAdditions, andWords);
                            } else {
                                if (!newRecord.contains(it.key())) newRecord[it.key()]=it.value();
                            }
                        }
                    }
                    if (coins.size()==1 && insertDirectlyAsRecord) { // if we find exactly one COinS record, we asume it describes the page:
                        QMapIterator<QString, QVariant> i(coins[0]);
                        while (i.hasNext()) {
                            i.next();
                            if ((i.key()=="authors") || (i.key()=="editors")) {
                                newRecord[i.key()]=reformatAuthors(i.value().toString(), namePrefixes, nameAdditions, andWords);
                            } else {
                                newRecord[i.key()]=i.value();
                            }
                        }
                    }
                    if (!insertDirectlyAsRecord) {
                        coins.append(newRecord);
                    }
                    if (coins.size()>1 || (insertDirectlyAsRecord && coins.size()>0)) { // if we found more than one coins record on the page: this page describes multiple records and we provide a possibility to add all of them
                        moreMetadata=coins;
                        //qDebug()<<"Found "<<moreMetadata.size()<<" COINS MetadataBlocks";
                        emit haveMoreMetadataItems(moreMetadata);
                    }
                }
                done();
            } else {
                setError(resp->errorString());
                done();
            }
            resp->deleteLater();
            replies.clear();
        }
    }
}

void WWWThread::done() {
    if (m_wasCanceled) {
        running=false;
    } else if (!m_wasError) {
        if (insertDirectlyAsRecord) {
            if (insertInRecord>=0) {
                emit storeInRecord(recordUUID, newRecord);
            } else if (insertInRecord==-1){
                emit insertRecords(newRecord);
            }
        }
        running=false;
    } else {
        running=false;
    }
    quit();
}

void WWWThread::doneDownloading() {
    //qDebug()<<"doneDownloading():  "<<m_wasCanceled<<m_wasError<<insertInRecord<<recordUUID<<storeToFilename;
    if (m_wasCanceled) {
        running=false;
    } else if (!m_wasError) {
        if (insertInRecord>=0) {
            emit downloadComplete(recordUUID, storeToFilename);
        } else {
            emit downloadComplete("", storeToFilename);
        }
        running=false;
    } else {
        running=false;
    }
    quit();
}

void WWWThread::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    emit setRange(0, 100);
    emit setProgress((double)bytesReceived/(double)bytesTotal*100.0);
}



void WWWThreadHelper::requestWork() {
    www->startWork();
}

void WWWThreadHelper::WWWReplyComplete() {
    www->WWWReplyComplete(qobject_cast<QNetworkReply*>(sender()));
};
void WWWThreadHelper::UrlStoreReplyComplete() {
    www->UrlStoreReplyComplete(qobject_cast<QNetworkReply*>(sender()));
};
void WWWThreadHelper::fromWWWReplyComplete() {
    www->fromWWWReplyComplete(qobject_cast<QNetworkReply*>(sender()));
};
void WWWThreadHelper::done() {
    www->done();
};
void WWWThreadHelper::doneDownloading() {
    www->doneDownloading();
};
void WWWThreadHelper::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    www->downloadProgress(bytesReceived, bytesTotal);
};
