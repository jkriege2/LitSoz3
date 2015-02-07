#ifndef CONTENTSPANE_H
#define CONTENTSPANE_H

#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "programoptions.h"
#include "../lib/ls3datastore.h"


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

