#ifndef RENAMEFILESPROGRESSDIALOG_H
#define RENAMEFILESPROGRESSDIALOG_H

#include <QDialog>

namespace Ui {
    class RenameFilesProgressDialog;
}

class RenameFilesProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RenameFilesProgressDialog(QWidget *parent = 0);
    ~RenameFilesProgressDialog();

    void setRange(int min, int max);
    void setProgress(int value);
    void incProgress(int inc=1);
    void setMessage(const QString& message);
    void addFilesPair(const QString& oldName, const QString& newName);
    void setCurrentStatus(const QString& status, bool isOK=false);
    bool cancelPressed() const { return m_cancel; }
    void setReportMode();
protected slots:
    void cancelClicked() { m_cancel=true; }
private:
    Ui::RenameFilesProgressDialog *ui;
    bool m_cancel;
};

#endif // RENAMEFILESPROGRESSDIALOG_H
