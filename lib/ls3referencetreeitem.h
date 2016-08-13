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

#ifndef LS3REFERENCETREEITEM_H
#define LS3REFERENCETREEITEM_H
#include <QObject>
#include <QString>
#include <QIcon>
#include <QList>
#include <QUuid>
#include <QSet>
#include <QPointer>
#include <QtXml>
#include "lib_imexport.h"

class LS3Datastore; // forward

/*! \brief class for LitSoz 3 reference trees which contains the node data

    This class stores the structure of a tree where references may be stored in folders.

*/
class LS3LIB_EXPORT LS3ReferenceTreeItem: public QObject {
        Q_OBJECT
    public:
        enum Type {
            FolderUser=1,
            FolderAllItems=2,
            FolderUserBase=3,
            FolderNewItems=4,
            FolderBasic=5,
            FolderFilterExact=6,
            FolderFilterContains=7,
            FolderFilterStartsWith=8,
            FolderFilterName=9,
            FolderBasicFilter=10
        };

        LS3ReferenceTreeItem(QString uuid, QString name, LS3ReferenceTreeItem* parent=NULL, Type type=FolderUser, LS3Datastore* datatsore=NULL);
        LS3ReferenceTreeItem(QString name, LS3ReferenceTreeItem* parent=NULL, Type type=FolderUser, LS3Datastore* datatsore=NULL);
        LS3ReferenceTreeItem(QString name, QString filterField, QString filter, LS3ReferenceTreeItem* parent, LS3Datastore* datastore=NULL, Type type=FolderFilterExact);
        ~LS3ReferenceTreeItem();

        void insertChildren(QDomDocument doc);
        void insertChildren(QDomNode doc);
        void appendChild(LS3ReferenceTreeItem *child);
        int childIndex(LS3ReferenceTreeItem *child);
        void moveChild(int from, int to);
        void moveChild(LS3ReferenceTreeItem* from, LS3ReferenceTreeItem* to);
        LS3ReferenceTreeItem* appendFolder(QString name);
        void removeChild(LS3ReferenceTreeItem *child);
        void removeChild(int child);


        bool isUserEditable() const;

        /** \brief move a tree item \a from from any position in the tree here. The item is removed from its old parent! If \c to==NULL the distant folder is appended */
        void moveDistantHere(LS3ReferenceTreeItem* from, LS3ReferenceTreeItem* to=NULL);

        void appendReference(QString uuid);
        void removeReference(QString uuid);
        /** \brief check whether a reference is contained in this subfolder */
        bool containsReferenceLocal(QString uuid) const;
        /** \brief check whether a reference is contained in this subfolder or any of its children */
        bool containsReference(QString uuid) const;

        LS3ReferenceTreeItem *child(int row);
        QSet<QString> references() const;
        int childCount() const;
        int referenceCount() const;
        QString name() const;
        QString uuid() const;
        QIcon icon() const;
        Type type() const;
        int row() const;
        LS3ReferenceTreeItem *parent();
        int getFilterAgeDays() const;
        void setFilterAgeDays(int days);

        void setName(QString newName);

        LS3ReferenceTreeItem* findUUID(QString uuid);

        QString filterField() const;
        QString filter() const;
        Qt::CaseSensitivity caseSensitivity() const;
        void setFilter(QString filterField, QString filter);
        void setFilter(QString filter);
        void setFilterField(QString filterField);
        void setCaseSecitivity(Qt::CaseSensitivity sens);

        QString toXml();
        void writeToXML(QXmlStreamWriter& writer);

        LS3Datastore* getDatastore() const;

    protected:
        QString m_name;
        QString m_uuid;
        QList<QPointer<LS3ReferenceTreeItem> > m_childItems;
        QSet<QString> m_references;
        Type m_type;
        LS3Datastore* m_datastore;
        QPointer<LS3ReferenceTreeItem> m_parent;
        QString m_filterField;
        QString m_filter;
        int m_days;
        Qt::CaseSensitivity m_caseSensitivity;



        void setUUID(QString newUuid);
        void setParent(LS3ReferenceTreeItem* parent);
        void insertChild(QDomElement n);
};

#endif // LS3REFERENCETREEITEM_H
