#ifndef FDF_H_INCLUDED
#define FDF_H_INCLUDED

#include <QVector>
#include <QMap>
#include <QString>
#include <QObject>
#include <QWidget>
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtXml>

#include "../lib/ls3datastore.h"


class fdfManager: public QObject {
        Q_OBJECT
    protected:
        /*! \brief this enum defines the set of widgets that may be contained in a FDF file */
        enum fdfWidget {
            fdfWidgetEdit,
            fdfWidgetRegexpEdit,
            fdfWidgetMemo,
            fdfWidgetComboBox,
            fdfWidgetWWWEdit,
            fdfWidgetAuthors,
            fdfWidgetDateEdit,
            fdfWidgetIntEdit,
            fdfWidgetFloatEdit,
            fdfWidgetIntSpinbox,
            fdfWidgetFloatSpinbox,
            fdfWidgetSeparator
        };

        /*! \brief this struct holds all possible properties for any widget */
        class fdfWidgetProperty {
            public:
                /** \brief caption for the edit, default */
                QString captionTextDefault;
                /** \brief caption for the edit, depending on the language */
                QMap<QString, QString> captionText;
                /** \brief datafield which is displayed/edited with the widget */
                QString dataField;
                /** \brief is the widget read only? */
                bool readonly;
                /** \brief precision for number edits */
                unsigned int precision;
                /** \brief digits for number edits */
                unsigned int digits;
                /** \brief minimum value for int/float edits */
                double minVal;
                /** \brief maximum value for int/float edits */
                double maxVal;
                /** \brief width of the widget in pixels (-1 is defaut) */
                int widgetWidth;
                /** \brief height of the widget in pixels (-1 is default) */
                int widgetHeight;
                /** \brief a mask string */
                QMap<QString, QString> mask;
                /** \brief a default mask string */
                QString maskDefault;
                /** \brief a format string (e.g. for dates) */
                QMap<QString, QString> format;
                /** \brief a default format string */
                QString formatDefault;
                /** \brief take completer data from this list */
                QString completerList;
                /** \brief a regular expression string for a validator */
                QMap<QString, QString> regexp;
                /** \brief a default regular expression string for a validator */
                QString regexpDefault;
                /** \brief are regexp ... case sensitive? */
                bool casesensitive;
                /** \brief the widget type */
                fdfWidget type;
                /** \brief items for a combobox */
                QMap<QString, QStringList> items;
                /** \brief items for a combobox */
                QStringList itemIDs;
                /** \brief is the item editable (e.g. a combobox) */
                bool editable;

        };

        /** \brief represents one FDF file */
        class fdfFile {
            public:
                /** \brief default constructor */
                fdfFile() { };
                /** \brief default destructor */
                ~fdfFile() {
                    for (int i=0; i<widget.size(); i++) {
                        delete widget[i];
                    }
                    widget.clear();
                };
                /** \brief the FDF file represented by this struct */
                QString filename;
                /** \brief FDF file ID */
                QString ID;
                /** \brief citation type name by language */
                QMap<QString, QString> name;
                /** \brief citation description by language */
                QMap<QString, QString> description;
                /** \brief widgets used for this citation */
                QVector<fdfWidgetProperty*> widget;
                /** \brief icon of the file */
                QPixmap icon;
        };
    private:
        /** \brief contains the data of the files managed by this class */
        QVector<fdfFile*> files;

        /** \brief maps FDF IDs to fdfFile num */
        QMap<QString, int> idmap;

        /** \brief description of the last error */
        QString lastError;

        /** \brief authors completer */
        QCompleter* authorsdata;

    public:
        /** \brief class constructor */
        fdfManager(QObject * parent = 0): QObject(parent) {
            lastError="";
            authorsdata=NULL;
        };

        /** \brief class destructor */
        ~fdfManager() { clear(); };

        /** \brief loads the specified file and adds it to the internal list
         *
         * \return \c true if loading file was successful. If it returns \c false
         *         an error description may be obtained by calling getLastError();
         */
        bool loadFile(QString filename);

        /** \brief return the last error ... if there was one */
        inline QString getLastError() { return lastError; };

        /** \brief returns the number of FDF files managed by this object */
        inline int getSize() { return files.size(); };

        inline void setAuthorsCompleter(QCompleter* authorsdata) { this->authorsdata=authorsdata; }

        /** \brief clear the managed FDF files */
        inline void clear() {
            for (int i=0; i<files.size(); i++) {
                delete files[i];
            }
            files.clear();
            idmap.clear();
        };

        /** \brief get the specified fdfFile object */
        inline fdfFile* get(int num) { return files[num]; };

        /** \brief get the specified fdfFile object */
        inline fdfFile* get(QString ID) { return files[idmap[ID]]; };

        /** \brief creates widgets for a specified FDF ID
         *
         * \return a QWidget which contains all necessary widgets in a layout
         * \param ID indicates which FDF file contents to use
         * \param language the language to use for the widgets
         * \param configDir directory that contains the LitSoz configuration files
         * \param parent a widget that will be parent of all created widgets
         * \param mapper a QDataWidgetMapper which is used to connect the widgets
         * \param model the model to connect to (used to resolve data column names)
         * \param widgetvec will be cleared and after the call contain pointers to all widgets that were
         *                  added to the \a mapper
         */
         QWidget* createWidgets(QString ID, QString language, QString configDir, QWidget* parent, LS3Datastore* datastore, QVector<QPointer<QWidget> > *widgetvec);
};







#endif // FDF_H_INCLUDED
