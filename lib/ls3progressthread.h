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

#ifndef LS3PROGRESSTHREAD_H
#define LS3PROGRESSTHREAD_H

#include <QThread>
#include "litsoz3tools_export.h"

class LITSOZ3TOOLS_EXPORT LS3ProgressThread : public QThread {
        Q_OBJECT
    public:
        explicit LS3ProgressThread(QObject *parent = 0);
        bool wasError() const;

    public slots:
        void cancelThread();
    signals:
        void rangeChanged(double min, double max);
        void progressChanged(double value);
        void messageChanged(const QString& message);
        void nameChanged(const QString& name);
        void errorOccured(const QString& message);
    protected:
        void setRange(int min, int max);
        void setProgress(int value);
        void incProgress(int inc=1);
        void setMessage(const QString& message);
        void setName(const QString& name);
        void setError(const QString& message);

        int m_min;
        int m_max;
        int m_value;
        QString m_message;
        QString m_name;
        bool m_wasCanceled;
        bool m_wasError;
        
};

#endif // LS3PROGRESSTHREAD_H
