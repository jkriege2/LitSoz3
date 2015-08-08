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
