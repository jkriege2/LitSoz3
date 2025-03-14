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

#ifndef LS3PDFWIDGET_H
#define LS3PDFWIDGET_H

#include <QtPdf/QtPdf>
#include <QtPdfWidgets/QPdfView>

class LS3PDFWidget : public QPdfView {
        Q_OBJECT
    public:

        explicit LS3PDFWidget(QWidget *parent = 0);
        virtual ~LS3PDFWidget();

};

#endif // LS3PDFWIDGET_H
