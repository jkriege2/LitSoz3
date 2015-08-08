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
