#ifndef JKVERTICALSCROLLAREA_H
#define JKVERTICALSCROLLAREA_H

#include "lib_imexport.h"
#include <QScrollArea>

/*! \brief a scrollarea which shows only vertical scrollbars whenever possible (minimum widget size) and otherwise tries
           to maximize the size of the widget.
    \ingroup tools_qt


 */
class LIB_EXPORT JKVerticalScrollArea : public QScrollArea {
        Q_OBJECT
    public:
        JKVerticalScrollArea(QWidget* parent);
        virtual ~JKVerticalScrollArea();
    protected:
        virtual void resizeEvent(QResizeEvent* event);
    private:
};

#endif // JKVERTICALSCROLLAREA_H
