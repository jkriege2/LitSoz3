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

#include "renamefilesdialog.h"
#include "ui_renamefilesdialog.h"

RenameFilesDialog::RenameFilesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenameFilesDialog)
{
    m_currentRecordOnly=false;
    ui->setupUi(this);
}

RenameFilesDialog::~RenameFilesDialog()
{
    delete ui;
}

RenameFilesDialog::WhichDatasets RenameFilesDialog::getWhich() {
    if (m_currentRecordOnly) return RenameFilesDialog::Current;
    if (ui->radSelected->isChecked()) return RenameFilesDialog::Selected;
    if (ui->radCurrent->isChecked()) return RenameFilesDialog::Current;
    return RenameFilesDialog::All;
}


bool RenameFilesDialog::renameFiles() {
    return ui->chkRename->isChecked();
}

bool RenameFilesDialog::moveFiles() {
    return ui->chkMove->isChecked();
}
bool RenameFilesDialog::deleteOldFile() {
    return ui->chkDeleteOld->isChecked();
}


QString RenameFilesDialog::renameScheme() {
    return ui->cmbRenameScheme->currentText();
}

QString RenameFilesDialog::directoryScheme() {
    return ui->cmbDirScheme->currentText();
}

bool RenameFilesDialog::simulate() {
    return ui->chkSimulate->isChecked();
}

void RenameFilesDialog::setCurrentRecordOnly(bool currentOnly) {
    m_currentRecordOnly=currentOnly;
}

int RenameFilesDialog::maxLength() const
{
    return ui->spinFNLength->value();
}


void RenameFilesDialog::readSettings(QSettings* settings, QString prefix) {
    int mode=settings->value(prefix+"which", 0).toInt();
    if (m_currentRecordOnly) {
        ui->radCurrent->setChecked(true);
        ui->grpWhich->setEnabled(false);
    } else {
        if (mode==2) ui->radCurrent->setChecked(true);
        else if (mode==1) ui->radSelected->setChecked(true);
        else ui->radAll->setChecked(true);
        ui->grpWhich->setEnabled(true);
    }

    ui->chkSimulate->setChecked(settings->value(prefix+"simulate", ui->chkSimulate->isChecked()).toBool());
    ui->chkMove->setChecked(settings->value(prefix+"moveFiles", ui->chkMove->isChecked()).toBool());
    ui->chkRename->setChecked(settings->value(prefix+"renameFiles", ui->chkRename->isChecked()).toBool());
    ui->chkDeleteOld->setChecked(settings->value(prefix+"chkDeleteOld", ui->chkDeleteOld->isChecked()).toBool());
    ui->spinFNLength->setValue(settings->value(prefix+"spinFNLength", ui->spinFNLength->value()).toInt());
    QStringList items, itemsOut;
    for (int i=0; i<ui->cmbDirScheme->count(); i++) items.append(ui->cmbDirScheme->itemText(i));
    ui->cmbDirScheme->clear();
    itemsOut=settings->value(prefix+"dirSchemes", items.join(";;")).toString().split(";;");
    itemsOut.append(items);
    itemsOut.removeDuplicates();
    ui->cmbDirScheme->addItems(itemsOut);
    ui->cmbDirScheme->setEditText(settings->value(prefix+"dirScheme", items.value(0, "")).toString());

    items.clear();
    for (int i=0; i<ui->cmbRenameScheme->count(); i++) items.append(ui->cmbRenameScheme->itemText(i));
    ui->cmbRenameScheme->clear();
    itemsOut=settings->value(prefix+"renameSchemes", items.join(";;")).toString().split(";;");
    itemsOut.append(items);
    itemsOut.removeDuplicates();
    ui->cmbRenameScheme->addItems(itemsOut);
    ui->cmbRenameScheme->setEditText(settings->value(prefix+"renameScheme", items.value(0, "")).toString());

}

void RenameFilesDialog::writeSettings(QSettings* settings, QString prefix) {
    if (!m_currentRecordOnly) {
        settings->setValue(prefix+"which", 0);
        if (ui->radSelected->isChecked()) settings->setValue(prefix+"which", 1);
        if (ui->radCurrent->isChecked()) settings->setValue(prefix+"which", 2);
    }

    settings->setValue(prefix+"simulate", ui->chkSimulate->isChecked());
    settings->setValue(prefix+"moveFiles", ui->chkMove->isChecked());
    settings->setValue(prefix+"renameFiles", ui->chkRename->isChecked());
    settings->setValue(prefix+"chkDeleteOld", ui->chkDeleteOld->isChecked());
    settings->setValue(prefix+"spinFNLength", ui->spinFNLength->value());

    QStringList items;
    items.clear();
    for (int i=0; i<ui->cmbDirScheme->count(); i++) items.append(ui->cmbDirScheme->itemText(i));
    if (!items.contains( ui->cmbDirScheme->currentText())) items.append( ui->cmbDirScheme->currentText());
    settings->setValue(prefix+"dirSchemes", items.join(";;"));
    settings->setValue(prefix+"dirScheme", ui->cmbDirScheme->currentText());

    items.clear();
    for (int i=0; i<ui->cmbRenameScheme->count(); i++) items.append(ui->cmbRenameScheme->itemText(i));
    if (!items.contains( ui->cmbRenameScheme->currentText())) items.append( ui->cmbRenameScheme->currentText());
    settings->setValue(prefix+"renameSchemes", items.join(";;"));
    settings->setValue(prefix+"renameScheme", ui->cmbRenameScheme->currentText());
}

