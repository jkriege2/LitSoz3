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

#include "pubmedsearchdialog.h"
#include "ui_pubmedsearchdialog.h"
#include "bibtools.h"
#include "ls3pluginarticletools.h"

#define ADDFIELD(name, id) {ui->cmbFields1->addItem(name, id); ui->cmbFields2->addItem(name, id);}

PubmedSearchDialog::PubmedSearchDialog(LS3PluginServices *services, QWidget *parent, bool oneRecOnly) :
    QDialog(parent),
    m_oneRecOnly(oneRecOnly),
    ui(new Ui::PubmedSearchDialog)
{
    this->services=services;
    ui->setupUi(this);
    ui->cmbCatalog->clear();
    ui->cmbCatalog->addItem(tr("Pubmed"), QString("pubmed"));
    ui->cmbCatalog->addItem(tr("Books"), QString("books"));
    ui->cmbCatalog->setCurrentIndex(0);
    list=new QCheckableStringListModel(this);
    items.clear();
    ui->listView->setModel(list);
    HTMLDelegate* del=new HTMLDelegate(ui->listView);
    del->setDisplayRichTextEditor(false);
    ui->listView->setItemDelegate(del);
    ui->btnAgain->setEnabled(false);


    ADDFIELD(tr("All Fields"), "ALL");
    ADDFIELD(tr("Author"), "AU");
    ADDFIELD(tr("Editor"), "ED");
    ADDFIELD(tr("ISBN"), "ISBN");
    ADDFIELD(tr("PMID"), "PMID");
    ADDFIELD(tr("Journal"), "TA");
    ADDFIELD(tr("Issue"), "IP");
    ADDFIELD(tr("Language"), "LA");
    ADDFIELD(tr("Pages"), "PG");
    ADDFIELD(tr("Volume"), "VI");
    ADDFIELD(tr("Title"), "TI");
    ADDFIELD(tr("Title/Abstract"), "TIAB");
    ADDFIELD(tr("Text Words"), "TW");
    ADDFIELD(tr("Pubmed ID"), "PMID");
    ADDFIELD(tr("Publisher"), "PUBN");
    ADDFIELD(tr("Place of Publication"), "PL");
    ADDFIELD(tr("MeSH Major Topic"), "MAJR");
    ADDFIELD(tr("MeSH Subheadings"), "SH");
    ADDFIELD(tr("MeSH Terms"), "MH");
    ADDFIELD(tr("Publication Date"), "DP");
    ADDFIELD(tr("Publication Type"), "PT");


    ui->cmbFields1->setCurrentIndex(0);
    ui->cmbFields2->setCurrentIndex(0);

    if (m_oneRecOnly) {
        connect(ui->listView, SIGNAL(activated(QModelIndex)), this, SLOT(selectRecord(QModelIndex)));
    }


}

PubmedSearchDialog::~PubmedSearchDialog()
{
    clear();
    delete ui;
}

bool PubmedSearchDialog::isImported(int i) {
    if (i>=0 && i<list->rowCount()) {
        return list->isChecked(i);
    }
    return false;
}

QList<QMap<QString, QVariant> > PubmedSearchDialog::getDataToImport()
{
    QList<QMap<QString, QVariant> > d;
    for (int i=0; i<data.size(); i++) {
        if (isImported(i)) d<<data[i];
    }
    return d;
}

void PubmedSearchDialog::setPhrase(const QString &title, const QString &author)
{
    if (!title.isEmpty() && author.isEmpty()) {
        ui->cmbFields1->setCurrentIndex(11);
        ui->edtPhrase1->setText(title);
    } else if (title.isEmpty() && !author.isEmpty()) {
        ui->cmbFields1->setCurrentIndex(1);
        ui->edtPhrase1->setText(author);
    } else if (!title.isEmpty() && !author.isEmpty()) {
        ui->cmbFields1->setCurrentIndex(11);
        ui->edtPhrase1->setText(title);
        ui->cmbFields2->setCurrentIndex(1);
        ui->edtPhrase2->setText(author);
    }

}

void PubmedSearchDialog::setRecord(int i, const QMap<QString, QVariant>& data) {
    QString refsum=formatShortReferenceSummary(data);
    while (i>=items.size()) items.append("---");
    items[i]=refsum;
    list->setStringList(items);
    list->setUnChecked(i);
    //list->checkAll();
}

void PubmedSearchDialog::clear()
{
    if (reply) {
        reply->abort();
        reply->deleteLater();
    }
    for (int i=0; i<replies.size(); i++) {
        if (replies[i]) {
            replies[i]->abort();
            replies[i]->deleteLater();
        }
    }
    for (int i=0; i<pmthreads.size(); i++) {
        if (pmthreads[i]) {
            pmthreads[i]->disconnect(this);
            pmthreads[i]->terminate();
        }
    }
}


void PubmedSearchDialog::on_cmbAND1_currentIndexChanged(int idx)
{
    ui->label3->setEnabled(idx>0);
    ui->label4->setEnabled(idx>0);
    ui->edtPhrase2->setEnabled(idx>0);
    ui->cmbFields2->setEnabled(idx>0);
}

void PubmedSearchDialog::on_btnSearch_clicked()
{
    manager = new QNetworkAccessManager(this);
    manager->setProxy(services->getNetworkProxy());

    QString lastSearch=ui->edtPhrase1->text();
    int f1=ui->cmbFields1->currentIndex();
    if (f1>=0) lastSearch+=QString(" [%1]").arg(ui->cmbFields1->itemData(f1).toString());
    int op1=ui->cmbAND1->currentIndex();
    if (op1>0 && !ui->edtPhrase2->text().isEmpty()) {
        if (op1==1) lastSearch+=QString(" AND ");
        if (op1==2) lastSearch+=QString(" OR ");
        lastSearch+=ui->edtPhrase2->text();
        int f2=ui->cmbFields2->currentIndex();
        if (f2>=0) lastSearch+=QString(" [%1]").arg(ui->cmbFields2->itemData(f2).toString());
    }

    QNetworkRequest request=PubmedThread::createPubmedSearchRequest(lastSearch, ui->spinMaxResults->value(), ui->cmbCatalog->itemData(ui->cmbCatalog->currentIndex()).toString());
    setMessage(tr("search URL: %1").arg(request.url().toString()));
    ui->labStatus->setText(tr("searching on PubMed catalog '%1' ...").arg(ui->cmbCatalog->itemData(ui->cmbCatalog->currentIndex()).toString()));
    reply = manager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(PubmedReplyComplete()));


    ui->groupBox->setEnabled(false);
    ui->progressBar->setMaximum(2);
    ui->progressBar->setValue(1);
    ui->btnAgain->setEnabled(true);
}

void PubmedSearchDialog::on_btnAgain_clicked()
{
    clear();
    items.clear();
    data.clear();
    list->setStringList(items);
    ui->groupBox->setEnabled(true);
    ui->progressBar->setMaximum(2);
    ui->progressBar->setValue(0);
    ui->btnAgain->setEnabled(false);
    ui->labStatus->setText("");
    ui->lstLog->clear();
}

void PubmedSearchDialog::PubmedReplyComplete () {
    QNetworkReply* resp=qobject_cast<QNetworkReply*>(sender());
    if (!resp) return;

    /*if (m_wasCanceled) {
        resp->abort();
        resp->deleteLater();
        done();
        return;
    }*/

    //qDebug()<<resp->error()<<resp->readAll();
    if (!resp->attribute(QNetworkRequest::RedirectionTargetAttribute).toString().isEmpty()) {
        if (redirectCount<WWWTHREAD_MAX_REDIRECTS) {
            QUrl newUrl=resp->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            if (newUrl.isRelative()) newUrl=resp->url().resolved(newUrl);
            QNetworkRequest request;
            request.setUrl(newUrl);
            request.setRawHeader("User-Agent", WWWTHREAD_USER_AGENT);
            QNetworkReply *reply = manager->get(request);
            connect(reply, SIGNAL(finished()), this, SLOT(PubmedReplyComplete()));

            replies.append(reply);
            resp->abort();
            resp->deleteLater();
            redirectCount++;
            //qDebug()<<newUrl;
            setMessage(tr("following new URL: %1 ...").arg(newUrl.toString()));
        } else {
            setError(tr("too many redirects"));
            //done();
        }
    } else {
        if (resp->isFinished()) {
            if (resp->error()==QNetworkReply::NoError) {

                QByteArray databa=resp->readAll();
                QByteArray encoding=guessEncoding(resp->rawHeader("Content-Encoding"), databa);
                //qDebug()<<encoding;
                //qDebug()<<databa;
                QString data=QTextCodec::codecForName(encoding)->toUnicode(databa);
                QString catalog=ui->cmbCatalog->itemData(ui->cmbCatalog->currentIndex()).toString();

                QList<QString> res=extractPMIDList(data);
                ui->progressBar->setMaximum(res.size()+2);
                ui->progressBar->setValue(2);
                ui->labStatus->setText(tr("found %1 results ... retrieving reference data ...").arg(res.size()));
                // now we have a list of PMIds to search for ...
                QSet<QString> searchedBookIDs;
                for (int i=0; i<res.size(); i++) {
                    if ((catalog.toLower()=="books" && !searchedBookIDs.contains(res[i])) || (catalog.toLower()!="books")) {
                        searchedBookIDs.insert(res[i]);
                        //qDebug()<<catalog<<searchedBookIDs;
                        //qDebug()<<"search: "<<res[i];
                        PubmedThread* t=new PubmedThread(this);
                        t->setInsertnewRecord();
                        connect(t, SIGNAL(insertRecords(QMap<QString,QVariant>)), this, SLOT(insertRecord(QMap<QString,QVariant>)));
                        connect(t, SIGNAL(storeInRecord(QString,QMap<QString,QVariant>)), this, SLOT(storeInRecord(QString,QMap<QString,QVariant>)));
                        connect(t, SIGNAL(messageChanged(QString)), this, SLOT(setMessage(QString)));
                        connect(t, SIGNAL(nameChanged(QString)), this, SLOT(setMessage(QString)));
                        connect(t, SIGNAL(errorOccured(QString)), this, SLOT(setError(QString)));

                        t->setCatalog(catalog);
                        if (catalog.toLower()=="books") {
                            t->requestFromPMIDBooks(res[i], services);
                        } else {
                            t->requestFromPMID(res[i], services);
                        }
                        t->addToRecordData("pubmed_search_result_index", i);
                        t->start();
                        pmthreads.append(t);
                    }
                }

            }
            resp->deleteLater();
        }
    }
}

void PubmedSearchDialog::insertRecord(QMap<QString, QVariant> data) {
    int idx=data["pubmed_search_result_index"].toInt();
    //qDebug()<<"insertRecord: "<<idx<<": PMID="<<data.value("pubmed")<<" TYPE="<<data.value("type")<<" DOI="<<data.value("doi");
    QMap<QString, QVariant> empty;
    if (idx>=0) {
        while (this->data.size()<=idx) {
            this->data.append(empty);
        }
        this->data[idx]=data;
        ui->progressBar->setValue(ui->progressBar->value()+1);
        setRecord(idx, data);
    }
}

void PubmedSearchDialog::storeInRecord(QString record, QMap<QString, QVariant> data) {
    insertRecord(data);
}

void PubmedSearchDialog::setMessage(const QString &message)
{
    ui->lstLog->addItem(tr(">>> %1").arg(message));
    ui->lstLog->setCurrentRow(ui->lstLog->count()-1);
}

void PubmedSearchDialog::setError(const QString &message)
{
    ui->lstLog->addItem(tr("ERROR> %1").arg(message));
    ui->lstLog->setCurrentRow(ui->lstLog->count()-1);
}

void PubmedSearchDialog::selectRecord(const QModelIndex &idx)
{
    list->unCheckAll();
    list->setChecked(idx.row());
}
