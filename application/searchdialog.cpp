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
