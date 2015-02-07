#ifndef LS3PLUGINISBNIMPORT_H
#define LS3PLUGINISBNIMPORT_H

/* Tests:
  9780451457813
  9783630620947
  9783630620946

  */

#include "ls3pluginbase.h"
#include <QWidget>
#include <QComboBox>
#include <QHttp>
#include <QProgressDialog>
#include <QLineEdit>
#include "jkstyledbutton.h"
#include "qenhancedlineedit.h"
#include "bibtools.h"
#include "multiplerecordinsertdialog.h"
#include "isbnthread.h"

/*! \brief Import books by ISBN plugin

    See <a href="http://code.google.com/intl/de-DE/apis/books/docs/gdata/developers_guide_protocol.html#SearchingForBooks">Google Data API Guide</a>
    for details on the Google Books search.
*/
class LS3PluginISBNImport : public LS3PluginBase {
        Q_OBJECT
    public:
        LS3PluginISBNImport();
        virtual ~LS3PluginISBNImport();


        /** \brief name for the plugin */
        virtual QString getName() { return tr("ISBN importer"); };

        /** \brief short description for the plugin */
        virtual QString getDescription() { return tr("import reference data from Internet with given ISBN"); };

        /** \brief author the plugin */
        virtual QString getAuthor() { return tr("J. Krieger"); };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() { return tr("(c) 2011 by J. Krieger"); };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() { return tr("http://www.jkrieger.de/"); };

        /** \brief icon for the plugin */
        virtual QString getIconFilename() { return QString(":/isbnimport/logo.png"); };

        /** \brief initialize the plugin (create buttons, menu entries, register import/export objects ...) */
        virtual void init(LS3PluginServices* pluginServices);

        /** \brief de-initialize the plugin (clean up when program is closed) */
        virtual void deinit(LS3PluginServices* pluginServices) ;

        /** \brief this function is called when a datastore is closed */
        virtual void openData(LS3Datastore* datastore);

        /** \brief this function is called when a datastore is closed */
        virtual void closeData(LS3Datastore* datastore);
    protected:
        QWidget* widMain;
        QComboBox* cmbSource;
        QEnhancedLineEdit* edtISBN;
        JKStyledButton* btnExecute;


        void insertFromISBNdb(const QString &ISBN, const QString &recordUUID=QString(""));
        void insertFromGoogleBooks(const QString &ISBN, const QString &recordUUID=QString(""));
        void insertFromCOPAC(const QString &ISBN, const QString &recordUUID=QString(""));

    protected slots:
        void insert();

        void insertMoreRecords(QList<QMap<QString, QVariant> > moreMetadata);
        void insertRecord(QMap<QString, QVariant> data);
        void storeInRecord(QString record, QMap<QString, QVariant> data);

    private:
};

#endif // LS3PLUGINISBNIMPORT_H
