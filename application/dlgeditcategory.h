#ifndef DLGEDITCATEGORY_H
#define DLGEDITCATEGORY_H

#include <QDialog>

namespace Ui {
    class DlgEditCategory;
}

class DlgEditCategory : public QDialog
{
        Q_OBJECT

    public:
        explicit DlgEditCategory(QWidget *parent , QStringList categories, const QString& oldCat);
        ~DlgEditCategory();

        bool getChangeSub() const;
        QString getOld() const;
        QString getNew() const;
        bool getIgnoreCase() const;
    protected slots:
        void copyDown();
        void on_cmbOldTopic_editTextChanged(const QString &text);
        void newChanged();
    private:
        Ui::DlgEditCategory *ui;
        bool newWasFocused;
};

#endif // DLGEDITCATEGORY_H
