#ifndef PUBMEDSEARCHDIALOG_H
#define PUBMEDSEARCHDIALOG_H

#include <QDialog>
#include "ls3pluginservices.h"
#include <QMap>
#include <QVariant>
#include <QString>
#include "qcheckablestringlistmodel.h"
#include "htmldelegate.h"
#include "pubmedthread.h"
#include "pubmedtools.h"
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

class LS3PluginArticleTools;

namespace Ui {
    class PubmedSearchDialog;
}

class PubmedSearchDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit PubmedSearchDialog(LS3PluginServices* services, QWidget *parent = 0);
        ~PubmedSearchDialog();
        
        bool isImported(int i);
        QList<QMap<QString, QVariant> > getDataToImport();

    protected slots:
        void on_cmbAND1_currentIndexChanged(int idx);
        void on_btnSearch_clicked();
        void on_btnAgain_clicked();
        void PubmedReplyComplete();

        void insertRecord(QMap<QString, QVariant> data);
        void storeInRecord(QString record, QMap<QString, QVariant> data);
        void setMessage(const QString& message);
        void setError(const QString& message);

    protected:
        void setRecord(int i, const QMap<QString, QVariant>& data);
        void clear();
    private:
        Ui::PubmedSearchDialog *ui;
        LS3PluginServices* services;
        QCheckableStringListModel* list;
        QStringList items;

        QList<QMap<QString, QVariant> > data;


        QNetworkAccessManager *manager;
        QPointer<QNetworkReply> reply;
        QNetworkProxy proxy;
        QList<QPointer<QNetworkReply> > replies;
        /** \brief coun the redirects during the current network retrival */
        int redirectCount;
        QList<QPointer<PubmedThread> > pmthreads;
};


#endif // PUBMEDSEARCHDIALOG_H
