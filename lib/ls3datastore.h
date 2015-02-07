#ifndef LS3DATASTORE_H
#define LS3DATASTORE_H

#include <QObject>
#include <QItemDelegate>
#include <QStringListModel>
#include <QSet>
#include <QString>
#include <QProgressBar>
#include "ls3pluginservices.h"
#include "lib_imexport.h"

class LS3ReferenceTreeModel;
class LS3ReferenceTreeItem;

/*! \brief virtual base class for LitSoz 3 datastores which encapsules database access for the application and plugins

    This class may be reduced to a core of virtual function prototypes that you may implement using ANY database/file
    technology. The fields that have to be available are stored in an internal list and accessible using getFieldDefinitions().
    The user may add additional fields using addFieldDefinitions(), but they are marked as non-basic. To make sure that only
    basic fields are in the fields list, call resetFieldDefinitions(), which will remove all non-basic defintions.

*/
class LS3LIB_EXPORT LS3Datastore : public QObject {
        Q_OBJECT
    public:
        enum FieldType { FTString, FTInt, FTFloat, FTNone};
        /** \brief database field description */
        struct LS3LIB_EXPORT FieldDescription {
            QString name;
            FieldType type;
            bool basic;

            FieldDescription() {
                name="";
                type=FTString;
                basic=true;
            }
        };

        /** \brief return a list of all fields that are available (together with their type) */
        QList<FieldDescription> getFieldDefinitions() const;

        QStringList getFieldNames() const;

        /** \brief add a non-basic field definition */
        void addFieldDefinitions(const QString& name, FieldType type);

        int getFieldDefinitionsCount() const;

        /** \brief returns true if field exists */
        bool hasField(const QString& name) const;

        /** \brief returns field type */
        FieldType getFieldType(const QString& name) const;

        /** \brief remove all non-basic definitions from fields list */
        void resetFieldDefinitions();

        /** Default constructor */
        LS3Datastore(LS3PluginServices* pservices, QObject* parent=NULL);

        /** Default destructor */
        virtual ~LS3Datastore();

        /*! \brief return \c true, if a database is loaded */
        virtual bool dbIsLoaded() const =0;

        /*! \brief return the number of the current record */
        virtual int currentRecordNum() const =0;

        /*! \brief return the number of records in the current database */
        virtual int recordCount() const =0;

        /*! \brief return the number of fields (columns) in the current database */
        virtual int fieldCount() const =0;

        /*! \brief return currently loaded database file */
        virtual QString currentFile() const =0;

        /*! \brief return the  name of the \a i -th field */
        virtual QString fieldName(int i) const ;

        /*! \brief return the column that conatins the given database field */
        virtual int fieldColumn(const QString& field) const ;

        /*! \brief return the base directory all relative pathes in the database depend on.
                   basically this is the path of currentFile() */
        virtual QString baseDirectory() const ;


        /*! \brief add a record to the current database

            \return \c true on sucess and \c false else
            \param data the data to add to database. The data is stored as QMap<QString, QVariant> , where each
                        <code>[QString, QVariant]</code> pair is a field in the record. For valid field
                        names see \link ls3_database LitSoz 3.0 Databases \endlink
            \param uuid returns the uuid

        */
        virtual bool addRecord(QMap<QString, QVariant>& data, bool moveToRecord=true, bool createID=false)=0;


        /*! \brief add a record to the current database

            \return \c true on sucess and \c false else
            \param data the data to add to database. The data is stored as QMap<QString, QVariant> , where each
                        <code>[QString, QVariant]</code> pair is a field in the record. For valid field
                        names see \link ls3_database LitSoz 3.0 Databases \endlink
            \param uuid returns the uuid

        */
        virtual bool addRecord(QMap<QString, QVariant>& data, QString& uuid, bool createID=false)=0;


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
        virtual bool setRecord(int index, QMap<QString, QVariant>& data)=0;

        /*! \brief get the data of the current record

            \return the data of the current record. The data is stored as QMap<QString, QVariant> , where each
                    <code>[QString, QVariant]</code> pair is a field in the record. For valid field
                    names see \link ls3_database LitSoz 3.0 Databases \endlink

        */
        virtual QMap<QString, QVariant> currentRecord() const;

        /*! \brief get the data of the given record

            \param i index of the record to access
            \return the data of the current record. The data is stored as QMap<QString, QVariant> , where each
                    <code>[QString, QVariant]</code> pair is a field in the record. For valid field
                    names see \link ls3_database LitSoz 3.0 Databases \endlink

        */
        virtual QMap<QString, QVariant> getRecord(int i) const=0;

        /** \brief create a uuid which is not yet stored in the database */
        virtual QString newUUID() const=0;

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
        virtual bool setField(int record, QString field, QVariant data)=0;


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
        virtual QVariant getField(int record, QString field) const =0;

        /*! \brief remove the given record */
        virtual void dbDelete(int record)=0;

        /*! \brief remove the given record */
        virtual void dbDelete(const QString& recordUUID);

        /*! \brief return a pointer to a model that can be used to access all fields */
        virtual QAbstractItemModel* getTableModel() const =0;


        /*! \brief return a pointer to a QStringListModel for keywords */
        virtual QStringListModel* getKeywordsModel() const =0;


        /*! \brief return a pointer to a QStringListModel for topics */
        virtual QStringListModel* getTopicsModel() const =0;

        /*! \brief return a QStringList containing all topics */
        virtual QStringList getTopics() const =0;

        /*! \brief return a QStringList containing all author names */
        virtual QStringList getAuthors() const =0;

        /*! \brief return a QStringList containing all keywords */
        virtual QStringList getKeywords() const =0;

        /*! \brief return a pointer to a QStringListModel for names */
        virtual QStringListModel* getNamesModel() const =0;

        /*! \brief map a widget to the datamodel */
        virtual void addMapping(QWidget* widget, QString field)=0;
        /*! \brief map a widget to the datamodel with a given QProperty */
        virtual void addMapping(QWidget* widget, QString field, QString property)=0;
        /*! \brief unmap a widget */
        virtual void removeMapping(QWidget* widget)=0;


        /*!  \brief select the current dataset */
        virtual void selectCurrent();
        /*!  \brief select the given dataset */
        virtual void select(QString uuid);
        /*!  \brief select the given dataset */
        virtual void select(QStringList uuid);
        /*!  \brief select the given dataset */
        virtual void select(int i);
        /*!  \brief select the current dataset */
        virtual void deselectCurrent();
        /*!  \brief select the given dataset */
        virtual void deselect(QString uuid);
        /*!  \brief select the given dataset */
        virtual void deselect(int i);
        /*!  \brief is thee current dataset selected */
        virtual bool isCurrentSelected() const ;
        /*!  \brief is the given dataset selected */
        virtual bool isSelected(QString uuid) const ;
        /*!  \brief is the given dataset selected */
        virtual bool isSelected(int i) const ;
        /*! \brief clear selection */
        virtual void clearSelection();
        /*! \brief load the selection from the given \a filename */
        virtual void loadSelection(QString& filename);
        /*! \brief save the selection to the given \a filename */
        virtual void saveSelection(QString& filename) const ;
        /*! \brief return the number of currently selected records */
        virtual int selectionCount() const ;
        /*! \brief return the UUID of the \a i -th selected record (allows for iteration in combination with selectionCount() ) */
        virtual QString getSelectedUUID(int i) const ;
        /*! \brief return the num of the record specified by the given UUID */
        virtual int getRecordByUUID(const QString& uuid) const ;
        /** \brief return \c true if we should provide means to save the database */
        virtual bool hasDbSave() const { return false; }

        /** \brief return a default value for a given field (depending on its field type) */
        virtual QVariant getFieldDefault(QString field) const;


        enum IDType {
            AuthorYearDefaultCase,
            AuthorYearUpperCase,
            Author3YearUpperCase,
            Author6YearUpperCase,
            InitialsYear
        };

        IDType QString2IDType(QString data);


        /** \brief return a default value for a given field (depending on its field type) */
        QVariant getFieldDefault(int field) const;

        /** \brief create an ID for the current record (does NOT save it to the datastore, but returns it */
        QString createIDForCurrentRecord(IDType type);
        QString createIDForCurrentRecord(QString type);

        /** \brief create an ID for the given record (does NOT save it to the datastore, but returns it */
        QString createID(int record, IDType type);
        QString createID(int record, QString type);


        /** \brief return a tree model that allows to store references in a tree structure */
        LS3ReferenceTreeModel* getReferencTreeModel() const;

        LS3PluginServices* getPluginServices() const;

    public slots:
        /*! \brief load an existing database and bind it to the data-aware widget

            \return \c true on success
         */
        virtual bool dbLoad(const QString &fileName, QProgressBar* progress=NULL)=0;

        /*! \brief create a new database with all required fields, but does NOT load it

            \return \c true on success
         */
        virtual bool dbNew(const QString &fileName)=0;

        /*! \brief save current database under given fileName

            \return \c true on success
         */
        virtual bool dbSave(const QString &fileName, QProgressBar* progress=NULL) { return false;};


        /*! \brief close the currently opened database (if one is opened).

            Does nothing if no database is opened.
        */
        virtual void dbClose(bool saveOnClose=false)=0;

        /*! \brief insert a new record and set the cursor to the new record */
        virtual void dbInsert()=0;

        /*! \brief insert a new record and return it's row, but don't move the cursor */
        virtual int dbInsertNoMoveCursor()=0;

        /*! \brief remove the current record */
        virtual void dbDelete();

        /*! \brief submit all current changes/editing */
        virtual void dbSubmit()=0;

        /*! \brief move to first record */
        virtual void dbFirst();

        /*! \brief move to last record */
        virtual void dbLast();

        /*! \brief move to next record */
        virtual void dbNext();

        /*! \brief move to previous record */
        virtual void dbPrevious();
        /*! \brief move current record to the given recordc */
        virtual void dbMove(int current)=0;

        /*! \brief move current record to the record with the given uuid */
        virtual void dbMoveUUID(QString uuid);


    signals:
        /*! \brief this signal is emitted when the database is loaded or closed */
        void databaseLoaded(bool isLoaded);

        /*! \brief this signal is emitted when the database is closed or loaded */
        void databaseClosed(bool isClosed);

        /*! \brief emitted whenever an error occurs */
        void dbError(QString errormessage);

        /*! \brief emitted when widgets have to be disconnected from the models */
        void disconnectWidgets();

        /*! \brief emitted when widgets have to be connected to the models */
        void connectWidgets();

        /*! \brief emitted when cursor moves to a new dataset */
        void currentRecordChanged(int currentRecord);

        /*! \brief emitted when data in the datastore changes, \a firstRow and \a lastRow
                   designates the range of records that have been changed
         */
        void dataChanged(int firstRow, int lastRow);

        /*! \brief emitted when the current filename changes */
        void filenameChanged(QString filename);

        /** \brief emitted when the selection changes */
        void selectionChanged(int recordNum, bool newSelectionState);

        /** \brief emitted when many selections change */
        void massiveSelectionChange();


        /** \brief emitted when the topics list changed */
        void topicsListChanged();


        /** \brief emitted when the keywords list changed */
        void keywordsListChanged();


        /** \brief emitted when the authors list changed */
        void authorsListChanged();
    private:
        /** \brief set used to store the selection */
        QSet<QString> selection_set;

        /** \brief reference tree model */
        LS3ReferenceTreeModel* treeModel;

        QList<FieldDescription> fields;
        QMap<QString, int> fieldsMap;
        void buildFieldsMap();
        LS3PluginServices* m_pluginServices;
    private slots:
        void onDatabaseClosed(bool isLoaded);
        void onDatabaseLoaded(bool isLoaded);
    protected:
        /*! \brief this may be used to invalidate/reset access models that include information
                   like the selection which is not directly stored in the database.
         */
        virtual void invalidateModels() {};
};


#endif // LS3DATASTORE_H
