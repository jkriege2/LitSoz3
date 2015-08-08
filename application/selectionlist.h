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

#ifndef SELECTIONLIST_H
#define SELECTIONLIST_H

#include <QDockWidget>
#include "ui_selectionlist.h"
#include "ls3datastore.h"
#include "ls3selectiondisplaymodel.h"
#include "programoptions.h"

class SelectionList : public QDockWidget, private Ui::SelectionList {
        Q_OBJECT
    public:
        SelectionList(LS3Datastore* datastore, ProgramOptions* settings, QWidget* parent=0);
        virtual ~SelectionList();
    signals:
        void itemSelected(QString uuid);
    private slots:
        void on_btnClear_clicked();
        void on_btnLoad_clicked();
        void on_btnSave_clicked();
        void on_lstSelection_clicked(const QModelIndex& index);
        void selectionChanged(int, bool);
        void storeSettings();
    private:
        LS3Datastore* m_datastore;
        ProgramOptions* m_settings;
        LS3SelectionDisplayModel* model;
};

#endif // SELECTIONLIST_H
