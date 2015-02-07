#include "ls3referencetreeitem.h"
#include "ls3datastore.h"
#include <QDebug>
#include <QtXml>

#include "ls3referencetreemodel.h"

LS3ReferenceTreeModel::LS3ReferenceTreeModel(LS3Datastore* datastore, QObject *parent) :
    QAbstractItemModel(parent)
{
    m_datastore=datastore;

    // create root node
    rootItem=new LS3ReferenceTreeItem("", NULL, LS3ReferenceTreeItem::FolderUserBase, datastore);

    // create some basic nodes
    itemAllReferences=new LS3ReferenceTreeItem("All References", NULL, LS3ReferenceTreeItem::FolderAllItems, datastore);
    itemNewReferences=new LS3ReferenceTreeItem("Recently Added References (3 days)", NULL, LS3ReferenceTreeItem::FolderNewItems, datastore);
    itemNewReferences->setFilterAgeDays(3);
        LS3ReferenceTreeItem* itemSubNewReferences;
        itemSubNewReferences=new LS3ReferenceTreeItem("last week", NULL, LS3ReferenceTreeItem::FolderNewItems, datastore);
        itemSubNewReferences->setFilterAgeDays(8);
        itemNewReferences->appendChild(itemSubNewReferences);
        itemSubNewReferences=new LS3ReferenceTreeItem("last 2 weeks", NULL, LS3ReferenceTreeItem::FolderNewItems, datastore);
        itemSubNewReferences->setFilterAgeDays(15);
        itemNewReferences->appendChild(itemSubNewReferences);
        itemSubNewReferences=new LS3ReferenceTreeItem("last month", NULL, LS3ReferenceTreeItem::FolderNewItems, datastore);
        itemSubNewReferences->setFilterAgeDays(30);
        itemNewReferences->appendChild(itemSubNewReferences);
    itemFolderRoot=new LS3ReferenceTreeItem("User Folders", NULL, LS3ReferenceTreeItem::FolderUserBase, datastore);
    itemTypeFilters=new LS3ReferenceTreeItem("Filter: Type", NULL, LS3ReferenceTreeItem::FolderBasicFilter, datastore);
    itemTopicFilters=new LS3ReferenceTreeItem("Filter: Topic", NULL, LS3ReferenceTreeItem::FolderBasicFilter, datastore);
    itemAuthorFilters=new LS3ReferenceTreeItem("Filter: Author/Editor", NULL, LS3ReferenceTreeItem::FolderBasicFilter, datastore);
    itemKeywordFilters=new LS3ReferenceTreeItem("Filter: Keywords", NULL, LS3ReferenceTreeItem::FolderBasicFilter, datastore);

    //qDebug()<<datastore;
    //qDebug()<<datastore->getPluginServices();
    //qDebug()<<datastore->getPluginServices()->GetLanguageID();

    QStringList sl=datastore->getPluginServices()->getFDFIDs();
    for (int i=0; i<sl.size(); i++) {
        itemTypeFilters->appendChild(new LS3ReferenceTreeItem(datastore->getPluginServices()->getFDFName(sl[i]), "type", sl[i], itemTypeFilters, datastore));
    }

    //topicsChanged();
    connect(datastore, SIGNAL(topicsListChanged()), this, SLOT(topicsChanged()));
    connect(datastore, SIGNAL(authorsListChanged()), this, SLOT(authorsChanged()));
    connect(datastore, SIGNAL(keywordsListChanged()), this, SLOT(keywordsChanged()));

    rootItem->appendChild(itemAllReferences);
    rootItem->appendChild(itemNewReferences);
    rootItem->appendChild(itemTopicFilters);
    rootItem->appendChild(itemKeywordFilters);
    rootItem->appendChild(itemAuthorFilters);
    rootItem->appendChild(itemTypeFilters);
    rootItem->appendChild(itemFolderRoot);
}

void LS3ReferenceTreeModel::writeToXML(QXmlStreamWriter& writer) {
    itemFolderRoot->writeToXML(writer);
}

void LS3ReferenceTreeModel::readFromXML(QDomNode& n) {
    clear();
    itemFolderRoot->insertChildren(n);
    reset();
}

void LS3ReferenceTreeModel::clear() {
    removeRows(0, itemFolderRoot->childCount(), index(rootItem->childCount()-1, 0));
    reset();
}

LS3ReferenceTreeModel::~LS3ReferenceTreeModel() {
    delete rootItem;
}

QVariant LS3ReferenceTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    LS3ReferenceTreeItem *item = static_cast<LS3ReferenceTreeItem*>(index.internalPointer());

    if (!item) return QVariant();

    if (role == Qt::DisplayRole) {
        if (item->referenceCount()>0) return item->name()+QString(" (%1)").arg(item->referenceCount());
        else return item->name();
    }
    if (role == Qt::EditRole) return item->name();
    if (role == Qt::DecorationRole) return item->icon();

    return QVariant();
}

Qt::ItemFlags LS3ReferenceTreeModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags f=Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (!index.isValid())
         return 0;
    LS3ReferenceTreeItem *item = static_cast<LS3ReferenceTreeItem*>(index.internalPointer());
    if (!item) return f;
    if (item->isUserEditable()) f = f | Qt::ItemIsEditable;
    if (item->type()==LS3ReferenceTreeItem::FolderUser) f = f | Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled;
    return f;
}


QVariant LS3ReferenceTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    return QVariant();
}


QModelIndex LS3ReferenceTreeModel::index(int row, int column, const QModelIndex &parent) const {
    //qDebug()<<"create Index ("<<row<<column<<parent<<")";
    if (!hasIndex(row, column, parent))
         return QModelIndex();

     LS3ReferenceTreeItem *parentItem;

     if (!parent.isValid())
         parentItem = rootItem;
     else
         parentItem = static_cast<LS3ReferenceTreeItem*>(parent.internalPointer());

     if (!parentItem) return QModelIndex();

     LS3ReferenceTreeItem *childItem = parentItem->child(row);
     if (childItem)
         return createIndex(row, column, childItem);
     else
         return QModelIndex();
}


QModelIndex LS3ReferenceTreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid())
        return QModelIndex();

    LS3ReferenceTreeItem *childItem = static_cast<LS3ReferenceTreeItem*>(index.internalPointer());
    LS3ReferenceTreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem) return QModelIndex();
    if (!parentItem) return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}


int LS3ReferenceTreeModel::rowCount(const QModelIndex &parent) const {
    LS3ReferenceTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<LS3ReferenceTreeItem*>(parent.internalPointer());

    if (!parentItem) return 0;
    return parentItem->childCount();
}


int LS3ReferenceTreeModel::columnCount(const QModelIndex &parent) const {
    /*if (parent.isValid())
        return static_cast<LS3ReferenceTreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();*/
    return 1;
}

bool LS3ReferenceTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.isValid() && (role == Qt::EditRole) ) {
        LS3ReferenceTreeItem *item = static_cast<LS3ReferenceTreeItem*>(index.internalPointer());
        if (item && item->type()==LS3ReferenceTreeItem::FolderUser) {
            item->setName(value.toString());
            emit dataChanged(index, index);
            return true;
        }
    }
    return false;
}

bool LS3ReferenceTreeModel::insertRows(int start, int count, const QModelIndex &parent) {
    beginInsertRows(parent, start, start+count-1);
    LS3ReferenceTreeItem *item = static_cast<LS3ReferenceTreeItem*>(parent.internalPointer());
    if (item) {
        for (int row = 0; row < count; ++row) {
            item->appendFolder("");
        }
    }

    endInsertRows();
    return true;
}

bool LS3ReferenceTreeModel::removeRows(int start, int count, const QModelIndex &parent) {
    if (count>0) {
        beginRemoveRows(parent, start, start+count-1);
        LS3ReferenceTreeItem *item = static_cast<LS3ReferenceTreeItem*>(parent.internalPointer());
        if (item) {
            for (int row = start+count-1; row >= start; row--) {
                //qDebug()<<"removing "<<row<<"/"<<item->childCount();
                item->removeChild(row);
            }
        }

        endRemoveRows();
        //qDebug()<<"removing done!";
        return true;
    }
    return false;
}

Qt::DropActions LS3ReferenceTreeModel::supportedDragActions () const {
    return Qt::CopyAction | Qt::MoveAction;
}

QStringList LS3ReferenceTreeModel::mimeTypes() const
{
    QStringList types;
    types << "litsoz3/reftree.folder" << "litsoz3/list.uuid";
    return types;
}

QMimeData* LS3ReferenceTreeModel::mimeData(const QModelIndexList &indexes) const {
     QMimeData *mimeData = new QMimeData();
     QByteArray encodedData;

     QDataStream stream(&encodedData, QIODevice::WriteOnly);

     foreach (QModelIndex index, indexes) {
         if (index.isValid()) {
             LS3ReferenceTreeItem *item = static_cast<LS3ReferenceTreeItem*>(index.internalPointer());
             if (item) {
                 QString text = item->toXml();
                 stream << text;
             }
         }
     }

     mimeData->setData("litsoz3/reftree.folder", encodedData);
     return mimeData;
 }

bool LS3ReferenceTreeModel::dropMimeData(const QMimeData *data,  Qt::DropAction action, int row, int column, const QModelIndex &parent) {
     if (action == Qt::IgnoreAction)
         return true;


     if (column > 0)
         return false;
     int beginRow;

     if (row != -1)
         beginRow = row;
     else if (parent.isValid())
         beginRow = parent.row();
     else
         beginRow = rowCount(QModelIndex());
     if (data->hasFormat("litsoz3/list.uuid")) {
         QByteArray encodedData = data->data("litsoz3/list.uuid");
         QDataStream stream(&encodedData, QIODevice::ReadOnly);
         QStringList newItems;
         int rows = 0;
         while (!stream.atEnd()) {
             QString text;
             stream >> text;
             newItems << text;
             ++rows;
         }

         if (parent.isValid()) {
             LS3ReferenceTreeItem *item = static_cast<LS3ReferenceTreeItem*>(parent.internalPointer());
             //qDebug()<<"drop onto "<<parent<<" ["<<item->name()<<"] row="<<row<<" column="<<column;
             if (item) {
                 foreach (QString uuid, newItems) {
                    item->appendReference(uuid);
                 }
             }

         }
         /*insertRows(beginRow, rows, QModelIndex());
         foreach (QString text, newItems) {
             QModelIndex idx = index(beginRow, 0, QModelIndex());
             setData(idx, text);
             beginRow++;
         }*/


         return true;
     } else if (data->hasFormat("litsoz3/reftree.folder")) {
         QByteArray encodedData = data->data("litsoz3/reftree.folder");

         QDomDocument doc;
         if ( parent.isValid() && doc.setContent(encodedData) ) {
             LS3ReferenceTreeItem *item = static_cast<LS3ReferenceTreeItem*>(parent.internalPointer());
             if (item) {
                 item->insertChildren(doc.documentElement());
             }

         }


         return true;
     }
     return false;
}

void LS3ReferenceTreeModel::topicsChanged() {
    //qDebug()<<"topicsChanged()";
    //qDebug()<<"topicsChanged()"<<m_datastore->getField(0,"topic");
    QStringList sl=m_datastore->getTopicsModel()->stringList();

    //qDebug()<<"topicsChanged(): 1. remove all unused children";
    // 1. remove all unused children
    for (int i=itemTopicFilters->childCount()-1; i>=0; i--) {
        LS3ReferenceTreeItem* c = itemTopicFilters->child(i);
        if (!sl.contains(c->filter())) {
            beginRemoveRows(index(rootItem->childIndex(itemTopicFilters), 0), i, i);
            itemTopicFilters->removeChild(i);
            endRemoveRows();
        } else {
            sl.removeAll(c->filter());
        }
    }

    //qDebug()<<"topicsChanged(): 2. add nodes for them";
    // now sl contains only those topics that have to be added, so we
    // 2. add nodes for them
    for (int i=0; i<sl.size(); i++) {
        int c=0;
        while (c<itemTopicFilters->childCount() && itemTopicFilters->child(c)->filter()<=sl[i]) {
            c++;
        }
        if (c<0) c=0;
        if (c>itemTopicFilters->childCount()) c=itemTopicFilters->childCount();

        beginInsertRows(index(rootItem->childIndex(itemTopicFilters), 0), c, c);
        LS3ReferenceTreeItem* it=new LS3ReferenceTreeItem(sl[i], "topic", sl[i], itemTypeFilters, m_datastore);
        itemTopicFilters->appendChild(it);
        itemTopicFilters->moveChild(itemTopicFilters->childCount()-1, c);
        endInsertRows();
    }
}

void LS3ReferenceTreeModel::keywordsChanged() {
    QStringList sl=m_datastore->getKeywords();

    // 1. remove all unused children
    for (int i=itemKeywordFilters->childCount()-1; i>=0; i--) {
        LS3ReferenceTreeItem* c = itemKeywordFilters->child(i);
        if (!sl.contains(c->filter())) {
            beginRemoveRows(index(rootItem->childIndex(itemKeywordFilters), 0), i, i);
            itemKeywordFilters->removeChild(i);
            endRemoveRows();
        } else {
            sl.removeAll(c->filter());
        }
    }

    //qDebug()<<"topicsChanged(): 2. add nodes for them";
    // now sl contains only those topics that have to be added, so we
    // 2. add nodes for them
    for (int i=0; i<sl.size(); i++) {
        int c=0;
        while (c<itemKeywordFilters->childCount() && itemKeywordFilters->child(c)->filter()<=sl[i]) {
            c++;
        }
        if (c<0) c=0;
        if (c>itemKeywordFilters->childCount()) c=itemKeywordFilters->childCount();

        beginInsertRows(index(rootItem->childIndex(itemKeywordFilters), 0), c, c);
        LS3ReferenceTreeItem* it=new LS3ReferenceTreeItem(sl[i], "keywords", sl[i], itemTypeFilters, m_datastore, LS3ReferenceTreeItem::FolderFilterContains);
        itemKeywordFilters->appendChild(it);
        itemKeywordFilters->moveChild(itemKeywordFilters->childCount()-1, c);
        endInsertRows();
    }
}

void LS3ReferenceTreeModel::authorsChanged() {
    QStringList sl=m_datastore->getAuthors();

    // 1. remove all unused children
    for (int i=itemAuthorFilters->childCount()-1; i>=0; i--) {
        LS3ReferenceTreeItem* c = itemAuthorFilters->child(i);
        if (!sl.contains(c->filter())) {
            beginRemoveRows(index(rootItem->childIndex(itemAuthorFilters), 0), i, i);
            itemAuthorFilters->removeChild(i);
            endRemoveRows();
        } else {
            sl.removeAll(c->filter());
        }
    }

    // now sl contains only those topics that have to be added, so we
    // 2. add nodes for them
    for (int i=0; i<sl.size(); i++) {
        int c=0;
        while (c<itemAuthorFilters->childCount() && itemAuthorFilters->child(c)->filter()<=sl[i]) {
            c++;
        }
        if (c<0) c=0;
        if (c>itemAuthorFilters->childCount()) c=itemAuthorFilters->childCount();

        beginInsertRows(index(rootItem->childIndex(itemAuthorFilters), 0), c, c);
        LS3ReferenceTreeItem* it=new LS3ReferenceTreeItem(sl[i], "", sl[i], itemTypeFilters, m_datastore, LS3ReferenceTreeItem::FolderFilterName);
        itemAuthorFilters->appendChild(it);
        itemAuthorFilters->moveChild(itemAuthorFilters->childCount()-1, c);
        endInsertRows();
    }
}
