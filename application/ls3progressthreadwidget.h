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
