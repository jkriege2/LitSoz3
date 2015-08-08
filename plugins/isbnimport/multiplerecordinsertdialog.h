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

#ifndef MULTIPLERECORDINSERTDIALOG_H
#define MULTIPLERECORDINSERTDIALOG_H

#include <QDialog>
#include <QMap>
#include <QVariant>
#include <QString>
#include "qcheckablestringlistmodel.h"

namespace Ui {
    class MultipleRecordInsertDialog;
}

class MultipleRecordInsertDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MultipleRecordInsertDialog(QWidget *parent = 0);
    ~MultipleRecordInsertDialog();

    bool isImported(int i);
    void addRecord(const QMap<QString, QVariant>& data);

private:
    Ui::MultipleRecordInsertDialog *ui;
    QCheckableStringListModel* list;
    QStringList items;
};

#endif // MULTIPLERECORDINSERTDIALOG_H
