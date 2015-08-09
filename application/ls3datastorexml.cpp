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

#include "ls3datastorexml.h"
#include "../version.h"
#include <iostream>
#include "ls3referencetreemodel.h"
#include "ls3referencetreeitem.h"


LS3DatastoreXML::LS3DatastoreXML(ProgramOptions* settings, LS3PluginServices* pservices, QObject* parent):
    LS3Datastore(pservices, parent)
{
    mappingEnabled=true;
    this->settings=settings;
    keywordsdata = new QStringListModel(this);
    topicsdata=new QStringListModel(this);
    authorsdata = new QStringListModel(this);
    data=new LS3DSStringTableModel(this);
    mapper = new QDataWidgetMapper(this);
    mapper->setModel(data);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    connect(data, SIGNAL(wasChangedChanged(bool)), this, SLOT(dataWasChangedChanged(bool)));
    connect(data, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(tableDataChanged(const QModelIndex&, const QModelIndex&)));
    m_currentFile="";
    m_databaseLoaded=false;
}

LS3DatastoreXML::~LS3DatastoreXML()
{
    //dtor
}

bool LS3DatastoreXML::dbLoad(const QString &fileName, QProgressBar* progress) {
    LS3ElapsedAutoTimer timer("LS3DatastoreXML::dbLoad");
    // if a database is opened: close it!
    dbClose();

    QDomDocument doc("mydocument");
    QFile file(fileName);
    QString errorMessage;
    int errorLine;
    int errorColumn;

    if (!file.open(QIODevice::ReadOnly)) {
        emit dbError(tr("Error opening databse '%1': File could not be opened.").arg(fileName));
        return m_databaseLoaded=false;
    }
    if (!doc.setContent(&file, &errorMessage, &errorLine, &errorColumn)) {
        file.close();
        //file.close();
        emit dbError(tr("Error parsing databse '%1': %2 (line %3, column %4).").arg(fileName).arg(errorMessage).arg(errorLine).arg(errorColumn));
        return m_databaseLoaded=false;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    if (docElem.tagName()!="litsoz3database") {
        emit dbError(tr("Error in databse '%1': This is not a LitSoz 3 databse file (root element is <%2>, but should be <litsoz3database>.").arg(fileName).arg(docElem.tagName()));
        return m_databaseLoaded=false;
    }

    QDomElement n = docElem.firstChildElement("records");
    data->loadFromXML(n, progress);

    timer.showMessage("XML loaded");

    //if (keywordsdata) delete keywordsdata
    //keywordsdata = new QStringListModel(this);


    // create a stringlist model for the keywords and read its contents from the keywords field in the metadata table
    // the (possibly changed) model will be written back in the closeDB() method
    QDomElement metadata = docElem.firstChildElement("metadata");
    if (!metadata.isNull()) {
        n = metadata.firstChildElement("item");
        while(!n.isNull()) {
            QString type=n.attribute("type").toLower();
            if (type=="keywords") {
                QString sdata=n.text().replace("\r\n", "\n");
                sdata=sdata.replace("\n\r", "\n");
                sdata=sdata.replace("\n\n", "\n");
                sdata=sdata.replace("\n\n", "\n");
                sdata=sdata.replace("\r\r", "");
                QStringList sl=sdata.split("\n", QString::SkipEmptyParts);
                sl.removeDuplicates();
                sl.sort();
                keywordsdata->setStringList(sl);
            }
            if (type=="topics") {
                QString sdata=n.text().replace("\r\n", "\n");
                sdata=sdata.replace("\n\r", "\n");
                sdata=sdata.replace("\n\n", "\n");
                sdata=sdata.replace("\n\n", "\n");
                sdata=sdata.replace("\r\r", "");
                QStringList sl=sdata.split("\n", QString::SkipEmptyParts);
                sl.removeDuplicates();
                sl.sort();
                topicsdata->setStringList(sl);
            }
            n=n.nextSiblingElement("item");
        }
    }

    timer.showMessage("METADATA read");


    // read the list of selected records
    QDomElement selectedNode = docElem.firstChildElement("selection_list");
    clearSelection();
    if (!selectedNode.isNull()) {
        n = selectedNode.firstChildElement("item");
        while(!n.isNull()) {
            QString uuid=n.attribute("uuid");
            select(uuid);
            n=n.nextSiblingElement("item");
        }
    }
    timer.showMessage("SELECTION read");


    // read the reference tree

    QDomElement reftreeNode = docElem.firstChildElement("litsoz3_reference_tree");
    getReferencTreeModel()->readFromXML(reftreeNode);
    timer.showMessage("REFTREE read");

    // read the authorsdata completer list from a file in config dir
    //if (authorsdata) delete authorsdata;
    //authorsdata = new QStringListModel(this);
    QFile f(settings->GetConfigDirectory()+"/completers/authors.lst");
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        //std::cout<<"OK\n";
        QString s=QString::fromUtf8(f.readAll());
        authorsdata->setStringList(s.split('\n', QString::SkipEmptyParts));
    }

    updateCompleters(0, recordCount()-1, false);
    timer.showMessage("AUTHORS_COMPLETER read");

    m_currentFile=fileName;
    emit filenameChanged(m_currentFile);
    emit connectWidgets();
    timer.showMessage("CONNECTED_WIDGETS");
    dbFirst();
    timer.showMessage("MOVED FIRST");

    emit databaseLoaded(true);
    emit databaseClosed(false);
    emit massiveSelectionChange();
    timer.showMessage("EMITED SIGNALS");
    return m_databaseLoaded=true;
}

bool LS3DatastoreXML::dbNew(const QString &fileName) {
    LS3ElapsedAutoTimer timer("LS3DatastoreXML::dbNew");
    dbClose();
    data->newFile();
    dbSave(fileName);
    emit filenameChanged(fileName);
    return m_databaseLoaded=true;
}

bool LS3DatastoreXML::dbSave(const QString &fileName, QProgressBar* progress) {
    LS3ElapsedAutoTimer timer("LS3DatastoreXML::dbSave");
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit dbError(tr("Error when writing database '%1': Could not open file!").arg(fileName));
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.writeStartDocument();
    writer.writeStartElement("litsoz3database");

    writer.writeStartElement("metadata");
    writer.writeStartElement("item");
    writer.writeAttribute("type", "keywords");
    writer.writeCDATA(keywordsdata->stringList().join("\n"));
    writer.writeEndElement();
    writer.writeStartElement("item");
    writer.writeAttribute("type", "topics");
    writer.writeCDATA(topicsdata->stringList().join("\n"));
    writer.writeEndElement();
    writer.writeEndElement();

    writer.writeStartElement("records");
    data->saveToXML(&writer, progress);
    writer.writeEndElement();

    // write the list of selected records
    writer.writeStartElement("selection_list");
    for (int i=0; i<selectionCount(); i++) {
        writer.writeStartElement("item");
        writer.writeAttribute("uuid", getSelectedUUID(i));
        writer.writeEndElement();
    }
    writer.writeEndElement();

    writer.writeStartElement("litsoz3_reference_tree");
    getReferencTreeModel()->writeToXML(writer);
    writer.writeEndElement();

    writer.writeEndElement();
    writer.writeEndDocument();

    file.close();

    QFile f(settings->GetConfigDirectory()+"/completers/authors.lst");
    if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
        //std::cout<<"OK\n";
        QStringList sl=authorsdata->stringList();
        QFile f1(f.fileName());
        if (f1.open(QIODevice::ReadOnly|QIODevice::Text)) {
            //std::cout<<"OK\n";
            QString s=QString::fromUtf8(f1.readAll());
            sl.append(s.split('\n', QString::SkipEmptyParts));
        } //else std::cout<<"ERROR\n";
        sl.removeDuplicates();
        sl.sort();
        for (int i=0; i<sl.size(); i++) {
            f.write(QString(sl[i]+"\n").toUtf8());
            //std::cout<<"   writing "<<itemText(i).toStdString()<<std::endl;
        }
    }

    if (m_currentFile!=fileName) emit filenameChanged(fileName);
    m_currentFile=fileName;

    return true;
}

void LS3DatastoreXML::dbClose(bool saveOnClose) {    
    LS3ElapsedAutoTimer timer("LS3DatastoreXML::dbClose");
    QElapsedTimer time;
    time.restart();
    if (data!=NULL) {
        data->setDoEmitSignals(false);
        mapper->submit();
    }
    //qDebug()<<"dbClose(saveOnClose="<<saveOnClose<<"): "<<time.elapsed()<<"ms: mapper->submit()"; time.restart();
    emit disconnectWidgets();
    //qDebug()<<"dbClose(saveOnClose="<<saveOnClose<<"): "<<time.elapsed()<<"ms: emit disconnectWidgets()"; time.restart();
    emit databaseLoaded(false);
    //qDebug()<<"dbClose(saveOnClose="<<saveOnClose<<"): "<<time.elapsed()<<"ms: emit databaseLoaded(false)"; time.restart();
    emit databaseClosed(true);
    //qDebug()<<"dbClose(saveOnClose="<<saveOnClose<<"): "<<time.elapsed()<<"ms: emit databaseClosed(true)"; time.restart();
    if (!m_currentFile.isEmpty() && saveOnClose) dbSave(m_currentFile);
    //qDebug()<<"dbClose(saveOnClose="<<saveOnClose<<"): "<<time.elapsed()<<"ms: dbSave(m_currentFile)"; time.restart();

    resetFieldDefinitions();
    //qDebug()<<"dbClose(saveOnClose="<<saveOnClose<<"): "<<time.elapsed()<<"ms: resetFieldDefinitions()"; time.restart();
    clearSelection();
    //qDebug()<<"dbClose(saveOnClose="<<saveOnClose<<"): "<<time.elapsed()<<"ms: clearSelection()"; time.restart();
    getReferencTreeModel()->clear();
    //qDebug()<<"dbClose(saveOnClose="<<saveOnClose<<"): "<<time.elapsed()<<"ms: getReferencTreeModel()->clear()"; time.restart();
    keywordsdata->setStringList(QStringList());
    //qDebug()<<"dbClose(saveOnClose="<<saveOnClose<<"): "<<time.elapsed()<<"ms: keywordsdata->setStringList()"; time.restart();
    authorsdata->setStringList(QStringList());
    //qDebug()<<"dbClose(saveOnClose="<<saveOnClose<<"): "<<time.elapsed()<<"ms: authorsdata->setStringList(QStringList())"; time.restart();
    topicsdata->setStringList(QStringList());
    //qDebug()<<"dbClose(saveOnClose="<<saveOnClose<<"): "<<time.elapsed()<<"ms: topicsdata->setStringList(QStringList())"; time.restart();
    if (data!=NULL) {
        data->setDoEmitSignals(true);
    }
    data->newFile();
    //qDebug()<<"dbClose(saveOnClose="<<saveOnClose<<"): "<<time.elapsed()<<"ms: data->newFile()"; time.restart();

    m_currentFile="";
    m_databaseLoaded=true;
    emit filenameChanged(m_currentFile);
    //qDebug()<<"dbClose(saveOnClose="<<saveOnClose<<"): "<<time.elapsed()<<"ms: emit filenameChanged(m_currentFile)"; time.restart();
}

bool LS3DatastoreXML::setField(int record, QString field, QVariant value) {
    LS3ElapsedAutoTimer timer("LS3DatastoreXML::setField("+QString::number(record)+", "+field+" = "+value.toString()+")");
    if (dbIsLoaded()) {
        data->setField(record,field, value);
        return true;
    } else return false;
}

QVariant LS3DatastoreXML::getField(int record, QString field) const {
    if (dbIsLoaded()) {
        return data->getField(record, field);
    } else return QVariant(getFieldDefault(field));
}

void LS3DatastoreXML::dbMove(int current) {
    LS3ElapsedAutoTimer timer("LS3DatastoreXML::dbMove("+QString::number(current)+")");
    if (current<0) return;
    if (dbIsLoaded()) {
        int oldidx=mapper->currentIndex();
        //qDebug()<<"dbMove("<<current<<")  was="<<oldidx;
        //QDataWidgetMapper::SubmitPolicy policy=mapper->submitPolicy();
        //qDebug()<<"2";
        //mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
        //qDebug()<<"3";
        //mapper->submit();
        //qDebug()<<"4";
        if (oldidx!=current) {
            //qDebug()<<"5";
            disableMapping();
            //qDebug()<<"5.1";
            mapper->setCurrentIndex(current);
            //qDebug()<<"5.2";
            enableMapping();
            //qDebug()<<"6";
            emit currentRecordChanged(current);
            //qDebug()<<"7";
        }
       // mapper->setSubmitPolicy(policy);
        //qDebug()<<"8";
    }
}

void LS3DatastoreXML::dbInsert(const QMap<QString, QVariant>& indata) {
    LS3ElapsedAutoTimer timer("LS3DatastoreXML::dbInsert(map)");
    int row=dbInsertNoMoveCursor(indata);
    if (row>=0) {
        mapper->setCurrentIndex(row);
    }

}

int LS3DatastoreXML::dbInsertNoMoveCursor(const QMap<QString, QVariant>& indata) {
    LS3ElapsedAutoTimer timer("LS3DatastoreXML::dbInsertNoMoveCursor(map)");
    int row=-1;
    if (dbIsLoaded()) {
        mapper->submit();
        //data->submitAll();


        QMap<QString, QVariant> idata=indata;
        idata["owner"]=settings->GetUsername();
        idata["viewdate"]=QDate::currentDate();
        idata["addeddate"]=QDate::currentDate();
        idata["statussince"]=QDate::currentDate();
        idata["status"]=settings->GetDefaultStatus();
        idata["currency"]=settings->GetDefaultCurrency();
        idata["price"]=double(0.0);
        idata["rating"]=int(0);

        QString langtest;
        langtest=idata.value("title").toString()+" "+idata.value("booktitle").toString()+" "+idata.value("abstract").toString()+" "+idata.value("comment").toString()+" "+idata.value("contents").toString()+" "+idata.value("keywords").toString()+" "+idata.value("topic").toString();
        idata["language"]=recognizeLanguage(langtest);
        if (idata["language"].toString().isEmpty()) idata.remove("language");
        //idata[""]=;
        row=data->insertRecord(idata);


        //data->setField(row, "uuid", newUUID());
    }
    return row;
}






void LS3DatastoreXML::dbSubmit() {
    LS3ElapsedAutoTimer timer("LS3DatastoreXML::dbSubmit()");
    int idx=currentRecordNum();
    mapper->submit();
    mapper->setCurrentIndex(idx);
}

int LS3DatastoreXML::currentRecordNum() const {
    if (dbIsLoaded()) return mapper->currentIndex();
    return -1;
}

QString LS3DatastoreXML::currentFile() const {
    return m_currentFile;
}

bool LS3DatastoreXML::addRecord(const QMap<QString, QVariant>& data, bool moveToRecord, bool createIDD) {
    LS3ElapsedAutoTimer timer("LS3DatastoreXML::addRecord(map, ...)");
    if (!dbIsLoaded()) return false;
    int row=dbInsertNoMoveCursor(data);
    if (row>=0) {
        if (createIDD) {
            setField(row, "id", createID(row, getPluginServices()->GetIDType()));
        }
        if (moveToRecord) {
            dbMove(row);
        }
    }
    return row>=0;
}

bool LS3DatastoreXML::addRecord(const QMap<QString, QVariant> &data, QString &uuid, bool createIDD) {
    LS3ElapsedAutoTimer timer("LS3DatastoreXML::addRecord(map, "+uuid+" ...)");
    if (!dbIsLoaded()) return false;
    int row=dbInsertNoMoveCursor(data);
    if (row>=0) {
        if (createIDD) {
            setField(row, "id", createID(row, getPluginServices()->GetIDType()));
        }
        uuid=getField(row, "uuid").toString();
    }
    return row>=0;
}

bool LS3DatastoreXML::setRecord(int index, const QMap<QString, QVariant>& datam) {
    LS3ElapsedAutoTimer timer("LS3DatastoreXML::setRecord("+QString::number(index)+", map)");
    if (!dbIsLoaded()) return false;

    QMap<QString, QVariant> data=datam;

    if (data.value("language").toString().isEmpty()) {
        QString langtest;
        langtest=data.value("title").toString()+" "+data.value("booktitle").toString()+" "+data.value("abstract").toString()+" "+data.value("comment").toString()+" "+data.value("contents").toString()+" "+data.value("keywords").toString()+" "+data.value("topic").toString();
        data["language"]=recognizeLanguage(langtest);
        if (data["language"].toString().isEmpty()) data.remove("language");
    }

    this->data->setRecord(index, data);
    return true;
}

QMap<QString, QVariant> LS3DatastoreXML::getRecord(int idx) const {
    return data->getRecord(idx);
}

int LS3DatastoreXML::recordCount() const {
    if ((data==NULL)||(!dbIsLoaded())) return 0;
    return data->rowCount();
}

int LS3DatastoreXML::fieldCount() const {
    if ((data==NULL)||(!dbIsLoaded())) return 0;
    return data->columnCount();
}

QAbstractItemModel* LS3DatastoreXML::getTableModel() const {
    return data;
}

QStringListModel* LS3DatastoreXML::getKeywordsModel() const {
    return keywordsdata;
}

QStringListModel* LS3DatastoreXML::getNamesModel() const {
    return authorsdata;
}

QStringListModel* LS3DatastoreXML::getTopicsModel() const {
    return topicsdata;
}

QStringList LS3DatastoreXML::getTopics() const {
    return topicsdata->stringList();
}

QStringList LS3DatastoreXML::getAuthors() const {
    return authorsdata->stringList();
}

QStringList LS3DatastoreXML::getKeywords() const {
    return keywordsdata->stringList();
}

void LS3DatastoreXML::updateCompleters(int r1, int r2, bool combine) {
    LS3ElapsedAutoTimer timer("LS3DatastoreXML::updateCompleters("+QString::number(r1)+", "+QString::number(r2)+", ...)");
    QString s="";
    QStringList slold=keywordsdata->stringList();
    for (int i=r1; i<=r2; i++) {
        s+=data->getField(i, "keywords").toString()+"\n";
    }
    QStringList sl=s.split("\n", QString::SkipEmptyParts);
    QStringList sl1;
    if (combine) {
        sl1=keywordsdata->stringList();
        for (int i=0; i<sl.count(); i++) {
            if (!sl1.contains(sl[i])) sl1.append(sl[i]);
        }
    } else {
        sl1=sl;
    }
    timer.showMessage("collected keywords");
    sl1.removeDuplicates();
    timer.showMessage("removed duplicates");
    sl1.sort();
    timer.showMessage("sorted");
    keywordsdata->setStringList(sl1);
    emit keywordsListChanged();
    timer.showMessage("written back");

    // update topic completer model
    s="";
    slold=topicsdata->stringList();
    for (int i=r1; i<=r2; i++) {
        s+=data->getField(i, "topic").toString()+"\n";
    }
    sl=s.split("\n", QString::SkipEmptyParts);
    if (combine) {
        sl1=topicsdata->stringList();
        for (int i=0; i<sl.count(); i++) {
            if (!sl1.contains(sl[i])) sl1.append(sl[i]);
        }
    } else {
        sl1=sl;
    }
    timer.showMessage("collected topics");
    sl1.removeDuplicates();
    timer.showMessage("removed duplicates");
    sl1.sort();
    timer.showMessage("sorted");
    topicsdata->setStringList(sl1);
    emit topicsListChanged();
    timer.showMessage("written back");

    // update authors and editors completer model
    s="";
    slold=authorsdata->stringList();
    for (int i=r1; i<=r2; i++) {
        s+=data->getField(i, "authors").toString()+"; "+data->getField(i, "editors").toString()+"; ";
    }
    sl=s.split(";", QString::SkipEmptyParts);
    sl1=authorsdata->stringList();
    for (int i=0; i<sl.count(); i++) {
        QString s=sl[i].trimmed();
        if (!sl1.contains(s)) sl1.append(s);
    }
    timer.showMessage("collected authors");
    sl1.removeDuplicates();
    timer.showMessage("removed duplicates");
    sl1.sort();
    timer.showMessage("sorted");
    authorsdata->setStringList(sl1);
    emit authorsListChanged();
}


void LS3DatastoreXML::tableDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {
    LS3ElapsedAutoTimer timer("LS3DatastoreXML::tableDataChanged("+QString::number(topLeft.row())+", "+QString::number(topLeft.column())+" ... "+QString::number(bottomRight.row())+",  "+QString::number(bottomRight.column())+")");
    if (dbIsLoaded()) {

        // update keywords completer model
        int r1=topLeft.row();
        int r2=bottomRight.row();
        updateCompleters(r1, r2, true);
        emit dataChanged(topLeft.row(), bottomRight.row());
    }
}

void LS3DatastoreXML::disableMapping()
{
    LS3ElapsedAutoTimer timer("LS3DatastoreXML::disableMapping()");
    mappingEnabled=false;
    //qDebug()<<"disableMapping";
}

void LS3DatastoreXML::enableMapping()
{
    LS3ElapsedAutoTimer timer("LS3DatastoreXML::enableMapping()");
    mappingEnabled=true;
    //qDebug()<<"enableMapping .. ";
    for (int i=0; i<storedMapping.size(); i++) {
        if (storedMapping[i].property.isEmpty()) {
            mapper->addMapping(storedMapping[i].widget, fieldColumn(storedMapping[i].field));
        } else {
            mapper->addMapping(storedMapping[i].widget, fieldColumn(storedMapping[i].field), storedMapping[i].property.toLatin1());
        }
    }
    storedMapping.clear();
    //qDebug()<<"enableMapping .. DONE!";
}

void LS3DatastoreXML::addMapping(QWidget* widget, QString field) {
    if (dbIsLoaded()) {
        //std::cout<<">>>> map '"<<field.toStdString()<<"' --> '"<<fieldColumn(field)<<"'\n";
        if (mappingEnabled) {
            mapper->addMapping(widget, fieldColumn(field));
        } else {
            storedMappingItems it;
            it.widget=widget;
            it.field=field;
            it.property.clear();
            storedMapping.append(it);
        }

    }
}

void LS3DatastoreXML::addMapping(QWidget* widget, QString field, QString property) {
    if (dbIsLoaded()) {
        if (mappingEnabled) {
            mapper->addMapping(widget, fieldColumn(field), property.toLatin1());
        } else {
            storedMappingItems it;
            it.widget=widget;
            it.field=field;
            it.property=property;
            storedMapping.append(it);
        }
    }
}

void LS3DatastoreXML::removeMapping(QWidget* widget) {
    if (dbIsLoaded()) {
        mapper->removeMapping(widget);
    }
}

void LS3DatastoreXML::ensureMappedWidgetsPopulated()
{
    if (dbIsLoaded()) {
        mapper->setCurrentIndex(mapper->currentIndex());
    }
}

void LS3DatastoreXML::invalidateModels() {
}

bool LS3DatastoreXML::wasChanged() {
    return data->wasChanged();
}


void LS3DatastoreXML::dbDelete(int record) {
    int row=currentRecordNum();
    QString uuid=getField(record, "uuid").toString();
    if (isSelected(uuid)) deselect(uuid);
    data->removeRecord(record);
    if (row<data->rowCount()-1) {
        mapper->setCurrentIndex(row);
    } else {
        mapper->toLast();
    }

}

