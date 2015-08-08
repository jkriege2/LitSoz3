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

#ifndef PDFDROPDOCKWIDGET_H
#define PDFDROPDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
    class PDFDropDockWidget;
}

class PDFDropDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit PDFDropDockWidget(QWidget *parent = 0);
    ~PDFDropDockWidget();

    void setTypes(const QStringList& names, const QStringList& ids);
    void setCurrentType(QString type);
    QString currentType();
    void setCurrentAction(int act);
    int currentAction();
signals:
    void pdfsDropped(QStringList files, QString type, int action);
    void typeChanged(QString type);
    void actionChanged(int action);
protected slots:
    void on_cmbType_currentIndexChanged(int type);
    void on_cmbFileAction_currentIndexChanged(int type);
protected:
    virtual void dropEvent(QDropEvent* event);
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void dragMoveEvent(QDragMoveEvent* event);
private:
    Ui::PDFDropDockWidget *ui;
};

#endif // PDFDROPDOCKWIDGET_H
