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

#include "isbnthread.h"
#include "modstools.h"
#include <QTimer>
#include <QtXml/QDomDocument>
#include <QtXml/QDomNode>
#include <QtCore5Compat/QTextCodec>

ISBNThread::ISBNThread(QObject *parent) :
    LS3ProgressThread(parent)
{
    insertInRecord=-1;
    manager=NULL;
}

ISBNThread::~ISBNThread()
{
    if (manager) delete manager;
}

void ISBNThread::requestISBNdb(const QString &isbn, LS3PluginServices *services) {
    setName(tr("request metadata for ISBN '%1' from ISBNdb ...").arg(isbn));

    namePrefixes=services->GetNamePrefixes();
    nameAdditions=services->GetNameAdditions();
    andWords=services->GetAndWords();
    proxy=services->getNetworkProxy();

    lastISBN=isbn;

    running=true;
    lastError="";

    redirectCount=0;
    newRecord.clear();

    currentInstruction=scanISBNDB;
}

void ISBNThread::requestCOPAC(const QString &isbn, LS3PluginServices *services)
{
    setName(tr("request metadata for ISBN '%1' from COPAC ...").arg(isbn));

    namePrefixes=services->GetNamePrefixes();
    nameAdditions=services->GetNameAdditions();
    andWords=services->GetAndWords();
    proxy=services->getNetworkProxy();

    lastISBN=isbn;

    running=true;
    lastError="";

    redirectCount=0;
    newRecord.clear();

    currentInstruction=scanCopac;

}

void ISBNThread::requestGoogleBooks(const QString &isbn, LS3PluginServices *services) {
    setName(tr("request metadata for ISBN '%1' from Google Books ...").arg(isbn));

    namePrefixes=services->GetNamePrefixes();
    nameAdditions=services->GetNameAdditions();
    andWords=services->GetAndWords();
    proxy=services->getNetworkProxy();

    lastISBN=isbn;

    running=true;
    lastError="";

    redirectCount=0;
    newRecord.clear();

    currentInstruction=scanGoogleBooks;
}

void ISBNThread::initRecordData(const QMap<QString, QVariant> &newRecord) {
    this->newRecord=newRecord;
}

void ISBNThread::setStoreInRecord(QString record) {
    insertInRecord=0;
    recordUUID=record;
}

void ISBNThread::setInsertnewRecord() {
    insertInRecord=-1;
}

void ISBNThread::setNoInsertRecord() {
    insertInRecord=-2;

}

void ISBNThread::startWork()
{
    //qDebug()<<"startWork "<<currentThread();
    if (!preRun()) quit();
}


void ISBNThread::run() {
    //qDebug()<<1;
    manager = new QNetworkAccessManager(NULL);
    //qDebug()<<2;

    manager->setProxy(proxy);
    //qDebug()<<3;
    manager->moveToThread(this);
    //qDebug()<<4;
    isbnHelper=new ISBNThreadHelper(this);
    //qDebug()<<5;
    isbnHelper->moveToThread(this);
    //qDebug()<<6;
    QTimer::singleShot(10, isbnHelper, SLOT(requestWork()));
    //qDebug()<<7;
    //if (preRun()) exec();
    exec();
}

bool ISBNThread::preRun() {
    bool ok=false;
    if (currentInstruction==scanISBNDB) {
        QNetworkRequest request=createISBNdbRequest(lastISBN);
        QNetworkReply* reply = manager->get(request);
        connect(reply, SIGNAL(finished()), isbnHelper, SLOT(ISBNdbReplyComplete()));
        ok=true;
    }
    if (currentInstruction==scanGoogleBooks) {
        QNetworkRequest request=createGoogleBooksRequest(lastISBN);
        QNetworkReply* reply = manager->get(request);
        connect(reply, SIGNAL(finished()), isbnHelper, SLOT(GoogleBooksReplyComplete()));
        ok=true;
    }
    if (currentInstruction==scanCopac) {
        QNetworkRequest request=createCopacRequest(lastISBN);
        QNetworkReply* reply = manager->get(request);
        connect(reply, SIGNAL(finished()), isbnHelper, SLOT(CopacReplyComplete()));
        ok=true;
    }
    return ok ;
}











QNetworkRequest ISBNThread::createGoogleBooksRequest(const QString &isbn) {

    // ISBNdb Access Key
    QString urlstr=QString("http://books.google.com/books/feeds/volumes?q=isbn:%1&start-index=1&max-results=100").arg(isbn);
    QUrl url=QUrl(urlstr);

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", ISBNThread_USER_AGENT);

    return request;
}

QNetworkRequest ISBNThread::createCopacRequest(const QString &isbn)
{
    // ISBNdb Access Key
    QString urlstr=QString("http://copac.ac.uk/search?isn=%1&format=XML+-+MODS").arg(isbn);
    QUrl url=QUrl(urlstr);

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", ISBNThread_USER_AGENT);

    return request;
}


QNetworkRequest ISBNThread::createISBNdbRequest(const QString &isbn) {

    // ISBNdb Access Key
    QString IDkey="Y4S2SGID";
    QString urlstr=QString("http://isbndb.com/api/books.xml?access_key=%1&results=texts&index1=isbn&value1=%2").arg(IDkey).arg(isbn);
    QUrl url=QUrl(urlstr);

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", ISBNThread_USER_AGENT);

    return request;
}


QString ISBNThread::networkErrorToString(QNetworkReply::NetworkError err) {
    switch(err) {
        case QNetworkReply::ConnectionRefusedError: return tr("connection refused"); break;
        case QNetworkReply::RemoteHostClosedError: return tr("remote host closed connection"); break;
        case QNetworkReply::HostNotFoundError: return tr("host not found"); break;
        case QNetworkReply::TimeoutError: return tr("timeout"); break;
        case QNetworkReply::OperationCanceledError: return tr("operation canceled"); break;
        case QNetworkReply::SslHandshakeFailedError: return tr("SSL handshake failed"); break;
        case QNetworkReply::TemporaryNetworkFailureError: return tr("temporary network failure"); break;
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




void ISBNThread::ISBNdbReplyComplete (QNetworkReply* reply_in) {
    //qDebug()<<"ISBNdbReplyComplete: "<<currentThread();
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
        if (redirectCount<ISBNThread_MAX_REDIRECTS) {
            QUrl newUrl=resp->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            if (newUrl.isRelative()) newUrl=resp->url().resolved(newUrl);
            QNetworkRequest request;
            request.setUrl(newUrl);
            request.setRawHeader("User-Agent", ISBNThread_USER_AGENT);
            QNetworkReply *reply = manager->get(request);
            connect(reply, SIGNAL(finished()), isbnHelper, SLOT(ISBNdbReplyComplete()));

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
                QString errormessage;


                setMessage(tr("parsing result of ISBNdb.com request ..."));
                QDomDocument doc("ISBNdb");
                bool error=!(doc.setContent(data, true, &errormessage));

                if (resp->error()==QNetworkReply::NoError) {

                    if (error) {
                        setError(tr("Parse error in request result"));
                    } else {
                        QDomElement docElem = doc.documentElement();
                        //std::cout<<"document tag: "<<docElem.tagName().toStdString()<<std::endl;
                        if ( docElem.tagName()=="ISBNdb" ) {
                            QDomElement n = docElem.firstChildElement("BookList");
                            if (n.isNull()) {
                                setError(tr("Didn't find <BookList...> tag!"));
                            } else {
                                if (n.attribute("total_results").toInt()>0) {
                                    QDomElement n1 = n.firstChildElement("BookData");
                                    if (n1.isNull()) {
                                        setError(tr("Didn't find <BookData...> tag!"));
                                    } else {

                                        newRecord["type"]="book";
                                        newRecord["isbn"]=n1.attribute("isbn", lastISBN);


                                        QString title=n1.firstChildElement("Title").text();
                                        QString titlelong=n1.firstChildElement("TitleLong").text();
                                        QString subtitle=titlelong;
                                        if (titlelong.startsWith(title)) {
                                            subtitle=titlelong.remove(0, title.size()+2);
                                        }
                                        QString pub=n1.firstChildElement("PublisherText").text();
                                        QString publisher=pub;
                                        if (pub.indexOf(':')>-1) {
                                            int p=pub.indexOf(':');
                                            newRecord["places"]=publisher.left(p).trimmed();
                                            publisher=publisher.remove(0, p+1).trimmed();
                                            p=publisher.indexOf(',');
                                            if (p>-1) {
                                                QString year="";
                                                for (int k=p; k<publisher.size(); k++) {
                                                    if (publisher[k].isDigit()) year=year+publisher[k];
                                                }
                                                newRecord["year"]=year.toInt();
                                                publisher.truncate(p);
                                            }

                                        }
                                        newRecord["title"]=title;
                                        newRecord["subtitle"]=subtitle;
                                        newRecord["publisher"]=publisher;
                                        newRecord["comments"]=n1.firstChildElement("Notes").text();
                                        newRecord["abstract"]=n1.firstChildElement("Summary").text();
                                        newRecord["urls"]=n1.firstChildElement("UrlsText").text();
                                        QString authorsText=reformatAuthors(n1.firstChildElement("AuthorsText").text(), namePrefixes, nameAdditions, andWords);
                                        newRecord["authors"]=authorsText;

                                    }
                                } else{
                                    setError(tr("Didn't find ISBN at ISBNdb.com!"));
                                }
                            }
                        } else {
                            setError(tr("Didn't find <ISBNdb...> tag in reply!"));
                        }
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



void ISBNThread::GoogleBooksReplyComplete (QNetworkReply* reply_in) {
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
        if (redirectCount<ISBNThread_MAX_REDIRECTS) {
            QUrl newUrl=resp->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            if (newUrl.isRelative()) newUrl=resp->url().resolved(newUrl);
            QNetworkRequest request;
            request.setUrl(newUrl);
            request.setRawHeader("User-Agent", ISBNThread_USER_AGENT);
            QNetworkReply *reply = manager->get(request);
            connect(reply, SIGNAL(finished()), isbnHelper, SLOT(ISBNdbReplyComplete()));

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
                QString errormessage;


                setMessage(tr("parsing result of Google Books request ..."));
                QDomDocument doc("ISBNdb");
                bool error=!(doc.setContent(data, true, &errormessage));

                if (resp->error()==QNetworkReply::NoError) {

                    if (error) {
                        setError(tr("Parse error in request result"));
                    } else {

                        QDomElement docElem = doc.documentElement();
                        //std::cout<<"document tag: "<<docElem.tagName().toStdString()<<std::endl;
                        if ( docElem.tagName()=="feed" ) {
                            QDomElement n1 = docElem.firstChildElement("entry");
                            if (n1.isNull()) {
                                setError(tr("Didn't find ISBN at Google Books!"));
                            } else {
                                newRecord["type"]="book";
                                newRecord["isbn"]=lastISBN;

                                // extract all <dc:creator> tags which contain the authors
                                QString author="";
                                QDomElement ep=n1.firstChildElement( "creator");
                                while (!ep.isNull()) {
                                    if (!author.isEmpty()) author=author+"; ";
                                    author=author+ep.text();
                                    ep=ep.nextSiblingElement( "creator");
                                }
                                newRecord["authors"]=reformatAuthors(author, namePrefixes, nameAdditions, andWords);

                                // extract <dc:date> tag which contain the publication date
                                ep=n1.firstChildElement( "date");
                                if (!ep.isNull()) newRecord["year"]=ep.text();

                                // extract <dc:publisher> tag which contain the publisher date
                                ep=n1.firstChildElement( "publisher");
                                if (!ep.isNull()) newRecord["publisher"]=ep.text();

                                // extract <dc:subject> tag which contain the publisher date
                                ep=n1.firstChildElement( "subject");
                                if (!ep.isNull()) newRecord["topic"]=ep.text();

                                // extract all <dc:title> tags which contain the authors
                                QString subtitle="";
                                ep=n1.firstChildElement( "title");
                                if (!ep.isNull()) {
                                    newRecord["title"]=ep.text();
                                    ep=ep.nextSiblingElement("title");
                                    while (!ep.isNull()) {
                                        if (!subtitle.isEmpty()) subtitle=subtitle+"\n ";
                                        subtitle=subtitle+ep.text();
                                        ep=ep.nextSiblingElement( "title");
                                    }
                                    newRecord["subtitle"]=subtitle;
                                }

                                // extract <dc:description> tag which contain something like an abstract
                                ep=n1.firstChildElement( "description");
                                if (!ep.isNull()) newRecord["abstract"]=ep.text();


                            }
                        }
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

void ISBNThread::CopacReplyComplete(QNetworkReply* reply_in)
{
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
        if (redirectCount<ISBNThread_MAX_REDIRECTS) {
            QUrl newUrl=resp->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            if (newUrl.isRelative()) newUrl=resp->url().resolved(newUrl);
            QNetworkRequest request;
            request.setUrl(newUrl);
            request.setRawHeader("User-Agent", ISBNThread_USER_AGENT);
            QNetworkReply *reply = manager->get(request);
            connect(reply, SIGNAL(finished()), isbnHelper, SLOT(ISBNdbReplyComplete()));

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
                QString errormessage;


                setMessage(tr("parsing result of COPAC results ..."));


                QMap<QString, QVariant> mods=extractMODSXMLMetadata(data, namePrefixes, nameAdditions, andWords);
                if (!mods.isEmpty()) {
                    QMapIterator<QString, QVariant> it(mods);
                    while (it.hasNext()) {
                        it.next();
                        newRecord[it.key()]=it.value();
                    }

                    done();
                } else {
                    setError(tr("Didn't find metadata for ISBN %1 in COPAC!").arg(lastISBN));
                    done();
                }
            } else {
                setError(resp->errorString());
                done();
            }
            resp->deleteLater();
            replies.clear();
        }
    }
}

void ISBNThread::done() {
    if (m_wasCanceled) {
        running=false;
    } else if (!m_wasError) {
        if (insertInRecord>=0) {
            emit storeInRecord(recordUUID, newRecord);
        } else if (insertInRecord==-1){
            emit insertRecords(newRecord);
        }
        running=false;
    } else {
        running=false;
    }
    quit();
}


void ISBNThreadHelper::requestWork()
{
    base->startWork();
}

void ISBNThreadHelper::ISBNdbReplyComplete()
{
    base->ISBNdbReplyComplete(qobject_cast<QNetworkReply*>(sender()));
}

void ISBNThreadHelper::GoogleBooksReplyComplete()
{
    base->GoogleBooksReplyComplete(qobject_cast<QNetworkReply*>(sender()));
}

void ISBNThreadHelper::CopacReplyComplete()
{
    base->CopacReplyComplete(qobject_cast<QNetworkReply*>(sender()));
}
