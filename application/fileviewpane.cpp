#include "fileviewpane.h"

static const int PageRole = Qt::UserRole + 1;

static void fillToc(Poppler::Document *doc, const QDomNode &parent, QTreeWidget *tree, QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem *newitem = 0;
    for (QDomNode node = parent.firstChild(); !node.isNull(); node = node.nextSibling()) {
        QDomElement e = node.toElement();

        if (!parentItem) {
            newitem = new QTreeWidgetItem(tree, newitem);
        } else {
            newitem = new QTreeWidgetItem(parentItem, newitem);
        }
        double pageNumber =0 ;
        if (e.hasAttribute("Destination")) {
            pageNumber=Poppler::LinkDestination(e.attribute(QString::fromLatin1("Destination"))).pageNumber();
        }

        Poppler::LinkDestination *dest = doc->linkDestination(e.attribute(QString::fromLatin1("DestinationName")));
        pageNumber = dest->pageNumber();// + dest->top();
        delete dest;


        newitem->setText(0, e.tagName());
        newitem->setData(0, PageRole, pageNumber+1.0);

        bool isOpen = false;
        if (e.hasAttribute(QString::fromLatin1("Open"))) {
            isOpen = QVariant(e.attribute(QString::fromLatin1("Open"))).toBool();
        }
        if (isOpen) {
            tree->expandItem(newitem);
        }

        if (e.hasChildNodes()) {
            fillToc(doc, node, tree, newitem);
        }
    }
}




FileViewPane::FileViewPane(ProgramOptions* settings, QWidget *parent) :
    QWidget(parent)
{
    m_currentRecord=-1;
    basedir=QApplication::applicationDirPath();
    resize(300, 200);
    this->settings=settings;
    datastore=NULL;
    connected=false;

    grid=new QGridLayout();
    setLayout(grid);


    actFirst=new QAction(QIcon(":/pdf_first.png"), tr("go to first page"), this);
    actPrevieous=new QAction(QIcon(":/pdf_previous.png"), tr("go to previous page"), this);
    actNext=new QAction(QIcon(":/pdf_next.png"), tr("go to next page"), this);
    actLast=new QAction(QIcon(":/pdf_last.png"), tr("go to last page"), this);
    actZoomIn=new QAction(QIcon(":/pdf_zoomin.png"), tr("zoom one step in"), this);
    actZoomOut=new QAction(QIcon(":/pdf_zoomout.png"), tr("zoom one step out"), this);
    actFindDown=new QAction(QIcon(":/pdf_finddown.png"), tr("find downwards"), this);
    actFindUp=new QAction(QIcon(":/pdf_findup.png"), tr("find upwards"), this);


    toolbar=new QToolBar("tbPDF", this);
    cmbDocument=new QComboBox(this);
    cmbDocument->setEditable(false);
    cmbDocument->setMinimumWidth(150);
    cmbDocument->setMaximumWidth(300);
    toolbar->addWidget(new QLabel(tr("file: ")));
    toolbar->addWidget(cmbDocument);
    toolbar->addSeparator();

    spinPage=new QSpinBox(this);
    toolbar->addAction(actFirst);
    toolbar->addAction(actPrevieous);
    toolbar->addWidget(spinPage);
    toolbar->addAction(actNext);
    toolbar->addAction(actLast);
    toolbar->addSeparator();

    cmbScale=new QComboBox(this);
    cmbScale->setValidator(new QRegExpValidator(QRegExp("\\d+ \\%"), this));
    cmbScale->addItem("10 %");
    cmbScale->addItem("25 %");
    cmbScale->addItem("50 %");
    cmbScale->addItem("75 %");
    cmbScale->addItem("100 %");
    cmbScale->addItem("125 %");
    cmbScale->addItem("150 %");
    cmbScale->addItem("175 %");
    cmbScale->addItem("200 %");
    cmbScale->addItem("225 %");
    cmbScale->addItem("250 %");
    cmbScale->addItem("300 %");
    cmbScale->addItem("400 %");
    cmbScale->addItem("500 %");
    cmbScale->setEditable(true);
    cmbScale->setCurrentIndex(4);

    toolbar->addWidget(new QLabel(tr("scale: ")));
    toolbar->addWidget(cmbScale);
    toolbar->addAction(actZoomIn);
    toolbar->addAction(actZoomOut);

    toolbar->addSeparator();
    edtFind=new QEnhancedLineEdit(this);
    edtFind->setMinimumWidth(150);
    toolbar->addWidget(new QLabel(tr("search document: ")));
    toolbar->addWidget(edtFind);
    toolbar->addAction(actFindDown);
    toolbar->addAction(actFindUp);


    scroll=new QScrollArea(this);

    pdf=new PopplerPDFWidget(this);
    scroll->setWidgetResizable(true);
    scroll->setWidget(pdf);

    QTabWidget* tabDocument=new QTabWidget(this);
    tabDocument->setTabPosition(QTabWidget::West);

    toc=new QTreeWidget(this);
    tabDocument->addTab(toc, tr("Contents"));
    toc->setAlternatingRowColors(true);
    toc->header()->hide();
    toc->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    thumbnails = new QListWidget(this);
    thumbnails->setViewMode(QListView::ListMode);
    thumbnails->setMovement(QListView::Static);
    thumbnails->setVerticalScrollMode(QListView::ScrollPerPixel);
    tabDocument->addTab(thumbnails, tr("Thumbnails"));


    splitter=new QSplitter(this);
    splitter->addWidget(tabDocument);
    splitter->addWidget(scroll);
    QList<int> sizes;
    splitter->setStretchFactor(0,1);
    splitter->setStretchFactor(1,5);
    splitter->setChildrenCollapsible(false);

    grid->addWidget(toolbar, 0, 0);
    grid->addWidget(splitter, 1, 0);

    connect(spinPage, SIGNAL(valueChanged(int)), pdf, SLOT(setPage(int)));
    connect(cmbScale, SIGNAL(editTextChanged(QString)), this, SLOT(scaleChanged(QString)));
    connect(pdf, SIGNAL(pageChanged(int,int)), this, SLOT(pageChanged(int,int)));
    connect(actFirst, SIGNAL(triggered()), pdf, SLOT(firstPage()));
    connect(actPrevieous, SIGNAL(triggered()), pdf, SLOT(previousPage()));
    connect(actNext, SIGNAL(triggered()), pdf, SLOT(nextPage()));
    connect(actLast, SIGNAL(triggered()), pdf, SLOT(lastPage()));
    connect(pdf, SIGNAL(hasNext(bool)), actNext, SLOT(setEnabled(bool)));
    connect(pdf, SIGNAL(hasNext(bool)), actLast, SLOT(setEnabled(bool)));
    connect(pdf, SIGNAL(hasPrevious(bool)), actPrevieous, SLOT(setEnabled(bool)));
    connect(pdf, SIGNAL(hasPrevious(bool)), actFirst, SLOT(setEnabled(bool)));
    connect(actZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(actZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
    connect(splitter, SIGNAL(splitterMoved(int,int)), this, SLOT(writeSettings()));
    connect(actFindDown, SIGNAL(triggered()), this, SLOT(findDown()));
    connect(edtFind, SIGNAL(returnPressed()), this, SLOT(findDown()));
    connect(actFindUp, SIGNAL(triggered()), this, SLOT(findUp()));
    connect(cmbDocument, SIGNAL(currentIndexChanged(int)), this, SLOT(docComboChanged(int)));
    connect(toc, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(tocItemActivated(QTreeWidgetItem*,int)));
    connect(thumbnails, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(thumbnailItemActivated(QListWidgetItem*)));

    readSettings();
}

void FileViewPane::connectWidgets(LS3Datastore *datastore) {
    connected=true;
    m_currentRecord=-1;
    if (this->datastore) disconnect(this->datastore, SIGNAL(currentRecordChanged(int)), this, SLOT(currentRecordChanged(int)));
    if (this->datastore) disconnect(this->datastore, SIGNAL(dataChanged(int,int)), this, SLOT(dataChanged(int,int)));
    this->datastore=datastore;
    setEnabled(false);

    if (this->datastore) {
        currentRecordChanged(this->datastore->currentRecordNum());
        connect(this->datastore, SIGNAL(currentRecordChanged(int)), this, SLOT(currentRecordChanged(int)));
        connect(this->datastore, SIGNAL(dataChanged(int,int)), this, SLOT(dataChanged(int,int)));
    }
    //datastore->addMapping(cmbOrigin, "origin", "text");
}

void FileViewPane::disconnectWidgets() {
    if (datastore) {
        //datastore->removeMapping(cmbOrigin);
        disconnect(datastore, SIGNAL(currentRecordChanged(int)), this, SLOT(currentRecordChanged(int)));
        disconnect(this->datastore, SIGNAL(dataChanged(int,int)), this, SLOT(dataChanged(int,int)));
    }
    datastore=NULL;
    connected=false;
}

void FileViewPane::setBaseDir(QString bd)
{
    basedir=bd;
}

void FileViewPane::currentRecordChanged(int currentRecord) {
    m_currentRecord=currentRecord;
    QStringList files=datastore->getField("files").toString().split('\n');
    setEnabled(false);
    toc->clear();
    pdf->setText(tr("<b>no PDF file in record available</b>"));
    disconnect(cmbDocument, SIGNAL(currentIndexChanged(int)), this, SLOT(docComboChanged(int)));
    cmbDocument->clear();
    QDir d(basedir);
    for (int i=0; i<files.size(); i++) {
        QString file=d.absoluteFilePath(files[i]);
        if (QFile::exists(file) && QFileInfo(file).suffix().toLower()=="pdf" ) {
            cmbDocument->addItem(QString("#%1: %2").arg(cmbDocument->count()+1).arg(QFileInfo(files[i]).fileName()), file);
        }
    }
    cmbDocument->setCurrentIndex(0);
    connect(cmbDocument, SIGNAL(currentIndexChanged(int)), this, SLOT(docComboChanged(int)));
    docComboChanged(0);

}

void FileViewPane::scaleChanged(QString scale) {
    pdf->setScale(scale);
}

void FileViewPane::pageChanged(int page, int pages) {
    spinPage->setRange(1,pages);
    spinPage->setSuffix(QString(" / %1").arg(pages));
    if (page!=spinPage->value()) spinPage->setValue(page);
}

void FileViewPane::dataChanged(int firstRow, int lastRow) {
    if (m_currentRecord>=firstRow && m_currentRecord<=lastRow) {
        int cmbIdx=cmbDocument->currentIndex();
        int page=pdf->getCurrentPage();
        currentRecordChanged(m_currentRecord);
        cmbDocument->setCurrentIndex(cmbIdx);
        pdf->setPage(page);

    }
}

void FileViewPane::tocItemActivated(QTreeWidgetItem *item, int column) {
    //qDebug()<<"goto "<<item->data(0, PageRole).toDouble();
    pdf->setPage(trunc(item->data(0, PageRole).toDouble()));
}

void FileViewPane::thumbnailItemActivated(QListWidgetItem *item) {
    pdf->setPage(item->data(PageRole).toInt());
}

void FileViewPane::zoomIn() {
    if (cmbScale->currentIndex()+1<cmbScale->count()) cmbScale->setCurrentIndex(cmbScale->currentIndex()+1);
}

void FileViewPane::zoomOut() {
    if (cmbScale->currentIndex()-1>0) cmbScale->setCurrentIndex(cmbScale->currentIndex()-1);
}

void FileViewPane::fillInfoWidgets() {
    const QDomDocument *tocD = NULL;
    if (pdf->document()) tocD=pdf->document()->toc();
    toc->clear();
    if (tocD) {
        fillToc(pdf->document(), *tocD, toc, 0);
        fillThumbnails();
    } else {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, tr("no table of contents"));
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        toc->addTopLevelItem(item);

        thumbnails->clear();
    }

}

void FileViewPane::writeSettings() {
    if (!settings) return;
    settings->GetQSettings()->setValue("pdfpreview/splitter", splitter->saveState());
}

void FileViewPane::readSettings() {
    if (!settings) return;
    splitter->restoreState(settings->GetQSettings()->value("pdfpreview/splitter").toByteArray());
}

void FileViewPane::findDown() {
    QRectF location;
    location = pdf->searchForwards(edtFind->text());
    QPoint target = pdf->matrix().mapRect(location).center().toPoint();
    scroll->ensureVisible(target.x(), target.y());
}

void FileViewPane::findUp() {
    QRectF location;
    location = pdf->searchBackwards(edtFind->text());
    QPoint target = pdf->matrix().mapRect(location).center().toPoint();
    scroll->ensureVisible(target.x(), target.y());

}

void FileViewPane::docComboChanged(int index) {
    QString file;
    QDir d(basedir);
    file=d.absoluteFilePath(cmbDocument->itemData(index).toString());
    if (QFile::exists(file) && QFileInfo(file).suffix().toLower()=="pdf" ) {
        if (pdf->setDocument(file)) {
            fillInfoWidgets();
            setEnabled(true);
            spinPage->setRange(1, pdf->document()->numPages());
            bool ok=true;
            int page=datastore->getField("pdf_lastpage").toInt(&ok);
            if (!ok) page=1;
            spinPage->setValue(page);
        }
    }
}

void FileViewPane::fillThumbnails() {
    const int num = pdf->document()->numPages();
    QSize maxSize;
    for (int i = 0; i < num; ++i) {
        const Poppler::Page *page = pdf->document()->page(i);
        const QImage image = page->thumbnail();
        if (!image.isNull()) {
            QListWidgetItem *item = new QListWidgetItem();
            item->setText(QString::number(i + 1));
            item->setData(Qt::DecorationRole, QPixmap::fromImage(image));
            item->setData(PageRole, i);
            thumbnails->addItem(item);
            maxSize.setWidth(qMax(maxSize.width(), image.width()));
            maxSize.setHeight(qMax(maxSize.height(), image.height()));
        }
        delete page;
    }
    if (num > 0) {
        thumbnails->setGridSize(maxSize);
        thumbnails->setIconSize(maxSize);
    }
}
