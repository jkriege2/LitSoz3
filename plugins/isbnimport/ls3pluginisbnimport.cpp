#include "ls3pluginisbnimport.h"

#include <QtGui>
#include <iostream>
#include <QtXml>

LS3PluginISBNImport::LS3PluginISBNImport():
    LS3PluginBase()
{
    //ctor
}

LS3PluginISBNImport::~LS3PluginISBNImport()
{
    //dtor
}


void LS3PluginISBNImport::init(LS3PluginServices* pluginServices) {
    LS3PluginBase::init(pluginServices);

    QIcon icon(":/isbnimport/search.png");
    icon.addFile(":/isbnimport/search_disabled.png", QSize(), QIcon::Disabled);
    icon.addFile(":/isbnimport/search_hover.png", QSize(), QIcon::Selected);
    icon.addFile(":/isbnimport/search_pressed.png", QSize(), QIcon::Active);

    QWidget* p=pluginServices->GetParentWidget();

    widMain=new QWidget(p);
    QHBoxLayout* l=new QHBoxLayout(p);
    //l->setContentsMargins(0,0,0,0);
    widMain->setLayout(l);

    QDockWidget* dw=new QDockWidget(tr("ISBN search"), p);
    dw->setObjectName("toolbar_isbnimport");

    cmbSource=new QComboBox(p);
    cmbSource->addItem(tr("ISBNdb.com"));
    cmbSource->addItem(tr("Google Books"));
    cmbSource->addItem(tr("COPAC"));
    l->addWidget(new QLabel(tr("source: "), p));
    l->addWidget(cmbSource);
    cmbSource->setCurrentIndex(pluginServices->GetQSettings()->value("plugins/isbnlookup_lastsource", 1).toInt());


    edtISBN=new QEnhancedLineEdit(widMain);
    btnExecute=new JKStyledButton(icon, edtISBN);
    edtISBN->addButton(btnExecute);
    edtISBN->setMaximumWidth(150);
    l->addWidget(new QLabel(tr("  ISBN:"), p));
    l->addWidget(edtISBN);
    QShortcut* sc=new QShortcut(QKeySequence("Enter,Return"), edtISBN);
    connect(sc, SIGNAL(activated()), this, SLOT(insert()));
    connect(btnExecute, SIGNAL(clicked()), this, SLOT(insert()));

    dw->setWidget(widMain);

    pluginServices->addDockWidget(dw);

    QAction* actInsert=new QAction(QIcon(":/isbnimport/insert.png"), tr("ISBN Web Search"), this);
    connect(actInsert, SIGNAL(triggered()), dw, SLOT(show()));
    pluginServices->getMenu(LS3PluginServices::ToolsMenu)->addAction(actInsert);



}

void LS3PluginISBNImport::deinit(LS3PluginServices* pluginServices) {
    LS3PluginBase::deinit(pluginServices);
}

void LS3PluginISBNImport::openData(LS3Datastore* datastore) {
    LS3PluginBase::openData(datastore);
}

void LS3PluginISBNImport::closeData(LS3Datastore* datastore) {
    LS3PluginBase::closeData(datastore);
}

void LS3PluginISBNImport::insert() {
    pluginServices()->GetQSettings()->setValue("plugins/isbnlookup_lastsource", cmbSource->currentIndex());
    //std::cout<<"inserting ISBN\n";
    if (cmbSource->currentIndex()==0) {
        // ISBNdb
        insertFromISBNdb(edtISBN->text());

    } else if (cmbSource->currentIndex()==1) {
        // Google Books
        insertFromGoogleBooks(edtISBN->text());

    } else if (cmbSource->currentIndex()==2) {
        // COPAC
        insertFromCOPAC(edtISBN->text());
    }
}


void LS3PluginISBNImport::insertFromISBNdb(const QString &ISBN_in, const QString &recordUUID) {
    QString ISBN=ISBN_in.simplified().trimmed();
    ISBN=ISBN.remove('-');
    ISBN=ISBN.remove(',');
    ISBN=ISBN.remove('.');
    ISBN=ISBN.remove('_');
    ISBN=ISBN.remove(QRegExp("[^\\d]"));
    ISBNThread* t=new ISBNThread(this);
    if (!recordUUID.isEmpty()) t->setStoreInRecord(recordUUID);
    QMap<QString, QVariant> newRecord;
    newRecord["type"]="book";
    if (!newRecord.isEmpty()) t->initRecordData(newRecord);
    connect(t, SIGNAL(insertRecords(QMap<QString,QVariant>)), this, SLOT(insertRecord(QMap<QString,QVariant>)));
    connect(t, SIGNAL(storeInRecord(QString,QMap<QString,QVariant>)), this, SLOT(storeInRecord(QString,QMap<QString,QVariant>)));
    connect(t, SIGNAL(haveMoreMetadataItems(QList<QMap<QString,QVariant> >)), this, SLOT(insertMoreRecords(QList<QMap<QString,QVariant> >)));
    pluginServices()->addProgressThread(t);
    t->requestISBNdb(ISBN, pluginServices());
    t->start();
}

void LS3PluginISBNImport::insertFromGoogleBooks(const QString &ISBN_in, const QString &recordUUID) {
    QString ISBN=ISBN_in.simplified().trimmed();
    ISBN=ISBN.remove('-');
    ISBN=ISBN.remove(',');
    ISBN=ISBN.remove('.');
    ISBN=ISBN.remove('_');
    ISBN=ISBN.remove(QRegExp("[^\\d]"));
    ISBNThread* t=new ISBNThread(this);
    if (!recordUUID.isEmpty()) t->setStoreInRecord(recordUUID);
    QMap<QString, QVariant> newRecord;
    newRecord["type"]="book";
    if (!newRecord.isEmpty()) t->initRecordData(newRecord);
    connect(t, SIGNAL(insertRecords(QMap<QString,QVariant>)), this, SLOT(insertRecord(QMap<QString,QVariant>)));
    connect(t, SIGNAL(storeInRecord(QString,QMap<QString,QVariant>)), this, SLOT(storeInRecord(QString,QMap<QString,QVariant>)));
    connect(t, SIGNAL(haveMoreMetadataItems(QList<QMap<QString,QVariant> >)), this, SLOT(insertMoreRecords(QList<QMap<QString,QVariant> >)));
    pluginServices()->addProgressThread(t);
    t->requestGoogleBooks(ISBN, pluginServices());
    t->start();
}

void LS3PluginISBNImport::insertFromCOPAC(const QString &ISBN_in, const QString &recordUUID) {
    QString ISBN=ISBN_in.simplified().trimmed();
    ISBN=ISBN.remove('-');
    ISBN=ISBN.remove(',');
    ISBN=ISBN.remove('.');
    ISBN=ISBN.remove('_');
    ISBN=ISBN.remove(QRegExp("[^\\d]"));
    ISBNThread* t=new ISBNThread(this);
    if (!recordUUID.isEmpty()) t->setStoreInRecord(recordUUID);
    QMap<QString, QVariant> newRecord;
    newRecord["type"]="book";
    if (!newRecord.isEmpty()) t->initRecordData(newRecord);
    connect(t, SIGNAL(insertRecords(QMap<QString,QVariant>)), this, SLOT(insertRecord(QMap<QString,QVariant>)));
    connect(t, SIGNAL(storeInRecord(QString,QMap<QString,QVariant>)), this, SLOT(storeInRecord(QString,QMap<QString,QVariant>)));
    connect(t, SIGNAL(haveMoreMetadataItems(QList<QMap<QString,QVariant> >)), this, SLOT(insertMoreRecords(QList<QMap<QString,QVariant> >)));
    pluginServices()->addProgressThread(t);
    t->requestCOPAC(ISBN, pluginServices());
    t->start();
}
void LS3PluginISBNImport::insertMoreRecords(QList<QMap<QString, QVariant> > moreMetadata) {
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

void LS3PluginISBNImport::insertRecord(QMap<QString, QVariant> data) {
    if (!m_currentDatastore) return;
    QString uuid;
    m_currentDatastore->addRecord(data, uuid, true);
}

void LS3PluginISBNImport::storeInRecord(QString record, QMap<QString, QVariant> data) {
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
}


Q_EXPORT_PLUGIN2(isbnimport, LS3PluginISBNImport)
