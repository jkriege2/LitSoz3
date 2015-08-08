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

#include "ls3progressthread.h"

LS3ProgressThread::LS3ProgressThread(QObject *parent) :
    QThread(parent)
{
    m_wasCanceled=false;
    m_min=0;
    m_max=100;
    m_value=0;
    m_message="";
    m_wasError=false;
}

bool LS3ProgressThread::wasError() const {
    return m_wasError;
}

void LS3ProgressThread::cancelThread() {
    m_wasCanceled=true;
    quit();
}

void LS3ProgressThread::setRange(int min, int max) {
    m_min=min;
    m_max=max;
    emit rangeChanged(m_min, m_max);
}

void LS3ProgressThread::setProgress(int value) {
    m_value=value;
    emit rangeChanged(m_min, m_max);
    emit progressChanged(m_value);
}

void LS3ProgressThread::incProgress(int inc) {
    setProgress(m_value+inc);
}

void LS3ProgressThread::setMessage(const QString &message) {
    m_message=message;
    emit messageChanged(m_message);
}

void LS3ProgressThread::setName(const QString &name) {
    m_name=name;
    emit nameChanged(name);
}

void LS3ProgressThread::setError(const QString &message) {
    m_wasError=true;
    emit errorOccured(message);
}
