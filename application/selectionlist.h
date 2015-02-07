#ifndef SELECTIONLIST_H
#define SELECTIONLIST_H

#include <QDockWidget>
#include "ui_selectionlist.h"
#include "ls3datastore.h"
#include "ls3selectiondisplaymodel.h"
#include "programoptions.h"

class SelectionList : public QDockWidget, private Ui::SelectionList {
        Q_OBJECT
    public:
        SelectionList(LS3Datastore* datastore, ProgramOptions* settings, QWidget* parent=0);
        virtual ~SelectionList();
    signals:
        void itemSelected(QString uuid);
    private slots:
        void on_btnClear_clicked();
        void on_btnLoad_clicked();
        void on_btnSave_clicked();
        void on_lstSelection_clicked(const QModelIndex& index);
        void selectionChanged(int, bool);
        void storeSettings();
    private:
        LS3Datastore* m_datastore;
        ProgramOptions* m_settings;
        LS3SelectionDisplayModel* model;
};

#endif // SELECTIONLIST_H
