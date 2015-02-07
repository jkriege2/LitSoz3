#ifndef QENHANCEDCOMBOBOX_H
#define QENHANCEDCOMBOBOX_H

#include <QComboBox>
#include "../lib_imexport.h"
#include <QKeyEvent>

/** \brief this QComboox descendent implements features, missing in Qt
 *         with a possibly defined range
 *  \ingroup tools_qt
 *
 *  Added features are:
 *    - setReadonly() slot, which shows the combobox in the standardway, but prevents editing
 *    - editingFinished() signal, which is emitted when "return" or "enter" key is pressed
 *    .
 */
class LIB_EXPORT QEnhancedComboBox : public QComboBox {
        Q_OBJECT
    public:
        explicit QEnhancedComboBox(QWidget *parent = 0);

        virtual void showPopup();
    signals:
        void editingFinished();
    public slots:
        void setReadOnly(bool readonly);


    protected:
        bool m_readonly;
        bool m_oldEditable;
        virtual void keyPressEvent ( QKeyEvent * e );
        virtual void keyReleaseEvent ( QKeyEvent * e );
        virtual void wheelEvent ( QWheelEvent * e );
};

#endif // QENHANCEDCOMBOBOX_H

