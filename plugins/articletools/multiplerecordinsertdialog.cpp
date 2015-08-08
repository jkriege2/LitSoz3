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

#include "multiplerecordinsertdialog.h"
#include "ui_multiplerecordinsertdialog.h"
#include "bibtools.h"
#include "htmldelegate.h"

MultipleRecordInsertDialog::MultipleRecordInsertDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MultipleRecordInsertDialog)
{
    ui->setupUi(this);
    list=new QCheckableStringListModel(this);
    items.clear();
    ui->listView->setModel(list);
    HTMLDelegate* del=new HTMLDelegate(ui->listView);
    del->setDisplayRichTextEditor(false);
    ui->listView->setItemDelegate(del);
}

MultipleRecordInsertDialog::~MultipleRecordInsertDialog()
{
    delete ui;
}

bool MultipleRecordInsertDialog::isImported(int i) {
    if (i>=0 && i<list->rowCount()) {
        return list->isChecked(i);
    }
    return false;
}

void MultipleRecordInsertDialog::addRecord(const QMap<QString, QVariant>& data) {
    QString refsum=formatShortReferenceSummary(data);
    items.append(refsum);
    list->setStringList(items);
    list->checkAll();
}
