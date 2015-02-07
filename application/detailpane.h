
#ifndef DETAILPANE_H
#define DETAILPANE_H

#include <QtGui>
#include "fdf.h"
#include "../libwidgets/flowlayout.h"
#include "programoptions.h"
#include "qmappablecombobox.h"
#include "qcompleterplaintextedit.h"
#include "qcompleterfromfile.h"
#include "../lib/bibtools.h"
#include "jkstarratingwidget.h"
#include <time.h>
#include "../lib/ls3datastore.h"

/*! \brief this contains the contents of the second reference details tab

 */
class DetailPane: public QWidget
{
    Q_OBJECT
public:
    /** \brief class constructor */
    DetailPane(ProgramOptions* settings, QWidget* parent=NULL);

    /** \brief connect the widget to the data source */
    void connectWidgets(LS3Datastore* datastore);

    /** \brief disconnect the widget from the data source */
    void disconnectWidgets();

    void readSettings(QSettings& settings);
    void writeSettings(QSettings& settings);
    void changeLayout(bool small=true);

private slots:
    /** \brief connected to settings object */
    void configDirReread();

    /** \brief this changes the widgets that are coded in the FDF file and is called
     *         whenever the contents of cmbType changes */
    void redisplayFDF( int index );

    /** \brief called when btnGenerateID is pushed */
    void generateID();

    void topicModelChanged();

private:
    /** \brief a combobox that is used to select an item style */
    QMappableComboBox* cmbType;
    /** \brief scroll area for the variable items */
    QScrollArea* scaMain;
    /** \brief edit for dataset number (readonly!) */
    QLineEdit* edtNum;
    /** \brief edit for dataset ID (KRIEGER2007b ...) */
    QLineEdit* edtID;
    /** \brief button to create an ID */
    QToolButton* btnGenerateID;
    /** \brief keywords edit */
    QCompleterPlainTextEdit* pedKeywords;
    /** \brief QCompleter used for the keywords */
    QCompleter* completeKeywords;
    /** \brief QCompleter used for the authors */
    QCompleter* completeAuthors;
    /** \brief combobox for topic */
    QMappableComboBox* cmbTopic;
    /** \brief combobox for language */
    QMappableComboBox* cmbLanguage;
    /** \brief rating widget */
    JKStarRatingWidget* starRating;


    /** \brief datastore for data access */
    LS3Datastore* datastore;

    /** \brief this object manages the program settings */
    ProgramOptions* settings;

    /** \brief if this widget is connected to a data source, this is \c true */
    bool connected;

    /** \brief used to store the widgets that re mapped to dta columns */
    QVector<QWidget*> widgetvec;

    /** \brief QMenu for btnGenerateID */
    QMenu* menGenerateID;
    /** \brief action group for the types of IDs */
    QActionGroup* agIDTypes;
    /** \brief QAction for AuthorYear ID type */
    QAction* actIDTypeAuthorYear;
    /** \brief QAction for upercase AuthorYear ID type */
    QAction* actIDTypeUCAuthorYear;
    /** \brief QAction for AUTHORYear ID type */
    QAction* actIDTypeAUTYear;
    /** \brief QAction for AUTYear ID type */
    QAction* actIDTypeAUTHORYear;
    /** \brief QAction for ABCYear ID type */
    QAction* actIDTypeABCYear;

    QLabel* labTopic;
    QLabel* labLanguage;
    QLabel* labKeywords;
    QGridLayout* flTopic;
    QSpacerItem* flTopicSpacer;
};

#endif // DETAILPANE_H
