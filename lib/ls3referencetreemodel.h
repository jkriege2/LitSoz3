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

#ifndef LS3REFERENCETREEMODEL_H
#define LS3REFERENCETREEMODEL_H

#include <QAbstractItemModel>
#include <QObject>
#include <QtXml>
#include "lib_imexport.h"

class LS3ReferenceTreeItem; // forward
class LS3Datastore; // forward

/*! \brief item model class for LitSoz 3 reference trees

*/
class LS3LIB_EXPORT LS3ReferenceTreeModel : public QAbstractItemModel {
        Q_OBJECT
    public:
        LS3ReferenceTreeModel(LS3Datastore* datastore, QObject *parent = 0);

        virtual ~LS3ReferenceTreeModel();



        void writeToXML(QXmlStreamWriter& writer);
        void readFromXML(QDomNode& n);
        void clear();

        QVariant data(const QModelIndex &index, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
        QModelIndex parent(const QModelIndex &index) const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
        bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
        bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());
        Qt::DropActions supportedDragActions () const;
        QStringList mimeTypes() const;
        QMimeData* mimeData(const QModelIndexList &indexes) const;
        bool dropMimeData(const QMimeData *data,  Qt::DropAction action, int row, int column, const QModelIndex &parent);
protected:
        LS3ReferenceTreeItem* rootItem;
        LS3ReferenceTreeItem* itemAllReferences;
        LS3ReferenceTreeItem* itemNewReferences;
        LS3ReferenceTreeItem* itemFolderRoot;
        LS3ReferenceTreeItem* itemTypeFilters;
        LS3ReferenceTreeItem* itemTopicFilters;
        LS3ReferenceTreeItem* itemAuthorFilters;
        LS3ReferenceTreeItem* itemKeywordFilters;
        LS3Datastore* m_datastore;
    protected slots:
        void topicsChanged();
        void authorsChanged();
        void keywordsChanged();


};

#endif // LS3REFERENCETREEMODEL_H
