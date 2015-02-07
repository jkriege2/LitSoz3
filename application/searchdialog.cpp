#include "searchdialog.h"

SearchDialog::SearchDialog(QWidget* parent):
    QDialog(parent)
{
    setupUi(this);
    labHelpRegExp->setVisible(false);
    labHelpWildcard->setVisible(false);
}

SearchDialog::~SearchDialog()
{
    //dtor
}

void SearchDialog::setFields(const QStringList& fields) {
    cmbFields1->clear();
    cmbFields1->addItems(fields);
    cmbFields2->clear();
    cmbFields2->addItems(fields);
    cmbFields3->clear();
    cmbFields3->addItems(fields);
    edtPhrase1->setFocus(Qt::TabFocusReason);
    edtPhrase1->selectAll();
}

SearchDialog::SearchMode SearchDialog::getSearchMode() {
    SearchDialog::SearchMode s=SearchDialog::Simple;
    if (radRegExp->isChecked()) s=SearchDialog::RegExp;
    if (radWildcards->isChecked()) s=SearchDialog::Wildcards;
    return s;
}

bool SearchDialog::logicTerm(bool result1, bool result2, bool result3) {
    bool result=result1;
    if (!getPhrase2().isEmpty()) {
        switch (getLogic12()) {
            case Not: result=result&&(!result2); break;
            default:
            case And: result=result&&result2; break;
        }
    }
    if (!getPhrase3().isEmpty()) {
        switch (getLogic23()) {
            case Not: result=result&&(!result3); break;
            default:
            case And: result=result&&result3; break;
        }
    }
    return result;
}
