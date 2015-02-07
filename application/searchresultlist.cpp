#include "searchresultlist.h"
#include <iostream>

SearchResultList::SearchResultList(LS3Datastore* datastore, ProgramOptions* settings, QWidget* parent):
    QDockWidget(tr("Search Result"), parent)
{
    m_datastore=datastore;
    m_settings=settings;
    model=new SearchResultModel(datastore, this);
    setupUi(this);

    lstResults->setModel(model);
    lstResults->setSelectionMode(QAbstractItemView::ExtendedSelection);
    lstResults->setItemDelegate(new HTMLDelegate(lstResults));
    lstResults->setDragEnabled(false);
    lstResults->setDragDropMode(QAbstractItemView::DragOnly);
    lstResults->setModel(model);
    connect(cmbSorting, SIGNAL(currentIndexChanged ( int )), model, SLOT(setSortingOrder(int)));
    connect(settings, SIGNAL(storeSettings()), this, SLOT(storeSettings()));
    cmbSorting->setCurrentIndex(settings->GetQSettings()->value("search_result_list/sorting", 1).toInt());
    connect(datastore, SIGNAL(databaseLoaded(bool)), btnSelect, SLOT(setEnabled(bool)));
    connect(datastore, SIGNAL(databaseLoaded(bool)), btnSelectAll, SLOT(setEnabled(bool)));
    connect(datastore, SIGNAL(databaseLoaded(bool)), btnClear, SLOT(setEnabled(bool)));
}

SearchResultList::~SearchResultList()
{
    //dtor
}

void SearchResultList::storeSettings() {
   m_settings->GetQSettings()->setValue("search_result_list/sorting",  cmbSorting->currentIndex());
}

void SearchResultList::on_btnClear_clicked() {
    model->clearResults();
}

void SearchResultList::on_btnSelect_clicked() {
    QModelIndexList idx=lstResults->selectionModel()->selectedIndexes();
    QStringList uuids;
    for (int i=0; i<idx.size(); i++) {
        QString uuid=model->getUUID(idx[i]);
        uuids.append(uuid);
    }
    m_datastore->select(uuids);
}

void SearchResultList::on_btnSelectAll_clicked() {
    QStringList uuids;
    for (int i=0; i<model->rowCount(); i++) {
        QString uuid=model->getUUID(model->index(i));
        uuids.append(uuid);
    }
    m_datastore->select(uuids);
}


void SearchResultList::selectionChanged(int recordNum, bool newSel) {
    labStatus->setText(tr("found: %1").arg(model->rowCount()));
}

void SearchResultList::on_lstResults_clicked(const QModelIndex& index) {
    QString uuid=model->getUUID(index);
    m_datastore->dbMoveUUID(uuid);
    emit itemSelected(uuid);
}
