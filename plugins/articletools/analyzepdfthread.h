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
#include <QtXml/QDomNode>
#include <QtXml/QDomDocument>
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
