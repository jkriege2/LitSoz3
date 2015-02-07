#ifndef LS3SUMMARYPROXYMODEL_H
#define LS3SUMMARYPROXYMODEL_H

#include <QAbstractProxyModel>
#include <QItemDelegate>
#include <QMimeData>
#define LS3SUMMARYPROXYMODEL_COLUMN_SELECTION 0
#define LS3SUMMARYPROXYMODEL_COLUMN_ID        1
#define LS3SUMMARYPROXYMODEL_COLUMN_TYPE      2
#define LS3SUMMARYPROXYMODEL_COLUMN_SUMMARY   3
#define LS3SUMMARYPROXYMODEL_COLUMN_RATING    4
#define LS3SUMMARYPROXYMODEL_COLUMN_URL       5
#define LS3SUMMARYPROXYMODEL_COLUMN_URL_DOI   6
#define LS3SUMMARYPROXYMODEL_COLUMN_URL_PUBMED 7
#define LS3SUMMARYPROXYMODEL_COLUMN_FILES     8
#define LS3SUMMARYPROXYMODEL_MAX_COLUMN_VISIBLE 8

#define LS3SUMMARYPROXYMODEL_COLUMN_UUID      (LS3SUMMARYPROXYMODEL_MAX_COLUMN_VISIBLE+1)
#define LS3SUMMARYPROXYMODEL_COLUMN_SORT_TITLE (LS3SUMMARYPROXYMODEL_MAX_COLUMN_VISIBLE+2)
#define LS3SUMMARYPROXYMODEL_COLUMN_SORT_YEARTITLE (LS3SUMMARYPROXYMODEL_MAX_COLUMN_VISIBLE+3)
#define LS3SUMMARYPROXYMODEL_COLUMN_SORT_AUTHORYEARTITLE (LS3SUMMARYPROXYMODEL_MAX_COLUMN_VISIBLE+4)
#define LS3SUMMARYPROXYMODEL_COLUMN_SORT_NUM (LS3SUMMARYPROXYMODEL_MAX_COLUMN_VISIBLE+5)

#define LS3SUMMARYPROXYMODEL_MAX_COLUMN      (LS3SUMMARYPROXYMODEL_MAX_COLUMN_VISIBLE+5)

// forward
class LS3Datastore;

/*! \brief model that summarized LitSoz 3 lit databases

    This works only correctly if a valid LS3Datastore is suppllied in the constructor.
    The class will use this datastore to find out which column in the sourceModel contains
    which field.

    This class reduces the contents of the full model to a smaller set of columns:
      - id of the record
      - type of the record
      - a summary of the citation
      - the rating
      - a colum which displays an icon (world marble), if a Weblink is stored in url
      - a colum which displays an icon, if a file is stored in the files field
    .

    In addition there are some columns that should NOT be displayed to the user, as they are used
    for internal functions (navigation, identification, sorting ...):
      - a column for the UUID. This may be used to move the "cursor" to a given dataset using LS3Datastore::dbMoveUUID()
      - These columns are used sorting:
        - sort by title
        - sort by year, title
        - sort by auhtors, year, title
      .
    .
*/
class LS3SummaryProxyModel : public QAbstractProxyModel {
        Q_OBJECT
    public:
        /** Default constructor */
        LS3SummaryProxyModel(LS3Datastore* datastore, QObject* parent=NULL);
        /** Default destructor */
        virtual ~LS3SummaryProxyModel();

        QModelIndex mapFromSource ( const QModelIndex & sourceIndex ) const;
        QModelIndex mapToSource ( const QModelIndex & proxyIndex ) const;
        QModelIndex index(int r, int c, const QModelIndex &ind=QModelIndex()) const;
        QModelIndex parent(const QModelIndex&) const;
        int rowCount(const QModelIndex &index) const;
        int columnCount(const QModelIndex &index) const;
        QVariant data(const QModelIndex &ind, int role) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        void setSourceModel(QAbstractItemModel *sourceModel);
        bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

        QStringList mimeTypes() const;
        QMimeData* mimeData(const QModelIndexList &indexes) const;

    public slots:
        void invalidate();
    protected slots:
        void _columnsInserted ( const QModelIndex & parent, int start, int end );
        void _columnsMoved ( const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationColumn );
        void _columnsRemoved ( const QModelIndex & parent, int start, int end );
        void _dataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight );
        void _headerDataChanged ( Qt::Orientation orientation, int first, int last );
        void _layoutChanged ();
        void _modelReset ();
        void _rowsInserted ( const QModelIndex & parent, int start, int end );
        void _rowsMoved ( const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow );
        void _rowsRemoved ( const QModelIndex & parent, int start, int end );
        void selectionChanged(int recordNum, bool newSelectionState);
    protected:
        LS3Datastore* datastore;
};

/*! \brief QItemDelegate that displays HTML markup and the rating as a number of stars. This is designed to work together with LS3SummaryProxyModel

    taken from <a href="http://www.qtwiki.de/wiki/%27HTML%27-Delegate">http://www.qtwiki.de/wiki/%27HTML%27-Delegate</a>
*/
class LS3SummaryProxyModelDelegate : public QItemDelegate {
        Q_OBJECT
    public:
        /** Default constructor */
        LS3SummaryProxyModelDelegate(LS3Datastore* datastore, QObject *parent=NULL);
        /** Default destructor */
        virtual ~LS3SummaryProxyModelDelegate();
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    protected:
        QPixmap starImage;
        QPixmap darkStarImage;
        QPixmap selectedImage;
        QPixmap deselectedImage;
        QPixmap iselectedImage;
        QPixmap ideselectedImage;
        int m_maximum;
        LS3Datastore* datastore;
        bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
};


#endif // LS3SUMMARYPROXYMODEL_H
