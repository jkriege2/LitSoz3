#ifndef RENAMEFILESDIALOG_H
#define RENAMEFILESDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
    class RenameFilesDialog;
}

class RenameFilesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RenameFilesDialog(QWidget *parent = 0);
    ~RenameFilesDialog();

    void readSettings(QSettings* settings, QString prefix=QString("RenameFilesDialog/"));
    void writeSettings(QSettings* settings, QString prefix=QString("RenameFilesDialog/"));

    enum WhichDatasets { All, Selected, Current };

    WhichDatasets getWhich();

    bool renameFiles();
    bool moveFiles();
    bool deleteOldFile();
    QString renameScheme();
    QString directoryScheme();
    bool simulate();
    void setCurrentRecordOnly(bool currentOnly=true);
    int maxLength() const;

private:
    Ui::RenameFilesDialog *ui;
    bool m_currentRecordOnly;
};

#endif // RENAMEFILESDIALOG_H
