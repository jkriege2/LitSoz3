#ifndef EXTENDEDPANE_H
#define EXTENDEDPANE_H

#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "programoptions.h"
#include "../lib/bibtools.h"
#include "../lib/ls3datastore.h"
#include "qmappablecombobox.h"
#include "jkstyledbutton.h"
#include "ls3pluginservices.h"
#include "qenhancedlineedit.h"
#include "qfilenameslistedit.h"



class ExtendedPane : public QWidget {
        Q_OBJECT
    public:
        /** Default constructor */
        ExtendedPane(ProgramOptions* settings, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~ExtendedPane();

        /** \brief connect the widget to the data source */
        void connectWidgets(LS3Datastore* datastore);

        /** \brief disconnect the widget from the data source */
        void disconnectWidgets();

        /** \brief set the directory of the current database */
        void setBaseDir(QString bd);

        void addEditButton(LS3PluginServices::AvailableEditWidgetsForButtons edit, QAction* action);
    protected slots:
        void setAccessdate();
    protected:
        /** \brief main data model which holds the literature table for viewing and editing */
        LS3Datastore* datastore;

        /** \brief this object manages the program settings */
        ProgramOptions* settings;

        /** \brief if this widget is connected to a data source, this is \c true */
        bool connected;

        /** \brief combobox for origin */
        QMappableComboBox* cmbOrigin;
        /** \brief combobox for status */
        QMappableComboBox* cmbStatus;
        /** \brief edit for owner of entry (who added it?) */
        QEnhancedLineEdit* edtOwner;
        /** \brief edit for library  */
        QEnhancedLineEdit* edtLibrary;
        /** \brief edit for library number */
        QEnhancedLineEdit* edtLibNum;
        /** \brief edit for price */
        QDoubleSpinBox* edtPrice;
        /** \brief edit for currency */
        QMappableComboBox* cmbCurrency;
        /** \brief dateedit for the date of addition */
        QDateEdit* dedWhenAdded;
        /** \brief dateedit for status since */
        QDateEdit* dedStatusSince;
        /** \brief edit for comment in status */
        QEnhancedLineEdit* edtStatusComment;
        /** \brief edit for doi */
        QEnhancedLineEdit* edtDoi;
        /** \brief edit for citeseer */
        QEnhancedLineEdit* edtCiteseer;
        /** \brief edit for pubmed */
        QEnhancedLineEdit* edtPubmed;
        /** \brief edit for PMCID */
        QEnhancedLineEdit* edtPMCID;
        /** \brief edit for ArXiv */
        QEnhancedLineEdit* edtArxiv;
        /** \brief edit for URL */
        QEnhancedLineEdit* edtUrl;
        /** \brief dateedit for the date of last access */
        QDateEdit* dedLastAccessed;

        /** \brief button which opens a webpage with the URL in edtUrl */
        JKStyledButton* btnUrl;
        /** \brief button which opens a DOI */
        JKStyledButton* btnDoi;
        /** \brief button which opens a PMID */
        JKStyledButton* btnPubmed;
        /** \brief button which opens a PMCID */
        JKStyledButton* btnPMCID;
        /** \brief button which opens a Arxiv ID */
        JKStyledButton* btnArxiv;
        /** \brief edit for File */
        QFilenamesListEdit* edtFile;
        ///** \brief button to select a file in edtFile */
        //JKStyledButton* btnFileSelect;
        ///** \brief button to open a file in edtFile */
        //JKStyledButton* btnFileExecute;
    private:
};

#endif // EXTENDEDPANE_H
