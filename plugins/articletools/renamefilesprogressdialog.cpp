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

#include "renamefilesprogressdialog.h"
#include "ui_renamefilesprogressdialog.h"
#include <iostream>

RenameFilesProgressDialog::RenameFilesProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenameFilesProgressDialog)
{
    ui->setupUi(this);
    m_cancel=false;
}

RenameFilesProgressDialog::~RenameFilesProgressDialog()
{
    delete ui;
}

void RenameFilesProgressDialog::setRange(int min, int max) {
    ui->progressBar->setRange(min, max);
}

void RenameFilesProgressDialog::setProgress(int value) {
    ui->progressBar->setValue(value);
}

void RenameFilesProgressDialog::incProgress(int inc) {
    ui->progressBar->setValue(ui->progressBar->value()+inc);
}

void RenameFilesProgressDialog::setMessage(const QString& message) {
    ui->label->setText(message);
}

void RenameFilesProgressDialog::addFilesPair(const QString& oldName, const QString& newName) {
    //std::cout<<"count_old="<<ui->tableWidget->rowCount()<<"\n";
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    //std::cout<<"count_new="<<ui->tableWidget->rowCount()<<"\n";
    QTableWidgetItem *newItem;
    newItem = new QTableWidgetItem("");
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 0, newItem);
    newItem = new QTableWidgetItem(oldName);
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 1, newItem);
    newItem = new QTableWidgetItem(newName);
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 2, newItem);

    ui->tableWidget->scrollToItem(ui->tableWidget->item(ui->tableWidget->rowCount()-1, 0));
    ui->tableWidget->resizeColumnsToContents();
}

void RenameFilesProgressDialog::setCurrentStatus(const QString& status, bool isOK) {
    if (ui->tableWidget->item(ui->tableWidget->rowCount()-1, 0)) {
        if (isOK) {
            ui->tableWidget->item(ui->tableWidget->rowCount()-1, 0)->setTextColor(QColor("darkblue"));
            ui->tableWidget->item(ui->tableWidget->rowCount()-1, 1)->setTextColor(QColor("darkblue"));
            ui->tableWidget->item(ui->tableWidget->rowCount()-1, 2)->setTextColor(QColor("darkblue"));
        } else {
            ui->tableWidget->item(ui->tableWidget->rowCount()-1, 0)->setTextColor(QColor("red"));
            ui->tableWidget->item(ui->tableWidget->rowCount()-1, 1)->setTextColor(QColor("red"));
            ui->tableWidget->item(ui->tableWidget->rowCount()-1, 2)->setTextColor(QColor("red"));
        }
        ui->tableWidget->item(ui->tableWidget->rowCount()-1, 0)->setText(status);
        ui->tableWidget->scrollToItem(ui->tableWidget->item(ui->tableWidget->rowCount()-1, 0));
    }
}

void RenameFilesProgressDialog::setReportMode() {
    ui->cancelBtn->setText("&Close");
    connect(ui->cancelBtn, SIGNAL(clicked()), this, SLOT(close()));
}
