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

#include "ls3selectiondisplaymodel.h"
#include "bibtools.h"
#include <QtAlgorithms>
#include <QTextEdit>
#include <QVariant>
#include <QTextDocument>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>

#include <iostream>

LS3SelectionDisplayModel::LS3SelectionDisplayModel(LS3Datastore* datastore, QObject* parent):
    QAbstractListModel(parent)
{
    m_datastore=datastore;
    loadData();
    connect(datastore, SIGNAL(selectionChanged(int, bool)), this, SLOT(resetModel()));
    connect(datastore, SIGNAL(massiveSelectionChange()), this, SLOT(resetModel()));
}

LS3SelectionDisplayModel::~LS3SelectionDisplayModel()
{
    //dtor
}

void LS3SelectionDisplayModel::resetModel() {
    loadData();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
beginResetModel();
endResetModel();
#else
reset();
#endif
}

LS3Datastore* LS3SelectionDisplayModel::datastore() const {
    return m_datastore;
}

int	LS3SelectionDisplayModel::rowCount(const QModelIndex & parent) const {
    return datalist.size();
}

QVariant LS3SelectionDisplayModel::data(const QModelIndex & index, int role) const {
    int section=index.column();
    if (role==Qt::DisplayRole) {
        if (section==0) return datalist[index.row()].reference;
        if (section==1) return datalist[index.row()].uuid;
    }
    return QVariant();
}

int	LS3SelectionDisplayModel::columnCount(const QModelIndex & parent) const {
    return 2;
}

QVariant LS3SelectionDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role==Qt::DisplayRole) {
        if (section==0) return tr("Reference");
        if (section==1) return tr("UUID");
    }
    return QVariant();
}


void LS3SelectionDisplayModel::setSortingOrder(int order) {
    m_sortingOrder=order;
    loadData();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
beginResetModel();
endResetModel();
#else
reset();
#endif
}

void LS3SelectionDisplayModel::loadData() {
    datalist.clear();
    for (int i=0; i<m_datastore->selectionCount(); i++) {
        ListItem it;
        it.uuid=m_datastore->getSelectedUUID(i);
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
        datalist.append(it);
    }
    qSort(datalist);
}

bool LS3SelectionDisplayModel::ListItem::operator<(const ListItem& b) const {
    if (sort1==b.sort1) {
        if (sort2==b.sort2) {
            return sort3<b.sort3;
        } else return sort2<b.sort2;
    } else return sort1<b.sort1;
}

QString LS3SelectionDisplayModel::getUUID(const QModelIndex & index) const {
    if ((index.row()<0)||(index.row()>=datalist.size())) return "";
    return datalist[index.row()].uuid;
}

Qt::DropActions LS3SelectionDisplayModel::supportedDropActions() const {
    return Qt::CopyAction|Qt::MoveAction;
}

QStringList LS3SelectionDisplayModel::mimeTypes() const {
    QStringList types;
    types << "litsoz3/list.uuid";
    return types;
}

QMimeData* LS3SelectionDisplayModel::mimeData(const QModelIndexList &indexes) const {
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

bool LS3SelectionDisplayModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    //std::cout<<"LS3SelectionDisplayModel::dropMimeData\n";
    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat("litsoz3/list.uuid"))
        return false;

    if (column > 0)
        return false;
    QByteArray encodedData = data->data("litsoz3/list.uuid");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QStringList newItems;
    int rows = 0;
    while (!stream.atEnd()) {
        QString uuid;
        stream >> uuid;
        newItems << uuid;
        ++rows;
    }
    if (newItems.size()>0) m_datastore->select(newItems);
    return newItems.size();
}

Qt::ItemFlags LS3SelectionDisplayModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}






LS3SelectionDisplayModelDelegate::LS3SelectionDisplayModelDelegate(LS3Datastore* datastore, QObject *parent):
    QStyledItemDelegate(parent)
{
    this->datastore=datastore;
    selDeleteImage=QPixmap(":/sel_delete_small.png");
}

LS3SelectionDisplayModelDelegate::~LS3SelectionDisplayModelDelegate() {
    //dtor
}


void LS3SelectionDisplayModelDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QVariant data=index.data(Qt::DisplayRole).toString();
    if (data.type()==QVariant::String) {
        //drawBackground(painter, option, index);
        QString pre, post;
        pre="<font color=\""+option.palette.text().color().name()+"\">";
        post="</font>";
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
            pre="<font color=\""+option.palette.highlightedText().color().name()+"\">";
            post="</font>";
        }

        QTextDocument doc;
        //std::cout<<"drawing "<<QString(pre+data.toString()+post).toStdString()<<std::endl;
        doc.setHtml(pre+data.toString()+post);
        painter->save();

        painter->translate(option.rect.topLeft());
        QRect r(QPoint(0, 0), option.rect.size());

        doc.drawContents(painter, r);

        painter->restore();
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }

    if (index.column()==0) {
        painter->drawPixmap(option.rect.topRight()+QPoint(-selDeleteImage.width(), (double)(option.rect.height()-selDeleteImage.height())/2.0), selDeleteImage);
    }
}

QSize LS3SelectionDisplayModelDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QSize s=QStyledItemDelegate::sizeHint(option, index);
    QVariant data=index.data(Qt::DisplayRole).toString();
    if (data.type()==QVariant::String) {
        QTextDocument doc;
        doc.setHtml(data.toString());
        s.setHeight(doc.size().toSize().height());
        s.setWidth(selDeleteImage.width()*2);
    }
    return s;
}

QWidget *LS3SelectionDisplayModelDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    return NULL;
}

bool LS3SelectionDisplayModelDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
     if (index.column() == 0) {
         if (event->type() == QEvent::MouseButtonPress) {
             QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

             QPoint pixtl=option.rect.topRight()+QPoint(-selDeleteImage.width(), (double)(option.rect.height()-selDeleteImage.height())/2.0);
             QRect r(pixtl, selDeleteImage.size());
             if (r.contains(mouseEvent->pos())) {
                 QString uuid=model->data(model->index(index.row(), 1)).toString();
                 datastore->deselect(uuid);
             }
             return false; //so that the selection can change
         } else if (event->type() == QEvent::KeyPress) {
             QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
             if (keyEvent->matches(QKeySequence::Delete)) {
                 QString uuid=model->data(model->index(index.row(), 1)).toString();
                 datastore->deselect(uuid);
             }
             return false; //so that the selection can change
         }
     }
     return QStyledItemDelegate::editorEvent(event, model, option, index);
}
