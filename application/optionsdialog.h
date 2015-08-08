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

#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "ui_optionsdialog.h"


class OptionsDialog : public QDialog, public Ui::OptionsDlg {
        Q_OBJECT
    public:
        /** Default constructor */
        OptionsDialog(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~OptionsDialog();
    public slots:
        void ensureCurrentStyle();
    protected slots:
        void on_tbutConfigDir_clicked(bool checked);
        void on_tbutInitialFile_clicked(bool checked);
        void on_cmbStylesheet_currentIndexChanged( const QString & text );
        void on_cmbStylesheet_highlighted( const QString & text );
        void on_cmbStyle_currentIndexChanged( const QString & text );
        void on_cmbStyle_highlighted( const QString & text );
        void on_btnDefaultNP_clicked();
        void on_btnDefaultNA_clicked();
        void on_btnDefaultAND_clicked();
        void on_btnDefaultSC_clicked();
    private:
};


#endif // OPTIONSDIALOG_H
