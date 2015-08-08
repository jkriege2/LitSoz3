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
