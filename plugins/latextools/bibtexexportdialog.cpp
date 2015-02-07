#include "bibtexexportdialog.h"
#include "ui_bibtexexportdialog.h"

BibTeXExportDialog::BibTeXExportDialog(LS3PluginServices *services, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BibTeXExportDialog)
{
    m_services=services;
    ui->setupUi(this);

    if (m_services) {
        ui->chkExportAbstract->setChecked( m_services->GetQSettings()->value("latextools/bibtexexport/exportabstract", false).toBool());
        ui->chkExportPDF->setChecked( m_services->GetQSettings()->value("latextools/bibtexexport/exportpdf", false).toBool());
        ui->cmbURL->setCurrentIndex( m_services->GetQSettings()->value("latextools/bibtexexport/exporturl", 1).toInt());
        ui->cmbStyle->setCurrentIndex( m_services->GetQSettings()->value("latextools/bibtexexport/exportstyle", 0).toInt());
    }
}

BibTeXExportDialog::~BibTeXExportDialog()
{
    if (m_services) {
         m_services->GetQSettings()->setValue("latextools/bibtexexport/exportabstract", ui->chkExportAbstract->isChecked());
         m_services->GetQSettings()->setValue("latextools/bibtexexport/exportpdf", ui->chkExportPDF->isChecked());
         m_services->GetQSettings()->value("latextools/bibtexexport/exporturl", ui->cmbURL->currentIndex());
         m_services->GetQSettings()->value("latextools/bibtexexport/exportstyle", ui->cmbStyle->currentIndex());
    }
    delete ui;
}

bool BibTeXExportDialog::getExportAbstract() const {
    return ui->chkExportAbstract->isChecked();
}

bool BibTeXExportDialog::getExportPDF() const {
    return ui->chkExportPDF->isChecked();
}

int BibTeXExportDialog::getExportURL() const
{
    return ui->cmbURL->currentIndex();
}

int BibTeXExportDialog::getExportStyle() const
{
    return ui->cmbStyle->currentIndex();

}
