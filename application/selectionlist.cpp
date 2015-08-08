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
