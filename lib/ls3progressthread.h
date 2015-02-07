#ifndef LS3PROGRESSTHREAD_H
#define LS3PROGRESSTHREAD_H

#include <QThread>
#include "lib_imexport.h"

class LS3LIB_EXPORT LS3ProgressThread : public QThread {
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
