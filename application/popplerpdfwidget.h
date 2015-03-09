#ifndef POPPLERPDFWIDGET_H
#define POPPLERPDFWIDGET_H

#include <poppler-qt5.h>
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
