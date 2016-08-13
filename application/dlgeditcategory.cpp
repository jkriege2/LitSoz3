#include "dlgeditcategory.h"
#include "ui_dlgeditcategory.h"

DlgEditCategory::DlgEditCategory(QWidget *parent, QStringList categories, const QString& oldCat) :
    QDialog(parent),
    ui(new Ui::DlgEditCategory),
    newWasFocused(false)
{
    ui->setupUi(this);
    categories.sort(Qt::CaseInsensitive);
    ui->cmbOldTopic->addItems(categories);
    ui->cmbOldTopic->setEditText(oldCat);
    ui->edtNewTopic->setText(oldCat);
    connect(ui->btnCopyDown, SIGNAL(clicked()), this, SLOT(copyDown()));
    connect(ui->edtNewTopic, SIGNAL(textChanged()), this, SLOT(newChanged()));
}

DlgEditCategory::~DlgEditCategory()
{
    delete ui;
}

QString DlgEditCategory::getNew() const {
    return ui->edtNewTopic->text();
}

QString DlgEditCategory::getOld() const {
    return ui->cmbOldTopic->currentText();
}

bool DlgEditCategory::getChangeSub() const {
    return ui->chkRenameSub->isChecked();
}

bool DlgEditCategory::getIgnoreCase() const {
    return ui->chkIgnoreCase->isChecked();
}

void DlgEditCategory::copyDown()  {
    ui->edtNewTopic->setText(getOld());
}

void DlgEditCategory::newChanged()  {
    newWasFocused=true;
}

void DlgEditCategory::on_cmbOldTopic_editTextChanged(const QString &text)
{
    if (!newWasFocused) {
        copyDown();
    }
}
