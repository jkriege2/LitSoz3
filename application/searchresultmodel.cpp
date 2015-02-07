#include "searchresultmodel.h"
#include "bibtools.h"
#include <QtAlgorithms>
#include <QTextEdit>
#include <QVariant>
#include <QTextDocument>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>

#include <iostream>

SearchResultModel::SearchResultModel(LS3Datastore* datastore, QObject* parent):
    QAbstractListModel(parent)
{
    m_datastore=datastore;
    loadData();
}

SearchResultModel::~SearchResultModel()
{
    //dtor
}

void SearchResultModel::resetModel() {
    loadData();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
beginResetModel();
endResetModel();
#else
reset();
#endif
}

LS3Datastore* SearchResultModel::datastore() const {
    return m_datastore;
}

int	SearchResultModel::rowCount(const QModelIndex & parent) const {
    return datalist.size();
}

QVariant SearchResultModel::data(const QModelIndex & index, int role) const {
    int section=index.column();
    if (role==Qt::DisplayRole) {
        if (section==0) return datalist[index.row()].reference;
        if (section==1) return datalist[index.row()].uuid;
    }
    return QVariant();
}

int	SearchResultModel::columnCount(const QModelIndex & parent) const {
    return 2;
}

QVariant SearchResultModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role==Qt::DisplayRole) {
        if (section==0) return tr("Reference");
        if (section==1) return tr("UUID");
    }
    return QVariant();
}


void SearchResultModel::setSortingOrder(int order) {
    m_sortingOrder=order;
    loadData();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
beginResetModel();
endResetModel();
#else
reset();
#endif
}

void SearchResultModel::clearResults() {
    datalist.clear();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
beginResetModel();
endResetModel();
#else
reset();
#endif
}

void SearchResultModel::addResult(const QString& uuid) {
    for (int i=0; i<datalist.size(); i++) {
        if (datalist[i].uuid==uuid) return;
    }
    ListItem it;
    it.uuid=uuid;
    datalist.append(it);
    loadData();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
beginResetModel();
endResetModel();
#else
reset();
#endif
}

void SearchResultModel::addResults(const QStringList& uuids) {
    for (int j=0; j<uuids.size(); j++) {
        bool ok=true;
        for (int i=0; i<datalist.size(); i++) {
            if (datalist[i].uuid==uuids[j]) {
                ok=false;
                break;
            }
        }
        if (ok) {
            ListItem it;
            it.uuid=uuids[j];
            datalist.append(it);
        }
    }
    loadData();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
beginResetModel();
endResetModel();
#else
reset();
#endif
}

void SearchResultModel::setResults(const QStringList& uuids) {
    datalist.clear();
    addResults(uuids);
}


void SearchResultModel::loadData() {
    for (int i=0; i<datalist.size(); i++) {
        ListItem& it=datalist[i];
        int idx=m_datastore->getRecordByUUID(it.uuid);
        QMap<QString, QVariant> rec=m_datastore->getRecord(idx);
        it.reference=formatShortReferenceSummary(rec);
        QString auth=rec["authors"].toString();
        if (auth.isEmpty()) auth=rec["editors"].toString();
        auth=auth.toLower();
        it.sort1=it.sort2=it.sort3="";
        if (m_sortingOrder==0) { // Number
            it.sort1=rec["num"].toString();
        } else if (m_sortingOrder==1) { // Author, Year, Title
            it.sort1=auth;
            it.sort2=rec["year"].toString().toLower();
            it.sort3=rec["title"].toString();
        } else if (m_sortingOrder==2) { // Author, Title, Year
            it.sort1=auth;
            it.sort2=rec["title"].toString().toLower();
            it.sort3=rec["year"].toString();
        } else if (m_sortingOrder==3) { // Title, Author, Year
            it.sort1=rec["title"].toString().toLower();
            it.sort2=auth;
            it.sort3=rec["year"].toString();
        } else if (m_sortingOrder==4) { // Title, Year, Author
            it.sort1=rec["title"].toString().toLower();
            it.sort2=rec["year"].toString();
            it.sort3=auth;
        } else if (m_sortingOrder==5) { // Year, Title, Author
            it.sort1=rec["year"].toString();
            it.sort2=rec["title"].toString().toLower();
            it.sort3=auth;
        } else if (m_sortingOrder==6) { // Year, Title, Author
            it.sort1=rec["year"].toString();
            it.sort2=rec["title"].toString().toLower();
            it.sort3=auth;
        } else {
            it.sort1=rec["num"].toString();
        }
    }
    qSort(datalist);
}

bool SearchResultModel::ListItem::operator<(const ListItem& b) const {
    if (sort1==b.sort1) {
        if (sort2==b.sort2) {
            return sort3<b.sort3;
        } else return sort2<b.sort2;
    } else return sort1<b.sort1;
}

QString SearchResultModel::getUUID(const QModelIndex & index) const {
    if ((index.row()<0)||(index.row()>=datalist.size())) return "";
    return datalist[index.row()].uuid;
}

Qt::DropActions SearchResultModel::supportedDropActions() const {
    return Qt::CopyAction|Qt::MoveAction;
}

QStringList SearchResultModel::mimeTypes() const {
    QStringList types;
    types << "litsoz3/list.uuid";
    return types;
}

QMimeData* SearchResultModel::mimeData(const QModelIndexList &indexes) const {
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            QString text = getUUID(index);
            stream << text;
        }
    }
    mimeData->setData("litsoz3/list.uuid", encodedData);
    return mimeData;
}


Qt::ItemFlags SearchResultModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDragEnabled | defaultFlags;
    else
        return defaultFlags;
}


