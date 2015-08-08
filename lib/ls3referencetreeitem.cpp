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

#include "ls3referencetreeitem.h"
#include <QObject>
#include <QDate>
#include <QDebug>
#include "ls3datastore.h"

LS3ReferenceTreeItem::LS3ReferenceTreeItem(QString uuid, QString name, LS3ReferenceTreeItem* parent, LS3ReferenceTreeItem::Type type, LS3Datastore* datastore):
    QObject(parent)
{
    m_parent=parent;
    m_type=type;
    m_datastore=datastore;
    if ((m_datastore==NULL) && parent) m_datastore=parent->m_datastore;
    m_uuid=uuid;
    if (m_uuid.isEmpty()) m_uuid=QUuid::createUuid().toString();
    m_name=name;
    m_filterField="";
    m_filter="";
    m_days=3;
    if (type==LS3ReferenceTreeItem::FolderUser) {
        if (m_name.isEmpty()) m_name=QObject::tr("New Folder");
    }
}

LS3ReferenceTreeItem::LS3ReferenceTreeItem(QString name, QString filterField, QString filter, LS3ReferenceTreeItem* parent, LS3Datastore* datastore, LS3ReferenceTreeItem::Type type):
    QObject(parent)
{
    m_parent=parent;
    m_datastore=datastore;
    if ((m_datastore==NULL) && parent) m_datastore=parent->m_datastore;
    m_uuid=QUuid::createUuid().toString();
    m_name=name;
    m_filterField=filterField;
    m_filter=filter;
    m_type=type;
}

LS3ReferenceTreeItem::LS3ReferenceTreeItem(QString name, LS3ReferenceTreeItem* parent, LS3ReferenceTreeItem::Type type, LS3Datastore* datastore) {
    m_parent=parent;
    m_type=type;
    m_datastore=datastore;
    if ((m_datastore==NULL) && parent) m_datastore=parent->m_datastore;
    m_name=name;
    m_uuid=QUuid::createUuid().toString();
    m_filterField="";
    m_filter="";
    if (type==LS3ReferenceTreeItem::FolderUser) {
        if (m_name.isEmpty()) m_name=QObject::tr("New Folder");
    }
}

void LS3ReferenceTreeItem::insertChild(QDomElement n) {
    LS3ReferenceTreeItem* f=appendFolder(n.attribute("name"));
    f->m_uuid=n.attribute("uuid");
    QDomElement c=n.firstChildElement("reference");
    while (!c.isNull()) {
        f->m_references.insert(c.attribute("uuid"));
        c=c.nextSiblingElement("reference");
    }
    c=n.firstChildElement("folder");
    while (!c.isNull()) {
        f->insertChild(c);
        c=c.nextSiblingElement("folder");
    }
}

void LS3ReferenceTreeItem::insertChildren(QDomDocument doc) {
    insertChildren(doc.documentElement());
}

void LS3ReferenceTreeItem::insertChildren(QDomNode doc) {
    QDomElement e=doc.toElement();
    if (e.tagName()=="litsoz3_reference_tree") {
        QDomElement c=e.firstChildElement("reference");
        while (!c.isNull()) {
            m_references.insert(c.attribute("uuid"));
            //qDebug()<<c.attribute("uuid");
            c=c.nextSiblingElement("reference");
        }
        c=e.firstChildElement("folder");
        while (!c.isNull()) {
            insertChild(c.toElement());
            c=c.nextSiblingElement("folder");
        }
    }

}

void LS3ReferenceTreeItem::writeToXML(QXmlStreamWriter& writer) {
    if (m_type==FolderUser) {
        writer.writeStartElement("folder");
        writer.writeAttribute("name", m_name);
        writer.writeAttribute("uuid", m_uuid);
    }
    foreach (const QString &ruuid, m_references) {
         writer.writeStartElement("reference");
         writer.writeAttribute("uuid", ruuid);
         writer.writeEndElement();
    }
    for (int i=0; i<m_childItems.size(); i++) {
        m_childItems[i]->writeToXML(writer);
    }
    if (m_type==FolderUser) writer.writeEndElement();
}

LS3Datastore *LS3ReferenceTreeItem::getDatastore() const
{
    return m_datastore;
}

QString LS3ReferenceTreeItem::toXml() {
    QString s="";
    QXmlStreamWriter writer(&s);
    writer.writeStartDocument();
    writer.writeStartElement("litsoz3_reference_tree");
    writeToXML(writer);
    writer.writeEndElement();
    writer.writeEndDocument();
    return s;
}

LS3ReferenceTreeItem::~LS3ReferenceTreeItem() {
    //qDeleteAll(m_childItems);
    for (int i=0; i<m_childItems.count(); i++) {
        if (m_childItems[i]) delete m_childItems[i];
    }
    m_childItems.clear();
}


void LS3ReferenceTreeItem::setParent(LS3ReferenceTreeItem* parent) {
    QObject::setParent(parent);
    m_parent=parent;
    if ((m_datastore==NULL) && parent) m_datastore=parent->m_datastore;
}

void LS3ReferenceTreeItem::appendChild(LS3ReferenceTreeItem *child) {
    if (!child) return;
    m_childItems.append(child);
    child->setParent(this);
}

int LS3ReferenceTreeItem::childIndex(LS3ReferenceTreeItem *child) {
    if (!child) return -1;
    return m_childItems.indexOf(child);
}

LS3ReferenceTreeItem* LS3ReferenceTreeItem::appendFolder(QString name) {
    LS3ReferenceTreeItem* child=new LS3ReferenceTreeItem("", name, this, LS3ReferenceTreeItem::FolderUser, m_datastore);
    appendChild(child);
    return child;
}

void LS3ReferenceTreeItem::moveChild(int from, int to) {
    if ((from>=0) && (from<m_childItems.size())) {

        if ( (to>=0) && (to<m_childItems.size())) m_childItems.move(from, to);
        else if (to<0) m_childItems.prepend(m_childItems.takeAt(from));
        else if (to>=m_childItems.size()) m_childItems.append(m_childItems.takeAt(from));
    }
}

void LS3ReferenceTreeItem::moveChild(LS3ReferenceTreeItem* from, LS3ReferenceTreeItem* to) {
    moveChild(m_childItems.indexOf(from), m_childItems.indexOf(to));
}

void LS3ReferenceTreeItem::moveDistantHere(LS3ReferenceTreeItem* from, LS3ReferenceTreeItem* to) {
    if (from->parent()) from->parent()->m_childItems.removeAll(from);
    from->setParent(this);
    if (to==NULL) {
        m_childItems.append(from);
    } else {
        m_childItems.insert(m_childItems.indexOf(to), from);
    }
}

void LS3ReferenceTreeItem::appendReference(QString uuid) {
    if (!m_references.contains(uuid)) m_references.insert(uuid);
}

void LS3ReferenceTreeItem::removeReference(QString uuid) {
    m_references.remove(uuid);
    for (int i=0; i<m_childItems.size(); i++) {
        if (m_childItems[i]) m_childItems[i]->removeReference(uuid);
    }
}

bool LS3ReferenceTreeItem::containsReferenceLocal(QString uuid) const {
    return m_references.contains(uuid);
}

bool LS3ReferenceTreeItem::containsReference(QString uuid) const {
    if (!m_datastore) return true;
    if (m_type==LS3ReferenceTreeItem::FolderUser) {
        bool c=containsReferenceLocal(uuid);
        if ((!c) && (m_childItems.size()>0)) {
            for (int i=0; i<m_childItems.size(); i++) {
                if (m_childItems[i] && m_childItems[i]->containsReference(uuid)) return true;
            }
        }
        return c;
    } else if (m_type==LS3ReferenceTreeItem::FolderAllItems) {
        return true;
    } else if (m_type==LS3ReferenceTreeItem::FolderFilterExact) {
        return m_datastore->getField(m_datastore->getRecordByUUID(uuid), m_filterField).toString()==m_filter;
    } else if (m_type==LS3ReferenceTreeItem::FolderFilterContains) {
        return m_datastore->getField(m_datastore->getRecordByUUID(uuid), m_filterField).toString().contains(m_filter);
    } else if (m_type==LS3ReferenceTreeItem::FolderFilterStartsWith) {
        return m_datastore->getField(m_datastore->getRecordByUUID(uuid), m_filterField).toString().startsWith(m_filter);
    } else if (m_type==LS3ReferenceTreeItem::FolderFilterName) {
        return m_datastore->getField(m_datastore->getRecordByUUID(uuid), "authors").toString().contains(m_filter) || m_datastore->getField(m_datastore->getRecordByUUID(uuid), "editors").toString().contains(m_filter) ;
    } else if (m_type==LS3ReferenceTreeItem::FolderNewItems) {
        return abs(m_datastore->getField(m_datastore->getRecordByUUID(uuid), "addeddate").toDate().daysTo(QDate::currentDate()))<=m_days;
    }

    return false;
}

void LS3ReferenceTreeItem::removeChild(LS3ReferenceTreeItem *child) {
    m_childItems.removeAll(child);
    delete child;
}

void LS3ReferenceTreeItem::removeChild(int child) {
    if ((child>=0) && (child<m_childItems.size())) {
        if (m_childItems[child]) delete m_childItems[child];
        m_childItems.removeAt(child);
    }
}

LS3ReferenceTreeItem *LS3ReferenceTreeItem::child(int row) {
    if ((row>=0) && (row<m_childItems.size())) return m_childItems[row];
    return NULL;
}

int LS3ReferenceTreeItem::childCount() const {
    return m_childItems.size();
}

int LS3ReferenceTreeItem::referenceCount() const {
    QSet<QString> r=m_references;
    for (int i=0; i<m_childItems.size(); i++) {
        if (m_childItems[i]) r.unite(m_childItems[i]->references());
    }
    return r.size();
}

QString LS3ReferenceTreeItem::name() const {
    if ((m_type==LS3ReferenceTreeItem::FolderAllItems) && (m_name.isEmpty())) return QObject::tr("All References");
    if ((m_type==LS3ReferenceTreeItem::FolderNewItems) && (m_name.isEmpty())) {
        if (m_days<7) return QObject::tr("Recently Added References (%n day(s))", "", m_days);
        else if (m_days<30) return QObject::tr("Recently Added References (%n week(s))", "", ceil(double(m_days)/7.0));
        else return QObject::tr("Recently Added References (%n month(s))", "", ceil(double(m_days)/30.0));
    }
    return m_name;
}

QString LS3ReferenceTreeItem::uuid() const {
    return m_uuid;
}

QIcon LS3ReferenceTreeItem::icon() const {
    if (m_type==LS3ReferenceTreeItem::FolderUserBase) return QIcon(":/ls3lib/reftree_folder.png");
    else if (m_type==LS3ReferenceTreeItem::FolderUser) return QIcon(":/ls3lib/reftree_folder.png");
    else if (m_type==LS3ReferenceTreeItem::FolderAllItems) return QIcon(":/ls3lib/reftree_folderall.png");
    else if (m_type==LS3ReferenceTreeItem::FolderNewItems) return QIcon(":/ls3lib/reftree_folderrecent.png");
    else if (m_type==LS3ReferenceTreeItem::FolderBasic) return QIcon(":/ls3lib/reftree_folder.png");
    else if (m_type==LS3ReferenceTreeItem::FolderBasicFilter) return QIcon(":/ls3lib/reftree_folderfilter.png");
    else if (m_type==LS3ReferenceTreeItem::FolderFilterExact) return QIcon(":/ls3lib/reftree_folderfilter.png");
    else if (m_type==LS3ReferenceTreeItem::FolderFilterContains) return QIcon(":/ls3lib/reftree_folderfilter.png");
    else if (m_type==LS3ReferenceTreeItem::FolderFilterStartsWith) return QIcon(":/ls3lib/reftree_folderfilter.png");
    else if (m_type==LS3ReferenceTreeItem::FolderFilterName) return QIcon(":/ls3lib/reftree_folderfilter.png");
    return QIcon();
}

LS3ReferenceTreeItem::Type LS3ReferenceTreeItem::type() const {
    return m_type;
}

QString LS3ReferenceTreeItem::filterField() const {
    return m_filterField;
}

QString LS3ReferenceTreeItem::filter() const {
    return m_filter;
}

void LS3ReferenceTreeItem::setFilter(QString filterField, QString filter) {
    m_filterField=filterField;
    m_filter=filter;
}

void LS3ReferenceTreeItem::setFilter(QString filter) {
    m_filter=filter;
}

void LS3ReferenceTreeItem::setFilterField(QString filterField) {
    m_filterField=filterField;
}


int LS3ReferenceTreeItem::row() const {
    if (m_parent)
        return m_parent->m_childItems.indexOf(const_cast<LS3ReferenceTreeItem*>(this));

    return 0;
}

LS3ReferenceTreeItem *LS3ReferenceTreeItem::parent() {
    return m_parent;
}

int LS3ReferenceTreeItem::getFilterAgeDays() const
{
    return m_days;
}

void LS3ReferenceTreeItem::setFilterAgeDays(int days)
{
    m_days=days;
}

void LS3ReferenceTreeItem::setName(QString newName) {
    if (m_type!=LS3ReferenceTreeItem::FolderUserBase) m_name=newName;
}
void LS3ReferenceTreeItem::setUUID(QString newUuid) {
    m_uuid=newUuid;
}

bool LS3ReferenceTreeItem::isUserEditable() const {
    if (m_type==LS3ReferenceTreeItem::FolderUser) return true;
    return false;
}

LS3ReferenceTreeItem* LS3ReferenceTreeItem::findUUID(QString uuid) {
    if (m_uuid==uuid) return this;
    for (int i=0; i<m_childItems.size(); i++) {
        LS3ReferenceTreeItem* u=m_childItems[i]->child(i)->findUUID(uuid);
        if (u) return u;
    }
    return NULL;
}

QSet<QString> LS3ReferenceTreeItem::references() const {
    return m_references;
}
