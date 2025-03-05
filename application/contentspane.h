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

#ifndef CONTENTSPANE_H
#define CONTENTSPANE_H

#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "programoptions.h"
#include "ls3datastore.h"


class ContentsPane : public QWidget {
        Q_OBJECT
    public:
        /** Default constructor */
        ContentsPane(ProgramOptions* settings, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~ContentsPane();

        /** \brief connect the widget to the data source */
        void connectWidgets(LS3Datastore* datastore);

        /** \brief disconnect the widget from the data source */
        void disconnectWidgets();
    protected:
        /** \brief main data model which holds the literature table for viewing and editing */
        LS3Datastore* datastore;

        /** \brief this object manages the program settings */
        ProgramOptions* settings;

        /** \brief if this widget is connected to a data source, this is \c true */
        bool connected;

        /** \brief widget for abstract */
        QPlainTextEdit* edtAbstract;
        /** \brief widget for comments */
        QPlainTextEdit* edtComments;
    private:
};

#endif // CONTENTSPANE_H

