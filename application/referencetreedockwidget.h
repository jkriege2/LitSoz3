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
