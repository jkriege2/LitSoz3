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
