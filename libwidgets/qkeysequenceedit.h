#ifndef QKEYSEQUENCEEDIT_H
#define QKEYSEQUENCEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>
#include "qenhancedlineedit.h"
#include "jkstyledbutton.h"
#include "../lib_imexport.h"
#include <QVariant>

/*! \brief enhanced QLineEdit which allows the user to edit a QKeySequence
    \ingroup tools_qt

    The new features are:
       - translates a pressed key combination into a text
       - switches off the context menu
       - adds a button to clear the widget
    .
 */
class LIB_EXPORT QKeySequenceEdit : public QEnhancedLineEdit
{
        Q_OBJECT
    public:
        explicit QKeySequenceEdit(QWidget *parent = 0);
        
        QKeySequence getKeySequence() const;
        void setKeySequence(QKeySequence sequence);
    signals:
        
    protected slots:
        void clearThis();

    protected:
        virtual void keyPressEvent(QKeyEvent* event);
        virtual void contextMenuEvent ( QContextMenuEvent * event );
        QKeySequence currentKS;
        JKStyledButton* btnClear;

};

#endif // QKEYSEQUENCEEDIT_H
