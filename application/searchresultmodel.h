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

#ifndef SEARCHRESULTMODEL_H
#define SEARCHRESULTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <QPair>
#include <QStyledItemDelegate>
#include "ls3datastore.h"
#include <QMimeData>

/*! \brief list model to manage a list of search results
*/
class SearchResultModel : public QAbstractListModel {
        Q_OBJECT
    public:
        SearchResultModel(LS3Datastore* datastore, QObject* parent=0);
        virtual ~SearchResultModel();

        /** \brief return the LS3Datastore this model depends on */
        LS3Datastore* datastore() const ;

        QString getUUID(const QModelIndex & index) const;

        void clearResults();
        void addResult(const QString& uuid);
        void addResults(const QStringList& uuids);
        void setResults(const QStringList& uuids);

        virtual int	rowCount(const QModelIndex & parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
        virtual int	columnCount(const QModelIndex & parent = QModelIndex()) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        Qt::DropActions supportedDropActions() const;
        QStringList mimeTypes() const;
        QMimeData* mimeData(const QModelIndexList &indexes) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
    public slots:
        /*! \brief set the sorting order

            These orders are available:
              - 0: Number
              - 1: Author, Year
              - 2: Author, Title
              - 3: Title, Author, Year
              - 4: Title, Year, Author
              - 5: Year, Author, Title
              - 6: Year, Title, Author
            .
         */
        void setSortingOrder(int order);
    protected slots:
        void resetModel();
    protected:
        struct ListItem {
            /*! \brief reference as HTML string for display, this is returned in column 0 of the model */
            QString reference;
            /*! \brief used for sorting, populated according to the sorting order given in setSortingOrder() and m_sortingOrder */
            QString sort1, sort2, sort3;
            /*! \brief uuid of the dataset (for referencing) */
            QString uuid;
            /*! \brief used for sorting: sort by sort1, sort2 and finally sort3 */
            bool operator<(const ListItem& b) const;
        };

        LS3Datastore* m_datastore;
        /** \brief list, used to store the search results (may be altered, using clearResults() and addResult() ) */
        QList<ListItem> datalist;
        /*! \brief set the sorting order

            These orders are available:
              - 0: Number
              - 1: Author, Year
              - 2: Author, Title
              - 3: Title, Author, Year
              - 4: Title, Year, Author
              - 5: Year, Author, Title
              - 6: Year, Title, Author
            .
         */
         int m_sortingOrder;
        /*! \brief populates the sort1, sort2, sort3 and reference properties in datalist */
        void loadData();
    private:
};


#endif // SEARCHRESULTMODEL_H
