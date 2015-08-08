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

#include "ls3progressthreadwidget.h"
#include "ui_ls3progressthreadwidget.h"

LS3ProgressThreadWidget::LS3ProgressThreadWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LS3ProgressThreadWidget)
{
    ui->setupUi(this);
    ui->progress->setSpin(true);
    ui->progress->setPercentageMode(QModernProgressWidget::Percent);
}

LS3ProgressThreadWidget::~LS3ProgressThreadWidget()
{
    delete ui;
}

void LS3ProgressThreadWidget::init(LS3ProgressThread *thread) {
    m_thread=thread;
    connect(thread, SIGNAL(finished()), this, SLOT(threadFinished()));
    connect(thread, SIGNAL(errorOccured(QString)), this, SLOT(errorOccured(QString)));
    connect(thread, SIGNAL(messageChanged(QString)), ui->labMessage, SLOT(setText(QString)));
    connect(thread, SIGNAL(nameChanged(QString)), ui->labName, SLOT(setText(QString)));
    connect(thread, SIGNAL(progressChanged(double)), ui->progress, SLOT(setValue(double)));
    connect(thread, SIGNAL(rangeChanged(double,double)), ui->progress, SLOT(setRange(double,double)));
    connect(ui->btnCancel, SIGNAL(clicked()), thread, SLOT(cancelThread()));
}

void LS3ProgressThreadWidget::threadFinished() {
    if (!m_thread->wasError()) {
        disconnect(this);
        close();
        deleteLater();
    } else {
        disconnect(m_thread, SIGNAL(finished()), this, SLOT(threadFinished()));
        connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(closeAndDelete()));
    }
}

void LS3ProgressThreadWidget::errorOccured(const QString &message) {
    ui->labMessage->setText(tr("<font color=\"red\"><b>Error:</b>&nbsp;&nbsp;%1</font>").arg(message));
    ui->progress->setSpin(false);
    ui->progress->setVisible(false);
}

void LS3ProgressThreadWidget::closeAndDelete() {
    close();
    deleteLater();
}

void LS3ProgressThreadWidget::setValue(double value) {
    ui->progress->setDisplayPercent(value>ui->progress->minimum());
    ui->progress->setValue(value);
}
