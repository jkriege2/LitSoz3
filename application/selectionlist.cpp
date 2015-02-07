#include "selectionlist.h"

SelectionList::SelectionList(LS3Datastore* datastore, ProgramOptions* settings, QWidget* parent):
    QDockWidget(tr("Selection List"), parent)
{
    m_datastore=datastore;
    m_settings=settings;
    model=new LS3SelectionDisplayModel(datastore, this);
    setupUi(this);

    lstSelection->setModel(model);
    lstSelection->setSelectionMode(QAbstractItemView::ExtendedSelection);
    lstSelection->setItemDelegate(new LS3SelectionDisplayModelDelegate(datastore, lstSelection));
    lstSelection->setDragEnabled(false);
    lstSelection->setDefaultDropAction(Qt::CopyAction);
    lstSelection->setAcceptDrops(true);
    lstSelection->viewport()->setAcceptDrops(true);
    lstSelection->setDropIndicatorShown(true);
    lstSelection->setDragDropMode(QAbstractItemView::DragDrop);
    lstSelection->setModel(model);
    connect(cmbSorting, SIGNAL(currentIndexChanged ( int )), model, SLOT(setSortingOrder(int)));
    connect(settings, SIGNAL(storeSettings()), this, SLOT(storeSettings()));
    connect(datastore, SIGNAL(selectionChanged(int, bool)), this, SLOT(selectionChanged(int, bool)));
    cmbSorting->setCurrentIndex(settings->GetQSettings()->value("selection_list/sorting", 1).toInt());
    connect(datastore, SIGNAL(databaseLoaded(bool)), btnLoad, SLOT(setEnabled(bool)));
    connect(datastore, SIGNAL(databaseLoaded(bool)), btnSave, SLOT(setEnabled(bool)));
    connect(datastore, SIGNAL(databaseLoaded(bool)), btnClear, SLOT(setEnabled(bool)));
    connect(datastore, SIGNAL(databaseLoaded(bool)), cmbSorting, SLOT(setEnabled(bool)));
}

SelectionList::~SelectionList()
{
    //dtor
}

void SelectionList::storeSettings() {
   m_settings->GetQSettings()->setValue("selection_list/sorting",  cmbSorting->currentIndex());
}

void SelectionList::on_btnClear_clicked() {
    m_datastore->clearSelection();
}


void SelectionList::on_btnLoad_clicked() {
    QString directory=m_settings->GetQSettings()->value("selection_list/last_directory", QFileInfo(m_datastore->currentFile()).absolutePath()).toString();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load Selection List ..."), directory, tr("Selection List (*.lst)"));
    if (!fileName.isEmpty()) {
        m_settings->GetQSettings()->setValue("selection_list/last_directory", QFileInfo(fileName).dir().absolutePath());

        QApplication::setOverrideCursor(Qt::WaitCursor);

        m_datastore->loadSelection(fileName);

        QApplication::restoreOverrideCursor();
    }
}

void SelectionList::on_btnSave_clicked() {
    QString directory=m_settings->GetQSettings()->value("selection_list/last_directory", QFileInfo(m_datastore->currentFile()).absolutePath()).toString();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Selection List ..."), directory, tr("Selection List (*.lst)"));
    if (!fileName.isEmpty()) {
        m_settings->GetQSettings()->setValue("selection_list/last_directory", QFileInfo(fileName).dir().absolutePath());

        QApplication::setOverrideCursor(Qt::WaitCursor);

        m_datastore->saveSelection(fileName);

        QApplication::restoreOverrideCursor();
    }
}

void SelectionList::selectionChanged(int recordNum, bool newSel) {
    labStatus->setText(tr("selected: %1").arg(model->rowCount()));
}

void SelectionList::on_lstSelection_clicked(const QModelIndex& index) {
    QString uuid=model->getUUID(index);
    m_datastore->dbMoveUUID(uuid);
    emit itemSelected(uuid);
}
