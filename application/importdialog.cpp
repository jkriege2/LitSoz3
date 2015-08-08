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

#include "importdialog.h"

ImportDialog::ImportDialog(QWidget* parent):
        QDialog(parent)
{
    setupUi(this);
    m_services=NULL;
}

ImportDialog::~ImportDialog()
{
    //dtor
}

void ImportDialog::init(LS3PluginServices* services) {
    m_services=services;
    cmbFormat->clear();
    QList<LS3Importer*> exp=services->getImporters();
    for (int i=0; i<exp.size(); i++) {
        cmbFormat->addItem(exp[i]->getIcon(), exp[i]->getName());
    }
}

void ImportDialog::on_cmbFormat_currentIndexChanged(int idx) {
    if (m_services) {
        QList<LS3Importer*> exp=m_services->getImporters();
        if ( (idx>=0) && (idx<exp.size())) labDescription->setText(exp[idx]->getDescription());
        else labDescription->setText("");
    }
}

LS3Importer* ImportDialog::getImporter() {
    if (m_services) {
        QList<LS3Importer*> exp=m_services->getImporters();
        int idx=cmbFormat->currentIndex();
        if ( (idx>=0) && (idx<exp.size())) return exp[idx];
    }
    return NULL;
}


