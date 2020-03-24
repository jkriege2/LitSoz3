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

#ifndef FILEVIEWPANE_H
#define FILEVIEWPANE_H

#include <QWidget>
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
        QAction* newSeparatorQAction();
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
        QAction* actCopySel;
        QMenu* menuSetAsField;
        QAction* actTextDummy;
        QMap<QAction*, QString> actsSetAsField;
        QComboBox* cmbScale;
        QSpinBox* spinPage;
        QSplitter* splitter;
        QTabWidget* tabDocument;
        QTreeWidget* toc;
        QListWidget* thumbnails;
        QEnhancedLineEdit* edtFind;
        QComboBox* cmbDocument;

        int m_currentRecord;

        QString selectedText;

    protected slots:
        void setSelectedText(const QString& text);
        void currentRecordChanged(int currentRecord);
        void scaleChanged(QString scale);
        void pageChanged(int page, int pages);
        void dataChanged(int firstRow, int lastRow);
        void setAsFieldActionTriggered();
        void setAsDOIActionTriggered();
        void setAsYearFieldActionTriggered();
        void appendToFieldCommaActionTriggered();
        void appendToFieldLinebreakActionTriggered();
        void appendToAuthorFieldActionTriggered();
        void copySelection();

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
