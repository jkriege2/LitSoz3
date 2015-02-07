#ifndef LS3DATASTORESQLITE_H
#define LS3DATASTORESQLITE_H

#include <QtSql>
#include <QObject>
#include <QSqlError>
#include <QItemDelegate>
#include "programoptions.h"
#include "../lib/ls3datastore.h"

// forward

class LS3DatastoreSQLite : public LS3Datastore {
        Q_OBJECT
    public:
        /** Default constructor */
        LS3DatastoreSQLite(ProgramOptions* settings, QObject* parent=NULL);
        /** Default destructor */
        virtual ~LS3DatastoreSQLite();

        /*! \brief display an error message caused by a SQL database error */
        QString convertDBError(const QSqlError &err) const ;

        /*! \brief return \c true, if a database is loaded */
        virtual bool dbIsLoaded() const ;

        /*! \brief return the number of the current record */
        virtual int currentRecordNum() const ;

        /*! \brief return the number of records in the current database */
        virtual int recordCount() const ;

        /*! \brief return the number of fields (columns) in the current database */
        virtual int fieldCount() const ;

        /*! \brief return currently loaded database file */
        virtual QString currentFile() const ;

        /*! \brief return the  name of the \a i -th field */
        virtual QString fieldName(int i) const ;

        /*! \brief return the column that conatins the given database field */
        virtual int fieldColumn(const QString& field) const ;


        /*! \brief add a set of records to the current database

            \return \c true on sucess and \c false else
            \param data the data to add to database. The data is stored as QMap<QString, QVariant> , where each
                        <code>[QString, QVariant]</code> pair is a field in the record. For valid field
                        names see \link ls3_database LitSoz 3.0 Databases \endlink

        */
        virtual bool addRecord(QMap<QString, QVariant>& data);

        /*! \brief set data of the current record

            \return \c true on sucess and \c false else
            \param data the data to store. The data is stored as QMap<QString, QVariant> , where each
                        <code>[QString, QVariant]</code> pair is a field in the record. For valid field
                        names see \link ls3_database LitSoz 3.0 Databases \endlink

        */
        virtual bool setCurrentRecord(QMap<QString, QVariant>& data);

        /*! \brief set data of the specified record

            \return \c true on sucess and \c false else
            \param index the record to alter
            \param data the data to store. The data is stored as QMap<QString, QVariant> , where each
                        <code>[QString, QVariant]</code> pair is a field in the record. For valid field
                        names see \link ls3_database LitSoz 3.0 Databases \endlink

        */
        virtual bool setRecord(int index, QMap<QString, QVariant>& data);

        /*! \brief get the data of the given record

            \param i index of the record to access
            \return the data of the current record. The data is stored as QMap<QString, QVariant> , where each
                    <code>[QString, QVariant]</code> pair is a field in the record. For valid field
                    names see \link ls3_database LitSoz 3.0 Databases \endlink

        */
        virtual QMap<QString, QVariant> getRecord(int i) const ;

        /** \brief create a uuid which is not yet stored in the database */
        virtual QString newUUID() const;

        /*! \brief set contents of given field

            \param field field name to set
            \param value value to set the field to
            \return \c true on success
         */
        virtual bool setField(QString field, QVariant data);

        /*! \brief set contents of given field in the given record

            \param record which record to access
            \param field field name to set
            \param value value to set the field to
            \return \c true on success
         */
        virtual bool setField(int record, QString field, QVariant data);


        /*! \brief get contents of given field

            \param field field name to set
            \return the field value on success or an invalid QVariant on failure
         */
        virtual QVariant getField(QString field) const ;


        /*! \brief get contents of given field in the given record

            \param record which record to access
            \param field field name to set
            \return the field value on success or an invalid QVariant on failure
         */
        virtual QVariant getField(int record, QString field) const ;

        /*! \brief return a pointer to a model that can be used to access all fields */
        virtual QAbstractItemModel* getTableModel() const ;

        /*! \brief return a pointer to a QStringListModel for keywords */
        virtual QStringListModel* getKeywordsModel() const ;

        /*! \brief return a pointer to a QStringListModel for names */
        virtual QStringListModel* getNamesModel() const ;

        /*! \brief map a widget to the datamodel */
        virtual void addMapping(QWidget* widget, QString field);
        /*! \brief map a widget to the datamodel with a given QProperty */
        virtual void addMapping(QWidget* widget, QString field, QString property);
        /*! \brief unmap a widget */
        virtual void removeMapping(QWidget* widget);
    public slots:
        /*! \brief load an existing database and bind it to the data-aware widget

            \return \c true on success
         */
        virtual bool dbLoad(const QString &fileName);

        /*! \brief create a new database with all required fields, but does NOT load it

            \return \c true on success
         */
        virtual bool dbNew(const QString &fileName);

        /*! \brief close the currently opened database (if one is opened).

            Does nothing if no database is opened.
        */
        virtual void dbClose();

        /*! \brief insert a new record */
        virtual void dbInsert();

        /*! \brief remove the current record */
        virtual void dbDelete();

        /*! \brief submit all current changes/editing */
        virtual void dbSubmit();

        /*! \brief move to first record */
        virtual void dbFirst();

        /*! \brief move to last record */
        virtual void dbLast();

        /*! \brief move to next record */
        virtual void dbNext();

        /*! \brief move to previous record */
        virtual void dbPrevious();

        /*! \brief move current record to the given recordc */
        virtual void dbMove(int current);
    protected slots:
        void currentIndexChanged(int currentRecord);
        void tableDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    protected:
        /** \brief main data model which holds the literature table for viewing and editing */
        QSqlTableModel* data;

        /** \brief main data model which holds the metadata table */
        QSqlTableModel* metadata;


        /** \brief main data model which holds the keywords table, read from the metadata */
        QStringListModel* keywordsdata;

        /** \brief main data model which holds the authors list used for completion */
        QStringListModel* authorsdata;

        /** \brief name of the connection used by data */
        QString dataConnectionName;

        /** \brief mapper that maps between widgets and the model <data> */
        QDataWidgetMapper* mapper;

        /** \brief settings object */
        ProgramOptions* settings;

        /** \brief currently loaded database file */
        QString m_currentFile;

    private:
};

#endif // LS3DATASTORESQLITE_H
