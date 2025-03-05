#ifndef QQEnhancedPdfView_H
#define QQEnhancedPdfView_H

#include <QPdfView>
#include <QMouseEvent>
#include <QRubberBand>

class QEnhancedPdfView : public QPdfView {
    Q_OBJECT
public:
    explicit QEnhancedPdfView(QWidget *parent = nullptr);

    QString selectedText() const;
signals:
    void textSelected(const QString& text);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QPoint dragStartPos;
    QString selText;
    QList<QPolygonF> selPoly;
};

#endif // QQEnhancedPdfView_H
