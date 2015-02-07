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
