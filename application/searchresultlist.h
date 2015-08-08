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

#ifndef SEARCHRESULTLIST_H
#define SEARCHRESULTLIST_H

#include <QDockWidget>
#include "ui_searchresultlist.h"
#include "ls3datastore.h"
#include "searchresultmodel.h"
#include "programoptions.h"
#include "htmldelegate.h"

class SearchResultList : public QDockWidget, private Ui::SearchResultList {
        Q_OBJECT
    public:
        SearchResultList(LS3Datastore* datastore, ProgramOptions* settings, QWidget* parent=0);
        virtual ~SearchResultList();
        SearchResultModel* searchResultModel() { return model; }
    signals:
        void itemSelected(QString uuid);
    private slots:
        void on_btnClear_clicked();
        void on_btnSelect_clicked();
        void on_btnSelectAll_clicked();
        void on_lstResults_clicked(const QModelIndex& index);
        void selectionChanged(int, bool);
        void storeSettings();
    private:
        LS3Datastore* m_datastore;
        ProgramOptions* m_settings;
        SearchResultModel* model;
};

#endif // SEARCHRESULTLIST_H
