#include "qfilenameslistedit.h"
#include <QtGui>
#include <QtCore>




QFilenamesListEditModel::QFilenamesListEditModel(const QStringList &strings, QObject *parent):
    QStringListModel(strings, parent)
{
    iconProvider=new QFileIconProvider();
    setStringList(strings);
}

QFilenamesListEditModel::QFilenamesListEditModel(QObject *parent):
    QStringListModel(parent)
{
    iconProvider=new QFileIconProvider();
}

QVariant QFilenamesListEditModel::data(const QModelIndex &index, int role) const {
    QVariant d=QStringListModel::data(index, role);
    if (role==Qt::DecorationRole) {
        QDir d(m_baseDir);
        QString fn=QFileInfo(d.absoluteFilePath(QStringListModel::data(index, Qt::DisplayRole).toString())).canonicalFilePath();
        //qDebug()<<m_baseDir<<QStringListModel::data(index, Qt::DisplayRole)<<fn;
        return iconProvider->icon(QFileInfo(fn));
    } else {
        return d;
    }
}

Qt::ItemFlags QFilenamesListEditModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags defaultFlags = QStringListModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsEditable | defaultFlags;
    else
        return defaultFlags | Qt::ItemIsDropEnabled;
}



void QFilenamesListEditModel::setBaseDir(const QString &baseDir) {
    m_baseDir=baseDir;
    reset();
}

void QFilenamesListEditModel::addNewFile(const QString &filename) {
    if (!index(rowCount()-1, 0).data().toString().isEmpty()) insertRow(rowCount());
    QDir d(m_baseDir);
    setData(index(rowCount()-1, 0), d.relativeFilePath(filename));
}

Qt::DropActions QFilenamesListEditModel::supportedDropActions() const {
    return Qt::MoveAction;
}









QFilenamesListEditDelegate::QFilenamesListEditDelegate(QObject *parent, QWidget *focusWidget):
    QItemDelegate(parent)
{
    pixOpen=QIcon(":/QFilenamesListEdit/openfile.png");
    pixExecute=QIcon(":/QFilenamesListEdit/execute.png");
    pixDelete=QIcon(":/QFilenamesListEdit/delete.png");
    pixAdd=QIcon(":/QFilenamesListEdit/add.png");
    pixWWW=QIcon(":/QFilenamesListEdit/www.png");
    pixUp=QIcon(":/QFilenamesListEdit/up.png");
    pixDown=QIcon(":/QFilenamesListEdit/down.png");
    pixWidth=16;
    pixDistance=3;
    this->focusWidget=focusWidget;

}

QFilenamesListEditDelegate::~QFilenamesListEditDelegate() {
}

int QFilenamesListEditDelegate::buttonXPos(QFilenamesListEditDelegate::Buttons i, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    int right=option.rect.left();
    int dir=+1; //-1
    int offset=0; //1
    //int right=focusWidget->rect().right();
    if (index.row()==index.model()->rowCount()-1) {
        if (i==QFilenamesListEditDelegate::AddButton) return right+dir*(0+offset)*(pixDistance+pixWidth);
        if (i==QFilenamesListEditDelegate::DeleteButton) return right+dir*(1+offset)*(pixDistance+pixWidth);
        if (i==QFilenamesListEditDelegate::OpenButton) return right+dir*(2+offset)*(pixDistance+pixWidth);
        if (i==QFilenamesListEditDelegate::ExecuteButton) return right+dir*(3+offset)*(pixDistance+pixWidth);
        if (i==QFilenamesListEditDelegate::RightOfButtons) return right+dir*(4+offset)*(pixDistance+pixWidth);
        return -1;
    } else {
        if (i==QFilenamesListEditDelegate::DeleteButton) return right+dir*(0+offset)*(pixDistance+pixWidth);
        if (i==QFilenamesListEditDelegate::ExecuteButton) return right+dir*(2+offset)*(pixDistance+pixWidth);
        if (i==QFilenamesListEditDelegate::OpenButton) return right+dir*(1+offset)*(pixDistance+pixWidth);
        if (i==QFilenamesListEditDelegate::RightOfButtons) return right+dir*(3+offset)*(pixDistance+pixWidth);
    }
    return -1;
}


void QFilenamesListEditDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItem option1=option;
    option1.rect.setLeft(buttonXPos(QFilenamesListEditDelegate::RightOfButtons, option, index)+1);
    QItemDelegate::paint(painter, option1, index);
    /*if (index.model()->rowCount()>1) {
        if (index.row()>0) painter->drawPixmap(buttonXPos(0,option), option.rect.top()+option.rect.height()/2-pixWidth/2, pixUp.pixmap(pixWidth, pixWidth/2, QIcon::Normal));
        if (index.row()<index.model()->rowCount()-1) painter->drawPixmap(buttonXPos(0,option), option.rect.top()+option.rect.height()/2, pixDown.pixmap(pixWidth, pixWidth/2, QIcon::Normal));
    }*/
    int x=buttonXPos(QFilenamesListEditDelegate::AddButton, option, index);
    if (x>=0) painter->drawPixmap(x, option.rect.top()+(option.rect.height()-pixWidth)/2, pixAdd.pixmap(pixWidth, QIcon::Normal));
    x=buttonXPos(QFilenamesListEditDelegate::DeleteButton, option, index);
    if (x>=0) painter->drawPixmap(x, option.rect.top()+(option.rect.height()-pixWidth)/2, pixDelete.pixmap(pixWidth, QIcon::Normal));
    x=buttonXPos(QFilenamesListEditDelegate::OpenButton, option, index);
    if (x>=0) painter->drawPixmap(x, option.rect.top()+(option.rect.height()-pixWidth)/2, pixOpen.pixmap(pixWidth, QIcon::Normal));
    QString fn=index.data().toString();
    x=buttonXPos(QFilenamesListEditDelegate::ExecuteButton, option, index);
    if (fn.isEmpty()) painter->drawPixmap(x, option.rect.top()+(option.rect.height()-pixWidth)/2, pixExecute.pixmap(pixWidth, QIcon::Disabled));
    else {
        if (isUrl(fn)) {
            painter->drawPixmap(x, option.rect.top()+(option.rect.height()-pixWidth)/2, pixWWW.pixmap(pixWidth, QIcon::Normal));
        } else {
            painter->drawPixmap(x, option.rect.top()+(option.rect.height()-pixWidth)/2, pixExecute.pixmap(pixWidth, QIcon::Normal));
        }
    }


}

bool QFilenamesListEditDelegate::isUrl(const QString& fn) const {
    return (fn.startsWith("http://") || fn.startsWith("https://") || fn.startsWith("ftp://") || fn.startsWith("ftps://"));
}

QSize QFilenamesListEditDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QSize s= QItemDelegate::sizeHint(option, index);
    s.setHeight(22);
    return s;
}

QWidget *QFilenamesListEditDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    return QItemDelegate::createEditor(parent, option, index);
}

void QFilenamesListEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QItemDelegate::setEditorData(editor, index);
}

void QFilenamesListEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QItemDelegate::setModelData(editor, model, index);
}

void QFilenamesListEditDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QItemDelegate::updateEditorGeometry(editor, option, index);
}

void QFilenamesListEditDelegate::setBaseDir(const QString &baseDir) {
    m_baseDir=baseDir;
}

bool QFilenamesListEditDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    if (event->type()==QEvent::MouseButtonDblClick) {
        QMouseEvent* me=dynamic_cast<QMouseEvent*>(event);
        if (me) {
            if (me->button()==Qt::LeftButton && me->modifiers()==Qt::NoModifier) {
                if ( me->x()>=option.rect.left() && me->x()<=option.rect.left()+option.decorationSize.width()) {
                    QString fn=model->data(index, Qt::DisplayRole).toString();
                    //qDebug()<<"double clicked "<<fn;
                    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                    if (!isUrl(fn)) {
                        QString a=QDir::cleanPath(QDir((m_baseDir.isEmpty())?QDir::currentPath():m_baseDir).absoluteFilePath(fn));
                        QDesktopServices::openUrl(QUrl("file:///"+a, QUrl::TolerantMode));
                    } else {
                        QDesktopServices::openUrl(QUrl(fn, QUrl::TolerantMode));
                    }
                    QApplication::restoreOverrideCursor();
                    if (focusWidget) focusWidget->setFocus();
                    return true;
                }
            }
        }
    } else if (event->type()==QEvent::MouseButtonPress) {
        QMouseEvent* me=dynamic_cast<QMouseEvent*>(event);
        if (me) {
            if (me->button()==Qt::LeftButton && me->modifiers()==Qt::NoModifier) {
                int xDel=buttonXPos(QFilenamesListEditDelegate::DeleteButton, option, index);
                int xAdd=buttonXPos(QFilenamesListEditDelegate::AddButton, option, index);
                int xEx=buttonXPos(QFilenamesListEditDelegate::ExecuteButton, option, index);
                int xOpn=buttonXPos(QFilenamesListEditDelegate::OpenButton, option, index);
                if (xDel>=0 && me->x()>=xDel && me->x()<=xDel+pixWidth) {
                    if (index.model()->rowCount()==1) {
                        model->setData(index, "");
                    } else {
                        model->removeRow(index.row(), index.parent());
                    }
                    if (focusWidget) {
                        focusWidget->setFocus();
                        focusWidget->clearFocus();
                        focusWidget->setFocus();
                    }
                    return true;
                } else if (xOpn>=0 && me->x()>=xOpn && me->x()<=xOpn+pixWidth) {
                    QDir d(m_baseDir);
                    QString fn=model->data(index, Qt::DisplayRole).toString();
                    if (!QFile::exists(d.absoluteFilePath(fn))) fn="";
                    QFileInfo fi(d.absoluteFilePath(fn));
                    QFileDialog dlg(0, tr("Select a file ..."), fi.absoluteDir().canonicalPath(), tr("All Files (*.*)"));
                    if (QFile::exists(d.absoluteFilePath(fn))) dlg.selectFile(fi.canonicalFilePath());
                    dlg.setAcceptMode(QFileDialog::AcceptOpen);
                    dlg.setFileMode(QFileDialog::ExistingFile);
                    if (dlg.exec()==QDialog::Accepted) {
                        QString nfn=dlg.selectedFiles().value(0,fn);
                        model->setData(index, d.relativeFilePath(nfn));
                    }
                    if (focusWidget) {
                        focusWidget->setFocus();
                        focusWidget->clearFocus();
                        focusWidget->setFocus();
                    }
                    return true;
                } else if (xAdd>=0 && me->x()>=xAdd && me->x()<=xAdd+pixWidth) {
                    QDir d(m_baseDir);
                    QFileDialog dlg(0, tr("Select a file ..."), d.absolutePath(), tr("All Files (*.*)"));
                    dlg.selectFile("");
                    dlg.setAcceptMode(QFileDialog::AcceptOpen);
                    dlg.setFileMode(QFileDialog::ExistingFile);
                    if (dlg.exec()==QDialog::Accepted) {
                        QString nfn=dlg.selectedFiles().value(0);
                        model->insertRow(model->rowCount());
                        model->setData(model->index(model->rowCount()-1, 0), d.relativeFilePath(nfn));
                    }
                    if (focusWidget) {
                        focusWidget->setFocus();
                        focusWidget->clearFocus();
                        focusWidget->setFocus();
                    }
                    return true;
                /*} else if (me->x()>=xEx && me->x()<=buttonXPos(0,option)+pixWidth) {
                    if (me->y()>option.rect.top()+option.rect.height()/2) { // move down
                        if (index.row()<index.model()->rowCount()-1) {
                            QString fn1=index.data().toString();
                            QModelIndex idx2=model->index(index.row()+1, index.column());
                            QString fn2=model->data(idx2).toString();
                            model->setData(index, fn2);
                            model->setData(idx2, fn1);
                        }
                    } else { // move up
                        QString fn1=index.data().toString();
                        QModelIndex idx2=model->index(index.row()-1, index.column());
                        QString fn2=model->data(idx2).toString();
                        model->setData(index, fn2);
                        model->setData(idx2, fn1);
                    }
                    return true;*/
                } else if (xEx>=0 && me->x()>=xEx && me->x()<=xEx+pixWidth) {
                    QString fn=model->data(index, Qt::DisplayRole).toString();
                    if (!fn.isEmpty()) {
                        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                        if (!isUrl(fn)) {
                            QString a=QDir::cleanPath(QDir((m_baseDir.isEmpty())?QDir::currentPath():m_baseDir).absoluteFilePath(fn));
                            QDesktopServices::openUrl(QUrl("file:///"+a, QUrl::TolerantMode));
                        } else {
                            QDesktopServices::openUrl(QUrl(fn, QUrl::TolerantMode));
                        }
                        QApplication::restoreOverrideCursor();
                    }
                    if (focusWidget) focusWidget->setFocus();
                    return true;
                }
            }
        }
    }
    return QItemDelegate::editorEvent(event, model, option, index);
}









QFilenamesListEdit::QFilenamesListEdit(QWidget *parent) :
    QWidget(parent)
{
    setAcceptDrops(true);
    m_files=new QFilenamesListEditModel(this);
    m_files->setStringList(QStringList(""));
    QHBoxLayout* vbl=new QHBoxLayout(this);
    vbl->setContentsMargins(0,0,0,0);
    setLayout(vbl);

    //list=new QTableView(this);
    list=new QListView(this);
    delegate=new QFilenamesListEditDelegate(this, list);
    list->setItemDelegate(delegate);
    list->setDragDropOverwriteMode(false);
    list->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);
    //list->setShowGrid(false);
    //list->verticalHeader()->setVisible(false);
    //list->horizontalHeader()->setStretchLastSection(true);
    //list->horizontalHeader()->setVisible(false);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    list->setAlternatingRowColors(true);
    //list->setDropIndicatorShown(true);
    //list->setDragDropMode(QAbstractItemView::InternalMove);
    //list->setDragEnabled(true);
    //list->setAcceptDrops(true);

    list->setModel(m_files);
    vbl->addWidget(list);

    vbl->setStretch(0,1);
    layButton=new QVBoxLayout(this);
    layButton->addStretch();
    vbl->addLayout(layButton);

    list->setFocusPolicy(Qt::StrongFocus);
    //setFocusProxy(list);
    setFocusPolicy(Qt::StrongFocus);

    QFilenamesListEditEventFilter* filt=new QFilenamesListEditEventFilter(this);
    list->installEventFilter(filt);
}

QStringList QFilenamesListEdit::getFiles() const {
    //qDebug()<<"getFiles()  "<<m_files->stringList();
    return m_files->stringList();
}

QString QFilenamesListEdit::getFilesAsString() const {
    //qDebug()<<"getFilesAsString()  "<<getFiles().join("\n");
    return getFiles().join("\n");
}

void QFilenamesListEdit::setBaseDir(const QString &baseDir) {
    m_files->setBaseDir(baseDir);
    delegate->setBaseDir(baseDir);
}

void QFilenamesListEdit::addButton(QAbstractButton *button) {
    layButton->insertWidget(layButton->count()-1, button);
}

void QFilenamesListEdit::focusOut() {
    //focusOutEvent(new QFocusEvent(QEvent::FocusOut, Qt::OtherFocusReason));
    setFocus();
    clearFocus();
}

void QFilenamesListEdit::setFilesFromString(const QString &files) {
    QStringList sl=files.split('\n');
    for (int i=sl.count()-1; i>=0; i--) {
        if (sl[i].isEmpty()) sl.removeAt(i);
    }
    setFiles(sl);
}

void QFilenamesListEdit::setFiles(const QStringList &list) {
    if (list.size()<=0) m_files->setStringList(QStringList(""));
    else m_files->setStringList(list);
}

void QFilenamesListEdit::addNewFile(const QString &filename) {
    m_files->addNewFile(filename);
}

void QFilenamesListEdit::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();

    // check for our needed mime type, here a file or a list of files
    if (mimeData->hasUrls())
    {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();

        // extract the local paths of the files
        for (int i = 0; i < urlList.size(); ++i) {
            QString filename=urlList.at(i).toLocalFile();
            if (QFile::exists(filename)) {
                //pathList.append(urlList.at(i).toLocalFile());
                addNewFile(filename);
            }
        }
        setFocus();
        clearFocus();
        setFocus();

    }

}


void QFilenamesListEdit::dragEnterEvent(QDragEnterEvent* event) {
    const QMimeData* mimeData = event->mimeData();
    // if some actions should not be usable, like move, this code must be adopted
    if (mimeData->hasUrls()) event->acceptProposedAction();
}

void QFilenamesListEdit::dragMoveEvent(QDragMoveEvent* event) {
    const QMimeData* mimeData = event->mimeData();
    // if some actions should not be usable, like move, this code must be adopted
    if (mimeData->hasUrls())event->acceptProposedAction();
}


QFilenamesListEditEventFilter::QFilenamesListEditEventFilter(QFilenamesListEdit *parent):
    QObject(parent)
{
    this->fnlist=parent;
}

bool QFilenamesListEditEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::FocusOut) {
        //qDebug()<<"focus out";
        fnlist->focusOut();
    }
    return QObject::eventFilter(obj, event);
}
/*
bool KeyPressEater::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        qDebug("Ate key press %d", keyEvent->key());
        return true;
    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}
*/
