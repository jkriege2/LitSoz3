#ifndef REFERENCETREEDOCKWIDGET_H
#define REFERENCETREEDOCKWIDGET_H

#include <QDockWidget>
#include "ls3datastore.h"
#include "ls3summarysortfilterproxymodel.h"
#include <QAction>
#include <QMenu>

namespace Ui {
    class ReferenceTreeDockWidget;
}

class ReferenceTreeDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ReferenceTreeDockWidget(QWidget *parent = 0);
    ~ReferenceTreeDockWidget();

    void setProxyModel(LS3SummarySortFilterProxyModel* proxyModel);
    void setDatastore(LS3Datastore* datastore);
    void setSelectionModel(QItemSelectionModel* selection);

protected slots:
    void clicked(const QModelIndex& index);
    void currentChanged(const QModelIndex& current, const QModelIndex& previous);
    void customContextMenuRequested(QPoint p);
    void addFolder();
    void removeFolder();
    void removeFromFolder();
private:
    Ui::ReferenceTreeDockWidget *ui;
    LS3Datastore* m_datastore;
    LS3SummarySortFilterProxyModel* m_proxyModel;
    QItemSelectionModel* m_selection;
    QAction* actAddFolder;
    QAction* actRemoveFolder;
    QAction* actRemoveFromFolder;
    QMenu* menuFolderContext;
    LS3ReferenceTreeItem* currentItem;
    QModelIndex currentIndex;
};

#endif // REFERENCETREEDOCKWIDGET_H
