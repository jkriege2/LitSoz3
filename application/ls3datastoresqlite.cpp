#include "ls3datastoresqlite.h"
#include "version.h"
#include <iostream>


LS3DatastoreSQLite::LS3DatastoreSQLite(ProgramOptions* settings, QObject* parent):
    LS3Datastore(parent)
{
    this->settings=settings;
    data=NULL;
    metadata=NULL;
    keywordsdata = new QStringListModel(this);
    authorsdata = new QStringListModel(this);
    mapper = new QDataWidgetMapper(this);
    //connect(mapper, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
    dataConnectionName="litsoz3_main";
    m_currentFile="";
}

LS3DatastoreSQLite::~LS3DatastoreSQLite()
{
    //dtor
}

QString LS3DatastoreSQLite::convertDBError(const QSqlError &err) const  {
    return tr("Error during database operation: \n   %1").arg(err.text());
}

bool LS3DatastoreSQLite::dbLoad(const QString &fileName) {
    // if a database is opened: close it!
    dbClose();

    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        emit dbError(tr("LitSoz %1 needs the SQLite driver for Qt").arg(AutoVersion::FULLVERSION_STRING));
        return false;
    }

    // connect to database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", dataConnectionName.toLatin1());
    db.setDatabaseName(fileName);

    if (!db.open()) {
        emit dbError(convertDBError(db.lastError()));
        return false;
    }


    metadata = new QSqlTableModel(this, db);
    metadata->setTable("metadata");
    metadata->setEditStrategy(QSqlTableModel::OnManualSubmit);

    // create a stringlist model for the keywords and read its contents from the keywords field in the metadata table
    // the (possibly changed) model will be written back in the closeDB() method
    keywordsdata = new QStringListModel(this);
    QSqlQuery* q=new QSqlQuery(db);
    QString sqlstr="SELECT cvalue FROM metadata WHERE name='keywords'";
    if (!q->exec(sqlstr.toLatin1())) {
        emit dbError(convertDBError(q->lastError()));
    } else {
        QString data="";
        while (q->next()) {
            data = q->value(0).toString();
        }
        keywordsdata->setStringList(data.split("\n", QString::SkipEmptyParts));
    }
    delete q;

    // read the authorsdata completer list from a file in config dir
    authorsdata = new QStringListModel(this);
    QFile f(settings->GetConfigDirectory()+"/completers/authors.lst");
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        //std::cout<<"OK\n";
        QString s=QString::fromUtf8(f.readAll());
        authorsdata->setStringList(s.split('\n', QString::SkipEmptyParts));
    }


    // Create the data model
    data = new QSqlTableModel(this, db);
    data->setTable("lit");
    data->setSort(data->fieldIndex("num"), Qt::AscendingOrder);
    data->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //data->setEditStrategy(QSqlTableModel::OnFieldChange);
    mapper->setModel(data);
    //mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    connect(data, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(tableDataChanged(const QModelIndex&, const QModelIndex&)));
    //mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    // TODO: implement mapper completely!!!!!!!!! connect to other widgets (QLineEdit, ...)
    // TODO: implement goto dataset with ID=<value>! (linear search with toNext() from mapper)


    // Populate the model
    if (!data->select()) {
        emit dbError(convertDBError(q->lastError()));
        return false;
    }

    if (!metadata->select()) {
        emit dbError(convertDBError(q->lastError()));
        return false;
    }

    m_currentFile=fileName;
    emit filenameChanged(m_currentFile);
    emit connectWidgets();
    dbFirst();

    emit databaseLoaded(true);
    emit databaseClosed(false);
    return true;
}

bool LS3DatastoreSQLite::dbNew(const QString &fileName) {
    QString connectionName="litsoz3_createconnection";

    QSqlDatabase crdb = QSqlDatabase::addDatabase("QSQLITE", connectionName.toLatin1());
    crdb.setDatabaseName(fileName);

    if (!crdb.open()) {
        emit dbError(convertDBError(crdb.lastError()));
        return false;
    }

    QStringList tables = crdb.tables();
    if (tables.contains("lit", Qt::CaseInsensitive)) {
        return dbLoad(fileName);
    }

    QSqlQuery* q=new QSqlQuery(crdb);
    QString sqlstr="";
    sqlstr ="create table lit(";
    QList<LS3Datastore::FieldDescription> d=getFieldDefinitions();
    for (int i=0; i<d.size(); i++) {
        if (i==0) {
            sqlstr+="  "+d[i].name;
            if (d[i].type==LS3Datastore::FTInt) sqlstr+=" integer";
            else if (d[i].type==LS3Datastore::FTFloat) sqlstr+=" float";
            else sqlstr+=" varchar";
            sqlstr+=" primary key";
        } else {
            sqlstr+=", "+d[i].name;
            if (d[i].type==LS3Datastore::FTInt) sqlstr+=" integer";
            else if (d[i].type==LS3Datastore::FTFloat) sqlstr+=" float";
            else sqlstr+=" varchar";
        }
    }
    /*sqlstr+="  num integer primary key";    // unique dataset number inside table
    sqlstr+=", uuid varchar";               // uuid, guaranteed to be unique within this table (nd should be unique within the world), used to specify a record within this database
    sqlstr+=", id varchar";                 // item identifier, used to cite this item (i.e. BibTeX ID)
    sqlstr+=", type varchar";               // type of publication (book, manula, article ...)
    sqlstr+=", title varchar";              // title of publication
    sqlstr+=", subtitle varchar";           // subtitles
    sqlstr+=", authors varchar";            // authors as "surname, first name; surname, first name; ..."
    sqlstr+=", editors varchar";            // editors as "surname, first name; surname, first name; ..."
    sqlstr+=", year integer";               // year of publication
    sqlstr+=", viewdate varchar";           // when has a webpage been viewed the last time
    sqlstr+=", isbn varchar";               // ISBN
    sqlstr+=", issn varchar";               // ISSN
    sqlstr+=", publisher varchar";          // publisher
    sqlstr+=", places varchar";             // where is the publisher situated
    sqlstr+=", institution varchar";        // institution of the reference (i.e. university for theses, company for technical reports ...)
    sqlstr+=", journal varchar";            // journal where an article appeared
    sqlstr+=", volume varchar";             // volume of the journal, in a book series, ...
    sqlstr+=", number varchar";             // issue of journal inside a volume
    sqlstr+=", pages varchar";              // pages in a journal or book
    sqlstr+=", booktitle varchar";          // title of the book the reference appeared in
    sqlstr+=", series varchar";             // series, a book appeared in
    sqlstr+=", chapter varchar";            // referenced chapter inside a book
    sqlstr+=", howpublished varchar";       // how has the reference been published
    sqlstr+=", subtype varchar";            // a more detailed type description
    sqlstr+=", edition integer";            // edition of the book
    sqlstr+=", price float";                // price of the reference
    sqlstr+=", currency varchar";           // currency of the price of the reference
    sqlstr+=", source varchar";             // where is the reference from
    sqlstr+=", abstract varchar";           // abstract, content, ... of the reference
    sqlstr+=", comments varchar";           // comments on the reference
    sqlstr+=", rating integer";             // rate the reference 1..5
    sqlstr+=", doi varchar";                // doi of an article
    sqlstr+=", citeseer varchar";           // citeseer URL
    sqlstr+=", pubmed varchar";             // PubmedID of an artice
    sqlstr+=", files varchar";              // file associated with this reference as "file\nfile\n..."
    sqlstr+=", url varchar";                // url asoociated with this reference
    sqlstr+=", urls varchar";               // additional urls asoociated with this reference as "URL\nURL\n..."
    sqlstr+=", status varchar";             // current status "to be ordered", "has been ordered", ...
    sqlstr+=", statuscomment varchar";      // comment on the current status (e.g. "return on feb 21st" ...)
    sqlstr+=", statussince varchar";        // date o last status change
    sqlstr+=", origin varchar";             // where did I get the reference from?
    sqlstr+=", librarynum varchar";         // number of reference within a library
    sqlstr+=", owner varchar";              // who added this reference to the database?
    sqlstr+=", keywords varchar";           // keywords for this reference
    sqlstr+=", addeddate varchar";          // date when this reference has been added
    sqlstr+=", topic varchar";              // topic covered by the publiction
    sqlstr+=", language varchar";           // language of the publiction*/
    sqlstr+=")";
    if (!q->exec(sqlstr.toLatin1())) {
        emit dbError(convertDBError(q->lastError()));
        delete q;
        dbClose();
        return false;
    }
    delete q;

    q=new QSqlQuery(crdb);
    sqlstr="";
    sqlstr+="create table metadata (";
    sqlstr+=" name varchar";              // name of the property
    sqlstr+=", cvalue varchar";            // character value
    sqlstr+=", ivalue integer";            // integer value
    sqlstr+=", fvalue float";            // float value
    sqlstr+=")";
    if (!q->exec(sqlstr.toLatin1())) {
        emit dbError(convertDBError(q->lastError()));
        delete q;
        dbClose();
        return false;
    }
    delete q;
    // close connection to the database that was used to create a new table
    // this is OK, as the QSqlQuery q has already been deleted.
    QSqlDatabase::removeDatabase(connectionName.toLatin1());

    return dbLoad(fileName);
}

void LS3DatastoreSQLite::dbClose() {
    if (data!=NULL) {
        mapper->submit();
        data->submitAll();
    }
    emit disconnectWidgets();
    emit databaseLoaded(false);
    emit databaseClosed(true);

    if (data!=NULL) {
        // write back keywords to metadata table
        QSqlQuery* q=new QSqlQuery(metadata->database());
        QString sqlstr="select cvalue from metadata where name='keywords'";
        if (!q->exec(sqlstr.toLatin1())) {
            emit dbError(convertDBError(data->lastError()));
            delete q;
        } else {
            QString data="";
            while (q->next()) {
                data = q->value(0).toString();
            }
            QStringList tdata=data.split("\n", QString::SkipEmptyParts);
            tdata=tdata<<keywordsdata->stringList();
            tdata.removeDuplicates();
            delete q;

            metadata->setFilter("name='keywords'");
            metadata->select();
            if (metadata->rowCount()<=0) {
                metadata->insertRows(0,1);
            }
            metadata->setData(metadata->index(0,metadata->fieldIndex("name")),"keywords");
            metadata->setData(metadata->index(0,metadata->fieldIndex("cvalue")), tdata.join("\n"));
            metadata->submitAll();
        }

        // write authors index to file
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
        disconnect(data, SIGNAL(tableDataChanged(const QModelIndex&, const QModelIndex&)), this, SIGNAL(tableDataChanged(const QModelIndex&, const QModelIndex&)));
        delete data;
        delete metadata;
        data=NULL;
        metadata=NULL;
        mapper->setModel(NULL);
        authorsdata->setStringList(QStringList());
        keywordsdata->setStringList(QStringList());
    }

    if (QSqlDatabase::contains(dataConnectionName.toLatin1())) {
        QSqlDatabase::database(dataConnectionName.toLatin1()).close();
        QSqlDatabase::removeDatabase(dataConnectionName.toLatin1());
    }

    m_currentFile="";;
    emit filenameChanged(m_currentFile);
}

bool LS3DatastoreSQLite::setField(QString field, QVariant value) {
    if (dbIsLoaded()) {
        if (data->fieldIndex(field)>=0) data->setData(data->index(mapper->currentIndex(), data->fieldIndex(field)), value);
        else return false;
        return true;
    } else return false;
}


QVariant LS3DatastoreSQLite::getField(QString field) const  {
    if (dbIsLoaded()) {
        return data->data(data->index(mapper->currentIndex(), data->fieldIndex(field)));
    } else return QVariant();
}

bool LS3DatastoreSQLite::setField(int record, QString field, QVariant value) {
    if (dbIsLoaded()) {
        if (data->fieldIndex(field)>=0) data->setData(data->index(record, data->fieldIndex(field)), value);
        else return false;
        return true;
    } else return false;
}


QVariant LS3DatastoreSQLite::getField(int record, QString field) const  {
    if (dbIsLoaded()) {
        return data->data(data->index(record, data->fieldIndex(field)));
    } else return QVariant();
}

void LS3DatastoreSQLite::dbMove(int current) {
    if (current<0) return;
    if (dbIsLoaded()) {
        if (mapper->currentIndex()!=current) {
            mapper->setCurrentIndex(current);
            emit currentIndexChanged(current);
        }
    }
}

void LS3DatastoreSQLite::dbInsert() {
    if (dbIsLoaded()) {
        mapper->submit();
        data->submitAll();
        int row=data->rowCount();

        QString insert="";
        for (int i=0; i<fieldCount(); i++) {
            if (!insert.isEmpty()) insert=insert+",";
            QString fn=fieldName(i);
            //if (fn=="num") insert+="NULL";
            //else if (fn=="owner") insert="'"+settings->GetUsername()"'"
            insert+="NULL";
        }

        QSqlQuery* q=new QSqlQuery(data->database());
        QString sqlstr="";
        sqlstr=QString("insert into lit values(%1)").arg(insert);

        if (!q->exec(sqlstr.toLatin1())) {
            emit dbError(convertDBError(q->lastError()));
            delete q;
            return;
        }
        delete q;

        data->select();
        mapper->setCurrentIndex(row);
        /*if (!data->insertRecord(-1, QSqlRecord())) {
            emit dbError(convertDBError(data->lastError()));
            return ;
        }*/

        setField("owner", settings->GetUsername());
        setField("addeddate", QDate::currentDate());
        setField("statussince", QDate::currentDate());
        setField("status", settings->GetDefaultStatus());
        setField("currency", settings->GetDefaultCurrency());
        setField("price", 0.0);
        setField("rating", 0);
        setField("uuid", newUUID());
        dbSubmit();
        //dbMove(row);
    }
}

QString LS3DatastoreSQLite::newUUID() const {
    QString uu=QUuid::createUuid().toString();
    if (dbIsLoaded()) {
        QSqlQuery* q=new QSqlQuery(data->database());
        QString sqlstr="";
        bool isOK=false;
        while (!isOK) {
            sqlstr=QString("select uuid from lit where uuid='%1'").arg(uu);
            if (!q->exec(sqlstr.toLatin1())) {
                std::cout<<"database error: "<<q->lastError().text().toStdString()<<std::endl;
            } else {
                if (q->first()) {
                    uu=QUuid::createUuid().toString();
                } else isOK=true;
            }
        }
        delete q;
    }
    return uu;
}




void LS3DatastoreSQLite::dbDelete() {
    if (dbIsLoaded()) {
        int row=mapper->currentIndex();
        //std::cout<<"deleting curretn row '"<<row<<"'!\n";
        if (!data->removeRows(row, 1)) {
            emit dbError(convertDBError(data->lastError()));
        }
        if (!data->submitAll()) emit dbError(convertDBError(data->lastError()));
        data->select();
        if (row<data->rowCount()-1) {
            mapper->setCurrentIndex(row);
        } else {
            mapper->toLast();
        }
    }
}

void LS3DatastoreSQLite::dbSubmit() {
    int idx=currentRecordNum();
    mapper->submit();
    data->submitAll();
    mapper->setCurrentIndex(idx);
}

void LS3DatastoreSQLite::currentIndexChanged(int currentRecord) {
    emit currentRecordChanged(currentRecord);
}

void LS3DatastoreSQLite::dbFirst() {
    if (dbIsLoaded()) {
        mapper->toFirst();
    }
}

void LS3DatastoreSQLite::dbLast() {
    if (dbIsLoaded()) mapper->toLast();
}

void LS3DatastoreSQLite::dbNext() {
    if (dbIsLoaded()) mapper->toNext();
}

void LS3DatastoreSQLite::dbPrevious() {
    if (dbIsLoaded()) mapper->toPrevious();
}

int LS3DatastoreSQLite::currentRecordNum() const  {
    if (dbIsLoaded()) return mapper->currentIndex();
    return -1;
}

QString LS3DatastoreSQLite::currentFile() const  {
    return m_currentFile;
}

bool LS3DatastoreSQLite::dbIsLoaded() const  {
    if (data==NULL) return false;
    if (!QSqlDatabase::contains(dataConnectionName.toLatin1())) return false;
    if (!QSqlDatabase::database(dataConnectionName.toLatin1()).isOpen()) return false;
    return true;
}

bool LS3DatastoreSQLite::addRecord(QMap<QString, QVariant>& data) {
    if (!dbIsLoaded()) return false;
    dbInsert();
    return setCurrentRecord(data);
}

bool LS3DatastoreSQLite::setCurrentRecord(QMap<QString, QVariant>& datam) {
    /*if (!dbIsLoaded()) return false;
    QMapIterator<QString, QVariant> i(datam);
    while (i.hasNext()) {
        i.next();
        this->data->setData(this->data->index(mapper->currentIndex(), this->data->fieldIndex(i.key())), i.value());
    }
    data->submit();
    return true;*/
    return setRecord(mapper->currentIndex(), datam);
}

bool LS3DatastoreSQLite::setRecord(int index, QMap<QString, QVariant>& datam) {
    if (!dbIsLoaded()) return false;
    QMapIterator<QString, QVariant> i(datam);
    while (i.hasNext()) {
        i.next();
        if (this->data->fieldIndex(i.key())>=0) this->data->setData(this->data->index(index, this->data->fieldIndex(i.key())), i.value());
    }
    data->submit();
    return true;
}

QMap<QString, QVariant> LS3DatastoreSQLite::getRecord(int idx) const  {
    QMap<QString, QVariant> rdata;
    if (idx<0) return rdata;
    if (!dbIsLoaded()) return rdata;
    for (int i=0; i<data->columnCount(); i++) {
        QString fname=data->headerData(i, Qt::Horizontal).toString();
        rdata[fname]=data->data(data->index(idx, i));
    }
    return rdata;
}

int LS3DatastoreSQLite::recordCount() const  {
    if ((data==NULL)||(!dbIsLoaded())) return 0;
    return data->rowCount();
}

int LS3DatastoreSQLite::fieldCount() const  {
    if ((data==NULL)||(!dbIsLoaded())) return 0;
    return data->columnCount();
}

QAbstractItemModel* LS3DatastoreSQLite::getTableModel() const  {
    return data;
}

QStringListModel* LS3DatastoreSQLite::getKeywordsModel() const  {
    return keywordsdata;
}

QStringListModel* LS3DatastoreSQLite::getNamesModel() const  {
    return authorsdata;
}

void LS3DatastoreSQLite::tableDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {
    if (dbIsLoaded()) {

        // update keywords completer model
        int r1=topLeft.row();
        int r2=bottomRight.row();
        int c = data->fieldIndex("keywords");
        QString s="";
        for (int i=r1; i<=r2; i++) {
            s+=data->data(data->index(i, c)).toString()+"\n";
        }
        QStringList sl=s.split("\n", QString::SkipEmptyParts);
        QStringList sl1=keywordsdata->stringList();
        for (int i=0; i<sl.count(); i++) {
            if (!sl1.contains(sl[i])) sl1.append(sl[i]);
        }
        keywordsdata->setStringList(sl1);

        // update authors and editors completer model
        c = data->fieldIndex("authors");
        int c1 = data->fieldIndex("editors");
        s="";
        for (int i=r1; i<=r2; i++) {
            s+=data->data(data->index(i, c)).toString()+"; "+data->data(data->index(i, c1)).toString()+"; ";
        }
        sl=s.split(";", QString::SkipEmptyParts);
        sl1=authorsdata->stringList();
        for (int i=0; i<sl.count(); i++) {
            QString s=sl[i].trimmed();
            if (!sl1.contains(s)) sl1.append(s);
        }
        authorsdata->setStringList(sl1);
        emit dataChanged(topLeft.row(), bottomRight.row());
    }
}

QString LS3DatastoreSQLite::fieldName(int i) const  {
    if ((data==NULL)||(!dbIsLoaded())) return QString("");
    return data->headerData(i, Qt::Horizontal).toString();
}

int LS3DatastoreSQLite::fieldColumn(const QString& field) const  {
    return data->fieldIndex(field);
}


void LS3DatastoreSQLite::addMapping(QWidget* widget, QString field) {
    if (dbIsLoaded()) {
        mapper->addMapping(widget, data->fieldIndex(field));
    }
}

void LS3DatastoreSQLite::addMapping(QWidget* widget, QString field, QString property) {
    if (dbIsLoaded()) {
        mapper->addMapping(widget, data->fieldIndex(field), property.toAscii());
    }
}

void LS3DatastoreSQLite::removeMapping(QWidget* widget) {
    if (dbIsLoaded()) {
        mapper->removeMapping(widget);
    }
}

