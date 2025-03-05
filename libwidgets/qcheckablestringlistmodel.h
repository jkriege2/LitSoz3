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

#ifndef QCHECKABLESTRINGLISTMODEL_H
#define QCHECKABLESTRINGLISTMODEL_H

#include <QStringListModel>
#include <QSet>
#include "litsoz3widgets_export.h"

class LITSOZ3WIDGETS_EXPORT QCheckableStringListModel : public QStringListModel {
    Q_OBJECT
public:
    explicit QCheckableStringListModel(QObject *parent = 0);

    void setChecked(int i);
    void setChecked(int i, bool c);
    void setUnChecked(int i);
    bool isChecked(int i) const;
    void unCheckAll();
    void checkAll();

    void setEditable(bool editable);
    bool isEditable() const { return m_editable; }

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;


protected:
    QSet<int> m_check;
    bool m_editable;
};

#endif // QCHECKABLESTRINGLISTMODEL_H
