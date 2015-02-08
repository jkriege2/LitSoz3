/*
  Test-DOIs (2. causes error):

10.1039/B911857H
10.1039/B910747AAC
10.1039/B910747A
10.1039/B911857H
10.1039/B910747AAC
10.1039/B910747A

  */


#include "ls3pluginarticletools.h"
#include "multiplerecordinsertdialog.h"
#include "pubmedsearchdialog.h"

#include <QDebug>
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
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


LS3PluginArticleTools::LS3PluginArticleTools():
    LS3PluginBase()
{
    qRegisterMetaType<QList<QMap<QString,QVariant> > >("QList<QMap<QString,QVariant> >");
}

LS3PluginArticleTools::~LS3PluginArticleTools()
{
    //dtor
}


void LS3PluginArticleTools::init(LS3PluginServices* pluginServices) {
    LS3PluginBase::init(pluginServices);

    QIcon icon(":/articletools/search.png");
    icon.addFile(":/articletools/search_disabled.png", QSize(), QIcon::Disabled);
    icon.addFile(":/articletools/search_hover.png", QSize(), QIcon::Selected);
    icon.addFile(":/articletools/search_pressed.png", QSize(), QIcon::Active);




    QWidget* p=pluginServices->GetParentWidget();


    // build article search widget
    widMain=new QWidget(p);
    QFormLayout* l=new QFormLayout(p);
    //l->setContentsMargins(0,0,0,0);
    widMain->setLayout(l);
    QDockWidget* dw=new QDockWidget(tr("Article search"), p);
    docArticleTools=dw;
    dw->setObjectName("toolbar_articletools");
    cmbWhat=new QComboBox(p);
    cmbWhat->addItem(tr("DOI (Digital Object Identifier)"));
    cmbWhat->addItem(tr("PMID (PubMed ID)"));
    //cmbWhat->addItem(tr("Pubmed"));
    l->addRow(tr("seach &type: "), cmbWhat);
    cmbWhat->setCurrentIndex(pluginServices->GetQSettings()->value("plugins/articlelookup_lasttype", 1).toInt());



    // build PDF drop widget
    PDFDropDockWidget* pdfdw=new PDFDropDockWidget(p);
    pdfdw->setTypes(pluginServices->getFDFNames(), pluginServices->getFDFIDs());
    pdfdw->setCurrentType(pluginServices->GetQSettings()->value("plugins/fileimport_lasttype", QString("article")).toString());
    pdfdw->setCurrentAction(pluginServices->GetQSettings()->value("plugins/fileimport_lastaction",0).toInt());
    pluginServices->addDockWidget(pdfdw);
    connect(pdfdw, SIGNAL(typeChanged(QString)), this, SLOT(typeChanged(QString)));
    connect(pdfdw, SIGNAL(actionChanged(int)), this, SLOT(actionChanged(int)));
    connect(pdfdw, SIGNAL(pdfsDropped(QStringList, QString, int)), this, SLOT(filesDropped(QStringList, QString, int)));


    edtArticle=new QEnhancedLineEdit(widMain);
    btnExecute=new JKStyledButton(icon, edtArticle);
    edtArticle->addButton(btnExecute);
    edtArticle->setMaximumWidth(150);
    l->addRow(tr("seach &phrase: "), edtArticle);
    QShortcut* sc=new QShortcut(QKeySequence("Enter,Return"), edtArticle);
    connect(sc, SIGNAL(activated()), this, SLOT(articleInsert()));
    connect(btnExecute, SIGNAL(clicked()), this, SLOT(articleInsert()));

    dw->setWidget(widMain);

    pluginServices->addDockWidget(dw);

    actShowArticleToolWindow=new QAction(tr("&show tool window"), this);
    connect(actShowArticleToolWindow, SIGNAL(triggered()), dw, SLOT(show()));

    actPubmedSearch=new QAction(QIcon(":/articletools/pubmedsearch.png"), tr("Search Pubmed ..."), this);
    connect(actPubmedSearch, SIGNAL(triggered()), this, SLOT(searchPubmed()));

    actFillFromDoi=new QAction(QIcon(":/articletools/insert.png"), tr("fill missing fields from &DOI ..."), this);
    connect(actFillFromDoi, SIGNAL(triggered()), this, SLOT(fillFromDoi()));
    pluginServices->addEditButton(LS3PluginServices::EditDOI, actFillFromDoi);

    actFillFromPMID=new QAction(QIcon(":/articletools/insert.png"), tr("fill missing fields from &PubMed ID ..."), this);
    connect(actFillFromPMID, SIGNAL(triggered()), this, SLOT(fillFromPMID()));
    pluginServices->addEditButton(LS3PluginServices::EditPubmed, actFillFromPMID);

    actFillFromPMCID=new QAction(QIcon(":/articletools/insert.png"), tr("fill missing fields from &PubMed Central ID ..."), this);
    connect(actFillFromPMCID, SIGNAL(triggered()), this, SLOT(fillFromPMCID()));
    pluginServices->addEditButton(LS3PluginServices::EditPMCID, actFillFromPMCID);

    actDownloadPDFFromPMCID=new QAction(QIcon(":/articletools/pdf.png"), tr("download PDF from &PubMed Central ..."), this);
    connect(actDownloadPDFFromPMCID, SIGNAL(triggered()), this, SLOT(downloadPDFFromPMCID()));
    pluginServices->addEditButton(LS3PluginServices::EditPMCID, actDownloadPDFFromPMCID);

    actDownloadPDFFromArxiv=new QAction(QIcon(":/articletools/pdf.png"), tr("download PDF from &Arxiv ..."), this);
    connect(actDownloadPDFFromArxiv, SIGNAL(triggered()), this, SLOT(downloadPDFFromArxiv()));
    pluginServices->addEditButton(LS3PluginServices::EditArXiv, actDownloadPDFFromArxiv);

    actDownloadPDFFromDoi=new QAction(QIcon(":/articletools/pdf.png"), tr("download PDF from &DOI ..."), this);
    connect(actDownloadPDFFromDoi, SIGNAL(triggered()), this, SLOT(downloadPDFFromDoi()));
    pluginServices->addEditButton(LS3PluginServices::EditDOI, actDownloadPDFFromDoi);

    actPasteDoi=new QAction(QIcon(":/articletools/paste.png"), tr("paste D&OI ..."), this);
    connect(actPasteDoi, SIGNAL(triggered()), this, SLOT(pasteDoi()));

    actPastePMID=new QAction(QIcon(":/articletools/paste.png"), tr("paste P&ubMed ID ..."), this);
    connect(actPastePMID, SIGNAL(triggered()), this, SLOT(pastePMID()));

    actPasteWebpage=new QAction(QIcon(":/articletools/pastewebpage.png"), tr("paste URL as &Webpage ..."), this);
    connect(actPasteWebpage, SIGNAL(triggered()), this, SLOT(pasteWebpage()));

    actPasteFromWebpage=new QAction(QIcon(":/articletools/pastewebpage.png"), tr("paste Metadata from &URL ..."), this);
    connect(actPasteFromWebpage, SIGNAL(triggered()), this, SLOT(pasteFromWebpage()));

    actPastePDFFromURL=new QAction(QIcon(":/articletools/pastewebpage.png"), tr("&Download PDF for current record from URL..."), this);
    connect(actPastePDFFromURL, SIGNAL(triggered()), this, SLOT(pastePDFFromURL()));

    actInsertePDFFromURL=new QAction(QIcon(":/articletools/insert.png"), tr("&Download PDF and add as new record..."), this);
    connect(actInsertePDFFromURL, SIGNAL(triggered()), this, SLOT(insertPDFFromURL()));

    menuArticleTools=pluginServices->getMenu(LS3PluginServices::ToolsMenu)->addMenu(QIcon(":/articletools/menu.png"), tr("&Article Import Tools ..."));
    menuArticleTools->addAction(actShowArticleToolWindow);
    menuArticleTools->addSeparator();
    menuArticleTools->addAction(actFillFromDoi);
    menuArticleTools->addAction(actPasteDoi);
    menuArticleTools->addSeparator();
    menuArticleTools->addAction(actFillFromPMID);
    menuArticleTools->addAction(actPastePMID);
    menuArticleTools->addSeparator();
    menuArticleTools->addAction(actPasteWebpage);
    menuArticleTools->addAction(actPasteFromWebpage);
    menuArticleTools->addAction(actPastePDFFromURL);
    menuArticleTools->addAction(actInsertePDFFromURL);
    menuArticleTools->addSeparator();
    menuArticleTools->addAction(actPubmedSearch);

    tbTools=pluginServices->getToolbar(LS3PluginServices::ToolsToolbar);
    tbTools->addAction(actPubmedSearch);


    QMenu* editMenu=pluginServices->getMenu(LS3PluginServices::EditMenu);
    editMenu->addAction(actPasteDoi);
    editMenu->addAction(actPastePMID);
    editMenu->addAction(actPasteWebpage);
    editMenu->addAction(actPasteFromWebpage);
    editMenu->addAction(actPastePDFFromURL);
    editMenu->addAction(actInsertePDFFromURL);


    dlgProgress=new QModernProgressDialog(p);
    dlgProgress->setWindowModality(Qt::WindowModal);
    dlgProgress->close();



    QMenu* menu=pluginServices->getMenu(LS3PluginServices::ToolsMenu)->addMenu(QIcon(":/filetools/menu_logo.png"), tr("Files Import"));
    actDockDisplay=new QAction(tr("display file import window"), this);
    connect(actDockDisplay, SIGNAL(triggered()), pdfdw, SLOT(show()));
    actInsertFilesArticle=new QAction(QIcon(":/filetools/insertfiles.png"), tr("insert articles from files"), this);
    connect(actInsertFilesArticle, SIGNAL(triggered()), this, SLOT(insertFilesArticle()));
    menu->addAction(actInsertFilesArticle);
    menu->addAction(actDockDisplay);

    actRenameFiles=new QAction(QIcon(":/filetools/logo.png"), tr("rename associated files ..."), pluginServices->GetParentWidget());
    connect(actRenameFiles, SIGNAL(triggered()), this, SLOT(renameFiles()));
    menu->addSeparator();
    menu->addAction(actRenameFiles);

    actRenameFilesCurrentrecord=new QAction(QIcon(":/filetools/logo.png"), tr("rename associated files in current record"), pluginServices->GetParentWidget());
    connect(actRenameFilesCurrentrecord, SIGNAL(triggered()), this, SLOT(renameFilesCurrentRecord()));
    pluginServices->addEditButton(LS3PluginServices::EditFile, actRenameFilesCurrentrecord);
    menu->addAction(actRenameFilesCurrentrecord);

}

void LS3PluginArticleTools::deinit(LS3PluginServices* pluginServices) {
    LS3PluginBase::deinit(pluginServices);
}

void LS3PluginArticleTools::openData(LS3Datastore* datastore) {
    LS3PluginBase::openData(datastore);

    actFillFromDoi->setEnabled(true);
    actFillFromPMID->setEnabled(true);
    actShowArticleToolWindow->setEnabled(true);
    docArticleTools->setEnabled(true);
    menuArticleTools->setEnabled(true);
    actPastePMID->setEnabled(true);
    actPasteDoi->setEnabled(true);
    actPasteWebpage->setEnabled(true);
    actPasteFromWebpage->setEnabled(true);
    actPastePDFFromURL->setEnabled(true);
    actInsertePDFFromURL->setEnabled(true);
    actPubmedSearch->setEnabled(true);
    actDownloadPDFFromArxiv->setEnabled(true);
    actDownloadPDFFromPMCID->setEnabled(true);
    actFillFromPMCID->setEnabled(true);
}

void LS3PluginArticleTools::closeData(LS3Datastore* datastore) {
    LS3PluginBase::closeData(datastore);

    actFillFromDoi->setEnabled(false);
    actFillFromPMID->setEnabled(false);
    actShowArticleToolWindow->setEnabled(false);
    docArticleTools->setEnabled(false);
    menuArticleTools->setEnabled(false);;
    actPastePMID->setEnabled(false);
    actPasteDoi->setEnabled(false);
    actPasteWebpage->setEnabled(false);
    actPasteFromWebpage->setEnabled(false);
    actPastePDFFromURL->setEnabled(false);
    actInsertePDFFromURL->setEnabled(false);
    actPubmedSearch->setEnabled(false);
    actDownloadPDFFromArxiv->setEnabled(false);
    actDownloadPDFFromPMCID->setEnabled(false);
    actFillFromPMCID->setEnabled(false);
}

void LS3PluginArticleTools::searchPubmed()
{
    PubmedSearchDialog* dlg=new PubmedSearchDialog(pluginServices(), pluginServices()->GetParentWidget());
    if (dlg->exec()) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QList<QMap<QString, QVariant> > data=dlg->getDataToImport();
        for (int i=0; i<data.size(); i++) {
            QMap<QString, QVariant> mrec=data[i];
            if (mrec["type"].toString().isEmpty()) mrec["type"]="misc";
            QString uuid;
            m_currentDatastore->addRecord(mrec, uuid, true);
            //int recN=m_currentDatastore->getRecordByUUID(uuid);
            QString DOI=mrec["doi"].toString().trimmed();
            if (!DOI.isEmpty()) {
                insertDOI(DOI, uuid);
            }
        }
        QApplication::restoreOverrideCursor();
    }
}

void LS3PluginArticleTools::clipboardDataChanged() {
    if (m_currentDatastore) {
        if (m_currentDatastore->dbIsLoaded()) {
            if (!QApplication::clipboard()->text().isEmpty()) {
                actPastePMID->setEnabled(true);
                actPasteDoi->setEnabled(true);
                actPasteWebpage->setEnabled(true);
                actPasteFromWebpage->setEnabled(true);
                actPastePDFFromURL->setEnabled(true);
                actInsertePDFFromURL->setEnabled(true);
                return;

            }
        }
    }
    actPastePMID->setEnabled(false);
    actPasteDoi->setEnabled(false);
    actPasteWebpage->setEnabled(false);
    actPasteFromWebpage->setEnabled(false);
    actPastePDFFromURL->setEnabled(false);
    actInsertePDFFromURL->setEnabled(false);
}

void LS3PluginArticleTools::articleInsert() {
    if (!m_currentDatastore) return;
    pluginServices()->GetQSettings()->setValue("plugins/articlelookup_lasttype", cmbWhat->currentIndex());
    if (cmbWhat->currentIndex()==0) { // DOI search
        insertDOI(edtArticle->text().trimmed());
    } else if (cmbWhat->currentIndex()==1) { // PMID search
        insertPubmed(edtArticle->text().trimmed());
    //} else if (cmbWhat->currentIndex()==2) { // search Pubmed
        //searchPubmed(edtArticle->text().trimmed());
    }
}

void LS3PluginArticleTools::pasteDoi() {
    QStringList sl=QApplication::clipboard()->text().split('\n');
    for (int i=0; i<sl.size(); i++) {
        insertDOI(sl[i].trimmed());
    }
}

void LS3PluginArticleTools::pastePMID() {
    QStringList sl=QApplication::clipboard()->text().split('\n');
    for (int i=0; i<sl.size(); i++) {
        insertPubmed(sl[i].trimmed());
    }
}


void LS3PluginArticleTools::fillFromDoi() {
    if (!m_currentDatastore) return;


    QMap<QString, QVariant> rec=m_currentDatastore->currentRecord();

    clearAndRestoreFocus();

    QString DOI=rec["doi"].toString().trimmed();
    QString UUID=rec["uuid"].toString();
    if (DOI.isEmpty()) {
        QMessageBox::critical(pluginServices()->GetParentWidget(), tr("Article Search Error"), tr("No DOI given in record"));
    } else {

        insertDOI(DOI, UUID);

    }
}



void LS3PluginArticleTools::fillFromPMID() {
    if (!m_currentDatastore) return;


    QMap<QString, QVariant> rec=m_currentDatastore->currentRecord();

    clearAndRestoreFocus();

    QString pubmed=rec["pubmed"].toString().trimmed();
    QString UUID=rec["uuid"].toString();
    if (pubmed.isEmpty()) {
        QMessageBox::critical(pluginServices()->GetParentWidget(), tr("Article Search Error"), tr("No Pubmed ID given in record"));
    } else {


        insertPubmed(pubmed, UUID);


    }
}

void LS3PluginArticleTools::fillFromPMCID() {
    if (!m_currentDatastore) return;


    QMap<QString, QVariant> rec=m_currentDatastore->currentRecord();

    clearAndRestoreFocus();

    QString pubmed=rec["pmcid"].toString().trimmed();
    QString UUID=rec["uuid"].toString();
    if (pubmed.isEmpty()) {
        QMessageBox::critical(pluginServices()->GetParentWidget(), tr("Article Search Error"), tr("No Pubmed Central ID given in record"));
    } else {
        insertPubmed(pubmed, UUID);
    }
}

void LS3PluginArticleTools::downloadPDFFromPMCID()
{
    if (!m_currentDatastore) return;

    QMap<QString, QVariant> rec=m_currentDatastore->currentRecord();
    clearAndRestoreFocus();
    QString pubmed=rec["pmcid"].toString().trimmed();
    QString UUID=rec["uuid"].toString();
    if (pubmed.isEmpty()) {
        QMessageBox::critical(pluginServices()->GetParentWidget(), tr("Article Search Error"), tr("No Pubmed Central ID given in record"));
    } else {
        QString wwwl=QString("http://www.ncbi.nlm.nih.gov/pmc/articles/%1/pdf").arg(pubmed);

        QDir dir(m_currentDatastore->baseDirectory());
        QString id=m_currentDatastore->getField("id").toString();
        QString files=m_currentDatastore->getField("files").toString();
        if (id.isEmpty()) id ="download";
        QUrl url=QUrl(wwwl);
        QString ext="pdf";
        ext=QFileInfo(url.path()).suffix();
        if (ext.isEmpty() || ext.size()>3) ext="pdf";
        QString filename=id+"."+ext;
        //qDebug()<<wwwl<<url<<ext<<id<<filename;
        int i=1;
        while (dir.exists(filename)) {
            filename=id+"_"+QString::number(i)+"."+ext;
            i++;
        }
        touchFile(filename);
        //qDebug()<<url<<dir.absoluteFilePath(filename);
        if (files.isEmpty()) files=filename;
        else files=files+"\n"+filename;
        m_currentDatastore->setField("files", files);
        downloadFile(wwwl, dir.absoluteFilePath(filename));
    }

}

void LS3PluginArticleTools::downloadPDFFromArxiv()
{
    if (!m_currentDatastore) return;

    QMap<QString, QVariant> rec=m_currentDatastore->currentRecord();
    clearAndRestoreFocus();
    QString pubmed=rec["arxiv"].toString().trimmed();
    QString UUID=rec["uuid"].toString();
    if (pubmed.isEmpty()) {
        QMessageBox::critical(pluginServices()->GetParentWidget(), tr("Article Search Error"), tr("No Arxiv ID given in record"));
    } else {
        QString wwwl=QString("http://arxiv.org/pdf/%1.pdf").arg(pubmed);

        QDir dir(m_currentDatastore->baseDirectory());
        QString id=m_currentDatastore->getField("id").toString();
        QString files=m_currentDatastore->getField("files").toString();
        if (id.isEmpty()) id ="download";
        QUrl url=QUrl(wwwl);
        QString ext="pdf";
        ext=QFileInfo(url.path()).suffix();
        if (ext.isEmpty() || ext.size()>3) ext="pdf";
        QString filename=id+"."+ext;
        //qDebug()<<wwwl<<url<<ext<<id<<filename;
        int i=1;
        while (dir.exists(filename)) {
            filename=id+"_"+QString::number(i)+"."+ext;
            i++;
        }
        touchFile(filename);
        //qDebug()<<url<<dir.absoluteFilePath(filename);
        if (files.isEmpty()) files=filename;
        else files=files+"\n"+filename;
        m_currentDatastore->setField("files", files);
        downloadFile(wwwl, dir.absoluteFilePath(filename));
    }
}

void LS3PluginArticleTools::downloadPDFFromDoi()
{
    if (!m_currentDatastore) return;

    QMap<QString, QVariant> rec=m_currentDatastore->currentRecord();
    clearAndRestoreFocus();
    QString id=rec["doi"].toString().trimmed();
    QString UUID=rec["uuid"].toString();
    int page1=extractNumber(rec["pages"].toString());
    int vol=rec["volume"].toInt();
    int issue=rec["number"].toInt();
    if (id.isEmpty()) {
        QMessageBox::critical(pluginServices()->GetParentWidget(), tr("PDF Download Error"), tr("No DOI given in record"));
    } else {
        QString wwwl;
        //PLoS one
        if (id.contains("journal.pone")) wwwl=QString("http://dx.plos.org/%1.pdf").arg(id);

        // Optics Express
        if (id.contains("/OE.")) wwwl=QString("http://www.opticsinfobase.org/oe/viewmedia.cfm?uri=oe-%1-%2-%3&seq=0").arg(vol).arg(issue).arg(page1);


        // http://www.opticsinfobase.org/oe/viewmedia.cfm?uri=oe-22-3-2358&seq=0
        //http://dx.doi.org/10.1364/OE.22.002358
        if (!wwwl.isEmpty()) {
            QDir dir(m_currentDatastore->baseDirectory());
            QString id=m_currentDatastore->getField("id").toString();
            QString files=m_currentDatastore->getField("files").toString();
            if (id.isEmpty()) id ="download";
            QUrl url=QUrl(wwwl);
            QString ext="pdf";
            ext=QFileInfo(url.path()).suffix();
            if (ext.isEmpty() || ext.size()>3) ext="pdf";
            QString filename=id+"."+ext;
            //qDebug()<<wwwl<<url<<ext<<id<<filename;
            int i=1;
            while (dir.exists(filename)) {
                filename=id+"_"+QString::number(i)+"."+ext;
                i++;
            }
            touchFile(filename);
            //qDebug()<<url<<dir.absoluteFilePath(filename);
            if (files.isEmpty()) files=filename;
            else files=files+"\n"+filename;
            m_currentDatastore->setField("files", files);
            downloadFile(wwwl, dir.absoluteFilePath(filename));
        } else {
            QMessageBox::critical(pluginServices()->GetParentWidget(), tr("PDF Download Error"), tr("Did not find PDF to download"));
        }
    }
}




void LS3PluginArticleTools::pasteWebpage() {
    QStringList wwwl=QApplication::clipboard()->text().split('\n');

    for (int i=0; i<wwwl.size(); i++) {
        QString www=wwwl[i];
        QMap<QString, QVariant> rec;
        rec["url"]=www;
        rec["type"]="webpage";
        insertWebpage(www, "", rec);
    }
}

void LS3PluginArticleTools::pastePDFFromURL() {
    QString wwwl=QApplication::clipboard()->text();

    QDir dir(m_currentDatastore->baseDirectory());
    QString id=m_currentDatastore->getField("id").toString();
    QString files=m_currentDatastore->getField("files").toString();
    if (id.isEmpty()) id ="download";
    QUrl url=QUrl(wwwl);
    QString ext="pdf";
    ext=QFileInfo(url.path()).suffix();
    if (ext.isEmpty() || ext.size()>3) ext="pdf";
    QString filename=id+"."+ext;
    //qDebug()<<wwwl<<url<<ext<<id<<filename;
    int i=1;
    while (dir.exists(filename)) {
        filename=id+"_"+QString::number(i)+"."+ext;
        i++;
    }
    touchFile(filename);
    //qDebug()<<url<<dir.absoluteFilePath(filename);
    if (files.isEmpty()) files=filename;
    else files=files+"\n"+filename;
    m_currentDatastore->setField("files", files);
    downloadFile(wwwl, dir.absoluteFilePath(filename));
}

void LS3PluginArticleTools::insertPDFFromURL() {
    QString wwwl=QApplication::clipboard()->text();

    QDir dir(m_currentDatastore->baseDirectory());
    QString id="download";
    QUrl url=QUrl(wwwl);
    QString ext="pdf";
    ext=QFileInfo(url.path()).suffix();
    if (ext.isEmpty() || ext.size()>3) ext="pdf";
    QString filename=id+"."+ext;
    //qDebug()<<wwwl<<url<<ext<<id<<filename;
    int i=1;
    while (dir.exists(filename)) {
        filename=id+"_"+QString::number(i)+"."+ext;
        i++;
    }
    touchFile(filename);

    downloadAndAnalyzeFile(wwwl, filename);
}

void LS3PluginArticleTools::pasteFromWebpage() {
    QStringList wwwl=QApplication::clipboard()->text().split('\n');

    for (int i=0; i<wwwl.size(); i++) {
        QMap<QString, QVariant> rec;
        QString www=wwwl[i];
        if (www.startsWith("https://")) www=www.replace("https://", "http://");
        QString lastPMID;
        QString lastDOI;

        bool gotPMID=false;
        bool gotDOI=false;

        QRegExp rxPubmed("http://www\\.ncbi\\.nlm\\.nih\\.gov/(pmc/articles|pubmed)/(\\d*)", Qt::CaseInsensitive);
        if (rxPubmed.indexIn(www)>=0) {
            // this is the result of a PubMed search, so we just have to request info from the PMID in the URL
            lastPMID=rxPubmed.cap(2);
            gotPMID=true;
        }


        if (gotPMID && gotDOI) insertPubmedAndDoi(lastPMID, lastDOI);
        else if (gotPMID && !gotDOI) insertPubmed(lastPMID);
        else if (!gotPMID && gotDOI) insertDOI(lastDOI);
        else insertWebpage(www);
    }
}



void LS3PluginArticleTools::insertRecord(QMap<QString, QVariant> data, bool scanPubmedDoi) {
    //qDebug()<<"insertRecord("<<data.size()<<")";
    if (!m_currentDatastore) return;
    QString uuid;
    if (m_currentDatastore->addRecord(data, uuid, true)) {
        if (scanPubmedDoi) {
            insertPubmedAndDoi(data["pubmed"].toString(), data["doi"].toString(), uuid);
        }
    }
}

void LS3PluginArticleTools::storeInRecord(QString record, QMap<QString, QVariant> data, bool scanPubmedDoi) {
    //qDebug()<<"storeInRecord("<<record<<data.size()<<")";
    if (!m_currentDatastore) return;
    QMap<QString, QVariant> rec=m_currentDatastore->getRecord(m_currentDatastore->getRecordByUUID(record));


    QMapIterator<QString, QVariant> it(data);
    while (it.hasNext()) {
        it.next();
        rec[it.key()]=it.value();
    }

    int recN=m_currentDatastore->getRecordByUUID(record);
    m_currentDatastore->setRecord(recN, rec);
    m_currentDatastore->setField(recN, "id", m_currentDatastore->createID(recN, m_pluginServices->GetIDType()));
    if (scanPubmedDoi) {
        rec=m_currentDatastore->getRecord(m_currentDatastore->getRecordByUUID(record));
        insertPubmedAndDoi(rec["pubmed"].toString(), rec["doi"].toString(), record);
    }
}

void LS3PluginArticleTools::insertDOI(const QString &DOI, const QString &recordUUID) {
    DoiThread* t=new DoiThread(NULL);
    if (!recordUUID.isEmpty()) t->setStoreInRecord(recordUUID);
    QMap<QString, QVariant> newRecord;
    newRecord["type"]="article";
    if (!newRecord.isEmpty()) t->initRecordData(newRecord);
    connect(t, SIGNAL(insertRecords(QMap<QString,QVariant>)), this, SLOT(insertRecord(QMap<QString,QVariant>)));
    connect(t, SIGNAL(storeInRecord(QString,QMap<QString,QVariant>)), this, SLOT(storeInRecord(QString,QMap<QString,QVariant>)));
    connect(t, SIGNAL(haveMoreMetadataItems(QList<QMap<QString,QVariant> >)), this, SLOT(insertMoreRecords(QList<QMap<QString,QVariant> >)));
    pluginServices()->addProgressThread(t);
    t->requestFromDoi(DOI, pluginServices());
    t->start();
}

void LS3PluginArticleTools::insertPubmed(const QString &PMID, const QString& recordUUID) {
    PubmedThread* t=new PubmedThread(NULL);
    if (!recordUUID.isEmpty()) t->setStoreInRecord(recordUUID);
    QMap<QString, QVariant> newRecord;
    newRecord["type"]="article";
    if (!newRecord.isEmpty()) t->initRecordData(newRecord);
    connect(t, SIGNAL(insertRecords(QMap<QString,QVariant>)), this, SLOT(insertRecord(QMap<QString,QVariant>)));
    connect(t, SIGNAL(storeInRecord(QString,QMap<QString,QVariant>)), this, SLOT(storeInRecord(QString,QMap<QString,QVariant>)));
    connect(t, SIGNAL(haveMoreMetadataItems(QList<QMap<QString,QVariant> >)), this, SLOT(insertMoreRecords(QList<QMap<QString,QVariant> >)));

    pluginServices()->addProgressThread(t);
    t->requestFromPMID(PMID, pluginServices());
    t->start();

}

void LS3PluginArticleTools::insertPubmedAndDoi(const QString &PMID, const QString &DOI, const QString &recordUUID) {
    //qDebug()<<"insertPubmedAndDoi("<<PMID<<DOI<<recordUUID<<")";
    PubmedThread* t=new PubmedThread(NULL);
    if (!recordUUID.isEmpty()) t->setStoreInRecord(recordUUID);
    QMap<QString, QVariant> newRecord;
    newRecord["type"]="article";
    if (!newRecord.isEmpty()) t->initRecordData(newRecord);
    connect(t, SIGNAL(insertRecords(QMap<QString,QVariant>)), this, SLOT(insertRecord(QMap<QString,QVariant>)));
    connect(t, SIGNAL(storeInRecord(QString,QMap<QString,QVariant>)), this, SLOT(storeInRecord(QString,QMap<QString,QVariant>)));
    connect(t, SIGNAL(haveMoreMetadataItems(QList<QMap<QString,QVariant> >)), this, SLOT(insertMoreRecords(QList<QMap<QString,QVariant> >)));

    pluginServices()->addProgressThread(t);
    t->requestFromPMIDandDOI(PMID, DOI, pluginServices());
    t->start();
}

void LS3PluginArticleTools::insertWebpage(const QString &WWW, const QString &recordUUID, const QMap<QString, QVariant> &newRecordd) {
    QMap<QString, QVariant> newRecord=newRecordd;
    WWWThread* t=new WWWThread(NULL);
    if (!recordUUID.isEmpty()) t->setStoreInRecord(recordUUID);
    if (newRecord.value("www", "").toString().isEmpty()) newRecord["www"]=WWW;
    if (newRecord.value("type", "").toString().isEmpty()) newRecord["type"]="webpage";
    if (!newRecord.isEmpty()) t->initRecordData(newRecord);
    connect(t, SIGNAL(insertRecords(QMap<QString,QVariant>)), this, SLOT(insertRecord(QMap<QString,QVariant>)));
    connect(t, SIGNAL(storeInRecord(QString,QMap<QString,QVariant>)), this, SLOT(storeInRecord(QString,QMap<QString,QVariant>)));
    connect(t, SIGNAL(haveMoreMetadataItems(QList<QMap<QString,QVariant> >)), this, SLOT(insertMoreRecords(QList<QMap<QString,QVariant> >)));

    pluginServices()->addProgressThread(t);
    t->requestWebpageMetadata(WWW, pluginServices());
    t->start();
}

void LS3PluginArticleTools::insertFromWebpage(const QString &WWW)
{
    WWWThread* t=new WWWThread(NULL);
    connect(t, SIGNAL(insertRecords(QMap<QString,QVariant>)), this, SLOT(insertRecord(QMap<QString,QVariant>)));
    connect(t, SIGNAL(storeInRecord(QString,QMap<QString,QVariant>)), this, SLOT(storeInRecord(QString,QMap<QString,QVariant>)));
    connect(t, SIGNAL(haveMoreMetadataItems(QList<QMap<QString,QVariant> >)), this, SLOT(insertMoreRecords(QList<QMap<QString,QVariant> >)));

    pluginServices()->addProgressThread(t);
    t->requestMetadatasFromWebpage(WWW, pluginServices());
    t->start();

}

void LS3PluginArticleTools::insertFile(const QString& filename, const QString& type, int action) {
    AnalyzePDFThread* t=new AnalyzePDFThread(NULL);
    //if (!recordUUID.isEmpty()) t->setStoreInRecord(recordUUID);
    QDir d_lit(m_currentDatastore->baseDirectory());
    t->initPDF(filename, action, type, d_lit, pluginServices());
    connect(t, SIGNAL(insertRecords(QMap<QString,QVariant>,bool)), this, SLOT(insertRecord(QMap<QString,QVariant>,bool)));
    connect(t, SIGNAL(storeInRecord(QString,QMap<QString,QVariant>,bool)), this, SLOT(storeInRecord(QString,QMap<QString,QVariant>,bool)));
    pluginServices()->addProgressThread(t);
    t->start();
}
void LS3PluginArticleTools::downloadFile(QString url, QString filename) {
    WWWThread* t=new WWWThread(NULL);
    pluginServices()->addProgressThread(t);
    t->initDownloadRequest(url, filename, m_pluginServices);
    t->start();
}

/*void LS3PluginArticleTools::searchPubmed(const QString &searchphrase)
{
    PubmedThread* t=new PubmedThread(this);
    QMap<QString, QVariant> newRecord;
    newRecord["type"]="article";
    if (!newRecord.isEmpty()) t->initRecordData(newRecord);
    connect(t, SIGNAL(insertRecords(QMap<QString,QVariant>)), this, SLOT(insertRecord(QMap<QString,QVariant>)));
    connect(t, SIGNAL(storeInRecord(QString,QMap<QString,QVariant>)), this, SLOT(storeInRecord(QString,QMap<QString,QVariant>)));
    connect(t, SIGNAL(haveMoreMetadataItems(QList<QMap<QString,QVariant> >)), this, SLOT(insertMoreRecords(QList<QMap<QString,QVariant> >)));

    pluginServices()->addProgressThread(t);
    t->requestSearchPubmed(searchphrase, pluginServices());
    t->start();

}*/

void LS3PluginArticleTools::downloadAndAnalyzeFile(QString url, QString filename) {
    WWWThread* t=new WWWThread(NULL);
    pluginServices()->addProgressThread(t);
    t->initDownloadRequest(url, filename, m_pluginServices);
    connect(t, SIGNAL(downloadComplete(QString,QString)), this, SLOT(scanPDFAndInsert(QString,QString)));
    t->start();
}

void LS3PluginArticleTools::clearAndRestoreFocus() {
    QWidget* focusw=QApplication::focusWidget();
    if (   focusw->inherits("QLineEdit")
           || focusw->inherits("QAbstactSpinBox")
           || focusw->inherits("QComboBox")
           || focusw->inherits("QPlainTextEdit")
           || focusw->inherits("QTextEdit") ) {
        focusw->clearFocus();
        QApplication::processEvents();
        focusw->setFocus(Qt::MouseFocusReason);
        QApplication::processEvents();
    }
}

void LS3PluginArticleTools::scanPDFAndInsert(QString record, QString filename) {
    //qDebug()<<"scanPDFAndInsert("<<record<<filename<<")";
    insertFile(filename, "article");
}

void LS3PluginArticleTools::insertMoreRecords(QList<QMap<QString, QVariant> > moreMetadata) {
    //qDebug()<<"insertMoreRecords("<<moreMetadata.size()<<" items)";
    MultipleRecordInsertDialog* dlg=new MultipleRecordInsertDialog();
    for (int i=0; i<moreMetadata.size();i++) {
        dlg->addRecord(moreMetadata[i]);
    }
    if (dlg->exec()==QDialog::Accepted) {
        for (int i=0; i<moreMetadata.size();i++) {
            if (dlg->isImported(i)) {
                QMap<QString, QVariant> mrec=moreMetadata[i];
                if (mrec["type"].toString().isEmpty()) mrec["type"]="misc";
                QString uuid;
                m_currentDatastore->addRecord(mrec, uuid, true);
                int recN=m_currentDatastore->getRecordByUUID(uuid);
                m_currentDatastore->setField(recN, "id", m_currentDatastore->createID(recN, m_pluginServices->GetIDType()));
            }
        }
    }

    delete dlg;
}

void LS3PluginArticleTools::renameFiles() {
    if (datastore()) {
        RenameFilesDialog* dlg=new RenameFilesDialog(m_pluginServices->GetParentWidget());
        dlg->readSettings(pluginServices()->GetQSettings());
        if (dlg->exec()==QDialog::Accepted) {
            dlg->writeSettings(pluginServices()->GetQSettings());

            QVector<QMap<QString, QVariant> > data;
            QStringList uuids=pluginServices()->getVisibleRecords();

            RenameFilesProgressDialog* pdlg=new RenameFilesProgressDialog();
            pdlg->setMessage(tr("renaming linked files:<br><center>preparing ...</center>"));
            pdlg->setRange(0,100);
            pdlg->setProgress(0);
            pdlg->show();

            QApplication::setOverrideCursor(Qt::WaitCursor);
            QApplication::processEvents();
            switch(dlg->getWhich()) {
                case RenameFilesDialog::Selected:
                    for (int i=0; i<uuids.size(); i++) {
                        if (datastore()->isSelected(uuids[i])) data.append(datastore()->getRecord(datastore()->getRecordByUUID(uuids[i])));
                        if (i%10==0) QApplication::processEvents();
                    }
                    break;
                case RenameFilesDialog::Current:
                    data.append(datastore()->currentRecord());
                    break;
                default:
                case RenameFilesDialog::All:
                    for (int i=0; i<uuids.size(); i++) {
                        data.append(datastore()->getRecord(datastore()->getRecordByUUID(uuids[i])));
                        if (i%10==0) QApplication::processEvents();
                    }
                    break;
            }
            if (!pdlg->cancelPressed()) {
                pdlg->setMessage(tr("renaming linked files:<br><center>renaming ...</center>"));
                QApplication::processEvents();
                if (data.size()>0) {
                    pdlg->setRange(0, data.size()-1);
                    for (int i=0; i<data.size(); i++) {
                        QString oldFilename=data[i].value("files", "").toString();
                        if (oldFilename.contains('\n')) oldFilename=oldFilename.split("\n").value(0,"");
                        QString oldFilenameBase=QFileInfo(oldFilename).baseName();
                        QString oldFilenameName=QFileInfo(oldFilename).fileName();
                        QString oldFilenameDir=QFileInfo(QDir(datastore()->baseDirectory()).absoluteFilePath(oldFilename)).absolutePath();
                        QString oldFilenameAbs=oldFilenameDir+"/"+oldFilenameName;
                        if (!oldFilename.isEmpty()) {
                            QString newFilename=oldFilenameName;
                            if (dlg->renameFiles()) newFilename=createFileName(dlg->renameScheme(), data[i], oldFilenameName, dlg->maxLength());
                            QString newFilenameDir=oldFilenameDir;
                            if (dlg->moveFiles()) newFilenameDir=createFileDir(dlg->directoryScheme(), data[i], oldFilenameDir, dlg->maxLength());

                            newFilenameDir=QFileInfo(QDir(datastore()->baseDirectory()).relativeFilePath(newFilenameDir+"/"+newFilename)).path();
                            QString newFilenameAbs=QDir(datastore()->baseDirectory()).absoluteFilePath(newFilenameDir+"/"+newFilename);
                            pdlg->addFilesPair(oldFilename, newFilenameDir+"/"+newFilename);
                            //std::cout<<"oldFilenameAbs="<<oldFilenameAbs.toStdString()<<"   newFilenameAbs="<<newFilenameAbs.toStdString()<<"\n";

                            if (newFilename.isEmpty()) {
                                pdlg->setCurrentStatus(tr("---"));
                            } else {
                                if (QFile::exists(oldFilenameAbs)) {
                                    if (QFile::exists(newFilenameAbs)) {
                                        pdlg->setCurrentStatus(tr("TE"));
                                    } else {
                                        if (dlg->simulate()) {
                                            pdlg->setCurrentStatus(tr("SIM.OK"), true);
                                        } else {
                                            bool ok=true;
                                            if (!QDir(datastore()->baseDirectory()).cd(newFilenameDir)) {
                                                ok=QDir(datastore()->baseDirectory()).mkpath(newFilenameDir);
                                            }
                                            if (ok) {
                                                if (QFile::copy(oldFilenameAbs, newFilenameAbs)) {
                                                    //if (QFile::remove(oldFilenameAbs)) std::cout<<"remove("<<oldFilenameAbs.toStdString()<<") = true\n";
                                                    //else std::cout<<"remove("<<oldFilenameAbs.toStdString()<<") = false\n";
                                                    QStringList filesList=data[i].value("files", "").toString().split("\n");
                                                    int rec=datastore()->getRecordByUUID(data[i].value("uuid", "").toString());
                                                    if (rec>=0) {
                                                        if (filesList.size()<=1) {
                                                            datastore()->setField(rec, "files", newFilenameDir+"/"+newFilename);
                                                        } else {
                                                            filesList.replace(0, newFilenameDir+"/"+newFilename);
                                                            datastore()->setField(rec, "files", filesList.join("\n"));
                                                        }
                                                    }
                                                    if (dlg->deleteOldFile()) QFile::remove(oldFilenameAbs);
                                                    pdlg->setCurrentStatus(tr("OK"), true);
                                                }  else {
                                                    pdlg->setCurrentStatus(tr("E"));
                                                }
                                            } else {
                                                pdlg->setCurrentStatus(tr("E"));
                                            }
                                        }
                                    }

                                } else {
                                    pdlg->setCurrentStatus(tr("NE"));
                                }
                            }
                        }
                        if (i%10==0) {
                            pdlg->setProgress(i);
                            QApplication::processEvents();
                        }

                    }
                    pdlg->setProgress(data.size()-1);
                    QApplication::restoreOverrideCursor();
                    pdlg->setReportMode();
                    while (pdlg->isVisible()) {
                        QApplication::processEvents();
                    }
                }  else {
                    pdlg->close();
                    QApplication::restoreOverrideCursor();
                    QMessageBox::information(NULL, tr("Rename Files"), tr("No datasets selected for rename!"));
                }
            }
            delete pdlg;


        }
        delete dlg;
    }
}


void LS3PluginArticleTools::renameFilesCurrentRecord() {
    if (datastore()) {
        RenameFilesDialog* dlg=new RenameFilesDialog(m_pluginServices->GetParentWidget());
        dlg->setCurrentRecordOnly(true);
        dlg->readSettings(pluginServices()->GetQSettings());
        if (dlg->exec()==QDialog::Accepted) {
            dlg->writeSettings(pluginServices()->GetQSettings());

            QVector<QMap<QString, QVariant> > data;
            QStringList uuids=pluginServices()->getVisibleRecords();

            RenameFilesProgressDialog* pdlg=new RenameFilesProgressDialog();
            pdlg->setMessage(tr("renaming linked files:<br><center>preparing ...</center>"));
            pdlg->setRange(0,100);
            pdlg->setProgress(0);
            pdlg->show();

            QApplication::setOverrideCursor(Qt::WaitCursor);
            QApplication::processEvents();
            data.append(datastore()->currentRecord());

            if (!pdlg->cancelPressed()) {
                pdlg->setMessage(tr("renaming linked files:<br><center>renaming ...</center>"));
                QApplication::processEvents();
                if (data.size()>0) {
                    pdlg->setRange(0, data.size()-1);
                    for (int i=0; i<data.size(); i++) {
                        QString oldFilename=data[i].value("files", "").toString();
                        if (oldFilename.contains('\n')) oldFilename=oldFilename.split("\n").value(0,"");
                        QString oldFilenameBase=QFileInfo(oldFilename).baseName();
                        QString oldFilenameName=QFileInfo(oldFilename).fileName();
                        QString oldFilenameDir=QFileInfo(QDir(datastore()->baseDirectory()).absoluteFilePath(oldFilename)).absolutePath();
                        QString oldFilenameAbs=oldFilenameDir+"/"+oldFilenameName;
                        if (!oldFilename.isEmpty()) {
                            QString newFilename=oldFilenameName;
                            if (dlg->renameFiles()) newFilename=createFileName(dlg->renameScheme(), data[i], oldFilenameName, dlg->maxLength());
                            QString newFilenameDir=oldFilenameDir;
                            if (dlg->moveFiles()) newFilenameDir=createFileDir(dlg->directoryScheme(), data[i], oldFilenameDir, dlg->maxLength());

                            newFilenameDir=QFileInfo(QDir(datastore()->baseDirectory()).relativeFilePath(newFilenameDir+"/"+newFilename)).path();
                            QString newFilenameAbs=QDir(datastore()->baseDirectory()).absoluteFilePath(newFilenameDir+"/"+newFilename);
                            pdlg->addFilesPair(oldFilename, newFilenameDir+"/"+newFilename);
                            //std::cout<<"oldFilenameAbs="<<oldFilenameAbs.toStdString()<<"   newFilenameAbs="<<newFilenameAbs.toStdString()<<"\n";

                            if (newFilename.isEmpty()) {
                                pdlg->setCurrentStatus(tr("---"));
                            } else {
                                if (QFile::exists(oldFilenameAbs)) {
                                    if (QFile::exists(newFilenameAbs)) {
                                        pdlg->setCurrentStatus(tr("TE"));
                                    } else {
                                        if (dlg->simulate()) {
                                            pdlg->setCurrentStatus(tr("SIM.OK"), true);
                                        } else {
                                            bool ok=true;
                                            if (!QDir(datastore()->baseDirectory()).cd(newFilenameDir)) {
                                                ok=QDir(datastore()->baseDirectory()).mkpath(newFilenameDir);
                                            }
                                            if (ok) {
                                                if (QFile::copy(oldFilenameAbs, newFilenameAbs)) {
                                                    //if (QFile::remove(oldFilenameAbs)) std::cout<<"remove("<<oldFilenameAbs.toStdString()<<") = true\n";
                                                    //else std::cout<<"remove("<<oldFilenameAbs.toStdString()<<") = false\n";
                                                    QStringList filesList=data[i].value("files", "").toString().split("\n");
                                                    int rec=datastore()->getRecordByUUID(data[i].value("uuid", "").toString());
                                                    if (rec>=0) {
                                                        if (filesList.size()<=1) {
                                                            datastore()->setField(rec, "files", newFilenameDir+"/"+newFilename);
                                                        } else {
                                                            filesList.replace(0, newFilenameDir+"/"+newFilename);
                                                            datastore()->setField(rec, "files", filesList.join("\n"));
                                                        }
                                                    }
                                                    if (dlg->deleteOldFile()) QFile::remove(oldFilenameAbs);
                                                    pdlg->setCurrentStatus(tr("OK"), true);
                                                }  else {
                                                    pdlg->setCurrentStatus(tr("E"));
                                                }
                                            } else {
                                                pdlg->setCurrentStatus(tr("E"));
                                            }
                                        }
                                    }

                                } else {
                                    pdlg->setCurrentStatus(tr("NE"));
                                }
                            }
                        }
                        if (i%10==0) {
                            pdlg->setProgress(i);
                            QApplication::processEvents();
                        }

                    }
                    pdlg->setProgress(data.size()-1);
                    QApplication::restoreOverrideCursor();
                    pdlg->setReportMode();
                    while (pdlg->isVisible()) {
                        QApplication::processEvents();
                    }
                }  else {
                    pdlg->close();
                    QApplication::restoreOverrideCursor();
                    QMessageBox::information(NULL, tr("Rename Files"), tr("No datasets selected for rename!"));
                }
            }
            delete pdlg;


        }
        delete dlg;
    }
}


void LS3PluginArticleTools::insertFilesArticle() {
    QString initDir=pluginServices()->GetQSettings()->value("plugins/fileimport_initDir", "").toString();
    QStringList files = QFileDialog::getOpenFileNames(
                         pluginServices()->GetParentWidget(),
                         QObject::tr("Select one or more files to import"),
                         initDir,
                         "Portable Document Format (*.pdf);;PostScript Files (*.ps *.eps);;Word Documents (*.doc *.docx);;All Files (*.*)");
    if (files.size()>0) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        insertFiles(files, "article", 0);
        QApplication::restoreOverrideCursor();
    }
}

void LS3PluginArticleTools::filesDropped(QStringList files, QString type, int action) {
    if (files.size()>0) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        insertFiles(files, type, action);
        QApplication::restoreOverrideCursor();
    }
}

void LS3PluginArticleTools::insertFiles(const QStringList& files, const QString& type, int action) {
    for (int i=0; i<files.size(); i++) {
        insertFile(files[i], type, action);
        QApplication::processEvents();
    }
}

/*void LS3PluginArticleTools::insertFile(const QString& filename, const QString& type, int action) {
    //std::cout<<"type="<<type.toStdString()<<"   file="<<filename.toStdString()<<"\n";
    if (!m_currentDatastore) return;
    if (!m_currentDatastore->dbIsLoaded()) return;
    QDir d_lit(m_currentDatastore->baseDirectory());
    if (QFile::exists(filename)) {
        QMap<QString, QString> xmp= extractXMP(filename, m_pluginServices->GetAndWords(), m_pluginServices->GetNameAdditions(), m_pluginServices->GetNamePrefixes());
        QMap<QString, QString> fromPDF;
        if (QFileInfo(filename).suffix().toLower()=="pdf") {
            fromPDF=extractFromPDF(filename, m_pluginServices->GetAndWords(), m_pluginServices->GetNameAdditions(), m_pluginServices->GetNamePrefixes(), m_pluginServices->getPluginDirectory());
        }
        //qDebug()<<"XMP: "<<xmp;
        //qDebug()<<"from PDF: "<<fromPDF;

        QString filename_base=QFileInfo(filename).baseName();
        QString filename_suffix=QFileInfo(filename).completeSuffix();

        if (d_lit.exists(filename_base+"."+filename_suffix)) {
            QString filename_base1=filename_base;
            int i=0;
            while (d_lit.exists(filename_base1+"."+filename_suffix)) {
                i++;
                filename_base1=filename_base+"_"+QString::number(i);
            }
            filename_base=filename_base1;
        }

        QString newFilename=filename_base+"."+filename_suffix;

        if (action==1) {
            QFile::copy(filename, d_lit.absoluteFilePath(newFilename));
        } else if (action==2) {
            QFile::copy(filename, d_lit.absoluteFilePath(newFilename));
            QFile::remove(filename);
        } else {
            newFilename = d_lit.relativeFilePath(filename);
        }

        QMap<QString, QVariant> data;
        if (!xmp.isEmpty()) {
            QMapIterator<QString, QString> it(xmp);
            while (it.hasNext()) {
                it.next();
                data[it.key()]=it.value();
            }
        }

        if (!fromPDF.isEmpty()) {
            QMapIterator<QString, QString> it(fromPDF);
            while (it.hasNext()) {
                it.next();
                data[it.key()]=it.value();
            }
        }

        data["files"]=newFilename;
        if (data["type"].toString().isEmpty()) data["type"]=type;

        QString doi=data["doi"].toString();
        QString pubmed=data["pubmed"].toString();
        QString uuid;
        if (m_currentDatastore->addRecord(data, uuid, true)) {
            if (!doi.isEmpty() && !pubmed.isEmpty()) {
                insertPubmedAndDoi(pubmed, doi, uuid);
            }
        }

    }
}*/


void LS3PluginArticleTools::typeChanged(QString item) {
    pluginServices()->GetQSettings()->setValue("plugins/fileimport_lasttype", item);
}

void LS3PluginArticleTools::actionChanged(int item) {
    pluginServices()->GetQSettings()->setValue("plugins/fileimport_lastaction", item);
}


#if (QT_VERSION <= QT_VERSION_CHECK(5, 0, 0))
Q_EXPORT_PLUGIN2(articletools, LS3PluginArticleTools)

#endif
