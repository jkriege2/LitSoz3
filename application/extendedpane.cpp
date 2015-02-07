#include "extendedpane.h"
#include <iostream>
#include <QToolButton>

ExtendedPane::ExtendedPane(ProgramOptions* settings, QWidget* parent):
    QWidget(parent)
{
    QLabel* label=NULL;
    QHBoxLayout* li=NULL;
    QWidget* w=NULL;
    QFrame* f=NULL;

    int extendedShortWidth=settings->GetExtendedShortWidth();
    int extendedLongWidth=settings->GetExtendedLongWidth();


    this->settings=settings;
    datastore=NULL;
    connected=false;

    QGridLayout* mlay=new QGridLayout(this);
    QFormLayout* lay=new QFormLayout(this);
    setLayout(mlay);
    mlay->addLayout(lay, 0, 0);
    mlay->setContentsMargins(10,10,10,10);
    mlay->setHorizontalSpacing(64);
    lay->setHorizontalSpacing(16);
    lay->setVerticalSpacing(4);

    cmbOrigin=new QMappableComboBox(this);
    cmbOrigin->setEditable(true);
    cmbOrigin->setInsertPolicy(QComboBox::InsertAlphabetically);
    cmbOrigin->setFilename(settings->GetConfigDirectory()+"/completers/origin.lst");
    if (extendedShortWidth>0) cmbOrigin->setMaximumWidth(extendedShortWidth);
    lay->addRow(tr("&Origin:"), cmbOrigin);

    edtLibNum=new QEnhancedLineEdit(this);
    lay->addRow(tr("&Library No.:"), edtLibNum);
    if (extendedShortWidth>0) edtLibNum->setMaximumWidth(extendedShortWidth);

    f=new QFrame(this);
    f->setFrameShape(QFrame::HLine);
    f->setMidLineWidth(1);
    f->setLineWidth(0);
    f->setMaximumHeight(5);
    f->setFrameShadow(QFrame::Raised);
    lay->addRow(f);

    edtOwner=new QEnhancedLineEdit(this);
    li=new QHBoxLayout(this);
    li->setContentsMargins (0,0,0,0);
    w=new QWidget(this);
    if (extendedShortWidth>0) w->setMaximumWidth(extendedShortWidth);
    w->setLayout(li);
    li->addWidget(edtOwner, 3);

    //lay->addRow(tr("O&wner:"), edtOwner);

    dedWhenAdded=new QDateEdit(this);
    //lay->addRow(tr("A&dded:"), dedWhenAdded);
    label=new QLabel(tr("      &Added:"), this);
    label->setBuddy(dedWhenAdded);
    li->addWidget(label);
    li->addWidget(dedWhenAdded, 1);
    lay->addRow(tr("O&wner:"), w);
    if (QLabel *lab = qobject_cast<QLabel *>(lay->labelForField(li))) {
        lab->setBuddy(edtOwner);
    }

    f=new QFrame(this);
    f->setFrameShape(QFrame::HLine);
    f->setMidLineWidth(1);
    f->setLineWidth(0);
    f->setMaximumHeight(5);
    f->setFrameShadow(QFrame::Raised);
    lay->addRow(f);


    cmbStatus=new QMappableComboBox(this);
    if (extendedShortWidth>0) cmbStatus->setMaximumWidth(extendedShortWidth);
    cmbStatus->setEditable(true);
    cmbStatus->setInsertPolicy(QComboBox::InsertAlphabetically);
    cmbStatus->setFilename(settings->GetConfigDirectory()+"/completers/status.lst");
    cmbStatus->setTranslationFilename(settings->GetConfigDirectory()+"/completers/status_"+settings->GetLanguageID().toLower()+".translation");

    dedStatusSince=new QDateEdit(this);
    li=new QHBoxLayout(this);
    li->setContentsMargins (0,0,0,0);
    li->addWidget(cmbStatus, 3);
    label=new QLabel(tr("      since:"), this);
    label->setBuddy(dedStatusSince);
    li->addWidget(label);
    li->addWidget(dedStatusSince, 1);
    label=new QLabel(tr("&Status:"), this);
    label->setBuddy(cmbStatus);
    w=new QWidget(this);
    if (extendedShortWidth>0) w->setMaximumWidth(extendedShortWidth);
    w->setLayout(li);
    lay->addRow(label, w);

    edtStatusComment=new QEnhancedLineEdit(this);
    if (extendedShortWidth>0) edtStatusComment->setMaximumWidth(extendedShortWidth);
    lay->addRow(tr("Status &Comment:"), edtStatusComment);


    f=new QFrame(this);
    f->setFrameShape(QFrame::HLine);
    f->setMidLineWidth(1);
    f->setLineWidth(0);
    f->setMaximumHeight(5);
    f->setFrameShadow(QFrame::Raised);
    lay->addRow(f);

    cmbCurrency=new QMappableComboBox(this);
    cmbCurrency->setEditable(true);
    cmbCurrency->setInsertPolicy(QComboBox::InsertAlphabetically);
    cmbCurrency->setFilename(settings->GetConfigDirectory()+"/completers/currency.lst");
    edtPrice=new QDoubleSpinBox(this);
    edtPrice->setRange(0,1e20);
    li=new QHBoxLayout(this);
    li->setContentsMargins (0,0,0,0);
    li->addWidget(cmbCurrency,1);
    li->addWidget(edtPrice,4);
    label=new QLabel(tr("&Price:"), this);
    label->setBuddy(cmbCurrency);
    w=new QWidget(this);
    if (extendedShortWidth>0) w->setMaximumWidth(extendedShortWidth);
    w->setLayout(li);
    lay->addRow(label, w);

    f=new QFrame(this);
    f->setFrameShape(QFrame::HLine);
    f->setMidLineWidth(1);
    f->setLineWidth(0);
    f->setMaximumHeight(5);
    f->setFrameShadow(QFrame::Raised);
    lay->addRow(f);

    li=new QHBoxLayout(this);
    li->setContentsMargins (0,0,0,0);
    edtDoi=new QEnhancedLineEdit(this);
    btnDoi=new JKStyledButton(JKStyledButton::OpenPrependedURL, edtDoi, this);
    btnDoi->setPrependURL("http://dx.doi.org/");
    QIcon dicon(":/doi/search.png");
    dicon.addFile(":/doi/search_disabled.png", QSize(), QIcon::Disabled);
    dicon.addFile(":/doi/search_hover.png", QSize(), QIcon::Selected);
    dicon.addFile(":/doi/search_pressed.png", QSize(), QIcon::Active);
    //btnDoi->setIcon(dicon);
    edtDoi->addButton(btnDoi);
    li->addWidget(edtDoi);
    label=new QLabel(tr("&DOI:"), this);
    label->setBuddy(edtDoi);
    w=new QWidget(this);
    if (extendedShortWidth>0) w->setMaximumWidth(extendedShortWidth);
    w->setLayout(li);
    lay->addRow(label, w);

    li=new QHBoxLayout(this);
    li->setContentsMargins (0,0,0,0);
    edtCiteseer=new QEnhancedLineEdit(this);
    li->addWidget(edtCiteseer);
    label=new QLabel(tr("&Citeseer:"), this);
    label->setBuddy(edtCiteseer);
    w=new QWidget(this);
    if (extendedShortWidth>0) w->setMaximumWidth(extendedShortWidth);
    w->setLayout(li);
    lay->addRow(label, w);

    li=new QHBoxLayout(this);
    li->setContentsMargins (0,0,0,0);
    edtPubmed=new QEnhancedLineEdit(this);
    btnPubmed=new JKStyledButton(JKStyledButton::OpenPrependedURL, edtPubmed, this);
    btnPubmed->setPrependURL("http://www.pubmed.org/");
    dicon=QIcon(":/pubmed/search.png");
    dicon.addFile(":/pubmed/search_disabled.png", QSize(), QIcon::Disabled);
    dicon.addFile(":/pubmed/search_hover.png", QSize(), QIcon::Selected);
    dicon.addFile(":/pubmed/search_pressed.png", QSize(), QIcon::Active);
    //btnPubmed->setIcon(dicon);
    edtPubmed->addButton(btnPubmed);
    li->addWidget(edtPubmed);
    label=new QLabel(tr("&PubMed:"), this);
    label->setBuddy(edtPubmed);
    w=new QWidget(this);
    if (extendedShortWidth>0) w->setMaximumWidth(extendedShortWidth);
    w->setLayout(li);
    lay->addRow(label, w);

    li=new QHBoxLayout(this);
    li->setContentsMargins (0,0,0,0);
    edtPMCID=new QEnhancedLineEdit(this);
    btnPMCID=new JKStyledButton(JKStyledButton::OpenPrependedURL, edtPMCID, this);
    btnPMCID->setPrependURL("http://www.ncbi.nlm.nih.gov/pmc/articles/");
    dicon=QIcon(":/pubmed/search.png");
    dicon.addFile(":/pubmed/search_disabled.png", QSize(), QIcon::Disabled);
    dicon.addFile(":/pubmed/search_hover.png", QSize(), QIcon::Selected);
    dicon.addFile(":/pubmed/search_pressed.png", QSize(), QIcon::Active);
    //btnPMCID->setIcon(dicon);
    edtPMCID->addButton(btnPMCID);
    li->addWidget(edtPMCID);
    label=new QLabel(tr("&PubMed Central:"), this);
    label->setBuddy(edtPMCID);
    w=new QWidget(this);
    if (extendedShortWidth>0) w->setMaximumWidth(extendedShortWidth);
    w->setLayout(li);
    lay->addRow(label, w);

    li=new QHBoxLayout(this);
    li->setContentsMargins (0,0,0,0);
    edtArxiv=new QEnhancedLineEdit(this);
    btnArxiv=new JKStyledButton(JKStyledButton::OpenPrependedURL, edtArxiv, this);
    btnArxiv->setPrependURL("http://arxiv.org/abs/");
    dicon=QIcon(":/arxiv/search.png");
    dicon.addFile(":/arxiv/search_disabled.png", QSize(), QIcon::Disabled);
    dicon.addFile(":/arxiv/search_hover.png", QSize(), QIcon::Selected);
    dicon.addFile(":/arxiv/search_pressed.png", QSize(), QIcon::Active);
    //btnArxiv->setIcon(dicon);
    edtArxiv->addButton(btnArxiv);
    li->addWidget(edtArxiv);
    label=new QLabel(tr("&ArXiv:"), this);
    label->setBuddy(edtArxiv);
    w=new QWidget(this);
    if (extendedShortWidth>0) w->setMaximumWidth(extendedShortWidth);
    w->setLayout(li);
    lay->addRow(label, w);


    edtUrl=new QEnhancedLineEdit(this);
    btnUrl=new JKStyledButton(JKStyledButton::OpenURL, edtUrl, this);
    edtUrl->addButton(btnUrl);
    if (extendedLongWidth>0) edtUrl->setMaximumWidth(extendedLongWidth);
    lay->addRow(tr("&URL:"), edtUrl);

    f=new QFrame(this);
    f->setFrameShape(QFrame::HLine);
    f->setMidLineWidth(1);
    f->setLineWidth(0);
    f->setMaximumHeight(5);
    f->setFrameShadow(QFrame::Raised);
    lay->addRow(f);


    /*edtFile=new QEnhancedLineEdit(this);
    btnFileExecute=new JKStyledButton(JKStyledButton::ExecuteFile, edtFile, this);
    btnFileSelect=new JKStyledButton(JKStyledButton::SelectFile, edtFile, this);
    edtFile->addButton(btnFileExecute);
    edtFile->addButton(btnFileSelect);*/
    edtFile=new QFilenamesListEdit(this);
    if (extendedLongWidth>0) edtFile->setMaximumWidth(extendedLongWidth);
    edtFile->setMaximumHeight(4*edtPubmed->height());
    lay->addRow(tr("&File(s):"), edtFile);
    lay->setLabelAlignment(Qt::AlignTop|Qt::AlignRight);

}

ExtendedPane::~ExtendedPane()
{
    //dtor
}

void ExtendedPane::setBaseDir(QString bd) {
    //btnFileSelect->setBasepath(bd);
    //btnFileExecute->setBasepath(bd);
    edtFile->setBaseDir(bd);
}

void ExtendedPane::addEditButton(LS3PluginServices::AvailableEditWidgetsForButtons edit, QAction *action) {
    if (edit==LS3PluginServices::EditArXiv) {
        JKStyledButton* btn=new JKStyledButton(action, edtArxiv);
        btn->setFocusPolicy(Qt::ClickFocus);
        btn->setBuddy(edtArxiv);
        edtArxiv->addButton(btn);
    } else if (edit==LS3PluginServices::EditCiteSeer) {
        JKStyledButton* btn=new JKStyledButton(action, edtCiteseer);
        btn->setFocusPolicy(Qt::ClickFocus);
        btn->setBuddy(edtCiteseer);
        edtCiteseer->addButton(btn);
    } else if (edit==LS3PluginServices::EditDOI) {
        JKStyledButton* btn=new JKStyledButton(action, edtDoi);
        btn->setFocusPolicy(Qt::ClickFocus);
        btn->setBuddy(edtDoi);
        edtDoi->addButton(btn);
    } else if (edit==LS3PluginServices::EditFile) {
        QToolButton* btn=new QToolButton(edtFile);
        btn->setFocusPolicy(Qt::ClickFocus);
        btn->setDefaultAction(action);
        //btn->setBuddy(edtFile);
        edtFile->addButton(btn);
    } else if (edit==LS3PluginServices::EditURL) {
        JKStyledButton* btn=new JKStyledButton(action, edtUrl);
        btn->setFocusPolicy(Qt::ClickFocus);
        btn->setBuddy(edtUrl);
        edtUrl->addButton(btn);
    } else if (edit==LS3PluginServices::EditPubmed) {
        JKStyledButton* btn=new JKStyledButton(action, edtPubmed);
        btn->setFocusPolicy(Qt::ClickFocus);
        btn->setBuddy(edtPubmed);
        edtPubmed->addButton(btn);
    } else if (edit==LS3PluginServices::EditPMCID) {
        JKStyledButton* btn=new JKStyledButton(action, edtPMCID);
        btn->setFocusPolicy(Qt::ClickFocus);
        btn->setBuddy(edtPMCID);
        edtPMCID->addButton(btn);
    /*} else if (edit==LS3PluginServices::Edit) {
        JKStyledButton* btn=new JKStyledButton(action, edtCiteseer);
        edtCiteseer->addButton(btn);*/
    }
}


void ExtendedPane::connectWidgets(LS3Datastore* datastore) {
    //std::cout<<"ExtendedPane::connectWidgets() ... ";
    connected=true;
    this->datastore=datastore;

    //std::cout<<"adding mappings ... ";
    datastore->addMapping(cmbOrigin, "origin", "text");
    datastore->addMapping(edtOwner, "owner");
    datastore->addMapping(dedWhenAdded, "addeddate", "date");
    datastore->addMapping(edtLibNum, "librarynum");
    datastore->addMapping(edtPrice, "price");
    datastore->addMapping(cmbCurrency, "currency", "text");
    datastore->addMapping(cmbStatus, "status", "text");
    datastore->addMapping(dedStatusSince, "statussince", "date");
    datastore->addMapping(edtDoi, "doi");
    datastore->addMapping(edtCiteseer, "citeseer");
    datastore->addMapping(edtPubmed, "pubmed");
    datastore->addMapping(edtPMCID, "pmcid");
    datastore->addMapping(edtArxiv, "arxiv");
    datastore->addMapping(edtUrl, "url");
    datastore->addMapping(edtFile, "files", "filesAsString");
    datastore->addMapping(edtStatusComment, "statuscomment");

    //std::cout<<"\n";
}

void ExtendedPane::disconnectWidgets() {
    if (datastore) {
        datastore->removeMapping(cmbOrigin);
        datastore->removeMapping(edtOwner);
        datastore->removeMapping(dedWhenAdded);
        datastore->removeMapping(edtLibNum);
        datastore->removeMapping(edtPrice);
        datastore->removeMapping(cmbCurrency);
        datastore->removeMapping(cmbStatus);
        datastore->removeMapping(dedStatusSince);
        datastore->removeMapping(edtStatusComment);
        datastore->removeMapping(edtDoi);
        datastore->removeMapping(edtCiteseer);
        datastore->removeMapping(edtPubmed);
        datastore->removeMapping(edtPMCID);
        datastore->removeMapping(edtArxiv);
        datastore->removeMapping(edtUrl);
        datastore->removeMapping(edtFile);
    }
    datastore=NULL;
    connected=false;
};

