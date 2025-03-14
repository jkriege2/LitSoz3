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

#ifndef LS3SELECTIONDISPLAYMODEL_H
#define LS3SELECTIONDISPLAYMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <QPair>
#include <QStyledItemDelegate>
#include "ls3datastore.h"
#include "litsoz3tools_export.h"
#include <QMimeData>
/*! \brief list model to access a list of all currently selected database records

    Simply instanciate this model with a given datastore and it will automatically be synchronized with the given datastore in future.
    This model returns a HTML formatted short reference in column 0, so you should use a HTMLDelegate to display it! But better use
    LS3SelectionDisplayModelDelegate which is specially tailored for this type of models!
    Column 1 will contain the uuid of the dataset. So you may use it to navigate to a dataset.
*/
class LITSOZ3TOOLS_EXPORT LS3SelectionDisplayModel : public QAbstractListModel {
        Q_OBJECT
    public:
        LS3SelectionDisplayModel(LS3Datastore* datastore, QObject* parent=0);
        virtual ~LS3SelectionDisplayModel();

        /** \brief return the LS3Datastore this model depends on */
        LS3Datastore* datastore() const ;

        QString getUUID(const QModelIndex & index) const;

        virtual int	rowCount(const QModelIndex & parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
        virtual int	columnCount(const QModelIndex & parent = QModelIndex()) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        Qt::DropActions supportedDropActions() const;
        QStringList mimeTypes() const;
        QMimeData* mimeData(const QModelIndexList &indexes) const;
        bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
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
        struct LITSOZ3TOOLS_EXPORT ListItem {
            /*! \brief reference as HTML string for display, this is returned in column 0 of the model */
            QString reference;
            /*! \brief used for sorting, populated according to the sorting order given in setSortingOrder() */
            QString sort1, sort2, sort3;
            /*! \brief uuid of the dataset (for referencing) */
            QString uuid;
            /*! \brief used for sorting: sort by sort1, sort2 and finally sort3 */
            bool operator<(const ListItem& b) const;
        };


        LS3Datastore* m_datastore;
        QList<ListItem> datalist;
        int m_sortingOrder;

        void loadData();
    private:
};

/*! \brief QItemDelegate that displays HTML markup and adds a delete button that allows to deselect an item! Tailored to work with LS3SelectionDisplayModel

    taken from <a href="http://www.qtwiki.de/wiki/%27HTML%27-Delegate">http://www.qtwiki.de/wiki/%27HTML%27-Delegate</a>
*/
class LITSOZ3TOOLS_EXPORT LS3SelectionDisplayModelDelegate : public QStyledItemDelegate {
        Q_OBJECT
    public:
        /** Default constructor */
        LS3SelectionDisplayModelDelegate(LS3Datastore* datastore, QObject *parent=NULL);
        /** Default destructor */
        virtual ~LS3SelectionDisplayModelDelegate();
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    protected:
        QPixmap selDeleteImage;
        LS3Datastore* datastore;
        bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
};


#endif // LS3SELECTIONDISPLAYMODEL_H
