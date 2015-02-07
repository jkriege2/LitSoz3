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
