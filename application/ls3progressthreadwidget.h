#ifndef QFPROGRESSTHREADWIDGET_H
#define QFPROGRESSTHREADWIDGET_H

#include <QWidget>
#include "ls3progressthread.h"

namespace Ui {
    class LS3ProgressThreadWidget;
}

class LS3ProgressThreadWidget : public QWidget
{
        Q_OBJECT
        
    public:
        explicit LS3ProgressThreadWidget(QWidget *parent = 0);
        ~LS3ProgressThreadWidget();

        void init(LS3ProgressThread* thread);
        
    protected slots:
        void threadFinished();
        void errorOccured(const QString& message);
        void closeAndDelete();
        void setValue(double value);
    private:
        Ui::LS3ProgressThreadWidget *ui;
        LS3ProgressThread* m_thread;
};

#endif // QFPROGRESSTHREADWIDGET_H
