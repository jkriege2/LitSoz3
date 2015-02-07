#ifndef QWWWTOOLBUTTON_H
#define QWWWTOOLBUTTON_H
#include <QToolBar>
#include <QToolButton>
#include <QLineEdit>
#include <QUrl>
#include <QString>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <iostream>
#include "lib_imexport.h"




/** \brief specialized QToolButton which takes the contents of its buddy as webpage adress (URL) and opens
 *         it in the system's standard application
 *  \ingroup tools_qt
 */
class LIB_EXPORT QWWWToolButton : public QToolButton {
        Q_OBJECT
    public:
        /** Default constructor */
        QWWWToolButton(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QWWWToolButton();

        inline QLineEdit* buddy() const { return m_buddy; };
        inline void setBuddy(QLineEdit* b) { m_buddy=b; }
    protected:
        QLineEdit* m_buddy;
    protected slots:
        void openBuddyContents();
    private:
};

/** \brief specialized QToolButton which allows to select a file and saves its path in the buddy
 *  \ingroup tools_qt
 */
class LIB_EXPORT QFileSelectToolButton : public QToolButton {
        Q_OBJECT
    public:
        /** Default constructor */
        QFileSelectToolButton(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFileSelectToolButton();

        inline QLineEdit* buddy() const { return m_buddy; };
        inline void setBuddy(QLineEdit* b) { m_buddy=b; }
        inline void setBaseDir(QString& b) { baseDir=b; }
    protected:
        QLineEdit* m_buddy;
        QString baseDir;
    protected slots:
        void openBuddyContents();
    private:
};

/** \brief specialized QToolButton which allows to open the file referenced by the buddy edit
 *  \ingroup tools_qt
 */
class LIB_EXPORT QFileExecuteToolButton : public QToolButton {
        Q_OBJECT
    public:
        /** Default constructor */
        QFileExecuteToolButton(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFileExecuteToolButton();

        inline QLineEdit* buddy() const { return m_buddy; };
        inline void setBuddy(QLineEdit* b) { m_buddy=b; }
        inline void setBaseDir(QString& b) { baseDir=b; }
    protected:
        QLineEdit* m_buddy;
        QString baseDir;
    protected slots:
        void openBuddyContents();
    private:
};

#endif // QWWWTOOLBUTTON_H
