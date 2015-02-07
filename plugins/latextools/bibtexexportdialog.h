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
