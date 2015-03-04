#ifndef LS3DSSTRINGTABLEMODEL_H
#define LS3DSSTRINGTABLEMODEL_H

#include <QXmlStreamWriter>
#include <QAbstractTableModel>
#include <QtXml>
#include <QList>
#include <QMap>
#include <QString>
#include <QDataWidgetMapper>
#include <QStringListModel>
#include <QUuid>
#include <QXmlStreamWriter>
#include <QProgressBar>
#include <QMimeData>

class LS3DatastoreXML; // forward

class LS3DSStringTableModel: public QAbstractTableModel {
        Q_OBJECT
    public:
        /** Default constructor */
        LS3DSStringTableModel(LS3DatastoreXML* parent);
        /** Default destructor */
        virtual ~LS3DSStringTableModel();

        /*! read database contents from a QDomElement


            The children have to have this format:
\verbatim
  <record>
    <field name="field1_name">field_value</field>
    <field name="field2_name">field_value</field>
    ...
  </record>
  <record>
    <field name="field1_name">field_value</field>
    <field name="field2_name">field_value</field>
    ...
  </record>
  ...
\endverbatim
        */
        void loadFromXML(QDomElement n, QProgressBar* progress=NULL);

        void saveToXML(QXmlStreamWriter* writer, QProgressBar* progress=NULL);

        void clear();

        QString newUUID() const;

        bool setRecord(int index, QMap<QString, QVariant>& data);
        QMap<QString, QVariant> getRecord(int i) const;
        bool setField(int record, QString field, QVariant data);
        QVariant getField(int record, QString field) const;
        int insertRecord(const QMap<QString, QVariant>& data=QMap<QString, QVariant>());
        void removeRecord(int i);

        /** \brief internally calls clear() and in addition sets wasChanged() to \c false */
        void newFile();


        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

        virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
        virtual Qt::ItemFlags flags(const QModelIndex& index ) const;

        Qt::DropActions supportedDragActions() const;

        QStringList mimeTypes() const;
        QMimeData* mimeData(const QModelIndexList &indexes) const;

        bool wasChanged() const { return m_wasChanged; }

        void setDoEmitSignals(bool emitSignals);
        bool getDoEmitSignals() const;

    signals:
        void wasChangedChanged(bool wasChange);
    protected:
        /** \brief internal datastore */
        QList<QMap<QString, QVariant> > m_data;

        /** \brief used as intex to map a uuid to a data entry */
        QMap<QString, int> indexUUID;

        /** \brief stores the currently larges num */
        int largestNum;

        LS3DatastoreXML* parent;

        bool m_wasChanged;

        bool doEmitSignal;

    private:
};

#endif // LS3DSSTRINGTABLEMODEL_H
