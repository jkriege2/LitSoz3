#ifndef FILEVIEWPANE_H
#define FILEVIEWPANE_H

#include <QWidget>
#include <QtGui>
#include "programoptions.h"
#include "../lib/bibtools.h"
#include "../lib/ls3datastore.h"
#include "qmappablecombobox.h"
#include "jkstyledbutton.h"
#include "ls3pluginservices.h"
#include "qenhancedlineedit.h"
#include "qfilenameslistedit.h"
#include "popplerpdfwidget.h"

class FileViewPane : public QWidget
{
        Q_OBJECT
    public:
        explicit FileViewPane(ProgramOptions* settings, QWidget *parent = 0);
        
        /** \brief connect the widget to the data source */
        void connectWidgets(LS3Datastore* datastore);



        /** \brief disconnect the widget from the data source */
        void disconnectWidgets();


        /** \brief set the directory of the current database */
        void setBaseDir(QString bd);


    protected:
        /** \brief main data model which holds the literature table for viewing and editing */
        LS3Datastore* datastore;

        QString basedir;

        /** \brief this object manages the program settings */
        ProgramOptions* settings;

        /** \brief if this widget is connected to a data source, this is \c true */
        bool connected;

        QGridLayout* grid;
        PopplerPDFWidget* pdf;
        QScrollArea* scroll;
        QToolBar* toolbar;
        QAction* actFirst;
        QAction* actPrevieous;
        QAction* actNext;
        QAction* actLast;
        QAction* actZoomIn;
        QAction* actZoomOut;
        QAction* actFindDown;
        QAction* actFindUp;
        QComboBox* cmbScale;
        QSpinBox* spinPage;
        QSplitter* splitter;
        QTabWidget* tabDocument;
        QTreeWidget* toc;
        QListWidget* thumbnails;
        QEnhancedLineEdit* edtFind;
        QComboBox* cmbDocument;

        int m_currentRecord;

    protected slots:
        void currentRecordChanged(int currentRecord);
        void scaleChanged(QString scale);
        void pageChanged(int page, int pages);
        void dataChanged(int firstRow, int lastRow);

        void tocItemActivated(QTreeWidgetItem* item,int column);
        void thumbnailItemActivated(QListWidgetItem* item);

        void zoomIn();
        void zoomOut();

        void fillInfoWidgets();
        void fillThumbnails();

        void writeSettings();
        void readSettings();

        void findDown();
        void findUp();
        
        void docComboChanged(int index);

};

#endif // FILEVIEWPANE_H
