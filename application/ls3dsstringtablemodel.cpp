#include "ls3dsstringtablemodel.h"
#include "ls3datastorexml.h"
#include <iostream>

LS3DSStringTableModel::LS3DSStringTableModel(LS3DatastoreXML* parent):
    QAbstractTableModel(parent)
{
    this->parent=parent;
    m_data.clear();
    indexUUID.clear();
    largestNum=-1;
    //setSupportedDragActions(Qt::CopyAction);
    m_wasChanged=false;
    doEmitSignal=true;
}

LS3DSStringTableModel::~LS3DSStringTableModel()
{
    //dtor
}

void LS3DSStringTableModel::clear(){
    m_data.clear();
    indexUUID.clear();
    largestNum=-1;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
beginResetModel();
endResetModel();
#else
reset();
#endif
}

void LS3DSStringTableModel::newFile() {
    clear();
    m_wasChanged=false;
    if (doEmitSignal) emit wasChangedChanged(m_wasChanged);
}

void LS3DSStringTableModel::loadFromXML(QDomElement n, QProgressBar* progress) {
    clear();

    if (!n.isNull()) {
        n = n.firstChildElement("record");
        if (progress) {
            progress->setRange(0,n.childNodes().size());
            progress->setValue(0);
        }
        while(!n.isNull()) {
            //std::cout<<"<record>\n";
            QString id="";
            bool ok=false;
            bool hasNum=false;
            bool hasUUID=false;
            QMap<QString, QVariant> d;
            QDomElement n1=n.firstChildElement("field");
            while(!n1.isNull()) {
                QString field=n1.attribute("name", "").toLower();
                QString sdata=n1.text();
                //std::cout<<"<field name=\""<<field.toStdString()<<"\">\n";
                if (!field.isEmpty()) {
                    sdata=sdata.replace("\r\n", "\n");
                    sdata=sdata.replace("\n\r", "\n");
                    sdata=sdata.replace("\r\r", "");
                    d[field]=sdata;
                    if (field=="num") {
                        hasNum=true;
                        int num=sdata.toInt(&ok);
                        if (num>largestNum) largestNum=num;
                    } else if (field=="uuid") {
                        hasUUID=true;
                        id=sdata;
                    } else if (field=="keywords") {
                        d[field]=d[field].toString().replace("\n\n", "\n");
                    }
                    if (!parent->hasField(field)) {
                        parent->addFieldDefinitions(field, LS3Datastore::FTString);
                    }
                }
                n1 = n1.nextSiblingElement("field");
            }
            if (!hasNum) {
                d["num"]=largestNum+1;
                largestNum++;
            }
            if (!hasUUID) {
                id=newUUID();
                d["uuid"]=id;
            }
            m_data.append(d);
            indexUUID[id]=m_data.size()-1;
            n = n.nextSiblingElement("record");
            if (progress) {
                progress->setValue(progress->value()+1);
                QApplication::processEvents();
            }
        }
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
beginResetModel();
endResetModel();
#else
reset();
#endif
    if (progress) progress->setValue(0);
    m_wasChanged=false;
    if (doEmitSignal) emit wasChangedChanged(m_wasChanged);
}

void LS3DSStringTableModel::saveToXML(QXmlStreamWriter* writer, QProgressBar* progress) {
    if (progress) {
        progress->setRange(0,m_data.size()/10);
        progress->setValue(0);
    }
    for (int idx=0; idx<m_data.size(); idx++) {
        writer->writeStartElement("record");

        QMapIterator<QString, QVariant> i(m_data[idx]);
        while (i.hasNext()) {
            i.next();
            QString v=i.value().toString();
            if (!v.isEmpty()) {
                writer->writeStartElement("field");
                writer->writeAttribute("name", i.key());
                if (v.contains('\n')) writer->writeCDATA(v);
                else writer->writeCharacters(v);
                writer->writeEndElement();
            }
        }
        writer->writeEndElement();
        if (progress && ((idx%10)==0)) {
            progress->setValue(idx);
            QApplication::processEvents();
        }
    }
    if (progress) progress->setValue(0);
    m_wasChanged=false;
    if (doEmitSignal) emit wasChangedChanged(m_wasChanged);
}

QString LS3DSStringTableModel::newUUID() const {
    QString uu=QUuid::createUuid().toString();
    bool isOK=false;
    while (!isOK) {
        isOK=true;
        for (int i=0; i<m_data.size(); i++) {
            if (m_data[i].value("uuid", "")==uu) {
                isOK=false;
                break;
            }
        }
        if (!isOK) uu=QUuid::createUuid().toString();
    }
    return uu;
}


int LS3DSStringTableModel::rowCount(const QModelIndex &parent) const {
    return m_data.size();
}

int LS3DSStringTableModel::columnCount(const QModelIndex &parent) const {
    return this->parent->getFieldDefinitionsCount();
}

QVariant LS3DSStringTableModel::data(const QModelIndex &index, int role) const {
    int row=index.row();
    int col=index.column();
    if ((row<0) || (row>=m_data.size())) return QVariant();
    if ((role==Qt::DisplayRole) || (role==Qt::EditRole)) {
        QString f=parent->fieldName(col);
        return m_data[row].value(f, QVariant(parent->getFieldDefault(col)));
    }
    return QVariant();
}

QVariant LS3DSStringTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation==Qt::Horizontal) {
        return parent->fieldName(section);
    }
    return QVariant();
}

static bool variantsEqual(const QVariant& v1, const QVariant& v2) {
    bool datachanged=false;
    if (!datachanged && v1.isNull() && v2.isNull()) {
        datachanged=true;
    }
    //qDebug()<<"variantsEqual("<<v1<<v2<<"). null "<<datachanged;
    if (!datachanged && v1.type()==QVariant::String && v2.type()==QVariant::String && v1.toString()==v2.toString()) {
        datachanged=true;
    }
    //qDebug()<<"variantsEqual("<<v1<<v2<<"). string "<<datachanged;
    if (!datachanged && v1==v2) {
        datachanged=true;
    }
    //qDebug()<<"variantsEqual("<<v1<<v2<<"). final "<<datachanged;
    return datachanged;
}

bool LS3DSStringTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (role != Qt::EditRole) return false;

    int row=index.row();
    int col=index.column();
    if ((row<0) || (row>=m_data.size())) return false;
    if ((col<0) || (col>=parent->getFieldDefinitionsCount())) return false;

    QString f=parent->fieldName(col);
    if ((f=="num") || (f=="uuid")) return false;
    bool datachanged = (!m_data[row].contains(f))&&(!(value.isNull() || (value.type()==QVariant::String && value.toString().isEmpty())));
    //qDebug()<<"LS3DSStringTableModel::setData("<<row<<col<<f<<value<<m_data[row].value(f, value)<<"   contains: "<<datachanged<<m_data[row].contains(f)<<m_data[row].keys();
    if (!datachanged) {
        datachanged=!variantsEqual(m_data[row].value(f, value), value);
    }
    //qDebug()<<"LS3DSStringTableModel::setData("<<row<<col<<f<<value<<m_data[row].value(f, value)<<"   varequal: "<<datachanged;
    if (datachanged) {
        bool oldc=m_wasChanged;
        m_wasChanged=true;
        if (doEmitSignal && m_wasChanged!=oldc) emit wasChangedChanged(m_wasChanged);
    }
    //qDebug()<<"LS3DSStringTableModel::setData("<<row<<col<<f<<value<<m_data[row].value(f, value)<<"   final: "<<datachanged;
    m_data[row].insert(f, value);


    if (doEmitSignal && datachanged) emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags LS3DSStringTableModel::flags(const QModelIndex& index ) const {
    Qt::ItemFlags f=Qt::ItemIsSelectable|Qt::ItemIsEnabled;
    QString fn=parent->fieldName(index.column());
    if ((fn!="num") && (fn!="uuid")) f=f|Qt::ItemIsEditable;
    if (index.isValid()) return Qt::ItemIsDragEnabled | f;
    else return f;
}

Qt::DropActions LS3DSStringTableModel::supportedDragActions() const
{
    return Qt::CopyAction;
}

QStringList LS3DSStringTableModel::mimeTypes() const {
    QStringList types;
    types << "litsoz3/list.uuid";
    return types;
}

QMimeData* LS3DSStringTableModel::mimeData(const QModelIndexList &indexes) const
{
     QMimeData *mimeData = new QMimeData();
     QByteArray encodedData;

     QDataStream stream(&encodedData, QIODevice::WriteOnly);

     foreach (QModelIndex index, indexes) {
         if (index.isValid()) {
             QString text = getField(index.row(), "uuid").toString();
             stream << text;
         }
     }

     //std::cout<<"LS3DSStringTableModel::mimeData()\n\n";

     mimeData->setData("litsoz3/list.uuid", encodedData);
     return mimeData;
}

void LS3DSStringTableModel::setDoEmitSignals(bool emitSignals) {
    doEmitSignal=emitSignals;
}

bool LS3DSStringTableModel::getDoEmitSignals() const
{
    return doEmitSignal;
}

bool LS3DSStringTableModel::setRecord(int row, const QMap<QString, QVariant> &data) {
    if ((row<0) || (row>=m_data.size())) return false;



    QMapIterator<QString, QVariant> i(data);
    bool oldc=m_wasChanged;
    while (i.hasNext()) {
        i.next();
        bool datachanged=(!m_data[row].contains(i.key())) || (!variantsEqual(m_data[row].value(i.key(), i.value()), i.value()));
        m_wasChanged=m_wasChanged||datachanged;
        m_data[row].insert(i.key(), i.value());
    }
    if (doEmitSignal && m_wasChanged!=oldc) emit wasChangedChanged(m_wasChanged);
    if (doEmitSignal) emit dataChanged(index(row, 0), index(row, columnCount()-1));
    return true;
}

bool LS3DSStringTableModel::setField(int row, QString field, QVariant data) {
    if ((row<0) || (row>=m_data.size())) return false;
    bool datachanged=(!m_data[row].contains(field)) || (!variantsEqual(m_data[row].value(field, data), data));

    if (datachanged) {
        bool oldc=m_wasChanged;
        m_wasChanged=true;
        if (doEmitSignal && m_wasChanged!=oldc) emit wasChangedChanged(m_wasChanged);
    }
    m_data[row].insert(field, data);
    if (doEmitSignal) emit dataChanged(index(row, parent->fieldColumn(field)), index(row, parent->fieldColumn(field)));
    return true;
}

QMap<QString, QVariant> LS3DSStringTableModel::getRecord(int idx) const {
    QMap<QString, QVariant> rdata;
    if ((idx<0)||(idx>=m_data.size())) return rdata;
    QMapIterator<QString, QVariant> i(m_data[idx]);
    while (i.hasNext()) {
        i.next();
        rdata[i.key()]=i.value();
    }
    return rdata;
}

QVariant LS3DSStringTableModel::getField(int idx, QString field) const {
    QMap<QString, QVariant> rdata;
    if ((idx<0)||(idx>=m_data.size())) return QVariant();
    return m_data[idx].value(field, QVariant(parent->getFieldDefault(field)));

}

int LS3DSStringTableModel::insertRecord(const QMap<QString, QVariant> &data) {
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    QMap<QString, QVariant> d=data;
    largestNum++;
    d["num"]=largestNum;
    d["uuid"]=newUUID();
    m_data.append(d);
    endInsertRows();
    bool oldc=m_wasChanged;
    m_wasChanged=true;
    if (doEmitSignal && m_wasChanged!=oldc) emit wasChangedChanged(m_wasChanged);
    return m_data.size()-1;
}

void LS3DSStringTableModel::removeRecord(int idx) {
    if ((idx<0)||(idx>=m_data.size())) return ;
    bool oldc=m_wasChanged;
    beginRemoveRows(QModelIndex(), idx, idx);
    m_data.removeAt(idx);
    m_wasChanged=true;
    endRemoveRows();
    if (doEmitSignal && m_wasChanged!=oldc) emit wasChangedChanged(m_wasChanged);
}

