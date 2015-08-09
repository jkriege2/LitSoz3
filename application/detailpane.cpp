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

#include "detailpane.h"
#include <iostream>
#include "ls3tools.h"


DetailPane::DetailPane(ProgramOptions* settings, QWidget* parent):
    QWidget(parent)
{
    this->settings=settings;
    datastore=NULL;
    connected=false;
    currentFDF="";

    QVBoxLayout* mainLayout=new QVBoxLayout();
    this->setLayout(mainLayout);
    mainLayout->setContentsMargins(10,10,10,10);

    QHBoxLayout* hbLayout;
    QWidget* wl;
    FlowLayout* fLayout=new FlowLayout();
    mainLayout->addLayout(fLayout);

    hbLayout=new QHBoxLayout();
    hbLayout->setContentsMargins(0,0,0,0);
    wl=new QWidget(this);
    wl->setLayout(hbLayout);
    fLayout->addWidget(wl);
    //wl=new QWidget(this);
    //wl->setLayout()
    QLabel* label=new QLabel(tr("&Number:"), this);
    edtNum=new QLineEdit(this);
    edtNum->setReadOnly(true);
    edtNum->setFrame(false);
    edtNum->setEnabled(false);
    edtNum->setMaximumWidth(100);
    label->setBuddy(edtNum);
    hbLayout->addWidget(label);
    hbLayout->addWidget(edtNum);
    hbLayout->addSpacing(30);

    hbLayout=new QHBoxLayout();
    hbLayout->setContentsMargins(0,0,0,0);
    wl=new QWidget(this);
    wl->setLayout(hbLayout);
    fLayout->addWidget(wl);
    label=new QLabel(tr("&ID:"), this);
    edtID=new QLineEdit(this);
    edtID->setMaximumWidth(150);
    label->setBuddy(edtID);

    btnGenerateID=new QToolButton(this);
    btnGenerateID->setText(tr("&Generate ID"));
    menGenerateID=new QMenu(btnGenerateID);
    agIDTypes=new QActionGroup(btnGenerateID);
    actIDTypeAuthorYear=new QAction(tr("AuthorYear"), agIDTypes);
    actIDTypeAuthorYear->setCheckable(true);
    agIDTypes->addAction(actIDTypeAuthorYear);
    actIDTypeUCAuthorYear=new QAction(tr("AUTHORYear"), agIDTypes);
    actIDTypeUCAuthorYear->setCheckable(true);
    agIDTypes->addAction(actIDTypeUCAuthorYear);
    actIDTypeAUTYear=new QAction(tr("AUT[1..3]Year"), agIDTypes);
    actIDTypeAUTYear->setCheckable(true);
    agIDTypes->addAction(actIDTypeAUTYear);
    actIDTypeAUTHORYear=new QAction(tr("AUTHOR[1..6]Year"), agIDTypes);
    actIDTypeAUTHORYear->setCheckable(true);
    agIDTypes->addAction(actIDTypeAUTHORYear);
    actIDTypeABCYear=new QAction(tr("ABCYear"), agIDTypes);
    actIDTypeABCYear->setCheckable(true);
    agIDTypes->addAction(actIDTypeABCYear);
    actIDTypeAuthorYear->setChecked(true);
    menGenerateID->addAction(actIDTypeAuthorYear);
    menGenerateID->addAction(actIDTypeUCAuthorYear);
    menGenerateID->addAction(actIDTypeAUTYear);
    menGenerateID->addAction(actIDTypeAUTHORYear);
    menGenerateID->addAction(actIDTypeABCYear);
    btnGenerateID->setMenu(menGenerateID);
    btnGenerateID->setPopupMode(QToolButton::MenuButtonPopup);

    hbLayout->addWidget(label);
    hbLayout->addWidget(edtID);
    hbLayout->addWidget(btnGenerateID);
    hbLayout->addSpacing(30);

    hbLayout=new QHBoxLayout();
    hbLayout->setContentsMargins(0,0,0,0);
    wl=new QWidget(this);
    wl->setLayout(hbLayout);
    fLayout->addWidget(wl);
    label=new QLabel(tr("&Type:"), this);
    cmbType=new QMappableComboBox(this);
    //label->setAlignment(Qt::AlignRight);
    cmbType->setMinimumWidth(250);
    label->setBuddy(cmbType);
    hbLayout->addWidget(label);
    hbLayout->addWidget(cmbType);
    hbLayout->addSpacing(30);

    hbLayout=new QHBoxLayout();
    hbLayout->setContentsMargins(0,0,0,0);
    wl=new QWidget(this);
    wl->setLayout(hbLayout);
    fLayout->addWidget(wl);
    starRating=new JKStarRatingWidget(this);
    starRating->setMaximum(5);
    label=new QLabel(tr("Your &Rating:"), this);
    //label->setAlignment(Qt::AlignRight);
    label->setBuddy(starRating);
    hbLayout->addWidget(label);
    hbLayout->addWidget(starRating);
    hbLayout->addStretch();

    scaMain=new QScrollArea;
    scaMain->setWidgetResizable(true);
    mainLayout->addWidget(scaMain, 4);


    flTopic=new QGridLayout();
    flTopic->setContentsMargins(0,0,0,0);

    labKeywords=new QLabel(tr("&Keywords:"), this);
    labKeywords->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    pedKeywords=new QCompleterPlainTextEdit(this);
    //completeKeywords=new QCompleterFromFile(pedKeywords);
    //completeKeywords->setFilename(settings->GetConfigDirectory()+"/completers/keywords.lst");
    completeKeywords=new QCompleter(pedKeywords);
    pedKeywords->setCompleter(completeKeywords);
    pedKeywords->setMaximumWidth(550);
    //connect(completeKeywords, SIGNAL(textChanged()), this, SLOT(keywordsChanged()));

    labKeywords->setBuddy(pedKeywords);
    flTopic->addWidget(labKeywords,0,0,0);
    flTopic->addWidget(pedKeywords,0,1,3,1);


    labTopic=new QLabel(tr("&Topic:"), this);
    cmbTopic=new QMappableComboBox(this);
    labTopic->setBuddy(cmbTopic);
    cmbTopic->setEditable(true);
    cmbTopic->setInsertPolicy(QComboBox::InsertAlphabetically);
    cmbTopic->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    //cmbTopic->setFilename(settings->GetConfigDirectory()+"/completers/topic.lst");
    cmbTopic->setMaximumWidth(550);
    cmbTopic->setMinimumWidth(150);
    flTopic->addWidget(labTopic, 0, 2);
    flTopic->addWidget(cmbTopic, 0, 3);

    labLanguage=new QLabel(tr("&Language:"), this);
    cmbLanguage=new QMappableComboBox(this);
    labLanguage->setBuddy(cmbLanguage);
    cmbLanguage->setEditable(true);
    cmbLanguage->setInsertPolicy(QComboBox::InsertAlphabetically);
    cmbLanguage->setTranslationFilename(settings->GetConfigDirectory()+"/completers/language_"+settings->GetLanguageID().toLower()+".translation");
    cmbLanguage->setImagesDir(":/language_icons/");
    cmbLanguage->setFilename(settings->GetConfigDirectory()+"/completers/language.lst");
    cmbLanguage->setMaximumWidth(350);
    cmbLanguage->setMinimumWidth(150);
    cmbLanguage->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    //hbkLayout->addWidget(label, 1,2);
    //hbkLayout->addWidget(cmbLanguage, 1,3);
    flTopic->addWidget(labLanguage, 1, 2);
    flTopic->addWidget(cmbLanguage, 1, 3);
    flTopic->setRowStretch(2,1);
    flTopicSpacer=new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Expanding);
    flTopic->addItem(flTopicSpacer,0,4);




    mainLayout->addLayout(flTopic);


    completeAuthors=new QCompleter(this);

    changeLayout(false);

    connect(settings, SIGNAL(configDirReread()), this, SLOT(configDirReread()));
    connect(btnGenerateID, SIGNAL(clicked()), this, SLOT(generateID()));
}

void DetailPane::topicModelChanged() {
    QString txt=cmbTopic->text();
    bool en=cmbTopic->updatesEnabled();
    cmbTopic->setUpdatesEnabled(false);
    cmbTopic->clear();
    if (datastore) cmbTopic->addItems(datastore->getTopicsModel()->stringList());
    cmbTopic->setText(txt);
    if (en) cmbTopic->setUpdatesEnabled(en);
}

void DetailPane::databaseCurentChanged(int /*index*/)
{
    if (datastore) {
        disconnect(cmbType, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChanged(int)));
        cmbType->setCurrentIndex(cmbType->findData(datastore->getField("type").toString()));
        connect(cmbType, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChanged(int)));
        QString id=cmbType->itemData(cmbType->currentIndex()).toString();
        //qDebug()<<"databaseCurentChanged("<<index<<"): "<<id<<currentFDF<<cmbType->currentIndex();
        if (currentFDF != id) {
            redisplayFDF(cmbType->currentIndex());
        }
    }
}

void DetailPane::typeChanged(int /*index*/)
{
    if (datastore) {
        datastore->setField("type", cmbType->data().toString());
        //qDebug()<<"typeChanged("<<index<<"): "<<cmbType->currentText()<<cmbType->itemText(index);
        QString id=cmbType->itemData(cmbType->currentIndex()).toString();
        //qDebug()<<"databaseCurentChanged("<<index<<"): "<<id<<currentFDF<<cmbType->currentIndex();
        if (currentFDF != id) {
            redisplayFDF(cmbType->currentIndex());
        }
    }
}

void DetailPane::connectWidgets(LS3Datastore* datastore) {
    LS3ElapsedAutoTimer timer("DetailPane::connectWidgets()");
    //qDebug()<<"DetailPane::connectWidgets() ... ";
    connected=true;
    this->datastore=datastore;
    topicModelChanged();
    connect(datastore, SIGNAL(topicsListChanged()), this, SLOT(topicModelChanged()));
    //cmbTopic->setModel(datastore->getTopicsModel());
    completeKeywords->setModel(datastore->getKeywordsModel());

    configDirReread();
    cmbType->setCurrentIndex(-1);
    connect(cmbType, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChanged(int)));
    connect(datastore, SIGNAL(currentRecordChanged(int)), this, SLOT(databaseCurentChanged(int)));

    //std::cout<<"adding details mappings ... ";
    fdfManager* fdf=settings->GetFDFManager();
    completeAuthors->setModel(datastore->getNamesModel());
    fdf->setAuthorsCompleter(completeAuthors);

    datastore->addMapping(edtNum, "num");
    datastore->addMapping(edtID, "id");
    //datastore->addMapping(cmbType, "type", "data");
    datastore->addMapping(pedKeywords, "keywords");
    datastore->addMapping(cmbTopic, "topic", "text");
    //datastore->addMapping(cmbTopic->lineEdit(), "topic");
    datastore->addMapping(cmbLanguage, "language", "translatedText");
    datastore->addMapping(starRating, "rating", "rating");
    //std::cout<<"\n";
}

void DetailPane::disconnectWidgets() {
    LS3ElapsedAutoTimer timer("DetailPane::disconnectWidgets()");
    //qDebug()<<"DetailPane::disconnectWidgets()";
    disconnect(cmbType, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChanged(int)));
    disconnect(datastore, SIGNAL(currentRecordChanged(int)), this, SLOT(databaseCurentChanged(int)));
    if (datastore) {
        if (datastore->getTopicsModel()) {
            disconnect(datastore, SIGNAL(topicsListChanged()), this, SLOT(topicModelChanged()));
        }
        datastore->removeMapping(pedKeywords);
        datastore->removeMapping(edtID);
        //datastore->removeMapping(cmbType);
        datastore->removeMapping(edtNum);
        datastore->removeMapping(cmbTopic);
        datastore->removeMapping(cmbLanguage);
        datastore->removeMapping(starRating);

    }
    redisplayFDF(-1);
    completeKeywords->setModel(new QStringListModel(completeKeywords));
    //cmbTopic->setModel(new QStringListModel(cmbTopic));
    completeAuthors->setModel(NULL);
    datastore=NULL;
    connected=false;
};

void DetailPane::configDirReread() {
    //std::cout<<"  configDirReread() ... ";
    fdfManager* fdf=settings->GetFDFManager();
    cmbType->clear();
    for (int i=0; i<fdf->getSize(); i++) {
        cmbType->addItem(QIcon(fdf->get(i)->icon), fdf->get(i)->name[settings->GetLanguageID()], fdf->get(i)->ID);
    }
    //std::cout<<"OK\n";
}

void DetailPane::redisplayFDF( int index ) {
    //qDebug()<<"DetailPane::redisplayFDF "<<index<<"    old widgtes: "<<widgetvec.size();
    for (int i=0; i<widgetvec.size(); i++) {
        if (widgetvec[i]) datastore->removeMapping(widgetvec[i]);
    }
    widgetvec.clear();
    //QApplication::processEvents();
    //QApplication::processEvents();
    //QApplication::processEvents();

    if (scaMain->widget()!=NULL) {
        //QApplication::processEvents();
        scaMain->takeWidget()->deleteLater();
        //QApplication::processEvents();
    }
    //QApplication::processEvents();
    //QApplication::processEvents();
    //QApplication::processEvents();
    if (index>=0) {
        //int idx=datastore->currentRecordNum();
        QString id=cmbType->itemData(index).toString();
        currentFDF=id;
        //QMap<QString, QVariant> rec=datastore->currentRecord();
        //rec["type"]=QVariant(id);
        //datastore->dbSubmit();
        //QApplication::processEvents();
        //qDebug()<<"loding "<<id;
        scaMain->setWidget(settings->GetFDFManager()->createWidgets(id, settings->GetLanguageID(), settings->GetConfigDirectory(), this, datastore, &widgetvec));
        datastore->ensureMappedWidgetsPopulated();
        //datastore->setRecord(idx, rec);
        //datastore->dbSubmit();
        //datastore->dbMove(idx);

    }
    //qDebug()<<"DetailPane::redisplayFDF "<<index<<" ... DONE";
}

void DetailPane::generateID() {
    QApplication::processEvents();

    //datastore->dbSubmit();

    QString id="";
    if (actIDTypeAuthorYear->isChecked()) {
        id=datastore->createIDForCurrentRecord(LS3Datastore::AuthorYearDefaultCase);
    } else if (actIDTypeUCAuthorYear->isChecked()) {
        id=datastore->createIDForCurrentRecord(LS3Datastore::AuthorYearUpperCase);
    } else if (actIDTypeAUTYear->isChecked()) {
        id=datastore->createIDForCurrentRecord(LS3Datastore::Author3YearUpperCase);
    } else if (actIDTypeAUTHORYear->isChecked()) {
        id=datastore->createIDForCurrentRecord(LS3Datastore::Author6YearUpperCase);
    } else if (actIDTypeABCYear->isChecked()) {
        id=datastore->createIDForCurrentRecord(LS3Datastore::InitialsYear);
    }

    datastore->setField("id", id);
    edtID->setText(id);
    edtID->setFocus(Qt::MouseFocusReason);
    QApplication::restoreOverrideCursor();
}

void DetailPane::readSettings(QSettings& settings) {
    QString s=settings.value("extended/idtype", "AuthorYear").toString();
    if (s=="AuthorYear") { actIDTypeAuthorYear->setChecked(true); }
    if (s=="USAuthorYear") { actIDTypeUCAuthorYear->setChecked(true); }
    if (s=="AUTYear") { actIDTypeAUTYear->setChecked(true); }
    if (s=="AUTHORYear") { actIDTypeAUTHORYear->setChecked(true); }
    if (s=="ABCYear") { actIDTypeABCYear->setChecked(true); }
}

void DetailPane::writeSettings(QSettings& settings) {
    if (actIDTypeAuthorYear->isChecked()) { settings.setValue("extended/idtype", "AuthorYear"); }
    if (actIDTypeUCAuthorYear->isChecked()) { settings.setValue("extended/idtype", "UCAuthorYear"); }
    if (actIDTypeAUTYear->isChecked()) { settings.setValue("extended/idtype", "AUTYear"); }
    if (actIDTypeAUTHORYear->isChecked()) { settings.setValue("extended/idtype", "AUTHORYear"); }
    if (actIDTypeABCYear->isChecked()) { settings.setValue("extended/idtype", "ABCYear"); }
}

void DetailPane::changeLayout(bool small) {

    flTopic->setRowStretch(2,0);
    flTopic->removeWidget(labKeywords);
    flTopic->removeWidget(labTopic);
    flTopic->removeWidget(labLanguage);
    flTopic->removeWidget(cmbTopic);
    flTopic->removeWidget(cmbLanguage);
    flTopic->removeWidget(pedKeywords);
    flTopic->removeItem(flTopicSpacer);

    if (small) {
        flTopic->addWidget(labLanguage, 0, 0);
        flTopic->addWidget(cmbLanguage, 0, 1);
        flTopic->addWidget(labTopic, 1, 0);
        flTopic->addWidget(cmbTopic, 1, 1);
        flTopic->addWidget(labKeywords,2,0);
        flTopic->addWidget(pedKeywords,2,1,1,3);
        //flTopic->addItem(flTopicSpacer,0,4);
    } else {
        flTopic->addWidget(labKeywords,0,0,0);
        flTopic->addWidget(pedKeywords,0,1,3,1);
        flTopic->addWidget(labTopic, 0, 2);
        flTopic->addWidget(cmbTopic, 0, 3);
        flTopic->addWidget(labLanguage, 1, 2);
        flTopic->addWidget(cmbLanguage, 1, 3);
        flTopic->setRowStretch(2,1);
        flTopic->addItem(flTopicSpacer,0,4);
    }

}
