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

#ifndef LS3TOOLS_H
#define LS3TOOLS_H

#include "lib_imexport.h"
#include <QString>
#include <QDebug>
#include <QElapsedTimer>

class LS3LIB_EXPORT LS3ElapsedAutoTimer {
    public:
        explicit inline LS3ElapsedAutoTimer(const QString& startmessage, const QString& endmessage, double warnlevel=100) {
            qDebug()<<QString(3+level*2, ' ')<<"STARING msg="<<startmessage;
            this->endmessage=endmessage;
            this->warnlevel=warnlevel;
            timer.start();
            level++;
        }
        explicit inline LS3ElapsedAutoTimer(const QString& message, double warnlevel=100) {
            qDebug()<<QString(3+level*2, ' ')<<"STARING msg="<<message;
            this->endmessage=message;
            this->warnlevel=warnlevel;
            timer.start();
            level++;
        }

        inline ~LS3ElapsedAutoTimer() {
            level--;
            double el=double(timer.nsecsElapsed())*1e-6;
            QString pre(3+level*2, ' ');
            if (el>warnlevel) {
                pre[0]='!';
                pre[1]='!';
                pre[2]='!';
            }
            qDebug()<<pre<<"FINISHED after "<<el<<"ms msg="<<endmessage;
        }

        inline double elapsed() const {
            return double(timer.nsecsElapsed())*1e-6;
        }

        void showMessage(const QString& message) {
            double el=double(timer.nsecsElapsed())*1e-6;
            QString pre(3+level*2-1, ' ');
            if (el>warnlevel) {
                pre[0]='!';
                pre[1]='!';
                pre[2]='!';
            }
            qDebug()<<pre<<message<<" after "<<el<<"ms";
        }

    protected:
        QElapsedTimer timer;
        QString endmessage;
        static int level;
        double warnlevel;
};

inline QString boolToString(bool data) {
    if (data) return "true";
    return "false";
}

#endif // LS3TOOLS_H
