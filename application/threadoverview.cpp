#include "threadoverview.h"
#include <QDebug>


ThreadOverview::ThreadOverview(ProgramOptions* settings, QWidget *parent) :
    QDockWidget(tr("running Tasks"), parent)
{
    m_settings=settings;
    sb=new JKVerticalScrollArea(this);
    sb->setWidgetResizable(true);
    setWidget(sb);
    QWidget* sbw=new QWidget(this);
    sb->setWidget(sbw);
    layWidgets=new QVBoxLayout();
    layWidgets->setContentsMargins(3,3,3,3);
    layWidgets->setSpacing(2);
    sbw->setLayout(layWidgets);
    space=new QSpacerItem(2,2,QSizePolicy::Minimum,QSizePolicy::Expanding);
    layWidgets->addItem(space);
}

void ThreadOverview::addThread(LS3ProgressThread *thread) {
    layWidgets->removeItem(space);
    LS3ProgressThreadWidget* w=new LS3ProgressThreadWidget(this);
    w->init(thread);
    widgets.append(w);
    layWidgets->addWidget(w);
    w->show();
    layWidgets->addItem(space);
    sb->show();
    //qDebug()<<"added thread widge ...";
}
