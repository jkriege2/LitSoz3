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

#ifndef QENHANCEDTABLEVIEW_H
#define QENHANCEDTABLEVIEW_H

#include <QTableView>
#include <QtPrintSupport/QPrinter>
#include <QLabel>
#include <QCheckBox>
#include "litsoz3widgets_export.h"

/*! \brief this class extends the QTableView
    \ingroup qf3lib_widgets

*/

class LITSOZ3WIDGETS_EXPORT QEnhancedTableView : public QTableView
{
        Q_OBJECT
    public:
        QEnhancedTableView(QWidget* parent=NULL);
        virtual ~QEnhancedTableView();
        void print(QPrinter* printer, bool onePageWide=false, bool onePageHigh=false);

        void paint(QPainter& painter, QRect pageRec=QRect());
        QSizeF getTotalSize() const;
    public slots:
        void copySelectionToExcel();

        void print();
    protected:
        virtual void keyPressEvent(QKeyEvent* event);
        virtual bool event(QEvent* event);

        void paint(QPainter &painter, double scale, int page, double hhh, double vhw, const QList<int>& pageCols, const QList<int>& pageRows, QPrinter* p=NULL);

        QPrinter* getPrinter(QPrinter* printerIn=NULL, bool *localPrinter=NULL);
    private:
};

#endif // QENHANCEDTABLEVIEW_H
