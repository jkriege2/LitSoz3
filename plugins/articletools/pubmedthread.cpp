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

#include "pubmedthread.h"
#include <QtCore5Compat/QTextCodec>

/*

    22385856
    22385856aa
    22207822


  */
int PubmedThread::max_result_count=1;

PubmedThread::PubmedThread(QObject *parent) :
    DoiThread(parent)
{
    catalog="pubmed";
}

void PubmedThread::setCatalog(const QString &catalog)
{
    this->catalog=catalog;
}

void PubmedThread::requestFromPMID(const QString &pmid, LS3PluginServices *services) {
    setName(tr("requesting metadata from Pubmed ID %1 ...").arg(pmid));

    namePrefixes=services->GetNamePrefixes();
    nameAdditions=services->GetNameAdditions();
    andWords=services->GetAndWords();
    proxy=services->getNetworkProxy();

    lastPMID=pmid;


    running=true;
    lastError="";

    redirectCount=0;

    newRecord.clear();
    newRecord["pubmed"]=lastPMID;
    newRecord["type"]="article";

    currentInstruction=scanNone;
    if (!lastPMID.isEmpty()) currentInstruction=scanPubmed;
}

void PubmedThread::requestFromPMIDBooks(const QString &pmid, LS3PluginServices *services)
{
    setName(tr("requesting metadata from Pubmed Books ID %1 ...").arg(pmid));

    namePrefixes=services->GetNamePrefixes();
    nameAdditions=services->GetNameAdditions();
    andWords=services->GetAndWords();
    proxy=services->getNetworkProxy();

    lastPMID=pmid;


    running=true;
    lastError="";

    redirectCount=0;

    newRecord.clear();
    newRecord["pubmed"]=lastPMID;
    newRecord["type"]="book";

    currentInstruction=scanNone;
    if (!lastPMID.isEmpty()) currentInstruction=scanPubmedBooks;

}



void PubmedThread::requestFromPMIDandDOI(const QString &pmid, const QString &DOI, LS3PluginServices *services) {
    setName(tr("requesting metadata from Pubmed ID %1 and DOI %2 ...").arg(pmid).arg(DOI));

    namePrefixes=services->GetNamePrefixes();
    nameAdditions=services->GetNameAdditions();
    andWords=services->GetAndWords();
    proxy=services->getNetworkProxy();

    lastPMID=pmid;
    lastDoi=DOI;


    running=true;
    lastError="";

    redirectCount=0;

    newRecord.clear();
    newRecord["pubmed"]=lastPMID;
    newRecord["doi"]=lastDoi;
    newRecord["type"]="article";

    currentInstruction=scanNone;
    if (!lastPMID.isEmpty() && lastDoi.isEmpty()) currentInstruction=scanPubmed;
    if (lastPMID.isEmpty() && !lastDoi.isEmpty()) currentInstruction=scanDOICrossref;
    if (!lastPMID.isEmpty() && !lastDoi.isEmpty()) currentInstruction=scanPubmedAndDoi;
    //qDebug()<<"currentInstruction = "<<currentInstruction;
}

bool PubmedThread::preRun() {
    pubmedHelper=new PubmedThreadHelper(this);
    //qDebug()<<"PubmedThread::preRun():"<<currentThread();
    bool ok=DoiThread::preRun();

    if (currentInstruction==scanPubmed) {
        if (lastPMID.startsWith("PMC")) currentInstruction=scanPubmedResolveIDs;
        if (lastPMID.startsWith("NIH")) currentInstruction=scanPubmedResolveIDs;
        if (lastPMID.startsWith("10.")) currentInstruction=scanPubmedResolveIDs;
    }

    if (currentInstruction==scanPubmedResolveIDs) {
        QNetworkRequest request=createPubmedResolveRequest(lastPMID);
        QNetworkReply* reply = manager->get(request);
        connect(reply, SIGNAL(finished()), pubmedHelper, SLOT(PubmedReplyComplete()));
        ok=true;
    }
    if (currentInstruction==scanPubmed) {
        //qDebug()<<"  Pubmed: creating network request ... ";
        QNetworkRequest request=createPMIDRequest(lastPMID, catalog);
        //qDebug()<<"  Pubmed: calling get() ... ";
        QNetworkReply* reply = manager->get(request);
        connect(reply, SIGNAL(finished()), pubmedHelper, SLOT(PubmedReplyComplete()));
        //qDebug()<<"  Pubmed: calling exec() ... ";
        ok=true;
    }
    if (currentInstruction==scanPubmedBooks) {
        //qDebug()<<"  PubmedBooks: creating network request ... ";
        QNetworkRequest request=createPMIDBooksRequest(lastPMID);
        //qDebug()<<"  PubmedBooks: calling get() ... ";
        QNetworkReply* reply = manager->get(request);
        connect(reply, SIGNAL(finished()), pubmedHelper, SLOT(PubmedReplyComplete()));
        //qDebug()<<"  PubmedBooks: calling exec() ... ";
        ok=true;
    }
    if (currentInstruction==scanPubmedAndDoi) {
        //qDebug()<<"  PubmedAndDoi: creating network request ... ";
        QNetworkRequest request=createPMIDRequest(lastPMID, catalog);
        //qDebug()<<"  PubmedAndDoi: calling get() ... ";
        QNetworkReply* reply = manager->get(request);
        connect(reply, SIGNAL(finished()), pubmedHelper, SLOT(PubmedReplyComplete()));
        //qDebug()<<"  PubmedAndDoi: calling exec() ... ";
        ok=true;
    }
    //qDebug()<<"PubmedThread::preRun(): exit";
    return ok ;
}



QNetworkRequest PubmedThread::createPMIDRequest(QString PMID, const QString& catalog) {
    QString urlstr="http://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?db=%3&id=%1&retmode=xml&retmax=%2";
    QUrl PMIDUrl=QUrl(urlstr.arg(PMID).arg(max_result_count).arg(catalog));

    QNetworkRequest request;
    request.setUrl(PMIDUrl);
    request.setRawHeader("User-Agent", WWWTHREAD_USER_AGENT);

    return request;
}

QNetworkRequest PubmedThread::createPubmedResolveRequest(QString PMID)
{
    QString urlstr="http://www.pubmedcentral.nih.gov/utils/idconv/v1.0/?ids=%1";
    QUrl PMIDUrl=QUrl(urlstr.arg(PMID));

    QNetworkRequest request;
    request.setUrl(PMIDUrl);
    request.setRawHeader("User-Agent", WWWTHREAD_USER_AGENT);

    return request;
}

QNetworkRequest PubmedThread::createPMIDBooksRequest(QString PMID) {

    QString urlstr="http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esummary.fcgi?db=books&id=%1&retmax=%2";
    QUrl PMIDUrl=QUrl(urlstr.arg(PMID).arg(max_result_count));

    QNetworkRequest request;
    request.setUrl(PMIDUrl);
    request.setRawHeader("User-Agent", WWWTHREAD_USER_AGENT);

    return request;
}

QNetworkRequest PubmedThread::createPubmedSearchRequest(QString phrase, int maxresults, const QString& catalog) {
    QString urlstr="http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?db=%3&rettype=uilist&term=%1&retmax=%2";
    QString phraset=QUrl::toPercentEncoding(phrase, "[] ");
    phraset=phraset.replace(QRegularExpression("\\s"), "+");
    QUrl PMIDUrl=QUrl(urlstr.arg(phraset).arg(maxresults).arg(catalog));

    QNetworkRequest request;
    request.setUrl(PMIDUrl);
    request.setRawHeader("User-Agent", WWWTHREAD_USER_AGENT);

    return request;
}

void PubmedThread::PubmedReplyComplete (QNetworkReply* reply_in) {

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
            connect(reply, SIGNAL(finished()), pubmedHelper, SLOT(PubmedReplyComplete()));

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
                bool append=false;


                if (currentInstruction==scanPubmed) {
                    // retrieve data from crossref result
                    setMessage(tr("requesting information for PubMed ID '%1' ...\nretrieving crossref data ...").arg(lastPMID));
                    //qDebug()<<data;

                    QMap<QString, QVariant> dc=extractPubmedMetadata(data);
                    //qDebug()<<dc;

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

                    //append=false;
                    QString doi=newRecord.value("doi").toString().simplified().trimmed();
                    if (doi.startsWith("10.")) {
                        resp->abort();
                        currentInstruction=scanDOICrossref;
                        QNetworkRequest request=createDoiCrossrefRequest(doi);
                        QNetworkReply *reply = manager->get(request);
                        connect(reply, SIGNAL(finished()), pubmedHelper, SLOT(DoiReplyComplete()));
                        //resp->deleteLater();
                        redirectCount=0;
                        setMessage(tr("resolving DOI: %1 ...").arg(request.url().toString()));


                        append=false;
                    } else {
                        append=true;
                    }
                }
                if (currentInstruction==scanPubmedAndDoi) {
                    // retrieve data from crossref result
                    setMessage(tr("requesting information for PubMed ID '%1' ...\nretrieving crossref data ...").arg(lastPMID));
                    //qDebug()<<data;

                    QMap<QString, QVariant> dc=extractPubmedMetadata(data);
                    //qDebug()<<dc;

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


                    // THERE MIGHT BE MORE METADATA FROM THE DOI
                    currentInstruction=scanDOICrossref;
                    resp->abort();
                    QNetworkRequest request=createDoiCrossrefRequest(lastDoi);
                    QNetworkReply *reply = manager->get(request);
                    connect(reply, SIGNAL(finished()), pubmedHelper, SLOT(DoiReplyComplete()));
                    //resp->deleteLater();
                    redirectCount=0;
                    setMessage(tr("resolving DOI: %1 ...").arg(request.url().toString()));


                    append=false;
                }
                if (currentInstruction==scanPubmedBooks) {
                    // retrieve data from crossref result
                    setMessage(tr("requesting information for PubMed Books ID '%1' ...\nretrieving Book ID ...").arg(lastPMID));
                    //qDebug()<<data;

                    QString bid=extractPubmedBookIDList(data);
                    //qDebug()<<"found book data for ID: "<<bid<<"\n"<<data;

                    if (!bid.isEmpty()) {
                        // THERE MIGHT BE MORE METADATA FROM THE DOI
                        resp->abort();
                        currentInstruction=scanCommonMetadata;
                        QString url=QString("http://www.ncbi.nlm.nih.gov/books/%1").arg(bid);
                        newRecord["url"]=url;
                        newRecord.remove("pubmed");
                        QNetworkRequest request=createURLRequest(url);
                        QNetworkReply *reply = manager->get(request);
                        connect(reply, SIGNAL(finished()), pubmedHelper, SLOT(WWWReplyComplete()));
                        //resp->deleteLater();
                        redirectCount=0;
                        setMessage(tr("resolving Pubmed Book ID: %1 ...").arg(request.url().toString()));
                    } else {
                        setError(tr("error: could not read Bookshelf ID"));
                        done();
                    }


                    append=false;
                }
                if (currentInstruction==scanPubmedResolveIDs) {
                    // retrieve data from crossref result
                    setMessage(tr("requesting information for PubMed ID '%1' ...\nretrieving other ID ...").arg(lastPMID));
                    //qDebug()<<data;

                    extractPubmedIDConvertIDs(newRecord, data);

                    QString pmid=newRecord.value("pubmed").toString();
                    QString pmcid=newRecord.value("pmcid").toString();
                    QString doi=newRecord.value("doi").toString();

                    //qDebug()<<pmid<<pmcid<<doi;
                    lastPMID=pmid;

                    if (!pmid.isEmpty()) {
                        //qDebug()<<"resolving PMID "<<pmid;
                        // THERE MIGHT BE MORE METADATA FROM THE Pubmed
                        currentInstruction=scanPubmed;
                        lastPMID=pmid;
                        QNetworkRequest request=createPMIDRequest(pmid);
                        QNetworkReply *reply = manager->get(request);
                        connect(reply, SIGNAL(finished()), pubmedHelper, SLOT(PubmedReplyComplete()));
                        resp->abort();
                        //resp->deleteLater();
                        redirectCount=0;
                        setMessage(tr("resolving Pubmed ID: %1 ...").arg(request.url().toString()));
                        append=false;
                    } else if (!doi.isEmpty()) {
                        //qDebug()<<"resolving DOI "<<doi;
                        // THERE MIGHT BE MORE METADATA FROM THE DOI
                        currentInstruction=scanDOICrossref;
                        lastDoi=doi;
                        QNetworkRequest request=createDoiCrossrefRequest(doi);
                        QNetworkReply *reply = manager->get(request);
                        connect(reply, SIGNAL(finished()), pubmedHelper, SLOT(DoiReplyComplete()));
                        resp->abort();
                        //resp->deleteLater();
                        redirectCount=0;
                        setMessage(tr("resolving DOI: %1 ...").arg(request.url().toString()));
                        append=false;
                    } else {
                        setError(tr("error: could not resolve ID"));
                        done();
                    }


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
}


void PubmedThreadHelper::PubmedReplyComplete()
{
    base->PubmedReplyComplete(qobject_cast<QNetworkReply*>(sender()));
}

void PubmedThreadHelper::DoiReplyComplete()
{
    base->DoiReplyComplete(qobject_cast<QNetworkReply*>(sender()));
}



void PubmedThreadHelper::requestWork() {
    base->startWork();
}

void PubmedThreadHelper::WWWReplyComplete() {
    base->WWWReplyComplete(qobject_cast<QNetworkReply*>(sender()));
};
void PubmedThreadHelper::UrlStoreReplyComplete() {
    base->UrlStoreReplyComplete(qobject_cast<QNetworkReply*>(sender()));
};
void PubmedThreadHelper::fromWWWReplyComplete() {
    base->fromWWWReplyComplete(qobject_cast<QNetworkReply*>(sender()));
};
void PubmedThreadHelper::done() {
    base->done();
};
void PubmedThreadHelper::doneDownloading() {
    base->doneDownloading();
};
void PubmedThreadHelper::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    base->downloadProgress(bytesReceived, bytesTotal);
};

