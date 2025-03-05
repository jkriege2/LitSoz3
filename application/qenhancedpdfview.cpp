#include "qenhancedpdfview.h"
#include <QPdfDocument>
#include <QPdfSelection>
#include <QPdfPageNavigator>
#include <QPainter>

QEnhancedPdfView::QEnhancedPdfView(QWidget *parent) :
    QPdfView(parent)
{
    setMouseTracking(true);
    //setTextSelectionMode(QPdfView::TextSelectionMode::TextSelectAllowed);
}

QString QEnhancedPdfView::selectedText() const
{
    return selText;
}

void QEnhancedPdfView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragStartPos = event->pos();
    }
    QPdfView::mousePressEvent(event);
}

void QEnhancedPdfView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (document()) {
            const QPdfSelection sel=document()->getSelection(pageNavigator()->currentPage(), dragStartPos, event->pos());
            selPoly=sel.bounds();
        }
    }
    QPdfView::mouseMoveEvent(event);
    repaint();
}

void QEnhancedPdfView::mouseReleaseEvent(QMouseEvent *event) {

    if (event->button() == Qt::LeftButton) {
        if (document()) {
            const QPdfSelection sel=document()->getSelection(pageNavigator()->currentPage(), dragStartPos, event->pos());
            selText=sel.text();
            selPoly=sel.bounds();
        }
        if (!selText.isEmpty()) {
            emit textSelected(selText);
        }
    }
    QPdfView::mouseReleaseEvent(event);
    repaint();
}

void QEnhancedPdfView::paintEvent(QPaintEvent *event)
{
    QPdfView::paintEvent(event);
    
    if (!selPoly.isEmpty()) {
        QPainter painter(viewport());
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 120, 215, 127));  // Semi-transparent blue
        
        // Draw each polygon in the selection
        for (const QPolygonF& polygon : selPoly) {
            painter.drawPolygon(polygon);
        }
    }
}
