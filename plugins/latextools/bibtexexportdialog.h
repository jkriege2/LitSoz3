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

#ifndef BIBTEXEXPORTDIALOG_H
#define BIBTEXEXPORTDIALOG_H

#include <QDialog>
#include "ls3pluginservices.h"

namespace Ui {
    class BibTeXExportDialog;
}

class BibTeXExportDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit BibTeXExportDialog(LS3PluginServices* services, QWidget *parent = 0);
        ~BibTeXExportDialog();

        bool getExportAbstract() const;
        bool getExportPDF() const;
        int getExportURL() const;
        int getExportStyle() const;

    private:
        Ui::BibTeXExportDialog *ui;
        LS3PluginServices* m_services;
};

#endif // BIBTEXEXPORTDIALOG_H
