#include "referencetreedockwidget.h"
#include "ui_referencetreedockwidget.h"
#include "ls3summaryproxymodel.h"
#include <QDebug>
#include <QMessageBox>
#include "ls3referencetreemodel.h"
#include "ls3referencetreeitem.h"

ReferenceTreeDockWidget::ReferenceTreeDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ReferenceTreeDockWidget)
{
    m_datastore=NULL;
    m_proxyModel=NULL;
    m_selection=NULL;
    ui->setupUi(this);
    currentItem=NULL;

    actAddFolder=new QAction(QIcon(":/reftree_addfolder.png"), tr("&insert folder"), this);
    actRemoveFolder=new QAction(QIcon(":/reftree_deletefolder.png"), tr("&remove current folder"), this);
    actRemoveFromFolder=new QAction(QIcon(":/reftree_delete.png"), tr("&remove selected items from current folder"), this);
    menuFolderContext=new QMenu(this);
    menuFolderContext->addAction(actAddFolder);
    menuFolderContext->addAction(actRemoveFolder);
    menuFolderContext->hide();

    connect(ui->treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(clicked(QModelIndex)));
    connect(ui->treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(currentChanged(QModelIndex,QModelIndex)));
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuRequested(QPoint)));
    connect(actAddFolder, SIGNAL(triggered()), this, SLOT(addFolder()));
    //connect(ui->btnRefTreeAdd, SIGNAL(clicked()), this, SLOT(addFolder()));
    ui->btnRefTreeAdd->setDefaultAction(actAddFolder);
    connect(actRemoveFolder, SIGNAL(triggered()), this, SLOT(removeFolder()));
    ui->btnRefTreeDelete->setDefaultAction(actRemoveFolder);
    //connect(ui->btnRefTreeDelete, SIGNAL(clicked()), this, SLOT(removeFolder()));
    connect(actRemoveFromFolder, SIGNAL(triggered()), this, SLOT(removeFromFolder()));
    ui->btnRefTreeDeleteReferences->setDefaultAction(actRemoveFromFolder);
    //connect(ui->btnRefTreeDeleteReferences, SIGNAL(clicked()), this, SLOT(removeFromFolder()));

    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeView->setDragEnabled(true);
    ui->treeView->setAcceptDrops(true);
    ui->treeView->setDropIndicatorShown(true);
}

ReferenceTreeDockWidget::~ReferenceTreeDockWidget()
{
    delete ui;
}

void ReferenceTreeDockWidget::setProxyModel(LS3SummarySortFilterProxyModel* proxyModel) {
    m_proxyModel=proxyModel;
}

void ReferenceTreeDockWidget::setSelectionModel(QItemSelectionModel* selection) {
    m_selection=selection;
}

void ReferenceTreeDockWidget::setDatastore(LS3Datastore* datastore) {
    disconnect(ui->treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(clicked(QModelIndex)));
    disconnect(ui->treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(currentChanged(QModelIndex,QModelIndex)));
    disconnect(datastore, SIGNAL(databaseLoaded(bool)), actAddFolder, SLOT(setEnabled(bool)));
    disconnect(datastore, SIGNAL(databaseLoaded(bool)), actRemoveFolder, SLOT(setEnabled(bool)));
    disconnect(datastore, SIGNAL(databaseLoaded(bool)), actRemoveFromFolder, SLOT(setEnabled(bool)));
    ui->treeView->setModel(NULL);
    if (m_proxyModel) m_proxyModel->setFilterItem(NULL);
    m_datastore=datastore;
    if (datastore) {
        ui->treeView->setModel(datastore->getReferencTreeModel());

        actAddFolder->setEnabled(datastore->dbIsLoaded());
        actRemoveFolder->setEnabled(datastore->dbIsLoaded());
        actRemoveFromFolder->setEnabled(datastore->dbIsLoaded());
        connect(ui->treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(clicked(QModelIndex)));
        connect(ui->treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(currentChanged(QModelIndex,QModelIndex)));
        connect(datastore, SIGNAL(databaseLoaded(bool)), actAddFolder, SLOT(setEnabled(bool)));
        connect(datastore, SIGNAL(databaseLoaded(bool)), actRemoveFolder, SLOT(setEnabled(bool)));
        connect(datastore, SIGNAL(databaseLoaded(bool)), actRemoveFromFolder, SLOT(setEnabled(bool)));

        ui->treeView->selectionModel()->select(datastore->getReferencTreeModel()->index(0,0,datastore->getReferencTreeModel()->index(0,0)), QItemSelectionModel::SelectCurrent);
    }
}

void ReferenceTreeDockWidget::clicked(const QModelIndex& index) {
    if (m_proxyModel) {
        LS3ReferenceTreeItem *item = static_cast<LS3ReferenceTreeItem*>(index.internalPointer());
        m_proxyModel->setFilterItem(item);
        currentItem=item;
        currentIndex=index;
    }
}

void ReferenceTreeDockWidget::currentChanged(const QModelIndex& current, const QModelIndex& previous) {
    clicked(current);
}

void ReferenceTreeDockWidget::customContextMenuRequested(QPoint p) {
    QModelIndex idx=ui->treeView->indexAt(p);
    if (idx.isValid()) {
         LS3ReferenceTreeItem *item = static_cast<LS3ReferenceTreeItem*>(idx.internalPointer());
         if ( (item->type()==LS3ReferenceTreeItem::FolderUser) || (item->type()==LS3ReferenceTreeItem::FolderUserBase) ) {
             currentItem=item;
             currentIndex=idx;
             menuFolderContext->exec(QCursor::pos());
         }
    }
}

void ReferenceTreeDockWidget::addFolder() {
    if (currentIndex.isValid() && m_datastore->getReferencTreeModel()) {
        m_datastore->getReferencTreeModel()->insertRow(0, currentIndex);
    }
}


void ReferenceTreeDockWidget::removeFolder() {
    if (currentIndex.isValid() && m_datastore->getReferencTreeModel()) {
        LS3ReferenceTreeItem *item = static_cast<LS3ReferenceTreeItem*>(currentIndex.internalPointer());
        //qDebug()<<"remove "<<item->row();
        if (currentIndex.parent().isValid()) m_datastore->getReferencTreeModel()->removeRow(item->row(), currentIndex.parent());
    }
}

void ReferenceTreeDockWidget::removeFromFolder() {
    if (currentIndex.isValid() && m_datastore->getReferencTreeModel() && m_selection) {
        LS3ReferenceTreeItem *item = static_cast<LS3ReferenceTreeItem*>(currentIndex.internalPointer());
        if ((item->type()==LS3ReferenceTreeItem::FolderUser) || (item->type()==LS3ReferenceTreeItem::FolderUserBase)) {
            QModelIndexList sel=m_selection->selectedRows();
            if (QMessageBox::question(this, tr("Delete record"), tr("Do you really want to delete the selected %1 records from the current reference tree folder '%2'?").arg(sel.size()).arg(item->name()), QMessageBox::Yes | QMessageBox::No)==QMessageBox::Yes) {
                for (int i=sel.size()-1; i>=0; i--) {
                    int row=sel[i].row();
                    QString uuid=m_proxyModel->data(m_proxyModel->index(row, LS3SUMMARYPROXYMODEL_COLUMN_UUID)).toString();
                    item->removeReference(uuid);
                }

            }

        }
    }
}
