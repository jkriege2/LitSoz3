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
