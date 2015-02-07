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
