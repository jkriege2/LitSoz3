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

#ifndef ImportDialog_H
#define ImportDialog_H

#include <QDialog>
#include "ui_importdialog.h"
#include "ls3pluginservices.h"
#include "ls3importexport.h"

/*! \brief allows to select an import filter for file import

*/
class ImportDialog : public QDialog, private Ui::ImportDialog {
        Q_OBJECT
    public:
        ImportDialog(QWidget* parent=NULL);
        virtual ~ImportDialog();

        void init(LS3PluginServices* services);
        LS3Importer* getImporter();

    protected:
        void on_cmbFormat_currentIndexChanged(int idx);
    private:
        LS3PluginServices* m_services;
};

#endif // ImportDialog_H
