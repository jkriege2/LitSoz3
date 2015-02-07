#ifndef QENHANCEDLINEEDIT_H
#define QENHANCEDLINEEDIT_H

#include <QAbstractButton>
#include <QLineEdit>
#include <QList>
#include "lib_imexport.h"
#include <QVariant>

/*! \brief enhanced QLineEdit which adds some features missing in the basic Qt implementation
    \ingroup tools_qt

    The new features are:
       - When this QLineEdit looses focus its contents is added to its completer (if any is defined)
       - it is possible to add QAbstractButton to the widget that will appear inside the line edit.
    .
 */
class LIB_EXPORT QEnhancedLineEdit : public QLineEdit {
        Q_OBJECT
    public:
        /** Default constructor */
        QEnhancedLineEdit(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QEnhancedLineEdit();

        /** \brief add a new QAbstractButton to the edit. The button will appear left of the previously added button */
        void addButton(QAbstractButton* button);
        /** \brief return the i-th button */
        QAbstractButton* getButton(int i);
        /** \brief return the number of added buttons */
        int getButtonCount();
        /** \brief remove the given button */
        void removeButton(int i);
        /** \brief remove the given button */
        void removeButton(QAbstractButton* button);
        /** \brief set distance between two buttons in pixels */
        void setButtonDistance(int d);
        /** \brief return distance between two buttons */
        int buttonDistance();

        void addContextMenuEntry(const QString& name, QVariant data=QVariant());
        void addContextMenuEntry(const QIcon &icon, const QString& name, QVariant data=QVariant());
        void addContextMenuEntry(const QString& name, const QObject * receiver, const char * method);
        void addContextMenuEntry(const QIcon & icon, const QString& name, const QObject * receiver, const char * method);
        void addInsertContextMenuEntry(const QString& name, const QString& insert);
        void addInsertContextMenuEntry(const QIcon & icon, const QString& name, const QString& insert);
        void clearContextMenu();


    protected:
        virtual void focusOutEvent ( QFocusEvent * event ) ;
        virtual void resizeEvent ( QResizeEvent * event );
        virtual void contextMenuEvent ( QContextMenuEvent * event );
        /** \brief return the width of the buttons until the i-th button */
        int getButtonsWidth(int i);
        void moveButtons();
        void invalidateButtons();
    protected slots:
        void insertActTriggered();
    private:
        int m_buttonDistance;
        QList<QAbstractButton*> buttons;
        QList<QAction*> contextmenuActions;
};



#endif // QENHANCEDLINEEDIT_H
