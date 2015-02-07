#ifndef ANALYZEPDFTHREAD_H
#define ANALYZEPDFTHREAD_H
#include "ls3progressthread.h"
#include <QThread>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QPointer>
#include "ls3pluginservices.h"
#include <QSet>
#include <QString>
#include <QList>
#include <QDebug>
#include <iostream>
#include <QtXml>
#include "bibtools.h"
#include "htmltools.h"
#include "crossreftools.h"
#include "pubmedtools.h"
#include "renamefilesdialog.h"
#include "renamefilesprogressdialog.h"
#include "filenametools.h"
#include "pdfdropdockwidget.h"
#include "xmptools.h"
#include "pdftools.h"
#include "wwwthread.h"
#include "doithread.h"
#include "pubmedthread.h"


/*! \brief this class is a thread with the possibility to analyze PDF files

 */
class AnalyzePDFThread : public LS3ProgressThread {
        Q_OBJECT
    public:
        explicit AnalyzePDFThread(QObject *parent = 0);
        void initPDF(const QString& PDF, int action, const QString& type, QDir d_lit, LS3PluginServices *services);

        void setStoreInRecord(QString record);
        void setInsertnewRecord();
        void done();
    protected:

        QString PDF;
        int action;
        QString type;
        QDir d_lit;
        /** \brief record we are currently assembling */
        QMap<QString, QVariant> newRecord;
        QString lastError;

        QSet<QString> namePrefixes;
        QSet<QString> nameAdditions;
        QList<QString> andWords;
        QString pluginDirectory;

        int insertInRecord;
        QString recordUUID;
    protected slots:

        virtual void run();

    signals:
        void insertRecords(QMap<QString, QVariant> data, bool scanPubmedDoi);
        void storeInRecord(QString record, QMap<QString, QVariant> data, bool scanPubmedDoi);

};

#endif // ANALYZEPDFTHREAD_H
