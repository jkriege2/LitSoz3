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

#ifndef LS3SUMMARYSORTFILTERPROXYMODEL_H
#define LS3SUMMARYSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QRegularExpression>

#include "ls3referencetreeitem.h"
#include "ls3datastore.h"

/*! \brief proxy that filters and sorts the contents of LS3SummaryProxyModel s

*/
class LS3SummarySortFilterProxyModel : public QSortFilterProxyModel {
        Q_OBJECT
    public:
        /** Default constructor */
        LS3SummarySortFilterProxyModel(QObject* parent=NULL);
        /** Default destructor */
        virtual ~LS3SummarySortFilterProxyModel();

        /** \brief return a list of all displayed records' UUID in the order of display */
        QStringList getUUIDs();

        void setFilterItem(LS3ReferenceTreeItem* filterItem);

        void setFilterTerm(const QString& m_term, bool m_regexp, const QString& m_fields);

        void setDatastore(LS3Datastore* ds);

    protected:
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
        virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

        QPointer<LS3ReferenceTreeItem> m_filterItem;

        QString m_term;
        bool m_regexp;
        QStringList m_fields;
        QRegularExpression m_rxTerm;
        QPointer<LS3Datastore> m_datastore;
};

#endif // LS3SUMMARYSORTFILTERPROXYMODEL_H
