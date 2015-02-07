#include "analyzepdfthread.h"

AnalyzePDFThread::AnalyzePDFThread(QObject *parent) :
    LS3ProgressThread(parent)
{
    insertInRecord=-1;
}

void AnalyzePDFThread::initPDF(const QString &PDF, int action, const QString& type, QDir d_lit, LS3PluginServices *services) {
    setName(tr("analyzing '%1'' ...").arg(PDF));
    namePrefixes=services->GetNamePrefixes();
    nameAdditions=services->GetNameAdditions();
    andWords=services->GetAndWords();
    pluginDirectory=services->getPluginDirectory();

    this->PDF=PDF;
    this->action=action;
    this->type=type;
    this->d_lit=d_lit;

    newRecord.clear();

}

void AnalyzePDFThread::setStoreInRecord(QString record) {
    insertInRecord=0;
    recordUUID=record;
}

void AnalyzePDFThread::setInsertnewRecord() {
        insertInRecord=-1;
}

void AnalyzePDFThread::run() {
    QString filename=PDF;
    if (QFile::exists(filename)) {
        QMap<QString, QString> xmp= extractXMP(filename, andWords,nameAdditions, namePrefixes);
        QMap<QString, QString> fromPDF;
        if (QFileInfo(filename).suffix().toLower()=="pdf") {
            fromPDF=extractFromPDF(filename, andWords,nameAdditions, namePrefixes, pluginDirectory);
        }
        //qDebug()<<"XMP: "<<xmp;
        //qDebug()<<"from PDF: "<<fromPDF;

        QString filename_base=QFileInfo(filename).baseName();
        QString filename_suffix=QFileInfo(filename).completeSuffix();

        if (d_lit.exists(filename_base+"."+filename_suffix)) {
            QString filename_base1=filename_base;
            int i=0;
            while (d_lit.exists(filename_base1+"."+filename_suffix)) {
                i++;
                filename_base1=filename_base+"_"+QString::number(i);
            }
            filename_base=filename_base1;
        }

        QString newFilename=filename_base+"."+filename_suffix;

        if (action==1) {
            QFile::copy(filename, d_lit.absoluteFilePath(newFilename));
        } else if (action==2) {
            QFile::copy(filename, d_lit.absoluteFilePath(newFilename));
            QFile::remove(filename);
        } else {
            newFilename = d_lit.relativeFilePath(filename);
        }

        //QMap<QString, QVariant> data;
        if (!xmp.isEmpty()) {
            QMapIterator<QString, QString> it(xmp);
            while (it.hasNext()) {
                it.next();
                newRecord[it.key()]=it.value();
            }
        }

        if (!fromPDF.isEmpty()) {
            QMapIterator<QString, QString> it(fromPDF);
            while (it.hasNext()) {
                it.next();
                newRecord[it.key()]=it.value();
            }
        }

        newRecord["files"]=newFilename;
        if (newRecord["type"].toString().isEmpty()) newRecord["type"]=type;
        done();

    }

}

void AnalyzePDFThread::done() {
    if (m_wasCanceled) {
    } else if (!m_wasError) {
        if (insertInRecord>=0) {
            emit storeInRecord(recordUUID, newRecord, true);
        } else if (insertInRecord==-1){
            emit insertRecords(newRecord, true);
        }
    }
    quit();
}

