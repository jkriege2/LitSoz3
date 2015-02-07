
#include "contentspane.h"
#include <iostream>

ContentsPane::ContentsPane(ProgramOptions* settings, QWidget* parent):
    QWidget(parent)
{
    this->settings=settings;
    datastore=NULL;
    connected=false;

    QFormLayout* l=new QFormLayout(this);
    setLayout(l);
    l->setContentsMargins(10,10,10,10);
    l->setHorizontalSpacing(32);
    l->setVerticalSpacing(32);
    l->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    l->setLabelAlignment(Qt::AlignTop|Qt::AlignRight);

    edtAbstract=new QPlainTextEdit(this);
    edtAbstract->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    l->addRow(tr("&Abstract:"), edtAbstract);
    edtComments=new QPlainTextEdit(this);
    edtComments->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    l->addRow(tr("&Comments:"), edtComments);
}

ContentsPane::~ContentsPane()
{
    //dtor
}

void ContentsPane::connectWidgets(LS3Datastore* datastore) {
    //std::cout<<"ContentsPane::connectWidgets() ... ";
    connected=true;
    this->datastore=datastore;

    //std::cout<<"adding contents mappings ... ";
    datastore->addMapping(edtAbstract, "abstract");
    datastore->addMapping(edtComments, "comments");
    //std::cout<<"\n";
}

void ContentsPane::disconnectWidgets() {
    if (datastore) {
        datastore->removeMapping(edtAbstract);
        datastore->removeMapping(edtComments);
    }
    datastore=NULL;
    connected=false;
};


