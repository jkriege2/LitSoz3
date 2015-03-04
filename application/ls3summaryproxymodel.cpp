#include "ls3summaryproxymodel.h"
#include <QPixmapCache>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QTextDocument>
#include <QPainter>
#include <QTextEdit>
#include "../lib/bibtools.h"
#include "../lib/ls3datastore.h"
#include <QCheckBox>
#include <QEvent>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QDebug>
#include <iostream>




LS3SummaryProxyModel::LS3SummaryProxyModel(LS3Datastore* datastore, QObject* parent):
    QAbstractProxyModel(parent)
{
    this->datastore=datastore;
    //connect(this, SIGNAL(modelReset()), this, SLOT(invalidate()));
    connect(datastore, SIGNAL(selectionChanged(int, bool)), this, SLOT(selectionChanged(int, bool)));
    connect(datastore, SIGNAL(massiveSelectionChange()), this, SLOT(invalidate()));
}

LS3SummaryProxyModel::~LS3SummaryProxyModel()
{
    //dtor
}

void LS3SummaryProxyModel::invalidate() {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
beginResetModel();
endResetModel();
#else
reset();
#endif
}

QModelIndex LS3SummaryProxyModel::mapFromSource ( const QModelIndex & sourceIndex ) const{
    if (datastore) {
        if (sourceIndex.column()==datastore->fieldColumn("rating")) {
            return createIndex(sourceIndex.row(), LS3SUMMARYPROXYMODEL_COLUMN_RATING);
        } else if (sourceIndex.column()==datastore->fieldColumn("url")) {
            return createIndex(sourceIndex.row(), LS3SUMMARYPROXYMODEL_COLUMN_URL);
        } else if (sourceIndex.column()==datastore->fieldColumn("pubmed")) {
            return createIndex(sourceIndex.row(), LS3SUMMARYPROXYMODEL_COLUMN_URL_PUBMED);
        } else if (sourceIndex.column()==datastore->fieldColumn("doi")) {
            return createIndex(sourceIndex.row(), LS3SUMMARYPROXYMODEL_COLUMN_URL_DOI);
        } else if (sourceIndex.column()==datastore->fieldColumn("files")) {
            return createIndex(sourceIndex.row(), LS3SUMMARYPROXYMODEL_COLUMN_FILES);
        } else if (sourceIndex.column()==datastore->fieldColumn("uuid")) {
            return createIndex(sourceIndex.row(), LS3SUMMARYPROXYMODEL_COLUMN_UUID);
        } else if (sourceIndex.column()==datastore->fieldColumn("id")) {
            return createIndex(sourceIndex.row(), LS3SUMMARYPROXYMODEL_COLUMN_ID);
        } else if (sourceIndex.column()==datastore->fieldColumn("type")) {
            return createIndex(sourceIndex.row(), LS3SUMMARYPROXYMODEL_COLUMN_TYPE);
        } else if (sourceIndex.column()==datastore->fieldColumn("title")) {
            return createIndex(sourceIndex.row(), LS3SUMMARYPROXYMODEL_COLUMN_SUMMARY);
        }
    }
    return createIndex(sourceIndex.row(), 0);
}

QModelIndex LS3SummaryProxyModel::mapToSource ( const QModelIndex & proxyIndex ) const{
    //qDebug()<<"LS3SummaryProxyModel::mapToSource("<<proxyIndex<<")";
    /* redirect:
         COLUMN_RATING -> rating
         COLUMN_URL    -> url
         COLUMN_FILES  -> files
         COLUMN_UUID   -> uuid
         COLUMN_ID     -> id
         COLUMN_SUMMARY-> title
         all other     -> 0
    */
    if (!sourceModel()) return QModelIndex();
    if (datastore) {
        if (proxyIndex.column()==LS3SUMMARYPROXYMODEL_COLUMN_RATING) {
            return sourceModel()->index(proxyIndex.row(), datastore->fieldColumn("rating"));
        } else if (proxyIndex.column()==LS3SUMMARYPROXYMODEL_COLUMN_URL) {
            return sourceModel()->index(proxyIndex.row(), datastore->fieldColumn("url"));
        } else if (proxyIndex.column()==LS3SUMMARYPROXYMODEL_COLUMN_URL_PUBMED) {
            return sourceModel()->index(proxyIndex.row(), datastore->fieldColumn("pubmed"));
        } else if (proxyIndex.column()==LS3SUMMARYPROXYMODEL_COLUMN_URL_DOI) {
            return sourceModel()->index(proxyIndex.row(), datastore->fieldColumn("doi"));
        } else if (proxyIndex.column()==LS3SUMMARYPROXYMODEL_COLUMN_FILES) {
            return sourceModel()->index(proxyIndex.row(), datastore->fieldColumn("files"));
        } else if (proxyIndex.column()==LS3SUMMARYPROXYMODEL_COLUMN_UUID) {
            return sourceModel()->index(proxyIndex.row(), datastore->fieldColumn("uuid"));
        } else if (proxyIndex.column()==LS3SUMMARYPROXYMODEL_COLUMN_ID) {
            return sourceModel()->index(proxyIndex.row(), datastore->fieldColumn("id"));
        } else if (proxyIndex.column()==LS3SUMMARYPROXYMODEL_COLUMN_SUMMARY) {
            return sourceModel()->index(proxyIndex.row(), datastore->fieldColumn("title"));
        } else if (proxyIndex.column()==LS3SUMMARYPROXYMODEL_COLUMN_TYPE) {
            return sourceModel()->index(proxyIndex.row(), datastore->fieldColumn("type"));
        }
    }
    //qDebug()<<"   return default";
    return sourceModel()->index(proxyIndex.row(), 0);
}

int LS3SummaryProxyModel::rowCount(const QModelIndex &index) const{
    if (sourceModel()) return sourceModel()->rowCount();
    return 0;
}

int LS3SummaryProxyModel::columnCount(const QModelIndex &index) const{
    // one additional empty column for stretching space
    return LS3SUMMARYPROXYMODEL_MAX_COLUMN+2;
}

bool LS3SummaryProxyModel::setData ( const QModelIndex & index, const QVariant & value, int role ) {
    // only set data of rating and selection ... ignore all other setData requests
    int col=index.column();
    int row=index.row();

    if (role == Qt::EditRole) {
        if (col==LS3SUMMARYPROXYMODEL_COLUMN_SELECTION) {
            //bool b=false;
            //b=value.toBool();
            if (value.toBool()) datastore->select(row);
            else datastore->deselect(row);
            return true;
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_RATING) {
            return QAbstractProxyModel::setData(index, value, role);
        }
    }
    return false;
}

QVariant LS3SummaryProxyModel::data(const QModelIndex &ind, int role) const {
    int row=ind.row();
    int col=ind.column();
    if (datastore) {
        /*if (role==Qt::BackgroundRole) {
            if (row%2) return QColor("white");
            else return QColor("steelblue").lighter(200);
        }*/
        if (col==LS3SUMMARYPROXYMODEL_COLUMN_SELECTION) {
            return datastore->isSelected(row);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_ID) {
            if (role==Qt::DisplayRole) return sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("id")), role);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_UUID) {
            if (role==Qt::DisplayRole) return sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("uuid")), role);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_TYPE) {
            if (role==Qt::DisplayRole) return sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("type")), role);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_SUMMARY) {
            QMap<QString, QVariant> data=datastore->getRecord(row);
            QString result=formatReferenceSummary(data);
            if (role==Qt::DisplayRole) return result;
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_RATING) {
            if (role==Qt::DisplayRole) return sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("rating")), role);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_URL) {
            QString url=sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("url")), Qt::DisplayRole).toString();
            if (role==Qt::DecorationRole) {
                if (!url.isEmpty()) return QIcon(":/worldlogo.png");
            } else if (role==Qt::ToolTipRole) {
                if (!url.isEmpty()) return url;
            }
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_URL_PUBMED) {
            QString url=(sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("pubmed")), Qt::DisplayRole).toString());
            if (role==Qt::DecorationRole) {
                if (!url.isEmpty()) return QIcon(":/pubmedlogo.png");
            } else if (role==Qt::ToolTipRole) {
                if (!url.isEmpty()) return QString("http://www.pubmed.org/")+url;
            }
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_URL_DOI) {
            QString url=(sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("doi")), Qt::DisplayRole).toString());
            if (role==Qt::DecorationRole) {
                if (!url.isEmpty()) return QIcon(":/doilogo.png");
            } else if (role==Qt::ToolTipRole) {
                if (!url.isEmpty()) return QString("http://dx.doi.org/")+url;
            }
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_FILES) {
            QString files=sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("files")), Qt::DisplayRole).toString();
            QStringList sl=files.split('\n');
            QString file="";
            if (sl.size()>0) { //file=sl[0];
                file=QDir::cleanPath(QDir(datastore->baseDirectory()).absoluteFilePath( sl[0] ));
            }
            if (role==Qt::DecorationRole) {
                if (!file.isEmpty()) {
                    QFileInfo info(file);
                    QString ext=info.suffix().toLower();
                    QIcon icon=QIcon();
                    if (!QFile::exists(info.absoluteFilePath())) icon=QIcon(":/filetypes/notexistent.png");
                    else if ((ext=="pdf")) icon=QIcon(":/filetypes/pdf.png");
                    else if ((ext=="ps")||(ext=="eps")) icon=QIcon(":/filetypes/ps.png");
                    else if ((ext=="zip")||(ext=="tar")||(ext=="gz")||(ext=="gz2")||(ext=="bz2")||(ext=="bz")||(ext=="rar")||(ext=="cab")||(ext=="7z")) icon=QIcon(":/filetypes/zip.png");
                    else if ((ext=="doc")||(ext=="docx")||(ext=="odf")) icon=QIcon(":/filetypes/doc.png");
                    else if ((ext=="txt")||(ext=="dat")) icon=QIcon(":/filetypes/txt.png");
                    else if ((ext=="avi")||(ext=="mpg")||(ext=="mpeg")||(ext=="ogg")) icon=QIcon(":/filetypes/video.png");
                    else if ((ext=="mp3")||(ext=="mid")||(ext=="midi")) icon=QIcon(":/filetypes/music.png");
                    else if ((ext=="tex")) icon=QIcon(":/filetypes/tex.png");
                    else if ((ext=="htm")||(ext=="html")||(ext=="xhtml")) icon=QIcon(":/filetypes/html.png");
                    else if ((ext=="png")||(ext=="jpg")||(ext=="jpeg")||(ext=="bmp")||(ext=="tif")||(ext=="tiff")||(ext=="jp2")) icon=QIcon(":/filetypes/image.png");
                    else if (!sl[0].isEmpty()) {
                        QFileIconProvider  provider;
                        icon=provider.icon(info);
                    }
                    return icon;
                }
            } else if (role==Qt::ToolTipRole) {
                if (!file.isEmpty()) return file;
            }
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_SORT_TITLE) {
            QString title=sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("title")), role).toString().simplified().remove(' ').toLower();
            if (role==Qt::DisplayRole) return QVariant(title);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_SORT_NUM) {
            int num=sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("num")), role).toInt();
            if (role==Qt::DisplayRole) return QVariant(num);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_SORT_YEARTITLE) {
            int year=sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("year")), role).toInt();
            if ((year>0)&&(year<100)) year=year+1900;
            QString years=QString("%1").arg((double)year, 4, 'f', 0, '0');
            QString title=sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("title")), role).toString().simplified().remove(' ').toLower();
            if (role==Qt::DisplayRole) return QVariant(years+title);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_SORT_AUTHORYEARTITLE) {
            int year=sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("year")), role).toInt();
            if ((year>0)&&(year<100)) year=year+1900;
            QString years=QString("%1").arg((double)year, 4, 'f', 0, '0');
            QString title=sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("title")), role).toString().simplified().remove(' ').toLower();
            QString author=sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("authors")), role).toString().simplified().remove(' ').toLower();
            QString editors=sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("editors")), role).toString().simplified().remove(' ').toLower();
            if (role==Qt::DisplayRole) return QVariant(author+editors+years+title);
        /*} else if (col==LS3SUMMARYPROXYMODEL_COLUMN_FILTER_YEAR) {
            if (role==Qt::DisplayRole) return sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("year")), role);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_FILTER_AUTHORS) {
            if (role==Qt::DisplayRole) return sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("authors")), role);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_FILTER_EDITORS) {
            if (role==Qt::DisplayRole) return sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("editors")), role);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_FILTER_TITLE) {
            if (role==Qt::DisplayRole) return sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("title")), role);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_FILTER_KEYWORDS) {
            if (role==Qt::DisplayRole) return sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("keywords")), role);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_FILTER_TOPIC) {
            if (role==Qt::DisplayRole) return sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("topic")), role);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_FILTER_PUBLISHER) {
            if (role==Qt::DisplayRole) return sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("publisher")), role);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_FILTER_BOOKTITLE) {
            if (role==Qt::DisplayRole) return sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("booktitle")), role);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_FILTER_INSTITUTION) {
            if (role==Qt::DisplayRole) return sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("institution")), role);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_FILTER_STATUS) {
            if (role==Qt::DisplayRole) return sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("status")), role);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_FILTER_ORIGIN) {
            if (role==Qt::DisplayRole) return sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("origin")), role);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_FILTER_LANGUAGE) {
            if (role==Qt::DisplayRole) return sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("language")), role);
        } else if (col==LS3SUMMARYPROXYMODEL_COLUMN_FILTER_JOURNAL) {
            if (role==Qt::DisplayRole) return sourceModel()->data(sourceModel()->index(row, datastore->fieldColumn("journal")), role);*/
        }
    }

    //return sourceModel()->data(mapToSource(ind), role);
    return QVariant();
}

QVariant LS3SummaryProxyModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation==Qt::Horizontal) {
        if (role==Qt::DisplayRole) {
            switch(section) {
                case LS3SUMMARYPROXYMODEL_COLUMN_ID: return QVariant("ID");
                case LS3SUMMARYPROXYMODEL_COLUMN_TYPE: return QVariant("Type");
                case LS3SUMMARYPROXYMODEL_COLUMN_SUMMARY: return QVariant("Reference");
                case LS3SUMMARYPROXYMODEL_COLUMN_RATING: return QVariant("Rating");
            }
        } else if (role==Qt::DecorationRole) {
            switch(section) {
                case LS3SUMMARYPROXYMODEL_COLUMN_FILES: return QVariant(QIcon(":/table_filetype.png"));
                case LS3SUMMARYPROXYMODEL_COLUMN_URL: return QVariant(QIcon(":/table_url.png"));
                case LS3SUMMARYPROXYMODEL_COLUMN_URL_PUBMED: return QVariant(QIcon(":/pubmedlogo.png"));
                case LS3SUMMARYPROXYMODEL_COLUMN_URL_DOI: return QVariant(QIcon(":/doilogo.png"));
                case LS3SUMMARYPROXYMODEL_COLUMN_SELECTION: return QVariant(QIcon(":/table_selection.png"));
            }
        }
    } else {
        if (role==Qt::DisplayRole) return QVariant(section);
    }
    return QVariant();
}

QModelIndex LS3SummaryProxyModel::index(int r, int c, const QModelIndex &parent) const {
    return createIndex(r, c);
}

QModelIndex LS3SummaryProxyModel::parent(const QModelIndex&) const {
    return QModelIndex();
}

void LS3SummaryProxyModel::setSourceModel(QAbstractItemModel *sourceModel) {
    if (this->sourceModel()!=NULL) {
        disconnect(this->sourceModel(), SIGNAL(columnsInserted(const QModelIndex &, int, int)), this, SLOT(_columnsInserted( const QModelIndex &, int, int)));
        disconnect(this->sourceModel(), SIGNAL(columnsMoved(const QModelIndex & , int, int, const QModelIndex &, int)), this, SLOT(_columnsMoved( const QModelIndex & , int, int, const QModelIndex &, int)));
        disconnect(this->sourceModel(), SIGNAL(columnsRemoved(const QModelIndex & , int, int)), this, SLOT(_columnsRemoved( const QModelIndex & , int, int)));
        disconnect(this->sourceModel(), SIGNAL(dataChanged (const QModelIndex &, const QModelIndex &)), this, SLOT(_dataChanged ( const QModelIndex &, const QModelIndex &)));
        disconnect(this->sourceModel(), SIGNAL(headerDataChanged ( Qt::Orientation, int, int)), this, SLOT(_headerDataChanged ( Qt::Orientation, int, int)));
        disconnect(this->sourceModel(), SIGNAL(layoutChanged()), this, SLOT(_layoutChanged()));
        disconnect(this->sourceModel(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(_rowsInserted( const QModelIndex &, int, int)));
        disconnect(this->sourceModel(), SIGNAL(rowsMoved(const QModelIndex & , int, int, const QModelIndex &, int)), this, SLOT(_rowsMoved( const QModelIndex & , int, int, const QModelIndex &, int)));
        disconnect(this->sourceModel(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(_rowsRemoved(const QModelIndex &, int, int)));
        disconnect(this->sourceModel(), SIGNAL(modelReset()), this, SLOT(_modelReset()));
    }
    QAbstractProxyModel::setSourceModel(sourceModel);
    if (sourceModel) {
        connect(sourceModel, SIGNAL(modelReset()), this, SLOT(_modelReset()));
        connect(this->sourceModel(), SIGNAL(columnsInserted(const QModelIndex &, int, int)), this, SLOT(_columnsInserted( const QModelIndex &, int, int)));
        connect(this->sourceModel(), SIGNAL(columnsMoved(const QModelIndex & , int, int, const QModelIndex &, int)), this, SLOT(_columnsMoved( const QModelIndex & , int, int, const QModelIndex &, int)));
        connect(this->sourceModel(), SIGNAL(columnsRemoved(const QModelIndex & , int, int)), this, SLOT(_columnsRemoved( const QModelIndex & , int, int)));
        connect(this->sourceModel(), SIGNAL(dataChanged (const QModelIndex &, const QModelIndex &)), this, SLOT(_dataChanged ( const QModelIndex &, const QModelIndex &)));
        connect(this->sourceModel(), SIGNAL(headerDataChanged ( Qt::Orientation, int, int)), this, SLOT(_headerDataChanged ( Qt::Orientation, int, int)));
        connect(this->sourceModel(), SIGNAL(layoutChanged()), this, SLOT(_layoutChanged()));
        connect(this->sourceModel(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(_rowsInserted( const QModelIndex &, int, int)));
        connect(this->sourceModel(), SIGNAL(rowsMoved(const QModelIndex & , int, int, const QModelIndex &, int)), this, SLOT(_rowsMoved( const QModelIndex & , int, int, const QModelIndex &, int)));
        connect(this->sourceModel(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(_rowsRemoved(const QModelIndex &, int, int)));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
beginResetModel();
endResetModel();
#else
reset();
#endif
    }
}

void  LS3SummaryProxyModel::_modelReset(){
    //std::cout<<">>>>   LS3SummaryProxyModel::_modelReset()\n";
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
beginResetModel();
endResetModel();
#else
reset();
#endif
}

void LS3SummaryProxyModel::_columnsInserted ( const QModelIndex & parent, int start, int end ) {
    //std::cout<<">>>>   LS3SummaryProxyModel::_columnsInserted\n";
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
beginResetModel();
endResetModel();
#else
reset();
#endif
}
void LS3SummaryProxyModel::_columnsMoved ( const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationColumn ) {
    //std::cout<<">>>>   LS3SummaryProxyModel::_columnsMoved\n";
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
beginResetModel();
endResetModel();
#else
reset();
#endif
}
void LS3SummaryProxyModel::_columnsRemoved ( const QModelIndex & parent, int start, int end ) {
    //std::cout<<">>>>   LS3SummaryProxyModel::_columnsRemoved\n";
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
beginResetModel();
endResetModel();
#else
reset();
#endif
}
void LS3SummaryProxyModel::_dataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight ) {
    //std::cout<<">>>>   LS3SummaryProxyModel::_dataChanged\n";
    //reset();
    emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight));
}
void LS3SummaryProxyModel::_headerDataChanged ( Qt::Orientation orientation, int first, int last ) {
    //std::cout<<">>>>   LS3SummaryProxyModel::_headerDataChanged\n";
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
beginResetModel();
endResetModel();
#else
reset();
#endif
}
void LS3SummaryProxyModel::_layoutChanged () {
    //reset();
    //std::cout<<">>>>   LS3SummaryProxyModel::_layoutChanged\n";
    emit layoutChanged();
}
void LS3SummaryProxyModel::_rowsInserted ( const QModelIndex & parent, int start, int end ) {
    //std::cout<<">>>>   LS3SummaryProxyModel::_rowsInserted\n";
    beginInsertRows(QModelIndex(), start, end);
    endInsertRows();
    //emit rowsInserted(mapFromSource(parent), start, end);
    //beginInsertRows(mapFromSource(parent), start, end);
    //endInsertRows();
}
void LS3SummaryProxyModel::_rowsMoved ( const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow ) {
    //std::cout<<">>>>   LS3SummaryProxyModel::_rowsMoved\n";
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
beginResetModel();
endResetModel();
#else
reset();
#endif
}
void LS3SummaryProxyModel::_rowsRemoved ( const QModelIndex & parent, int start, int end ) {
    //std::cout<<">>>>   LS3SummaryProxyModel::_rowsRemoved\n";
   // beginRemoveRows(QModelIndex(), start, end);
   // endInsertRows();
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
    #else
    reset();
    #endif

}

void LS3SummaryProxyModel::selectionChanged(int recordNum, bool newSelectionState) {
    emit dataChanged(index(recordNum, LS3SUMMARYPROXYMODEL_COLUMN_SELECTION), index(recordNum, LS3SUMMARYPROXYMODEL_COLUMN_SELECTION));
}

QStringList LS3SummaryProxyModel::mimeTypes() const {
    QStringList types;
    types << "litsoz3/list.uuid";
    return types;
}

QMimeData* LS3SummaryProxyModel::mimeData(const QModelIndexList &indexes) const {
     QMimeData *mimeData = new QMimeData();
     QByteArray encodedData;

     QDataStream stream(&encodedData, QIODevice::WriteOnly);

     foreach (QModelIndex index, indexes) {
         if (index.isValid()) {
             QString text = datastore->getField(index.row(), "uuid").toString();
             stream << text;
         }
     }


     mimeData->setData("litsoz3/list.uuid", encodedData);
     return mimeData;
}











LS3SummaryProxyModelDelegate::LS3SummaryProxyModelDelegate(LS3Datastore* datastore, QObject *parent):
    QItemDelegate(parent)
{
    this->datastore=datastore;
    starImage=    QPixmap(":/jkstarratingwidget/smallstar.png");
    darkStarImage=QPixmap(":/jkstarratingwidget/smalldarkstar.png");
    selectedImage=QPixmap(":/selected_small.png");
    deselectedImage=QPixmap(":/deselected_small.png");
    iselectedImage=QPixmap(":/invselected_small.png");
    ideselectedImage=QPixmap(":/invdeselected_small.png");
    m_maximum=5;
}

LS3SummaryProxyModelDelegate::~LS3SummaryProxyModelDelegate() {
    //dtor
}


void LS3SummaryProxyModelDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    drawBackground(painter, option, index);

    if (index.column()==LS3SUMMARYPROXYMODEL_COLUMN_SUMMARY) {
        QString cname=option.palette.text().color().name();
        //std::cout<<QString::number((ulong)option.state, 2).toStdString()<<std::endl;
        if ( (option.state & QStyle::State_Selected ) && (option.state & QStyle::State_Active ) ) {
             cname=option.palette.highlightedText().color().name();
        }

        //doc.setTextWidth((option.rect.width()<100)?100:option.rect.width());
        QString html=QString("<font color=\"%1\">%2</a>").arg(cname).arg(index.data().toString());
        QRect r(QPoint(0, 0), option.rect.size());
        QString id=QString::number(r.width())+"_"+QString::number(r.height())+"_"+html;


        QPixmap pix;
        if (!QPixmapCache::find(id, &pix)) {
            //qDebug()<<"   draw html: create pixmap dT="<<double(et.nsecsElapsed())/1000000.0<<"ms";
            {
                QImage img=QImage(r.width(), r.height(), QImage::Format_ARGB32_Premultiplied);
                img.fill(Qt::transparent);
                QPainter pixp;
                pixp.begin(&img);
                QTextDocument doc;
                doc.setHtml(html);
                doc.drawContents(&pixp, r);
                pixp.end();
                pix=QPixmap::fromImage(img);
            }
            /*bool ok=*/
            QPixmapCache::insert(id, pix);;
            //qDebug()<<" inserted pixmap: "<<ok<<"   cache_limit="<<QPixmapCache::cacheLimit();
        }
        painter->save();

        painter->translate(option.rect.topLeft());
        painter->drawPixmap(0,0, pix);
        painter->restore();




        drawFocus(painter, option, option.rect);
    } else if (index.column()==LS3SUMMARYPROXYMODEL_COLUMN_SELECTION) {
        painter->save();
        painter->translate(option.rect.topLeft());
        painter->setClipRect(0,0,option.rect.width(),option.rect.height());
        painter->setClipping(true);
        bool s=index.data().toBool();
        if (option.state & QStyle::State_Selected) {
            if (s) painter->drawPixmap(4,2,iselectedImage);
            else painter->drawPixmap(4,2,ideselectedImage);
        } else {
            if (s) painter->drawPixmap(4,2,selectedImage);
            else painter->drawPixmap(4,2,deselectedImage);
        }
        painter->restore();
        drawFocus(painter, option, option.rect);
    } else if (index.column()==LS3SUMMARYPROXYMODEL_COLUMN_RATING) {
        painter->save();
        painter->translate(option.rect.topLeft());
        int w=qMin((option.rect.width()-2)/m_maximum, starImage.width());
        int m_rating=index.data().toInt();
        int x=1;
        for (int i=1; i<=m_maximum; i++) {
            if (m_rating>=i) painter->drawPixmap(x,2,starImage.scaled(w, w));
            else painter->drawPixmap(x,2,darkStarImage.scaled(w, w));
            x=x+w;
        }
        painter->restore();
        drawFocus(painter, option, option.rect);
    } else {
        QItemDelegate::paint(painter, option, index);
    }
}

QSize LS3SummaryProxyModelDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (index.column()==LS3SUMMARYPROXYMODEL_COLUMN_SUMMARY) {
        QTextDocument doc;
        doc.setHtml(index.data().toString());

        return doc.size().toSize();
    } else if (index.column()==LS3SUMMARYPROXYMODEL_COLUMN_RATING) {
        QSize s((starImage.width())*m_maximum+2, starImage.height()+4);
        return s;
    } else if (index.column()==LS3SUMMARYPROXYMODEL_COLUMN_SELECTION) {
        QSize s(selectedImage.width()+8, selectedImage.height()+4);
        return s;
    }
    return QItemDelegate::sizeHint(option, index);
}

QWidget *LS3SummaryProxyModelDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    return NULL; //QItemDelegate::createEditor(parent, option, index);
}

void LS3SummaryProxyModelDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QItemDelegate::setEditorData(editor, index);
}

void LS3SummaryProxyModelDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QItemDelegate::setModelData(editor, model, index);
}

void LS3SummaryProxyModelDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QItemDelegate::updateEditorGeometry(editor, option, index);
}

bool LS3SummaryProxyModelDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    //qDebug()<<"LS3SummaryProxyModelDelegate::editorEvent("<<index<<")";
     if (index.column() == LS3SUMMARYPROXYMODEL_COLUMN_RATING) {
         if (event->type() == QEvent::MouseButtonPress) {
             QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
             int w=qMin(option.rect.width()/m_maximum, starImage.width());
             int stars = qBound(0, int(0.7 + qreal(mouseEvent->pos().x()
                 - option.rect.x()) / w), 5);
             model->setData(index, QVariant(stars));
             return false; //so that the selection can change
         }
     } else if (index.column() == LS3SUMMARYPROXYMODEL_COLUMN_SELECTION) {
         if (event->type() == QEvent::MouseButtonPress) {
             model->setData(index, QVariant(!model->data(index).toBool()));
             return false; //so that the selection can change
         }
     } else if ((index.column() == LS3SUMMARYPROXYMODEL_COLUMN_URL) || (index.column() == LS3SUMMARYPROXYMODEL_COLUMN_URL_PUBMED) || (index.column() == LS3SUMMARYPROXYMODEL_COLUMN_URL_DOI)) {
         if (event->type() == QEvent::MouseButtonDblClick) {
             QString data=index.data(Qt::ToolTipRole).toString();
             if (!data.isEmpty()) {
                 QDesktopServices::openUrl(QUrl(data, QUrl::TolerantMode));
             }
             return false; //so that the selection can change
         }
     } else if (index.column() == LS3SUMMARYPROXYMODEL_COLUMN_FILES) {
         if (event->type() == QEvent::MouseButtonDblClick) {
             QString data=index.data(Qt::ToolTipRole).toString();
             QString a=QDir::cleanPath (QDir(datastore->baseDirectory()).absoluteFilePath( data ));
             if (!data.isEmpty()) {
                 QDesktopServices::openUrl(QUrl("file:///"+a, QUrl::TolerantMode));
             }
             return false; //so that the selection can change
         }
     }
     return QItemDelegate::editorEvent(event, model, option, index);
     //return false;
}


