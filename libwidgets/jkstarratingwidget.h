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

#ifndef JKSTARRATINGWIDGET_H
#define JKSTARRATINGWIDGET_H

#include <QWidget>
#include <QIcon>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QFrame>
#include "litsoz3widgets_export.h"
/*! \brief a rating widget which displays five stars where the user may select how many are selected

*/
class LITSOZ3WIDGETS_EXPORT JKStarRatingWidget : public QFrame {
        Q_OBJECT
        Q_PROPERTY(int data READ rating WRITE setRating )
        Q_PROPERTY(int rating READ rating WRITE setRating USER true)
    public:
        /** Default constructor */
        JKStarRatingWidget(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~JKStarRatingWidget();
        int rating();
        void setRating(int value);
        int maximum();
        void setMaximum(int value);
        void setFrameStyle(int);
        void setFrameShape(Shape);
        void setFrameShadow(Shadow);
        void setLineWidth(int);
        void setMidLineWidth(int);
        void setFrameRect(const QRect &);
    protected:
        QPixmap starImage;
        QPixmap noStarImage;
        QPixmap darkStarImage;
        int m_rating;
        int m_maximum;
        void setWidgetSizes();
        virtual void paintEvent ( QPaintEvent * event );
        virtual void mouseReleaseEvent ( QMouseEvent * event );
    private:
};

#endif // JKSTARRATINGWIDGET_H
