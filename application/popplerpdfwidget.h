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

#ifndef POPPLERPDFWIDGET_H
#define POPPLERPDFWIDGET_H

#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <poppler/qt5/poppler-qt5.h>
#else
    #include <poppler/qt4/poppler-qt4.h>
#endif

#include <QApplication>
#include <QLabel>
#include <QRectF>
#include <QRubberBand>
#include <QRegExp>

class PopplerPDFWidget : public QLabel {
        Q_OBJECT
    public:

        enum SelectionMode {
            SelectRectangle=0,
            SelectText=1
        };

        explicit PopplerPDFWidget(QWidget *parent = 0);
        ~PopplerPDFWidget();

        Poppler::Document *document();
        QMatrix matrix() const;
        qreal scale() const;
        int getCurrentPage() const;
        SelectionMode getSelectionMode() const;

    public slots:
        QRectF searchBackwards(const QString &text);
        QRectF searchForwards(const QString &text);
        bool setDocument(const QString &fileName);
        void setPage(int page = -1);
        void setScale(float scale);
        void setScale(QString scale);
        void nextPage();
        void previousPage();
        void firstPage();
        void lastPage();
        void setSelectText();
        void setSelectRectangle();

    signals:
        void pageChanged(int page, int pages);
        void hasNext(bool hasn);
        void hasPrevious(bool hasp);
        void textSelected(QString text);
        /** \brief emitted, when text selection changes, hasText is true, when the selected text has a size>0 **/
        void hasSelectedText(bool hasText);
        
    public slots:


    protected:
        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void wheelEvent(QWheelEvent *event);

        void showPage(int page = -1);
        void selectedText(const QRectF &rect);

        bool belongsToTextSelection(const QRectF& selectedRect, const QRectF& boundingBox);

        void clearTextSelection();
        void updateTextSelection();

        Poppler::Document *doc;
        int currentPage;
        QPoint dragPosition;
        QRubberBand *rubberBand;
        QRectF searchLocation;
        qreal scaleFactor;
        SelectionMode selectionMode;
        QList<QRubberBand*> textSelectionRectangles;
        QRectF selectionRect;
        QString m_selectedText;
};

#endif // POPPLERPDFWIDGET_H
