#ifndef LS3DATASTOREXML_H
#define LS3DATASTOREXML_H

#include <QtXml>
#include <QList>
#include <QMap>
#include <QString>
#include <QDataWidgetMapper>
#include <QStringListModel>
#include <QXmlStreamWriter>
#include "ls3datastore.h"
#include "programoptions.h"
#include "ls3dsstringtablemodel.h"


class LS3DatastoreXML : public LS3Datastore {
        Q_OBJECT
    public:
        /** Default constructor */
        LS3DatastoreXML(ProgramOptions* settings, LS3PluginServices* pservices, QObject* parent=NULL);
        /** Default destructor */
        virtual ~LS3DatastoreXML();

        /** \brief returns \c true, if the database has been altered since the last opening/new/saving action */
        virtual bool wasChanged();

        /*! \brief return \c true, if a database is loaded */
        virtual bool dbIsLoaded() const { return m_databaseLoaded; }

        /*! \brief remove the given record */
        virtual void dbDelete(int record);

        /*! \brief return the number of the current record */
        virtual int currentRecordNum() const;

        /*! \brief return the number of records in the current database */
        virtual int recordCount() const;

        /*! \brief return the number of fields (columns) in the current database */
        virtual int fieldCount() const;

        /*! \brief return currently loaded database file */
        virtual QString currentFile() const;

        /*! \brief add a set of records to the current database

            \return \c true on sucess and \c false else
            \param data the data to add to database. The data is stored as QMap<QString, QVariant> , where each
                        <code>[QString, QVariant]</code> pair is a field in the record. For valid field
                        names see \link ls3_database LitSoz 3.0 Databases \endlink

        */
        virtual bool addRecord(QMap<QString, QVariant>& data, bool moveToRecord=true, bool createID=false);
        virtual bool addRecord(QMap<QString, QVariant>& data, QString& uuid, bool createIDD=false);


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
        virtual QMap<QString, QVariant> getRecord(int i) const;

        /** \brief create a uuid which is not yet stored in the database */
        virtual QString newUUID() const { return data->newUUID(); }


        /*! \brief set contents of given field in the given record

            \param record which record to access
            \param field field name to set
            \param value value to set the field to
            \return \c true on success
         */
        virtual bool setField(int record, QString field, QVariant data);




        /*! \brief get contents of given field in the given record

            \param record which record to access
            \param field field name to set
            \return the field value on success or an invalid QVariant on failure
         */
        virtual QVariant getField(int record, QString field) const;

        /*! \brief return a pointer to a model that can be used to access all fields */
        virtual QAbstractItemModel* getTableModel() const;


        /*! \brief return a pointer to a QStringListModel for keywords */
        virtual QStringListModel* getKeywordsModel() const;

        /*! \brief return a pointer to a QStringListModel for names */
        virtual QStringListModel* getNamesModel() const;
        /*! \brief return a pointer to a QStringListModel for topics */
        virtual QStringListModel* getTopicsModel() const;

        /*! \brief return a QStringList containing all topics */
        virtual QStringList getTopics() const;

        /*! \brief return a QStringList containing all author names */
        virtual QStringList getAuthors() const;

        /*! \brief return a QStringList containing all keywords */
        virtual QStringList getKeywords() const;


        /*! \brief map a widget to the datamodel */
        virtual void addMapping(QWidget* widget, QString field);
        /*! \brief map a widget to the datamodel with a given QProperty */
        virtual void addMapping(QWidget* widget, QString field, QString property);
        /*! \brief unmap a widget */
        virtual void removeMapping(QWidget* widget);
        /** \brief return \c true if we should provide means to save the database */
        virtual bool hasDbSave() const { return true; }
        virtual void ensureMappedWidgetsPopulated();


public slots:
        /*! \brief load an existing database and bind it to the data-aware widget

            \return \c true on success
         */
        virtual bool dbLoad(const QString &fileName, QProgressBar* progress=NULL);

        /*! \brief create a new database with all required fields, but does NOT load it

            \return \c true on success
         */
        virtual bool dbNew(const QString &fileName);

        /*! \brief save current database under given fileName

            \return \c true on success
         */
        virtual bool dbSave(const QString &fileName, QProgressBar* progress=NULL);

        /*! \brief close the currently opened database (if one is opened).

            Does nothing if no database is opened.
        */
        virtual void dbClose(bool saveOnClose=false);

        /*! \brief insert a new record */
        virtual void dbInsert(const QMap<QString, QVariant> &data=QMap<QString, QVariant>());

        virtual int dbInsertNoMoveCursor(const QMap<QString, QVariant> &data=QMap<QString, QVariant>());



        /*! \brief submit all current changes/editing */
        virtual void dbSubmit();

        /*! \brief move current record to the given recordc */
        virtual void dbMove(int current);

    signals:
        void wasChangedChanged(bool wasChamged);

    protected slots:
        void tableDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
        void dataWasChangedChanged(bool wasChanged) {
            emit wasChangedChanged(wasChanged);
        }
    protected:
        /* Qt seems to be instable under some conditions, when addMapping and mapper->setCurrentIndex() are mixed. Therefore addMapping can be disabled
         * and all calls will be stored, until the feature is re-enabled.
         * */
        struct storedMappingItems {
            QPointer<QWidget> widget;
            QString field;
            QString property;
        };
        QList<storedMappingItems> storedMapping;
        bool mappingEnabled;

        void disableMapping();
        void enableMapping();

        /** \brief main data model which holds the keywords table, read from the metadata */
        QStringListModel* keywordsdata;

        /** \brief main data model which holds the topics table, read from the metadata */
        QStringListModel* topicsdata;

        /** \brief main data model which holds the authors list used for completion */
        QStringListModel* authorsdata;

        /** \brief mapper that maps between widgets and the model <data> */
        QDataWidgetMapper* mapper;

        /** \brief settings object */
        ProgramOptions* settings;

        /** \brief currently loaded database file */
        QString m_currentFile;

        LS3DSStringTableModel* data;

        bool m_databaseLoaded;

        /*! \brief this may be used to invalidate/reset access models that include information
                   like the selection which is not directly stored in the database.
         */
        virtual void invalidateModels();
        void updateCompleters(int r1, int r2, bool combine=true);

};

#endif // LS3DATASTOREXML_H
