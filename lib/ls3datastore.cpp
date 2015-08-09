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

#include "ls3referencetreemodel.h"
#include "ls3referencetreeitem.h"

#include "ls3datastore.h"
#include <QFile>
#include <QtXml>
#include <iostream>
#include "bibtools.h"
#include "ls3tools.h"

LS3Datastore::LS3Datastore(LS3PluginServices* pservices, QObject* parent):
    QObject(parent)
{
    m_pluginServices=pservices;

    FieldDescription d;

    // add all fields
    d.type=FTInt; d.name="num"; fields.append(d); // unique dataset number inside table
    d.type=FTString; d.name="uuid"; fields.append(d); // uuid, guaranteed to be unique within this table (nd should be unique within the world), used to specify a record within this database
    d.type=FTString; d.name="id"; fields.append(d); // item identifier, used to cite this item (i.e. BibTeX ID)
    d.type=FTString; d.name="type"; fields.append(d); // type of publication (book, manula, article ...)
    d.type=FTString; d.name="title"; fields.append(d); // title of publication
    d.type=FTString; d.name="subtitle"; fields.append(d); // subtitles
    d.type=FTString; d.name="authors"; fields.append(d); // authors as "surname, first name; surname, first name; ..."
    d.type=FTString; d.name="editors"; fields.append(d); // editors as "surname, first name; surname, first name; ..."
    d.type=FTInt; d.name="year"; fields.append(d); // year of publication
    d.type=FTString; d.name="viewdate"; fields.append(d); // when has a webpage been viewed the last time
    d.type=FTString; d.name="isbn"; fields.append(d); // ISBN
    d.type=FTString; d.name="issn"; fields.append(d); // ISSN
    d.type=FTString; d.name="publisher"; fields.append(d); // Publisher
    d.type=FTString; d.name="places"; fields.append(d); // where is the publisher situated
    d.type=FTString; d.name="institution"; fields.append(d); // institution of the reference (i.e. university for theses, company for technical reports ...)
    d.type=FTString; d.name="journal"; fields.append(d); // journal where an article appeared
    d.type=FTString; d.name="volume"; fields.append(d); // volume of the journal, in a book series, ...
    d.type=FTString; d.name="number"; fields.append(d); // issue of journal inside a volume
    d.type=FTString; d.name="pages"; fields.append(d); // pages in a journal or book
    d.type=FTString; d.name="booktitle"; fields.append(d); // title of the book the reference appeared in
    d.type=FTString; d.name="series"; fields.append(d); // series, a book appeared in
    d.type=FTString; d.name="chapter"; fields.append(d); // referenced chapter inside a book
    d.type=FTString; d.name="howpublished"; fields.append(d); // how has the reference been published
    d.type=FTString; d.name="subtype"; fields.append(d); // a more detailed type description
    d.type=FTInt; d.name="edition"; fields.append(d); // edition of the book
    d.type=FTFloat; d.name="price"; fields.append(d); // price of the reference
    d.type=FTString; d.name="currency"; fields.append(d); // currency of the price of the reference
    d.type=FTString; d.name="source"; fields.append(d); // where is the reference from
    d.type=FTString; d.name="abstract"; fields.append(d); // abstract, contents, ... of the reference
    d.type=FTString; d.name="comments"; fields.append(d); // comments on the reference
    d.type=FTInt; d.name="rating"; fields.append(d); // rate the reference 1..5
    d.type=FTString; d.name="doi"; fields.append(d); // doi of an article
    d.type=FTString; d.name="arxiv"; fields.append(d); // ArXiv ID of an article
    d.type=FTString; d.name="citeseer"; fields.append(d); // citeseer URL
    d.type=FTString; d.name="pubmed"; fields.append(d); // PubmedID of an artice
    d.type=FTString; d.name="pmcid"; fields.append(d); // PubmedID of an artice
    d.type=FTString; d.name="files"; fields.append(d); // file associated with this reference as "file\nfile\n..."
    d.type=FTString; d.name="url"; fields.append(d); // url asoociated with this reference
    d.type=FTString; d.name="urls"; fields.append(d); // additional urls asoociated with this reference as "URL\nURL\n..."
    d.type=FTString; d.name="status"; fields.append(d); // current status "to be ordered", "has been ordered", ...
    d.type=FTString; d.name="statuscomment"; fields.append(d); // comment on the current status (e.g. "return on feb 21st" ...)
    d.type=FTString; d.name="statussince"; fields.append(d); // date o last status change
    d.type=FTString; d.name="origin"; fields.append(d); // where did I get the reference from?
    d.type=FTString; d.name="library"; fields.append(d); // library, in which the item can be found/was found
    d.type=FTString; d.name="librarynum"; fields.append(d); // number of reference within a library
    d.type=FTString; d.name="owner"; fields.append(d); // who added this reference to the database?
    d.type=FTString; d.name="keywords"; fields.append(d); // keywords for this reference
    d.type=FTString; d.name="addeddate"; fields.append(d); // date when this reference has been added
    d.type=FTString; d.name="topic"; fields.append(d); // topic covered by the publiction
    d.type=FTString; d.name="language"; fields.append(d); // language of the publiction


    selection_set.clear();
    treeModel=new LS3ReferenceTreeModel(this, this);

    connect(this, SIGNAL(databaseClosed(bool)), this, SLOT(onDatabaseClosed(bool)));
    connect(this, SIGNAL(databaseLoaded(bool)), this, SLOT(onDatabaseLoaded(bool)));
}

LS3Datastore::~LS3Datastore() {

}

QList<LS3Datastore::FieldDescription> LS3Datastore::getFieldDefinitions() const  {
    return fields;
}

int LS3Datastore::getFieldDefinitionsCount() const {
    return fields.size();
}

bool LS3Datastore::hasField(const QString& name) const {
    return fieldsMap.contains(name.toLower());
}

LS3Datastore::FieldType LS3Datastore::getFieldType(const QString& name) const  {
    int f=fieldColumn(name);
    if (f<0) return FTNone;
    return fields[f].type;
}

QVariant LS3Datastore::getFieldDefault(QString field) const {
    switch(getFieldType(field)) {
        case FTInt: return QVariant(0);
        case FTFloat: return QVariant(0.0);
        default: return QVariant(QString(""));
    }
}

int LS3Datastore::getRecordByUUID(const QString& uuid) const  {
    if (dbIsLoaded()) {
        int pos=-1;
        for (int i=0; i<recordCount(); i++) {
            if (getField(i, "uuid").toString()==uuid) {
                pos=i;
            }
        }
        return pos;
    }
    return -1;
}

void LS3Datastore::dbMoveUUID(QString uuid) {
    LS3ElapsedAutoTimer timer("LS3Datastore::dbMoveUUID("+uuid+")");
    dbMove(getRecordByUUID(uuid));
}

QMap<QString, QVariant> LS3Datastore::currentRecord() const {
    return getRecord(currentRecordNum());
}

void LS3Datastore::selectCurrent() {
    LS3ElapsedAutoTimer timer("LS3Datastore::selectCurrent()");
    select(getField("uuid").toString());
}

void LS3Datastore::select(QString uuid) {
    LS3ElapsedAutoTimer timer("LS3Datastore::selectCurrent("+uuid+")");
    selection_set.insert(uuid);
    emit selectionChanged(getRecordByUUID(uuid), true);
    //std::cout<<"selecting "<<uuid.toStdString()<<std::endl;
    //invalidateModels();
}

void LS3Datastore::select(QStringList uuid) {
    LS3ElapsedAutoTimer timer("LS3Datastore::selectCurrent("+uuid.join(", ")+")");
    for (int i=0; i<uuid.size(); i++) {
        selection_set.insert(uuid[i]);
        //select(uuid[i]);
    }
    emit massiveSelectionChange();
}

void LS3Datastore::select(int i) {
    select(getField(i, "uuid").toString());
}

void LS3Datastore::deselectCurrent(){
    select(getField("uuid").toString());
}

void LS3Datastore::deselect(QString uuid) {
    selection_set.remove(uuid);
    emit selectionChanged(getRecordByUUID(uuid), false);
    //invalidateModels();
}

void LS3Datastore::deselect(int i) {
    deselect(getField(i, "uuid").toString());
}

bool LS3Datastore::isCurrentSelected() const  {
    QString uuid=getField("uuid").toString();
    return selection_set.contains(uuid);
}

bool LS3Datastore::isSelected(QString uuid) const {
    return selection_set.contains(uuid);
}

bool LS3Datastore::isSelected(int i) const {
    QString uuid=getField(i, "uuid").toString();
    return selection_set.contains(uuid);
}

void LS3Datastore::clearSelection() {
    LS3ElapsedAutoTimer timer("LS3Datastore::clearSelection()");
    QList<QString> sel=QList<QString>::fromSet(selection_set);
    selection_set.clear();
    for (int i=0; i<sel.size(); i++) {
        emit selectionChanged(getRecordByUUID(sel[i]), false);
    }
    //invalidateModels();
}

void LS3Datastore::loadSelection(QString& filename) {
    clearSelection();

    QFile f(filename);
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QDomDocument doc;
        if (doc.setContent(&f)) {
            QDomElement e=doc.documentElement();
            if (e.tagName()=="litlist") {
                QDomElement c=e.firstChildElement("litcitation");
                while (!c.isNull()) {
                    if (c.hasAttribute("dataid")) {
                        QString uuid=c.attribute("dataid");
                        selection_set.insert(uuid);
                        emit selectionChanged(getRecordByUUID(uuid), true);
                    }
                    c=c.nextSiblingElement("litcitation");
                }
            }
        }
    }
    f.close();
    //emit selectionChanged();
    //invalidateModels();
}

void LS3Datastore::saveSelection(QString& filename) const  {
    QFile f(filename);
    if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QXmlStreamWriter xml(&f);
        xml.writeStartDocument();
        xml.writeStartElement("litlist");
        xml.writeAttribute("date", QDate::currentDate().toString());
        xml.writeAttribute("databasename", currentFile());

        QSetIterator<QString> i(selection_set);
        while (i.hasNext()) {
            QString uuid=i.next();
            xml.writeStartElement("litcitation");
            xml.writeAttribute("dataid", uuid);
            xml.writeEndElement();
        }
        xml.writeEndElement();
        xml.writeEndDocument();
    }
    f.close();
}

int LS3Datastore::selectionCount() const  {
    return selection_set.size();
}

QString LS3Datastore::getSelectedUUID(int id) const  {
    QSet<QString>::const_iterator i;
    int n=0;
    for (i = selection_set.begin(); i != selection_set.end(); ++i) {
        QString uuid = *i;
        if (n==id) return uuid;
        n++;
    }
    return "";
}

QString LS3Datastore::baseDirectory() const  {
    QFileInfo info(currentFile());
    return info.absolutePath();
}

void LS3Datastore::onDatabaseClosed(bool isLoaded) {
    if (isLoaded) clearSelection();
}

void LS3Datastore::onDatabaseLoaded(bool isLoaded) {
    if (!isLoaded) clearSelection();
}

void LS3Datastore::invalidateModels()
{

}

void LS3Datastore::resetFieldDefinitions() {
    for (int i=fields.size()-1; i>=0; i--) {
        if (!fields[i].basic) fields.removeAt(i);
    }
    buildFieldsMap();
}

void LS3Datastore::buildFieldsMap() {
    fieldsMap.clear();
    for (int i=0; i<fields.size(); i++) {
        fieldsMap[fields[i].name.toLower()]=i;
    }
}

void LS3Datastore::addFieldDefinitions(const QString& name, LS3Datastore::FieldType type) {
    FieldDescription def1;
    def1.name=name.toLower();
    def1.type=type;
    def1.basic=false;
    fields.append(def1);
    fieldsMap[def1.name.toLower()]=fields.size()-1;
};

QString LS3Datastore::fieldName(int i) const  {
    if ((i<0)||(i>=fields.size())) return "";
    return fields[i].name.toLower();
}

int LS3Datastore::fieldColumn(const QString& field) const  {
    return fieldsMap.value(field.toLower(), -1);
}

bool LS3Datastore::setCurrentRecord(const QMap<QString, QVariant> &data) {
    return setRecord(currentRecordNum(), data);
}

bool LS3Datastore::setField(QString field, QVariant data) {
    return setField(currentRecordNum(), field, data);
}

QVariant LS3Datastore::getField(QString field) const  {
    return getField(currentRecordNum(), field);
}

void LS3Datastore::dbFirst() {
    LS3ElapsedAutoTimer timer("LS3Datastore::dbFirst()");
    dbMove(0);
};

void LS3Datastore::dbLast() {
    LS3ElapsedAutoTimer timer("LS3Datastore::dbLast()");
    dbMove(recordCount()-1);
}

void LS3Datastore::dbNext() {
    LS3ElapsedAutoTimer timer("LS3Datastore::dbNext()");
    dbMove(currentRecordNum()+1);
}

void LS3Datastore::dbPrevious() {
    LS3ElapsedAutoTimer timer("LS3Datastore::dbPrevious()");
    dbMove(currentRecordNum()-1);
}

QStringList LS3Datastore::getFieldNames() const {
    QStringList sl;
    for (int i=0; i<fields.size(); i++) {
        sl.append(fields[i].name);
    }
    return sl;
}




 QString LS3Datastore::createID(int record, LS3Datastore::IDType type) {
     LS3ElapsedAutoTimer timer("LS3Datastore::createID("+QString::number(record)+")");
    bool addLetterCode=true; // if this is false, no a, b, c,... letter code will be added

    // read and process information from database
    QString a=getField(record, "authors").toString();
    QStringList ag, af;
    parseAuthors(a, &ag, &af);
    QString e=getField(record, "editors").toString();
    QStringList eg, ef;
    parseAuthors(e, &eg, &ef);
    QString y=getField(record, "year").toString();

    // create base ID
    QString id="";
    if (type==AuthorYearDefaultCase) {
        if (af.size()>0) {
            id=cleanStringForFilename(af[0]);
        } else if (ef.size()>0) {
            id=cleanStringForFilename(ef[0]);
        }
        id=id+y;
    } else if (type==AuthorYearUpperCase) {
        if (af.size()>0) {
            id=cleanStringForFilename(af[0]);
        } else if (ef.size()>0) {
            id=cleanStringForFilename(ef[0]);
        }
        id=id.toUpper()+y;
    } else if (type==Author3YearUpperCase) {
        if (af.size()>0) {
            id=cleanStringForFilename(af[0]).left(3).toUpper();
        } else if (ef.size()>0) {
            id=cleanStringForFilename(ef[0]).left(3).toUpper();
        }
        id=id+y;
    } else if (type==Author6YearUpperCase) {
        if (af.size()>0) {
            id=cleanStringForFilename(af[0]).left(6).toUpper();
        } else if (ef.size()>0) {
            id=cleanStringForFilename(ef[0]).left(6).toUpper();
        }
        id=id+y;
    } else if (type==InitialsYear) {
        if (af.size()>0) {
            for (int i=0; i<af.size(); i++) {
                id=id+cleanStringForFilename(af[i]).left(1).toUpper();
            }
        } else if (ef.size()>0) {
            for (int i=0; i<ef.size(); i++) {
                id=id+cleanStringForFilename(ef[i]).left(1).toUpper();
            }
        }
        id=id+y;
    }

    // look whether ID is unique or whether something has to be added (a, b, c, ...)
    bool isOK=false;
    QString idd=id;
    if (addLetterCode) {
        int k=0;
        while (!isOK) {
            idd=id+intToLetterCode(k);
            int i=0;
            isOK=true;
            while(isOK && (i<recordCount())) {
                if (i!=record) {
                    if (getField(i, "id")==idd) {
                        isOK=false;
                    }
                }
                i++;
            }
            k++;
        }
    }


    return idd;
}


LS3Datastore::IDType LS3Datastore::QString2IDType(QString data) {
    if (data=="AuthorYear") return LS3Datastore::AuthorYearDefaultCase;
    if (data=="USAuthorYear") return LS3Datastore::AuthorYearUpperCase;
    if (data=="AUTYear") return LS3Datastore::Author3YearUpperCase;
    if (data=="AUTHORYear") return LS3Datastore::Author6YearUpperCase;
    if (data=="ABCYear") return LS3Datastore::InitialsYear;
    return LS3Datastore::AuthorYearDefaultCase;
}

LS3ReferenceTreeModel* LS3Datastore::getReferencTreeModel() const {
    return treeModel;
}

void LS3Datastore::dbDelete(const QString& recordUUID) {
    LS3ElapsedAutoTimer timer("LS3Datastore::dbDelete("+recordUUID+")");
    dbDelete(getRecordByUUID(recordUUID));
}

void LS3Datastore::dbDelete() {
    LS3ElapsedAutoTimer timer("LS3Datastore::dbDelete()");
    dbDelete(currentRecordNum());
}

QVariant LS3Datastore::getFieldDefault(int field) const {
    return getFieldDefault(fieldName(field));
}

QString LS3Datastore::createIDForCurrentRecord(IDType type) {
    return createID(currentRecordNum(), type);
}

QString LS3Datastore::createIDForCurrentRecord(QString type) {
    return createID(currentRecordNum(), QString2IDType(type));
}


QString LS3Datastore::createID(int record, QString type) {
    return createID(record, QString2IDType(type));
}

LS3PluginServices* LS3Datastore::getPluginServices() const {
    return m_pluginServices;
}

bool LS3Datastore::dbSave(const QString &/*fileName*/, QProgressBar */*progress*/)
{
    return false;
}
