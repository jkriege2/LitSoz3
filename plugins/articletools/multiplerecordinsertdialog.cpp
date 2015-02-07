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
