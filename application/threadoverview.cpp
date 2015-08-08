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
