#ifndef SEARCHRESULTLIST_H
#define SEARCHRESULTLIST_H

#include <QDockWidget>
#include "ui_searchresultlist.h"
#include "ls3datastore.h"
#include "searchresultmodel.h"
#include "programoptions.h"
#include "htmldelegate.h"

class SearchResultList : public QDockWidget, private Ui::SearchResultList {
        Q_OBJECT
    public:
        SearchResultList(LS3Datastore* datastore, ProgramOptions* settings, QWidget* parent=0);
        virtual ~SearchResultList();
        SearchResultModel* searchResultModel() { return model; }
    signals:
        void itemSelected(QString uuid);
    private slots:
        void on_btnClear_clicked();
        void on_btnSelect_clicked();
        void on_btnSelectAll_clicked();
        void on_lstResults_clicked(const QModelIndex& index);
        void selectionChanged(int, bool);
        void storeSettings();
    private:
        LS3Datastore* m_datastore;
        ProgramOptions* m_settings;
        SearchResultModel* model;
};

#endif // SEARCHRESULTLIST_H
