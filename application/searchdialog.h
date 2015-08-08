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

#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include "ui_searchdialog.h"


class SearchDialog : public QDialog, private Ui::SearchDialog {
        Q_OBJECT
    public:
        SearchDialog(QWidget* parent=NULL);
        virtual ~SearchDialog();
        void setFields(const QStringList& fields);
        enum SearchMode { Simple=0, Wildcards=1, RegExp=2};
        enum LogicMode {And=0, Not=1};
        SearchMode getSearchMode();
        QString getPhrase1() { return edtPhrase1->text(); }
        int getFields1() { return cmbFields1->currentIndex(); }
        QString getPhrase2() { return edtPhrase2->text(); }
        int getFields2() { return cmbFields2->currentIndex(); }
        QString getPhrase3() { return edtPhrase3->text(); }
        int getFields3() { return cmbFields3->currentIndex(); }
        LogicMode getLogic12() { return (LogicMode)cmbLogic12->currentIndex(); }
        LogicMode getLogic23() { return (LogicMode)cmbLogic23->currentIndex(); }
        bool logicTerm(bool result1, bool result2, bool result3);
    protected:
    private:
};

#endif // SEARCHDIALOG_H
