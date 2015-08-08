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

#include "ls3summarysortfilterproxymodel.h"
#include "ls3summaryproxymodel.h"
#include "ls3datastore.h"
#include <QDebug>

LS3SummarySortFilterProxyModel::LS3SummarySortFilterProxyModel(QObject* parent):
    QSortFilterProxyModel(parent)
{
    m_filterItem=NULL;
    m_term="";
    m_regexp=false;
    m_fields.clear();
    setDynamicSortFilter(true);
}

LS3SummarySortFilterProxyModel::~LS3SummarySortFilterProxyModel()
{
    //dtor
}

bool LS3SummarySortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    //qDebug()<<"filterAcceptsRow("<<sourceRow<<"):  m_filterItem="<<m_filterItem;
    bool accept=true;
    if (m_filterItem) accept=m_filterItem->containsReference(sourceModel()->data(sourceModel()->index(sourceRow, LS3SUMMARYPROXYMODEL_COLUMN_UUID, sourceParent)).toString());
    if (accept && m_datastore && !m_term.isEmpty() && m_fields.size()>0) {
        accept=false;
        //qDebug()<<"QF: "<<m_term;
        for (int i=0; i<m_fields.size(); i++) {
            int col=m_datastore->fieldColumn(m_fields[i]);
            //qDebug()<<"QF:   "<<m_fields[i]<<"("<<col<<") ";
            if (col>=0) {
                const QString data=m_datastore->getField(sourceRow, m_fields[i]).toString();
                if (data.contains(m_rxTerm)) {
                    accept=true;
                    break;
                } else {
                    //qDebug()<<"didn't find "<<m_term<<" in "<<m_fields[i]<<"("<<col<<data<<") ";
                }
            }
        }
    }
    return accept;
}

bool LS3SummarySortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    return QSortFilterProxyModel::lessThan(left, right);
}

QStringList LS3SummarySortFilterProxyModel::getUUIDs() {
    QStringList sl;
    for (int i=0; i<rowCount(); i++) {
        sl.append(data(index(i, LS3SUMMARYPROXYMODEL_COLUMN_UUID)).toString());
    }
    return sl;
}

void LS3SummarySortFilterProxyModel::setFilterItem(LS3ReferenceTreeItem* filterItem) {
    m_filterItem=filterItem;
    invalidateFilter();
}

void LS3SummarySortFilterProxyModel::setFilterTerm(const QString &term, bool regexp, const QString &fields)
{
    //qDebug()<<"QF_set: "<<term<<regexp<<fields;
    this->m_term=term;
    this->m_regexp=regexp;
    this->m_fields=fields.split(";");
    if (regexp) {
        m_rxTerm=QRegExp(term, Qt::CaseInsensitive, QRegExp::RegExp);
        m_rxTerm.setMinimal(false);
    } else {
        m_rxTerm=QRegExp(term, Qt::CaseInsensitive, QRegExp::WildcardUnix);
        m_rxTerm.setMinimal(false);
    }
    invalidateFilter();

}

void LS3SummarySortFilterProxyModel::setDatastore(LS3Datastore *ds)
{
    m_datastore=ds;
}
