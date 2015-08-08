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
