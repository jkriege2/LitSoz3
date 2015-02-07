#ifndef PDFDROPDOCKWIDGET_H
#define PDFDROPDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
    class PDFDropDockWidget;
}

class PDFDropDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit PDFDropDockWidget(QWidget *parent = 0);
    ~PDFDropDockWidget();

    void setTypes(const QStringList& names, const QStringList& ids);
    void setCurrentType(QString type);
    QString currentType();
    void setCurrentAction(int act);
    int currentAction();
signals:
    void pdfsDropped(QStringList files, QString type, int action);
    void typeChanged(QString type);
    void actionChanged(int action);
protected slots:
    void on_cmbType_currentIndexChanged(int type);
    void on_cmbFileAction_currentIndexChanged(int type);
protected:
    virtual void dropEvent(QDropEvent* event);
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void dragMoveEvent(QDragMoveEvent* event);
private:
    Ui::PDFDropDockWidget *ui;
};

#endif // PDFDROPDOCKWIDGET_H
