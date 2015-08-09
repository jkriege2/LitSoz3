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

#include "qcheckablestringlistmodel.h"
#include <QDebug>
QCheckableStringListModel::QCheckableStringListModel(QObject *parent) :
    QStringListModel(parent)
{
    m_check.clear();
    m_editable=false;
}

void QCheckableStringListModel::setChecked(int i) {
    m_check.insert(i);
}

void QCheckableStringListModel::setChecked(int i, bool c) {
    if (c) m_check.insert(i);
    else m_check.remove(i);
}

void QCheckableStringListModel::setUnChecked(int i) {
    m_check.remove(i);
}

bool QCheckableStringListModel::isChecked(int i) const {
    //qDebug()<<i<<m_check;
    return m_check.contains(i);
}

void QCheckableStringListModel::unCheckAll() {
    m_check.clear();
}

void QCheckableStringListModel::checkAll() {
    m_check.clear();
    for (int i=0; i<stringList().size(); i++) {
        m_check.insert(i);
    }
}

QVariant QCheckableStringListModel::data(const QModelIndex &index, int role) const {
    if (role==Qt::CheckStateRole) {
        if (isChecked(index.row())) return QVariant(Qt::Checked);
        else return QVariant(Qt::Unchecked);
    } else {
        return QStringListModel::data(index, role);
    }
}

bool QCheckableStringListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role==Qt::CheckStateRole) {
        setChecked(index.row(), value.toBool());
        return true;
    } else {
        return QStringListModel::setData(index, value, role);
    }
}

Qt::ItemFlags QCheckableStringListModel::flags(const QModelIndex &/*index*/) const {
    if (m_editable) return Qt::ItemIsUserCheckable|Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable;
    else return Qt::ItemIsUserCheckable|Qt::ItemIsSelectable|Qt::ItemIsEnabled;
}

void QCheckableStringListModel::setEditable(bool editable) {
    m_editable=editable;
}
