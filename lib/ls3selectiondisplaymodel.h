#ifndef LS3SELECTIONDISPLAYMODEL_H
#define LS3SELECTIONDISPLAYMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <QPair>
#include <QStyledItemDelegate>
#include "ls3datastore.h"
#include "lib_imexport.h"

/*! \brief list model to access a list of all currently selected database records

    Simply instanciate this model with a given datastore and it will automatically be synchronized with the given datastore in future.
    This model returns a HTML formatted short reference in column 0, so you should use a HTMLDelegate to display it! But better use
    LS3SelectionDisplayModelDelegate which is specially tailored for this type of models!
    Column 1 will contain the uuid of the dataset. So you may use it to navigate to a dataset.
*/
class LS3LIB_EXPORT LS3SelectionDisplayModel : public QAbstractListModel {
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
        struct ListItem {
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
class LS3LIB_EXPORT LS3SelectionDisplayModelDelegate : public QStyledItemDelegate {
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
