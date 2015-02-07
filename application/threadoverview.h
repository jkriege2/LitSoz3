#ifndef THREADOVERVIEW_H
#define THREADOVERVIEW_H

#include <QDockWidget>
#include "programoptions.h"
#include "ls3progressthread.h"
#include "ls3progressthreadwidget.h"
#include "jkverticalscrollarea.h"
#include <QVBoxLayout>
#include <QSpacerItem>

class ThreadOverview : public QDockWidget
{
        Q_OBJECT
    public:
        explicit ThreadOverview(ProgramOptions* settings, QWidget *parent = 0);
        
        void addThread(LS3ProgressThread* thread);

    signals:
        
    public slots:
        
    protected:
        ProgramOptions* m_settings;
        QVBoxLayout* layWidgets;
        QList<QPointer<LS3ProgressThreadWidget> > widgets;
        QSpacerItem* space;
        JKVerticalScrollArea* sb;
};

#endif // THREADOVERVIEW_H
