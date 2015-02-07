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
