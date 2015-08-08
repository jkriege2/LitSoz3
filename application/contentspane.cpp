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


