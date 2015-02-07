#include "jkverticalscrollarea.h"

#include <QResizeEvent>

JKVerticalScrollArea::JKVerticalScrollArea(QWidget* parent):
    QScrollArea(parent)
{
    //ctor
}

JKVerticalScrollArea::~JKVerticalScrollArea()
{
    //dtor
}

void JKVerticalScrollArea::resizeEvent(QResizeEvent * event) {
    QWidget* w=widget();
    if (w) {
        int minw=w->minimumWidth();
        int neww=event->size().width()-2;
        int wid=qMax(minw, neww);
        w->resize(wid, w->height());
    }

    QScrollArea::resizeEvent(event);
}
