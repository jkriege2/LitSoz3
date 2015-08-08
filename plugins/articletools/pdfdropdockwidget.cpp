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

#include "pdfdropdockwidget.h"
#include "ui_pdfdropdockwidget.h"
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif

PDFDropDockWidget::PDFDropDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::PDFDropDockWidget)
{
    ui->setupUi(this);
    setAcceptDrops(true);
}

PDFDropDockWidget::~PDFDropDockWidget()
{
    delete ui;
}

void PDFDropDockWidget::dropEvent(QDropEvent* event) {
    const QMimeData* mimeData = event->mimeData();

    // check for our needed mime type, here a file or a list of files
    if (mimeData->hasUrls())
    {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();

        // extract the local paths of the files
        for (int i = 0; i < urlList.size(); ++i) {
            QString filename=urlList.at(i).toLocalFile();
            if (QFile::exists(filename) && (QFileInfo(filename).suffix().toLower()=="pdf")) {
                pathList.append(urlList.at(i).toLocalFile());
            }
        }

        if (pathList.size()>0) {
            emit pdfsDropped(pathList, currentType(), currentAction());
        }
    }
}

void PDFDropDockWidget::on_cmbType_currentIndexChanged(int type) {
    emit typeChanged(currentType());
}
void PDFDropDockWidget::on_cmbFileAction_currentIndexChanged(int type) {
    emit actionChanged(type);
}

void PDFDropDockWidget::setCurrentType(QString type) {
    ui->cmbType->setCurrentIndex(ui->cmbType->findData(type));
}

QString PDFDropDockWidget::currentType() {
    return ui->cmbType->itemData(ui->cmbType->currentIndex()).toString();
}

void PDFDropDockWidget::setTypes(const QStringList& names, const QStringList& ids) {
    ui->cmbType->clear();
    for (int i=0; i<ids.size(); i++) {
        ui->cmbType->addItem(names[i], ids[i]);
    }
}

void PDFDropDockWidget::setCurrentAction(int act) {
    ui->cmbFileAction->setCurrentIndex(act);
}

int PDFDropDockWidget::currentAction() {
    return ui->cmbFileAction->currentIndex();
}


void PDFDropDockWidget::dragEnterEvent(QDragEnterEvent* event) {
    const QMimeData* mimeData = event->mimeData();
    // if some actions should not be usable, like move, this code must be adopted
    if (mimeData->hasUrls()) event->acceptProposedAction();
}

void PDFDropDockWidget::dragMoveEvent(QDragMoveEvent* event) {
    const QMimeData* mimeData = event->mimeData();
    // if some actions should not be usable, like move, this code must be adopted
    if (mimeData->hasUrls())event->acceptProposedAction();
}
