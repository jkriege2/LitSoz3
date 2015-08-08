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
