#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include "ui_exportdialog.h"
#include "ls3pluginservices.h"
#include "ls3importexport.h"


class ExportDialog : public QDialog, private Ui::ExportDialog {
        Q_OBJECT
    public:
        ExportDialog(QWidget* parent=NULL);
        virtual ~ExportDialog();

        void init(LS3PluginServices* services);
        LS3Exporter* getExporter();

        enum WhichDatasets { All, Selected, Current };

        WhichDatasets getWhich();
    protected:
        void on_cmbFormat_currentIndexChanged(int idx);
    private:
        LS3PluginServices* m_services;
};

#endif // EXPORTDIALOG_H
