#include "exportdialog.h"

ExportDialog::ExportDialog(QWidget* parent):
        QDialog(parent)
{
    setupUi(this);
    m_services=NULL;
}

ExportDialog::~ExportDialog()
{
    //dtor
}

void ExportDialog::init(LS3PluginServices* services) {
    m_services=services;
    cmbFormat->clear();
    QList<LS3Exporter*> exp=services->getExporters();
    for (int i=0; i<exp.size(); i++) {
        cmbFormat->addItem(exp[i]->getIcon(), exp[i]->getName());
    }
}

void ExportDialog::on_cmbFormat_currentIndexChanged(int idx) {
    if (m_services) {
        QList<LS3Exporter*> exp=m_services->getExporters();
        if ( (idx>=0) && (idx<exp.size())) labDescription->setText(exp[idx]->getDescription());
        else labDescription->setText("");
    }
}

LS3Exporter* ExportDialog::getExporter() {
    if (m_services) {
        QList<LS3Exporter*> exp=m_services->getExporters();
        int idx=cmbFormat->currentIndex();
        if ( (idx>=0) && (idx<exp.size())) return exp[idx];
    }
    return NULL;
}


ExportDialog::WhichDatasets ExportDialog::getWhich() {
    if (radSelected->isChecked()) return ExportDialog::Selected;
    if (radCurrent->isChecked()) return ExportDialog::Current;
    return ExportDialog::All;
}
