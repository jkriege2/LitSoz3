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


