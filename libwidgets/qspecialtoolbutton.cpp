#include "qspecialtoolbutton.h"

QWWWToolButton::QWWWToolButton(QWidget* parent):
    QToolButton(parent)
{
    this->connect(this, SIGNAL(clicked()), this, SLOT(openBuddyContents()));
    setIcon(QIcon(tr(":/wwwtoolbutton.png")));
}

QWWWToolButton::~QWWWToolButton()
{
    disconnect(this, SIGNAL(clicked()), this, SLOT(openBuddyContents()));
}

void QWWWToolButton::openBuddyContents() {
    if (m_buddy==NULL) return;
    QString a=m_buddy->text();
    QDesktopServices::openUrl(QUrl(a, QUrl::TolerantMode));
}



QFileSelectToolButton::QFileSelectToolButton(QWidget* parent):
    QToolButton(parent)
{
    this->connect(this, SIGNAL(clicked()), this, SLOT(openBuddyContents()));
    setIcon(QIcon(tr(":/fileselecttoolbutton.png")));
}

QFileSelectToolButton::~QFileSelectToolButton()
{
    disconnect(this, SIGNAL(clicked()), this, SLOT(openBuddyContents()));
}

void QFileSelectToolButton::openBuddyContents() {
    if (m_buddy==NULL) return;
    static QString dir="";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select File ..."), dir, tr("All Files (*.*)"));
    if (!fileName.isEmpty()) {
        dir=QFileInfo(fileName).dir().absolutePath();

        m_buddy->setText(QDir((baseDir.isEmpty())?QDir::currentPath():baseDir).relativeFilePath( fileName));
        m_buddy->setFocus(Qt::MouseFocusReason);
    }
}



QFileExecuteToolButton::QFileExecuteToolButton(QWidget* parent):
    QToolButton(parent)
{
    this->connect(this, SIGNAL(clicked()), this, SLOT(openBuddyContents()));
    setIcon(QIcon(tr(":/executetoolbutton.png")));
}

QFileExecuteToolButton::~QFileExecuteToolButton()
{
    disconnect(this, SIGNAL(clicked()), this, SLOT(openBuddyContents()));
}

void QFileExecuteToolButton::openBuddyContents() {
    if (m_buddy==NULL) return;
    QString a=QDir::cleanPath (QDir((baseDir.isEmpty())?QDir::currentPath():baseDir).absoluteFilePath( m_buddy->text()));
    //std::cout<<"executing: "<<a.toStdString()<<std::endl;
    QDesktopServices::openUrl(QUrl("file:///"+a, QUrl::TolerantMode));
}

