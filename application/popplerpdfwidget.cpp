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

#include "popplerpdfwidget.h"
#include <QPainter>
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtCore>

PopplerPDFWidget::PopplerPDFWidget(QWidget *parent) :
    QLabel(parent)
{
    currentPage = -1;
    doc = 0;
    rubberBand = 0;
    scaleFactor = 1.0;
    setAlignment(Qt::AlignCenter);
    setMouseTracking(true);
    selectionMode=PopplerPDFWidget::SelectText;
    setCursor(Qt::IBeamCursor);
}

PopplerPDFWidget::~PopplerPDFWidget() {
    clearTextSelection();
    delete doc;
}

Poppler::Document *PopplerPDFWidget::document() {
    return doc;
}

QMatrix PopplerPDFWidget::matrix() const {
    return QMatrix(scaleFactor * physicalDpiX() / 72.0, 0, 0, scaleFactor * physicalDpiY() / 72.0, 0, 0);
}

qreal PopplerPDFWidget::scale() const {
    return scaleFactor;
}

int PopplerPDFWidget::getCurrentPage() const
{
    return currentPage+1;
}

PopplerPDFWidget::SelectionMode PopplerPDFWidget::getSelectionMode() const
{
    return selectionMode;
}

bool PopplerPDFWidget::setDocument(const QString &fileName) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    clear();

    Poppler::Document *oldDocument = doc;
    QFile f(fileName);
    if (f.open( QIODevice::ReadOnly)) {
        QByteArray pdfdata=f.readAll();
        f.close();
        doc=Poppler::Document::loadFromData(pdfdata);
        //doc = Poppler::Document::load(fileName);
        if (doc) {
            if (oldDocument) delete oldDocument;
            doc->setRenderHint(Poppler::Document::Antialiasing);
            doc->setRenderHint(Poppler::Document::TextAntialiasing);
            searchLocation = QRectF();
            currentPage = -1;
            setPage(1);
        }
    }

    QApplication::restoreOverrideCursor();

    return doc != 0;
}

void PopplerPDFWidget::setPage(int page) {
    if (page != currentPage + 1) {
        searchLocation = QRectF();
        showPage(page);
    }
}

void PopplerPDFWidget::setScale(float scale) {
    if (scaleFactor != scale) {
        scaleFactor = scale;
        showPage(currentPage+1);
    }
}

void PopplerPDFWidget::setScale(QString scale) {
    QString d=scale.trimmed();
    QRegExp rx("(\\d+)\\s*\\%");
    int pos = rx.indexIn(d);
    if (pos > -1) {
        setScale(rx.cap(1).toDouble()/100.0);
    }
}

void PopplerPDFWidget::nextPage() {
    showPage(currentPage+1+1);
}

void PopplerPDFWidget::previousPage() {
    showPage(currentPage-1+1);
}

void PopplerPDFWidget::firstPage() {
    showPage(1);
}

void PopplerPDFWidget::lastPage() {
    if (!doc) return;
    showPage(doc->numPages());
}

void PopplerPDFWidget::setSelectText()
{
    selectionMode=PopplerPDFWidget::SelectText;
    clearTextSelection();
    if (rubberBand) delete rubberBand;
    rubberBand=NULL;
    setCursor(Qt::IBeamCursor);
    update();
}

void PopplerPDFWidget::setSelectRectangle()
{
    selectionMode=PopplerPDFWidget::SelectRectangle;
    clearTextSelection();
    if (rubberBand) delete rubberBand;
    rubberBand=NULL;
    setCursor(Qt::ArrowCursor);
    update();
}

void PopplerPDFWidget::mousePressEvent(QMouseEvent *event) {

    if (doc && event->button()==Qt::LeftButton){
        if (selectionMode==PopplerPDFWidget::SelectRectangle) {
            clearTextSelection();
            dragPosition = event->pos();
            if (!rubberBand)
                rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
            rubberBand->setGeometry(QRect(dragPosition, dragPosition));
            rubberBand->show();
        } else if (selectionMode==PopplerPDFWidget::SelectText) {
            clearTextSelection();
            dragPosition = event->pos();
            selectionRect=QRectF(dragPosition, dragPosition);
            updateTextSelection();
        }
        event->accept();
    } else {
        QLabel::mousePressEvent(event);
    }
}

void PopplerPDFWidget::mouseMoveEvent(QMouseEvent *event) {
    //qDebug()<<event->button()<<event->buttons()<<Qt::LeftButton;
    if (doc && event->buttons()==Qt::LeftButton){
        if (selectionMode==PopplerPDFWidget::SelectRectangle) {
            if (!rubberBand)
                return;
            if (event->buttons()==Qt::LeftButton) rubberBand->setGeometry(QRect(dragPosition, event->pos()).normalized());
        } else if (selectionMode==PopplerPDFWidget::SelectText) {
            if (event->buttons()==Qt::LeftButton) {
                selectionRect=QRect(dragPosition, event->pos()).normalized();
                updateTextSelection();
            }
        }
        event->accept();
    } else {
        QLabel::mouseMoveEvent(event);
    }
}

void PopplerPDFWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (doc && event->buttons()==Qt::LeftButton){
        if (selectionMode==PopplerPDFWidget::SelectRectangle) {
            if (!rubberBand)
                return;

            if (!rubberBand->size().isEmpty()) {
                // Correct for the margin around the image in the label.
                QRectF rect = QRectF(rubberBand->pos(), rubberBand->size());
                rect.moveLeft(rect.left() - (width() - pixmap()->width()) / 2.0);
                rect.moveTop(rect.top() - (height() - pixmap()->height()) / 2.0);
                selectedText(rect);
            }
        } else if (selectionMode==PopplerPDFWidget::SelectText) {
            if (event->buttons()==Qt::LeftButton) {
                if (selectionRect.size().isEmpty()) {
                    clearTextSelection();
                }
                updateTextSelection();
            }
        }
        event->accept();
    } else {
        QLabel::mouseReleaseEvent(event);
    }

}

void PopplerPDFWidget::showPage(int page) {
    if (page != -1 && page != currentPage + 1) {
        currentPage = page - 1;
        //emit pageChanged(page);
    }



    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    selectionRect=QRect();
    dragPosition=QPoint();
    clearTextSelection();

    Poppler::Page* ppage=doc->page(currentPage);
    QImage image;
    if (ppage) image= ppage->renderToImage(scaleFactor * physicalDpiX(), scaleFactor * physicalDpiY());

    if (!searchLocation.isEmpty()) {
        QRect highlightRect = matrix().mapRect(searchLocation).toRect();
        highlightRect.adjust(-2, -2, 2, 2);
        QImage highlight = image.copy(highlightRect);
        QPainter painter;
        painter.begin(&image);
        painter.fillRect(image.rect(), QColor(0, 0, 0, 64));
        painter.drawImage(highlightRect, highlight);
        painter.end();
    }

    setPixmap(QPixmap::fromImage(image));
    emit pageChanged(page, doc->numPages());
    emit hasNext(page<doc->numPages());
    emit hasPrevious(page>1);
    QApplication::restoreOverrideCursor();
}

void PopplerPDFWidget::selectedText(const QRectF &rect) {
    QRectF selectedRect = matrix().inverted().mapRect(rect);
    // QString text = doc->page(currentPage)->text(selectedRect);

    QString text;
    bool hadSpace = false;
    QPointF center;
    foreach (Poppler::TextBox *box, doc->page(currentPage)->textList()) {
        if (belongsToTextSelection(selectedRect, box->boundingBox())) {
            if (hadSpace)
                text += " ";
            if (!text.isEmpty() && box->boundingBox().top() > center.y())
                text += "\n";
            text += box->text();
            hadSpace = box->hasSpaceAfter();
            center = box->boundingBox().center();
        }
    }

    //if (!text.isEmpty())
    emit textSelected(text);
    emit hasSelectedText(!text.isEmpty());
    m_selectedText=text;
}

bool PopplerPDFWidget::belongsToTextSelection(const QRectF &selectedRect, const QRectF &boundingBox)
{
    //qDebug()<<"belongsToTextSelection("<<selectedRect<<boundingBox<<"): "<<selectedRect.contains(boundingBox)<<selectedRect.intersects(boundingBox);
    return selectedRect.contains(boundingBox) ||
           (selectedRect.intersects(boundingBox) && boundingBox.right()<=selectedRect.right());
}

void PopplerPDFWidget::clearTextSelection()
{
    for (int i=0; i<textSelectionRectangles.size(); i++) {
        if (textSelectionRectangles[i]) delete textSelectionRectangles[i];
    }
    textSelectionRectangles.clear();
}

void PopplerPDFWidget::updateTextSelection()
{
    //qDebug()<<"PopplerPDFWidget::updateTextSelection(): selectionRect="<<selectionRect<<"   dragPosition="<<dragPosition;
    QRectF rin=selectionRect;
    rin.moveLeft(rin.left() - (width() - pixmap()->width()) / 2.0);
    rin.moveTop(rin.top() - (height() - pixmap()->height()) / 2.0);

    QRectF selectedRect = matrix().inverted().mapRect(rin);
    // QString text = doc->page(currentPage)->text(selectedRect);

    QString text;
    bool hadSpace = false;
    QPointF center;
    bool upd=updatesEnabled();
    if (upd) setUpdatesEnabled(false);
    clearTextSelection();
    foreach (Poppler::TextBox *box, doc->page(currentPage)->textList()) {
        //if (selectedRect.right()>box->boundingBox().left() && selectedRect.bottom()>box->boundingBox().top()) {
        //qDebug()<<"  check rects: "<<box->boundingBox()<<selectedRect<<selectedRect.contains(box->boundingBox())<<selectedRect.intersects(box->boundingBox());
        if (belongsToTextSelection(selectedRect, box->boundingBox())) {
            if (hadSpace)
                text += " ";
            if (!text.isEmpty() && box->boundingBox().top() > center.y())
                text += "\n";
            text += box->text();
            hadSpace = box->hasSpaceAfter();
            center = box->boundingBox().center();

            const QRectF popplerTextBoxRect = matrix().mapRect(box->boundingBox());
            QRubberBand* rb = new QRubberBand(QRubberBand::Rectangle, this);
            rb->setGeometry(QRect(popplerTextBoxRect.left()+(width() - pixmap()->width()) / 2.0, popplerTextBoxRect.top()+(height() - pixmap()->height()) / 2.0,popplerTextBoxRect.width(), popplerTextBoxRect.height()));
            rb->show();
            textSelectionRectangles.append(rb);

        }
    }
    if (upd) setUpdatesEnabled(upd);
    //if (!text.isEmpty())
    emit textSelected(text);
    emit hasSelectedText(!text.isEmpty());
    m_selectedText=text;
}


void PopplerPDFWidget::wheelEvent(QWheelEvent *event) {
    int numDegrees = event->delta() / 8;
    double numSteps = numDegrees / 15;

    if (event->modifiers()==Qt::ControlModifier && event->orientation() == Qt::Vertical) {
        if (numSteps>0) setScale(scale()*pow(1.3, numSteps));
        if (numSteps<0) setScale(scale()*pow(0.8, -numSteps));
        event->accept();
    } else {
        event->ignore();
    }
}
QRectF PopplerPDFWidget::searchBackwards(const QString &text)
{
    if (text.isEmpty()) {
        searchLocation=QRectF(0,0,0,0);
        showPage(currentPage);
    }
    QRectF oldLocation = searchLocation;

    int page = currentPage;
    if (oldLocation.isNull())
        page -= 1;

    while (page > -1) {

        QList<QRectF> locations;
        searchLocation = QRectF();

        double l=searchLocation.left();
        double t=searchLocation.top();
        double r=searchLocation.right();
        double b=searchLocation.bottom();

        while (doc->page(page)->search(text, l,t,r,b,
            Poppler::Page::NextResult, Poppler::Page::CaseInsensitive)) {
            searchLocation.setLeft(l);
            searchLocation.setRight(r);
            searchLocation.setBottom(b);
            searchLocation.setTop(t);
            if (searchLocation != oldLocation)
                locations.append(searchLocation);
            else
                break;
        }
        searchLocation.setLeft(l);
        searchLocation.setRight(r);
        searchLocation.setBottom(b);
        searchLocation.setTop(t);
        int index = locations.indexOf(oldLocation);
        if (index == -1 && !locations.isEmpty()) {
            searchLocation = locations.last();
            showPage(page + 1);
            return searchLocation;
        } else if (index > 0) {
            searchLocation = locations[index - 1];
            showPage(page + 1);
            return searchLocation;
        }

        oldLocation = QRectF();
        page -= 1;
    }

    if (currentPage == doc->numPages() - 1)
        return QRectF();

    oldLocation = QRectF();
    page = doc->numPages() - 1;

    while (page > currentPage) {

        QList<QRectF> locations;
        searchLocation = QRectF();
        double l=searchLocation.left();
        double t=searchLocation.top();
        double r=searchLocation.right();
        double b=searchLocation.bottom();

        while (doc->page(page)->search(text, l,t,r,b,
            Poppler::Page::NextResult, Poppler::Page::CaseInsensitive)) {
            searchLocation.setLeft(l);
            searchLocation.setRight(r);
            searchLocation.setBottom(b);
            searchLocation.setTop(t);
            locations.append(searchLocation);
        }
        searchLocation.setLeft(l);
        searchLocation.setRight(r);
        searchLocation.setBottom(b);
        searchLocation.setTop(t);
        if (!locations.isEmpty()) {
            searchLocation = locations.last();
            showPage(page + 1);
            return searchLocation;
        }
        page -= 1;
    }

    return QRectF();
}

QRectF PopplerPDFWidget::searchForwards(const QString &text)
{
    if (text.isEmpty()) {
        searchLocation=QRectF(0,0,0,0);
        showPage(currentPage);
    }
    int page = currentPage;
    while (page < doc->numPages()) {
        double l=searchLocation.left();
        double t=searchLocation.top();
        double r=searchLocation.right();
        double b=searchLocation.bottom();

        if (doc->page(page)->search(text, l,t,r,b,
            Poppler::Page::NextResult, Poppler::Page::CaseInsensitive)) {
            searchLocation.setLeft(l);
            searchLocation.setRight(r);
            searchLocation.setBottom(b);
            searchLocation.setTop(t);
            if (!searchLocation.isNull()) {
                showPage(page + 1);
                return searchLocation;
            }
        }
        page += 1;
        searchLocation = QRectF();
    }

    page = 0;

    while (page < currentPage) {

        searchLocation = QRectF();
        double l=searchLocation.left();
        double t=searchLocation.top();
        double r=searchLocation.right();
        double b=searchLocation.bottom();

        if (doc->page(page)->search(text, l,t,r,b,
            Poppler::Page::NextResult, Poppler::Page::CaseInsensitive)) {
            searchLocation.setLeft(l);
            searchLocation.setRight(r);
            searchLocation.setBottom(b);
            searchLocation.setTop(t);
            if (!searchLocation.isNull()) {
                showPage(page + 1);
                return searchLocation;
            }
        }
        page += 1;
    }

    return QRectF();
}
