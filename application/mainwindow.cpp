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

#include "mainwindow.h"
#include "ls3summaryproxymodel.h"
#include "ui_about.h"
#include "ui_aboutplugins.h"
#include "../version.h"

#include <iostream>


MainWindow::MainWindow(ProgramOptions* settings, QSplashScreen* splash):
    QMainWindow(NULL)
{
    dockReferenceData=NULL;
    tabReferenceData=NULL;
    curDisplayStyle=-1;
    tabMain=NULL;
    setIconSize(QSize(16,16));
    splashPix=splash->pixmap();
    connect(this, SIGNAL(showSplashMessage(const QString&)), splash, SLOT(showMessage(const QString&)));
    this->settings=settings;
    datastore=new LS3DatastoreXML(settings, this, this);
    summaryProxy=new LS3SummaryProxyModel(datastore, this);
    tvMainSortProxy=new LS3SummarySortFilterProxyModel(this);
    tvMainSortProxy->setSourceModel(summaryProxy);
    tvMainSortProxy->setDatastore(datastore);
    amazonTimestamp=clock();
    setDockNestingEnabled(true);
    setWindowIcon(QIcon(":/ls3icon.png"));
    init();
    connect(datastore, SIGNAL(dbError(QString)), this, SLOT(showDBError(QString)));
    connect(datastore, SIGNAL(connectWidgets()), this, SLOT(connectWidgets()));
    connect(datastore, SIGNAL(disconnectWidgets()), this, SLOT(disconnectWidgets()));
    connect(datastore, SIGNAL(currentRecordChanged(int)), this, SLOT(currentRecordChanged(int)));
    connect(datastore, SIGNAL(dataChanged(int, int)), this, SLOT(tableDataChanged(int, int)));
    connect(datastore, SIGNAL(filenameChanged(QString)), this, SLOT(setCurrentFile(QString)));
    connect(datastore, SIGNAL(wasChangedChanged(bool)), this, SLOT(wasChanged(bool)));
    connect(settings, SIGNAL(configDataChanged()), this, SLOT(readSettings()));
    connect(settings, SIGNAL(storeSettings()), this, SLOT(writeSettings()));


    if ((!settings->GetStartupFile().isEmpty())&&(QFile::exists(settings->GetStartupFile()))) {
        QTimer::singleShot(100, this, SLOT(loadStartup()));
    } else {
        emit showSplashMessage(tr("READY !!!"));
    }
}

MainWindow::~MainWindow() {
    clearPlugins();
}

void MainWindow::loadStartup() {
    emit showSplashMessage(tr("loading file '%1' ...").arg(settings->GetStartupFile()));
    QApplication::processEvents();
    datastore->dbLoad(settings->GetStartupFile(), progress);
    emit showSplashMessage(tr("READY !!!"));
    QApplication::processEvents();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (maybeSave()) {
        writeSettings();
        datastore->dbClose();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    exportDlg->init(this);
    importDlg->init(this);
}


bool MainWindow::save() {
    QString curFile=datastore->currentFile();
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        QModernProgressDialog pdlg;
        pdlg.setLabelText(tr("saving database '%1' ...").arg(curFile));
        pdlg.setCancelButtonText(tr("&Cancel"));
        pdlg.setHasCancel(false);
        pdlg.openDelayed(500);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        bool ok=datastore->dbSave(curFile, progress);
        pdlg.close();
        QApplication::restoreOverrideCursor();
        return ok;
    }
}

bool MainWindow::saveAs() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Create New Database ..."), settings->GetLastNewDBDirectory(), tr("Literature Databases (*.xld)"));
    if (fileName.isEmpty())
        return false;

    QModernProgressDialog pdlg;
    pdlg.setLabelText(tr("saving database '%1' ...").arg(fileName));
    pdlg.setCancelButtonText(tr("&Cancel"));
    pdlg.setHasCancel(false);
    pdlg.openDelayed(500);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool ok=datastore->dbSave(fileName, progress);
    pdlg.close();
    QApplication::restoreOverrideCursor();
    return ok;

}



bool MainWindow::maybeSave() {
    //qDebug()<<"maybeSaved()   loaded="<<datastore->dbIsLoaded()<<"   wasChanged="<<datastore->wasChanged();
    if (datastore->dbIsLoaded() && datastore->wasChanged()) {
         QMessageBox::StandardButton ret;
         ret = QMessageBox::warning(this, tr("LitSoz 3.0"),
                      tr("The document has been modified.\n"
                         "Do you want to save your changes?"),
                      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
         if (ret == QMessageBox::Save)
             return save();
         else if (ret == QMessageBox::Cancel)
             return false;
    }
    return true;
}

void MainWindow::newFile() {
    if (maybeSave()) {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Create New Database ..."), settings->GetLastNewDBDirectory(), tr("Literature Databases (*.xld)"));
        if (!fileName.isEmpty()) {
            settings->SetLastNewDBDirectory(QFileInfo(fileName).dir().absolutePath());
            statusBar()->showMessage(tr("creating database '%1' ...").arg(fileName), 2000);
            QApplication::setOverrideCursor(Qt::WaitCursor);

            if (datastore->dbNew(fileName)) {
                statusBar()->showMessage(tr("database '%1' created and opened ...").arg(fileName), 2000);
            } else {
                statusBar()->showMessage(tr("error creating database '%1' ...").arg(fileName), 2000);
            }

            QApplication::restoreOverrideCursor();
        }
    }
}

void MainWindow::closeDatabase() {
    if (maybeSave()) {
        datastore->dbClose();
    }
}

void MainWindow::open() {
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open Database ..."), settings->GetLastOpenDBDirectory(), tr("Literature Databases (*.xld)"));
        if (!fileName.isEmpty()) {
            QModernProgressDialog pdlg;
            pdlg.setLabelText(tr("opening database '%1' ...").arg(fileName));
            pdlg.setCancelButtonText(tr("&Cancel"));
            pdlg.setHasCancel(false);
            pdlg.openDelayed(500);
            settings->SetLastOpenDBDirectory(QFileInfo(fileName).dir().absolutePath());
            statusBar()->showMessage(tr("opening database '%1' ...").arg(fileName), 2000);
            QApplication::setOverrideCursor(Qt::WaitCursor);

            if (datastore->dbLoad(fileName, progress)) {
                statusBar()->showMessage(tr("database '%1' opened ...").arg(fileName), 2000);
            } else {
                statusBar()->showMessage(tr("error opening database '%1' ...").arg(fileName), 2000);
            }

            tvMain->selectRow(0);
            pdlg.close();
            QApplication::restoreOverrideCursor();
        }
    }
}

void MainWindow::openRecentFile(const QString &filename) {
    if (maybeSave()) {
        QModernProgressDialog pdlg;
        pdlg.setLabelText(tr("opening database '%1' ...").arg(filename));
        pdlg.setCancelButtonText(tr("&Cancel"));
        pdlg.setHasCancel(false);
        pdlg.openDelayed(500);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        if (datastore->dbLoad(filename, progress)) {
            statusBar()->showMessage(tr("database '%1' opened ...").arg(filename), 2000);
        } else {
            statusBar()->showMessage(tr("error opening database '%1' ...").arg(filename), 2000);
        }
        pdlg.close();
        QApplication::restoreOverrideCursor();
    }
}

void MainWindow::wasChanged(bool changed) {
    //qDebug()<<"wasChanged("<<changed<<")";
    //setCurrentFile(curFile);
    setWindowModified(changed);
    setWindowTitle(tr("LitSoz %2 - %1 [*]").arg(curFile).arg(QApplication::applicationVersion()));
}


void MainWindow::about() {
    QDialog *widget = new QDialog(this);
    Ui::About ui;
    ui.setupUi(widget);
    QTextEdit* ui_textEdit =widget->findChild<QTextEdit*>("edtInfo");
    QLabel* ui_label = widget->findChild<QLabel*>( "labSplash");
    ui_label->setPixmap(splashPix);
    ui_textEdit->setText(tr("<b>Copyright:</b><blockquote>%2</blockquote><b>external libraries, used by %3:</b><ul>"
                            "<li>Qt %1</li>"
                            "<li>Poppler</li>"
                            "<li>Freetype</li>"
                            "<li>LibJpeg</li>"
                            "<li>LibPNG</li>"
                            "<li>LibTIFF</li>"
                            "</ul>").arg(QT_VERSION_STR).arg(LS3_COPYRIGHT).arg(QApplication::applicationName()));
    widget->exec();
    delete widget;
}


void MainWindow::aboutPlugins() {
    QDialog *widget = new QDialog(this);
    Ui::AboutPlugins ui;
    ui.setupUi(widget);
    QTextEdit* ui_textEdit = widget->findChild<QTextEdit*>( "edtInfo");

    QString text="<ul>";

    for (int i=0; i<m_plugins.size(); i++) {
        text+="<li>";
        text+=QString("<b><img src=\"%2\">&nbsp;%1</b>:").arg(m_plugins[i]->getName()).arg(m_plugins[i]->getIconFilename());
        text+="<blockquote>";
        text+=QString("<i>description:</i> %1<br>").arg(m_plugins[i]->getDescription());
        text+=QString("<i>author:</i> %1<br>").arg(m_plugins[i]->getAuthor());
        text+=QString("<i>copyright:</i> %1<br>").arg(m_plugins[i]->getCopyright());
        text+=QString("<i>weblink:</i> %1<br>").arg(m_plugins[i]->getWeblink());
        text+=QString("<i>file:</i> %1<br>").arg(m_pluginFilenames[i]);
        text+="</blockquote>";
        text+="</li>";
    }
    text+="</ul>";
    ui_textEdit->setText(text);
    widget->exec();
    delete widget;
}

void MainWindow::init()
{
    emit showSplashMessage(tr("initializing user interface ..."));
    createWidgets();
    //setCentralWidget(tabReferenceData);
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWidgets();

    emit showSplashMessage(tr("searching plugins ..."));
    searchPlugins(getPluginDirectory());
    initPlugins();

    readSettings();

    setUnifiedTitleAndToolBarOnMac(true);
    setCurrentFile("");

}

void MainWindow::clearPlugins() {
    /*for (int i=0; i<m_importers.size(); i++) {
        delete m_importers[i];
    }
    for (int i=0; i<m_exporters.size(); i++) {
        delete m_exporters[i];
    }*/
    deinitPlugins();
    m_plugins.clear();
    m_pluginFilenames.clear();
    m_importers.clear();
    m_exporters.clear();
}

void MainWindow::searchPlugins(QString directory) {
    clearPlugins();
    QDir pluginsDir = QDir(directory);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            LS3Plugin* iRecord = qobject_cast<LS3Plugin*>(plugin);
            if (iRecord) {
                m_plugins.append(iRecord);
                m_pluginFilenames.append(pluginsDir.relativeFilePath(fileName));
                emit showSplashMessage(tr("loaded plugin '%2' (%1) ...").arg(fileName).arg(iRecord->getName()));
            }
        }
    }
}

void MainWindow::deinitPlugins() {
    for (int i=0; i<m_plugins.size(); i++) {
        m_plugins[i]->deinit(this);
    }
}

void MainWindow::initPlugins() {
    for (int i=0; i<m_plugins.size(); i++) {
        m_plugins[i]->init(this);
    }
}
void MainWindow::createDockWidgets() {

    dockReferenceTree=new ReferenceTreeDockWidget(this);
    dockReferenceTree->setWindowFlags( Qt::WindowStaysOnTopHint);
    viewDockMenu->addAction(dockReferenceTree->toggleViewAction());
    dockReferenceTree->setFloating(true);
    QMainWindow::addDockWidget(Qt::LeftDockWidgetArea, dockReferenceTree);

    QDockWidget* w;
    w=new QDockWidget(tr("Special Characters"), this);
    w->setWindowFlags( Qt::WindowStaysOnTopHint);
    w->setObjectName("dock_special_chars");
    w->setFloating(true);
    kbSpecial=new JKCharacterScreenKeyboard(w);
    w->setWidget(kbSpecial);
    kbSpecial->setKBFocusWidget(this);
    kbSpecial->show();
    viewDockMenu->addAction(w->toggleViewAction());
    QMainWindow::addDockWidget(Qt::RightDockWidgetArea, w);

    lstSelection=new SelectionList(datastore, settings, this);
    w=lstSelection;
    w->setFloating(true);
    w->setObjectName("dock_selection_list");
    viewDockMenu->addAction(w->toggleViewAction());
    QMainWindow::addDockWidget(Qt::LeftDockWidgetArea, w);


    lstSearchResults=new SearchResultList(datastore, settings, this);
    w=lstSearchResults;
    w->setFloating(true);
    w->setObjectName("dock_search_result_list");
    viewDockMenu->addAction(w->toggleViewAction());
    QMainWindow::addDockWidget(Qt::LeftDockWidgetArea, w);

    w=dockThreadOverview=new ThreadOverview(settings, this);
    w->setFloating(true);
    w->setObjectName("dock_running_tasks");
    viewDockMenu->addAction(w->toggleViewAction());
    QMainWindow::addDockWidget(Qt::LeftDockWidgetArea, w);

}

void MainWindow::connecttvMain() {
    //connect(tvMain->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(tableRowChanged(const QModelIndex&, const QModelIndex&)));
    connect(tvMain, SIGNAL(activated(const QModelIndex &)), this, SLOT(tableRowActivted(const QModelIndex &)));
    connect(tvMain, SIGNAL(pressed(const QModelIndex &)), this, SLOT(tableRowActivted(const QModelIndex &)));
    connect(tvMain, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(tableRowDoubleClicked(const QModelIndex &)));
    connect(tvMain->selectionModel(), SIGNAL(currentRowChanged ( const QModelIndex &, const QModelIndex &)), this, SLOT(tableRowActivted(const QModelIndex &)));
    if (dockReferenceData) connect(dbinsertAct, SIGNAL(triggered()), dockReferenceData, SLOT(show()));
}

void MainWindow::disconnecttvMain() {
    //disconnect(tvMain->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(tableRowChanged(const QModelIndex&, const QModelIndex&)));
    disconnect(tvMain, SIGNAL(activated(const QModelIndex &)), this, SLOT(tableRowActivted(const QModelIndex &)));
    disconnect(tvMain, SIGNAL(pressed(const QModelIndex &)), this, SLOT(tableRowActivted(const QModelIndex &)));
    disconnect(tvMain, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(tableRowDoubleClicked(const QModelIndex &)));
    disconnect(tvMain->selectionModel(), SIGNAL(currentRowChanged ( const QModelIndex &, const QModelIndex &)), this, SLOT(tableRowActivted(const QModelIndex &)));
    if (dockReferenceData) disconnect(dbinsertAct, SIGNAL(triggered()), dockReferenceData, SLOT(show()));
}


void MainWindow::connectWidgets() {
    LS3ElapsedAutoTimer timer("MainWindow::connectWidgets()");
    //std::cout<<"\n>>>> connecting widgets ... \n";
    tvMain->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //tvMain->setEditTriggers(QAbstractItemView::SelectedClicked);
    summaryProxy->setSourceModel(NULL);
    tvMainSortProxy->setDatastore(datastore);
    summaryProxy->setSourceModel(datastore->getTableModel());
    if (cmbSorting && cmbSortingDirection) changeSorting();
    tvMain->setModel(tvMainSortProxy);
    for (int i=tvMain->model()->columnCount()-1; i>=0; i--) {
        tvMain->setColumnHidden(i, i>LS3SUMMARYPROXYMODEL_MAX_COLUMN_VISIBLE);
    }
    tvMain->resizeColumnsToContents();
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    tvMain->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tvMain->horizontalHeader()->setSectionResizeMode(LS3SUMMARYPROXYMODEL_COLUMN_SUMMARY, QHeaderView::Stretch);
    #else
    tvMain->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    tvMain->horizontalHeader()->setResizeMode(LS3SUMMARYPROXYMODEL_COLUMN_SUMMARY, QHeaderView::Stretch);
    #endif


    QFontMetrics fm(tvMain->font());
    tvMain->verticalHeader()->setDefaultSectionSize(qMax(20,(int)round((double)fm.height()*1.3)));
    //tvMain->horizontalHeader()->setStretchLastSection(true);

    tvMain->setVisible(true);
    if (tabReferenceData) tabReferenceData->setVisible(true);
    tvMain->setAlternatingRowColors(true);

    dockReferenceTree->setDatastore(datastore);
    dockReferenceTree->setProxyModel(tvMainSortProxy);
    dockReferenceTree->setSelectionModel(tvMain->selectionModel());


    connecttvMain();

    //std::cout<<"\n>>>>  connecting details widgets ... \n";
    details->connectWidgets(datastore);
    extended->connectWidgets(datastore);
    contents->connectWidgets(datastore);
    fileView->connectWidgets(datastore);

    //std::cout<<"\n>>>>  connecting plugins ... \n";

    for (int i=0; i<m_plugins.size(); i++) {
        m_plugins[i]->openData(datastore);
    }

    //std::cout<<"...done!\n";
}

void MainWindow::disconnectWidgets() {
    LS3ElapsedAutoTimer timer("MainWindow::disconnectWidgets()");
    //std::cout<<"\n>>>> disconnecting widgets ... \n";
    for (int i=0; i<m_plugins.size(); i++) {
        m_plugins[i]->closeData(datastore);
    }

    dockReferenceTree->setProxyModel(NULL);
    dockReferenceTree->setDatastore(NULL);
    dockReferenceTree->setSelectionModel(NULL);


    if (tabReferenceData) tabReferenceData->setVisible(false);
    tvMain->setVisible(false);
    details->disconnectWidgets();
    extended->disconnectWidgets();
    contents->disconnectWidgets();
    fileView->disconnectWidgets();



    disconnecttvMain();

    emit databaseLoaded(false);
}


void MainWindow::createWidgets() {
    searchDlg=new SearchDialog(this);
    searchDlg->hide();
    exportDlg=new ExportDialog(this);
    exportDlg->hide();
    importDlg=new ImportDialog(this);
    importDlg->hide();
    //std::cout<<"createWidgets()";
    splitter=new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);

    tvMain = new QEnhancedTableView(this);
    tvMain->setSelectionBehavior(QAbstractItemView::SelectRows);
    tvMain->setSelectionMode(QAbstractItemView::SingleSelection);
    tvMain->setVisible(false);
    tvMain->setSortingEnabled(false);
    tvMain->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tvMain->setDragEnabled(true);
    //tvMain->setItemDelegate(new QStyledItemDelegate(tvMain));
    QFontMetrics fm(tvMain->font());
    tvMain->verticalHeader()->setDefaultSectionSize((int)round((double)fm.height()*1.1));
    tvMain->horizontalHeader()->setStretchLastSection(false);
    tvMain->setItemDelegate(new LS3SummaryProxyModelDelegate(datastore, tvMain));

    tabPreview=new QTabWidget(this);

    txtPreview=new QWebView(this);
//    qDebug()<<txtPreview->textSizeMultiplier();
    txtPreview->setTextSizeMultiplier(0.75);

    //txtPreview->setOpenLinks(false);
    //txtPreview->setOpenExternalLinks(false);
    //txtPreview->setReadOnly(true);
    tabPreview->addTab(txtPreview, tr("&Preview"));
    //connect(txtPreview, SIGNAL(anchorClicked(const QUrl&)), this, SLOT(previewAnchorClicked(const QUrl&)));
    connect(txtPreview, SIGNAL(linkClicked(QUrl)), this, SLOT(previewAnchorClicked(const QUrl&)));

    splitter->addWidget(tvMain);
    splitter->addWidget(tabPreview);


    //tabReferenceData = new QTabWidget(this);
    //tabReferenceData->setTabPosition(QTabWidget::West);
    //tabReferenceData->setVisible(false);
    details=new DetailPane(settings, this);
    extended=new ExtendedPane(settings, this);
    fileView=new FileViewPane(settings, this);
    tabPreview->addTab(fileView, QIcon(":/tab_fileview.png"), tr("Document View"));
    contents=new ContentsPane(settings, this);
    /*tabReferenceData->addTab(details, QIcon(":/tab_reference.png"), tr("Reference"));
    tabReferenceData->addTab(extended, QIcon(":/tab_additional.png"), tr("Additional"));
    tabReferenceData->addTab(contents, QIcon(":/tab_contents.png"), tr("Contents"));*/

    tabMain=new QTabWidget(this);
    tabMain->addTab(splitter, QIcon(":/tab_overview.png"), tr("Reference Overview"));
    //tabMain->addTab(tabReferenceData, QIcon(":/tab_details.png"), tr("Reference Details"));
    setCentralWidget(tabMain);

    cmbQuickFindFields=new QComboBox(this);
    cmbQuickFindFields->addItem(tr("titles & authors/editors"), QString("authors;editors;title;subtitle;booktitle;chapter;series;year"));
    cmbQuickFindFields->addItem(tr("titles"), QString("title;subtitle;booktitle;chapter;series"));
    cmbQuickFindFields->addItem(tr("authors/editors"), QString("authors;editors"));
    cmbQuickFindFields->addItem(tr("ID"), QString("id"));
    cmbQuickFindFields->addItem(tr("electronic IDs"), QString("doi;arxiv;citeseer;pubmed;pmcid;url;urls"));
    cmbQuickFindFields->addItem(tr("year"), QString("year"));
    cmbQuickFindFields->addItem(tr("topic"), QString("topic"));
    cmbQuickFindFields->addItem(tr("keywords"), QString("keywords;topic"));
    cmbQuickFindFields->addItem(tr("contents"), QString("title;subtitle;booktitle;chapter;series;topic;keywords;abstract;comments"));
    cmbQuickFindFields->addItem(tr("institution"), QString("publisher;institution"));
    cmbQuickFindFields->addItem(tr("major fields"), QString("title;subtitle;booktitle;chapter;series;topic;keywords;abstract;comments;authors;editors;id;keywords;topic;journal;publisher;institution"));

    edtQuickFindTerm=new QEnhancedLineEdit(this);
    edtQuickFindTerm->addButton(new JKStyledButton(JKStyledButton::ClearLineEdit, edtQuickFindTerm, edtQuickFindTerm));
    chkQuickFindRegExp=new QCheckBox(tr("RX"), this);
    chkQuickFindRegExp->setToolTip(tr("if checked the filter phrase is interpreted as regular expression"));
    chkQuickFindRegExp->setChecked(false);
    cmbQuickFindFields->setEnabled(false);
    edtQuickFindTerm->setEnabled(false);
    chkQuickFindRegExp->setEnabled(false);
    connect(cmbQuickFindFields, SIGNAL(currentIndexChanged(int)), this, SLOT(quickfind()));
    connect(edtQuickFindTerm, SIGNAL(textChanged(QString)), this, SLOT(quickfind()));
    connect(chkQuickFindRegExp, SIGNAL(toggled(bool)), this, SLOT(quickfind()));

    //cw->setLayout(l);
    //setCentralWidget(cw);
    //setCentralWidget(splitter);

    //std::cout<<std::endl;
}

void MainWindow::createActions()
{
    //std::cout<<"createActions()";
    newAct = new QAction(QIcon(":/new.png"), tr("&New Database"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new database"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/open.png"), tr("&Open Database ..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing database"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
    saveAct = new QAction(QIcon(":/save.png"), tr("&Save Database"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the current database"));
    saveAct->setEnabled(datastore->hasDbSave());
    saveAct->setVisible(datastore->hasDbSave());
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(QIcon(":/saveas.png"), tr("&Save Database As ..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the current database as..."));
    saveAsAct->setEnabled(datastore->hasDbSave());
    saveAsAct->setVisible(datastore->hasDbSave());
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));


    closeAct = new QAction(tr("&Close Database"), this);
    closeAct->setShortcut(QKeySequence::Close);
    closeAct->setStatusTip(tr("Close the current database"));
    connect(closeAct, SIGNAL(triggered()), this, SLOT(closeDatabase()));
    connect(datastore, SIGNAL(databaseLoaded(bool)), closeAct, SLOT(setEnabled(bool)));
    closeAct->setEnabled(false);

    exportAct = new QAction(QIcon(":/export.png"), tr("&Export Datasets ..."), this);
    exportAct->setShortcut(tr("Alt+X"));
    exportAct->setStatusTip(tr("Export some datasets from the current database"));
    connect(exportAct, SIGNAL(triggered()), this, SLOT(exportDatabase()));
    connect(datastore, SIGNAL(databaseLoaded(bool)), exportAct, SLOT(setEnabled(bool)));
    exportAct->setEnabled(false);

    importAct = new QAction(QIcon(":/import.png"), tr("&Import Datasets ..."), this);
    importAct->setShortcut(tr("Alt+I"));
    importAct->setStatusTip(tr("Import some datasets from external database into the current database"));
    connect(importAct, SIGNAL(triggered()), this, SLOT(importDatabase()));
    connect(datastore, SIGNAL(databaseLoaded(bool)), importAct, SLOT(setEnabled(bool)));
    importAct->setEnabled(false);

    exitAct = new QAction(QIcon(":/exit.png"), tr("E&xit"), this);
    exitAct->setShortcut(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    optionsAct = new QAction(QIcon(":/configure.png"), tr("&Settings"), this);
    optionsAct->setStatusTip(tr("Application settings dialog"));
    optionsAct->setShortcut(QKeySequence::Preferences);
    connect(optionsAct, SIGNAL(triggered()), settings, SLOT(openSettingsDialog()));

    aboutAct = new QAction(QIcon(":/info.png"), tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutPluginsAct = new QAction(QIcon(":/info.png"), tr("About &Plugins"), this);
    connect(aboutPluginsAct, SIGNAL(triggered()), this, SLOT(aboutPlugins()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    dbinsertAct = new QAction(QIcon(":/add.png"), tr("&Insert"), this);
    dbinsertAct->setStatusTip(tr("insert a new record"));
    dbinsertAct->setShortcut(QKeySequence("Ctrl++"));
    connect(dbinsertAct, SIGNAL(triggered()), datastore, SLOT(dbInsert()));
    connect(datastore, SIGNAL(databaseLoaded(bool)), dbinsertAct, SLOT(setEnabled(bool)));
    dbinsertAct->setEnabled(false);

    dbdeleteAct = new QAction(QIcon(":/delete.png"), tr("&Delete"), this);
    dbdeleteAct->setStatusTip(tr("delete the current record"));
    connect(dbdeleteAct, SIGNAL(triggered()), this, SLOT(dbDelete()));
    connect(datastore, SIGNAL(databaseLoaded(bool)), dbdeleteAct, SLOT(setEnabled(bool)));
    dbdeleteAct->setEnabled(false);

    dbnextAct = new QAction(QIcon(":/next.png"), tr("&Next record"), this);
    dbnextAct->setStatusTip(tr("move to next record"));
    connect(dbnextAct, SIGNAL(triggered()), this, SLOT(dbNext()));
    connect(datastore, SIGNAL(databaseLoaded(bool)), dbnextAct, SLOT(setEnabled(bool)));
    dbnextAct->setEnabled(false);

    dbpreviousAct = new QAction(QIcon(":/previous.png"), tr("&Previous record"), this);
    dbpreviousAct->setStatusTip(tr("move to previous record"));
    connect(dbpreviousAct, SIGNAL(triggered()), this, SLOT(dbPrevious()));
    connect(datastore, SIGNAL(databaseLoaded(bool)), dbpreviousAct, SLOT(setEnabled(bool)));
    dbpreviousAct->setEnabled(false);

    searchAct = new QAction(QIcon(":/find.png"), tr("&Find"), this);
    searchAct->setStatusTip(tr("search for a record by given criteria"));
    searchAct->setShortcut(QKeySequence::Find);
    connect(searchAct, SIGNAL(triggered()), this, SLOT(search()));
    connect(datastore, SIGNAL(databaseLoaded(bool)), searchAct, SLOT(setEnabled(bool)));
    searchAct->setEnabled(false);

    actQuickFindEnabled = new QAction(QIcon(":/quickfind.png"), tr("&Quick Find"), this);
    actQuickFindEnabled->setStatusTip(tr("toggle the quick find filtering option"));
    actQuickFindEnabled->setCheckable(true);
    actQuickFindEnabled->setChecked(false);
    connect(actQuickFindEnabled, SIGNAL(toggled(bool)), cmbQuickFindFields, SLOT(setEnabled(bool)));
    connect(actQuickFindEnabled, SIGNAL(toggled(bool)), edtQuickFindTerm, SLOT(setEnabled(bool)));
    connect(actQuickFindEnabled, SIGNAL(toggled(bool)), chkQuickFindRegExp, SLOT(setEnabled(bool)));
    connect(actQuickFindEnabled, SIGNAL(toggled(bool)), this, SLOT(quickfind()));
    connect(datastore, SIGNAL(databaseLoaded(bool)), actQuickFindEnabled, SLOT(setEnabled(bool)));
    actQuickFindEnabled->setEnabled(false);



    createMissingIDsAct = new QAction(tr("Create missing Record IDs ..."), this);
    createMissingIDsAct->setStatusTip(tr("create an ID (of the currently set type) for every record which is missing one"));
    connect(createMissingIDsAct, SIGNAL(triggered()), this, SLOT(createMissingIDs()));
    connect(datastore, SIGNAL(databaseLoaded(bool)), createMissingIDsAct, SLOT(setEnabled(bool)));
    createMissingIDsAct->setEnabled(false);

    actCleanupTopics = new QAction(tr("Clean-Up Topics ..."), this);
    actCleanupTopics->setStatusTip(tr("Clean-up the topic entries in the database"));
    connect(actCleanupTopics, SIGNAL(triggered()), this, SLOT(cleanupTopics()));
    connect(datastore, SIGNAL(databaseLoaded(bool)), actCleanupTopics, SLOT(setEnabled(bool)));
    actCleanupTopics->setEnabled(false);


    actCopyFormatted=new QAction(QIcon(":/csl_copyformated.png"), tr("copy formatted"), this);
    connect(actCopyFormatted, SIGNAL(triggered()), this, SLOT(copyCSLFormatted()));
    actCopyPlainText=new QAction(QIcon(":/csl_copyplaintext.png"), tr("copy lpaintext"), this);
    connect(actCopyPlainText, SIGNAL(triggered()), this, SLOT(copyCSLPlainText()));
    actCopyHTMLtags=new QAction(QIcon(":/csl_copyhtmltags.png"), tr("copy HTML Tags"), this);
    connect(actCopyHTMLtags, SIGNAL(triggered()), this, SLOT(copyCSLHTMLTags()));
    //std::cout<<std::endl;
}

void MainWindow::createMenus()
{
    //std::cout<<"createMenus()";
    recentFilesMenu=new QRecentFilesMenu(this);
    recentFilesMenu->setIcon(QIcon(":/open.png"));
    recentFilesMenu->setTitle(tr("&Recently Opened Databases ..."));
    connect(recentFilesMenu, SIGNAL(openRecentFile(QString)), this, SLOT(openRecentFile(QString)));
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addMenu(recentFilesMenu);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(closeAct);
    fileMenu->addSeparator();
    fileMenu->addAction(optionsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));

    databaseMenu = menuBar()->addMenu(tr("&Database"));
    databaseMenu->addAction(dbpreviousAct);
    databaseMenu->addAction(dbnextAct);
    databaseMenu->addSeparator();
    databaseMenu->addAction(dbinsertAct);
    databaseMenu->addAction(dbdeleteAct);
    databaseMenu->addSeparator();
    databaseMenu->addAction(searchAct);
    databaseMenu->addAction(actQuickFindEnabled);
    databaseMenu->addSeparator();
    databaseMenu->addAction(importAct);
    databaseMenu->addAction(exportAct);


    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(createMissingIDsAct);
    toolsMenu->addAction(actCleanupTopics);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewToolbarsMenu=viewMenu->addMenu(tr("&Toolbars"));
    viewDockMenu=viewMenu->addMenu(tr("&Docking Windows"));


    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutPluginsAct);
    helpMenu->addAction(aboutQtAct);



    // workaround, so the widgets loose focus, when any button is clicked and therefore
    // QWidgetdataMapper write data back into the model!
    fileMenu->setFocusPolicy(Qt::StrongFocus);
    databaseMenu->setFocusPolicy(Qt::StrongFocus);
    helpMenu->setFocusPolicy(Qt::StrongFocus);
    //std::cout<<std::endl;
}

void MainWindow::createToolBars() {
    //std::cout<<"createToolBars()";
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->setObjectName("toolbar_file");
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    viewToolbarsMenu->addAction(fileToolBar->toggleViewAction());

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->setObjectName("toolbar_edit");
    viewToolbarsMenu->addAction(editToolBar->toggleViewAction());

    databaseToolBar = addToolBar(tr("Database"));
    databaseToolBar->setObjectName("toolbar_database");
    databaseToolBar->addAction(dbpreviousAct);
    databaseToolBar->addAction(dbnextAct);
    databaseToolBar->addSeparator();
    databaseToolBar->addAction(dbinsertAct);
    databaseToolBar->addAction(dbdeleteAct);
    databaseToolBar->addSeparator();
    databaseToolBar->addAction(importAct);
    databaseToolBar->addAction(exportAct);
    viewToolbarsMenu->addAction(databaseToolBar->toggleViewAction());
    databaseToolBar->addSeparator();
    databaseToolBar->addAction(searchAct);
    databaseToolBar->addSeparator();
    databaseToolBar->addWidget(new QLabel(tr("  sort by:  "), databaseToolBar));
    cmbSorting=new QComboBox(databaseToolBar);
    cmbSorting->addItem(tr("number"), LS3SUMMARYPROXYMODEL_COLUMN_SORT_NUM);
    cmbSorting->addItem(tr("ID"), LS3SUMMARYPROXYMODEL_COLUMN_ID);
    cmbSorting->addItem(tr("selected"), LS3SUMMARYPROXYMODEL_COLUMN_SELECTION);
    cmbSorting->addItem(tr("author, year, title"), LS3SUMMARYPROXYMODEL_COLUMN_SORT_AUTHORYEARTITLE);
    cmbSorting->addItem(tr("title"), LS3SUMMARYPROXYMODEL_COLUMN_SORT_TITLE);
    cmbSorting->addItem(tr("year, title"), LS3SUMMARYPROXYMODEL_COLUMN_SORT_YEARTITLE);
    cmbSorting->addItem(tr("rating"), LS3SUMMARYPROXYMODEL_COLUMN_RATING);
    connect(cmbSorting, SIGNAL(currentIndexChanged(int)), this, SLOT(changeSorting()));
    databaseToolBar->addWidget(cmbSorting);
    cmbSortingDirection=new QComboBox(databaseToolBar);
    cmbSortingDirection->addItem(tr("ascending"));
    cmbSortingDirection->addItem(tr("descending"));
    databaseToolBar->addWidget(cmbSortingDirection);
    connect(cmbSortingDirection, SIGNAL(currentIndexChanged(int)), this, SLOT(changeSorting()));

    quickfindToolBar = addToolBar(tr("Quick Find"));
    quickfindToolBar->setObjectName("toolbar_quickfind");
    quickfindToolBar->addAction(actQuickFindEnabled);
    quickfindToolBar->addWidget(edtQuickFindTerm);
    quickfindToolBar->addWidget(chkQuickFindRegExp);
    quickfindToolBar->addWidget(cmbQuickFindFields);
    viewToolbarsMenu->addAction(quickfindToolBar->toggleViewAction());

    toolsToolBar = addToolBar(tr("Tools"));
    toolsToolBar->setObjectName("toolbar_tools");
    viewToolbarsMenu->addAction(toolsToolBar->toggleViewAction());

    cmbCSLs=new QComboBox(databaseToolBar);
    cmbCSLs->addItems(settings->GetPreviewStyleManager()->styles());
    cmbCSLs->setMaximumWidth(250);

    cmbCSLLocales=new QComboBox(databaseToolBar);
    cmbCSLLocales->addItems(settings->GetPreviewStyleManager()->locales());
    cmbCSLLocales->setMaximumWidth(250);


    cmbCSLWhich=new QComboBox(databaseToolBar);
    cmbCSLWhich->addItem(tr("current rec."));
    cmbCSLWhich->addItem(tr("selected recs."));
    cmbCSLWhich->addItem(tr("all recs."));
    cmbCSLWhich->setCurrentIndex(0);


    referencesToolBar = addToolBar(tr("Formated References"));
    referencesToolBar->setObjectName("toolbar_references");
    referencesToolBar->addWidget(cmbCSLs);
    referencesToolBar->addWidget(cmbCSLLocales);
    referencesToolBar->addSeparator();
    referencesToolBar->addWidget(cmbCSLWhich);
    referencesToolBar->addAction(actCopyFormatted);
    referencesToolBar->addAction(actCopyPlainText);
    referencesToolBar->addAction(actCopyHTMLtags);
    viewToolbarsMenu->addAction(referencesToolBar->toggleViewAction());

    // workaround, so the widgets loose focus, when any button is clicked and therefore
    // QWidgetdataMapper write data back into the model!
    databaseToolBar->setFocusPolicy(Qt::StrongFocus);
    quickfindToolBar->setFocusPolicy(Qt::StrongFocus);
    referencesToolBar->setFocusPolicy(Qt::StrongFocus);
    fileToolBar->setFocusPolicy(Qt::StrongFocus);
    editToolBar->setFocusPolicy(Qt::StrongFocus);
    toolsToolBar->setFocusPolicy(Qt::StrongFocus);

    //std::cout<<std::endl;
}

void MainWindow::createStatusBar() {
    statusBar()->showMessage(tr("Ready"));
    labDBStatusToolBar=new QLabel(tr("database status"), this);
    statusBar()->addPermanentWidget(labDBStatusToolBar);
    progress=new QProgressBar(this);
    progress->setMaximumWidth(200);
    progress->setTextVisible(true);
    progress->setRange(0,100);
    progress->setValue(0);
    statusBar()->addPermanentWidget(progress);
}

void MainWindow::readSettings() {
    bool upEn=updatesEnabled();
    if (upEn) setUpdatesEnabled(false);
    QSettings* s=settings->GetQSettings();
    s->sync();
    recentFilesMenu->readSettings(*s, QString("mainwindow/"));
    if (splitter) splitter->restoreState(s->value("mainwindow/splitterSizes", splitter->saveState()).toByteArray());
    if (details) details->readSettings(*s);
    kbSpecial->setCharacters(settings->GetSpecialCharacters());
    QFont f=tvMain->font();
    f.setFamily(settings->GetTableFontName());
    f.setPointSizeF(settings->GetTableFontSize());
    tvMain->setFont(f);
    cmbSorting->setCurrentIndex(s->value("mainwindow/sorting",0).toInt());
    cmbSortingDirection->setCurrentIndex(s->value("mainwindow/sortingDirection",0).toInt());
    tvMain->horizontalHeader()->resizeSection(LS3SUMMARYPROXYMODEL_COLUMN_SUMMARY, s->value("mainwindow/summarySectionSize",tvMain->horizontalHeader()->sectionSize(LS3SUMMARYPROXYMODEL_COLUMN_SUMMARY)).toInt());

    cmbQuickFindFields->setCurrentIndex(s->value("mainwindow/qf_fields",0).toInt());;
    edtQuickFindTerm->setText(s->value("mainwindow/qf_term","").toString());;
    chkQuickFindRegExp->setChecked(s->value("mainwindow/qf_regexp",false).toBool());

    cmbCSLLocales->setCurrentIndex(cmbCSLLocales->findText(s->value("mainwindow/csl_locale","en-US").toString()));;
    cmbCSLs->setCurrentIndex(cmbCSLLocales->findText(s->value("mainwindow/csl","").toString()));;
    cmbCSLWhich->setCurrentIndex(s->value("mainwindow/cmbCSLWhich",cmbCSLWhich->currentIndex()).toInt());;

    if (curDisplayStyle!=settings->GetHowDisplayReferenceDetails()) {
        curDisplayStyle=settings->GetHowDisplayReferenceDetails();
        // clean up main display widgets for later recreation!
        /*for (int j=tabReferenceData->count(); j>=0; j--) {
            tabReferenceData->removeTab(j);
        }*/
        tabReferenceData=NULL;
        for (int j=tabPreview->count(); j>=2; j--) {
            tabPreview->removeTab(j);
        }
        for (int j=tabMain->count(); j>=1; j--) {
            tabMain->removeTab(j);
        }
        if (dockReferenceData!=NULL) {
            details->setParent(tabMain);
            extended->setParent(tabMain);
            contents->setParent(tabMain);
            dockReferenceData->close();
            delete dockReferenceData;
            dockReferenceData=NULL;
        }
        if (curDisplayStyle==0) {
            dockReferenceData=new QDockWidget(tr("Reference Details"), this);
            dockReferenceData->setObjectName("dock_reference_details");
            tabReferenceData=new QTabWidget(dockReferenceData);
            tabReferenceData->addTab(details, QIcon(":/tab_reference.png"), tr("Reference"));
            tabReferenceData->addTab(extended, QIcon(":/tab_additional.png"), tr("Additional"));
            tabReferenceData->addTab(contents, QIcon(":/tab_contents.png"), tr("Contents"));
            dockReferenceData->setWidget(tabReferenceData);
            dockReferenceData->setFloating(true);
            addDockWidget(dockReferenceData);
        } else if (settings->GetHowDisplayReferenceDetails()==1) {
            //tabMain->addTab(tabReferenceData, QIcon(":/tab_details.png"), tr("Reference Details"));
            tabMain->addTab(details, QIcon(":/tab_reference.png"), tr("Reference"));
            tabMain->addTab(extended, QIcon(":/tab_additional.png"), tr("Additional"));
            tabMain->addTab(contents, QIcon(":/tab_contents.png"), tr("Contents"));
            tabReferenceData=tabMain;
        } else {
            tabPreview->addTab(details, QIcon(":/tab_reference.png"), tr("Reference"));
            tabPreview->addTab(extended, QIcon(":/tab_additional.png"), tr("Additional"));
            tabPreview->addTab(contents, QIcon(":/tab_contents.png"), tr("Contents"));
            tabReferenceData=tabPreview;
        }
        details->changeLayout(settings->GetHowDisplayReferenceDetails()==0);
    }
    restoreGeometry(s->value("mainwindow/geometry", saveGeometry()).toByteArray());
    restoreState(s->value("mainwindow/windowState", saveState()).toByteArray());
    if (dockReferenceData) {
        //std::cout<<"<<<<<< read dockReferenceData/size = ("<<s->value("dockReferenceData/size", dockReferenceData->size()).toSize().width()<<", "<<s->value("dockReferenceData/size", dockReferenceData->size()).toSize().height()<<")\n";
        dockReferenceData->resize(s->value("dockReferenceData/size", dockReferenceData->size()).toSize());
        dockReferenceData->move(s->value("dockReferenceData/pos", dockReferenceData->pos()).toPoint());
    }
    if (upEn) setUpdatesEnabled(true);

}

void MainWindow::writeSettings() {
    QSettings* s=settings->GetQSettings();
    if (details) details->writeSettings(*s);
    s->setValue("mainwindow/geometry", saveGeometry());
    s->setValue("mainwindow/windowState", saveState());
    s->setValue("mainwindow/splitterSizes", splitter->saveState());
    s->setValue("mainwindow/sorting", cmbSorting->currentIndex());
    s->setValue("mainwindow/sortingDirection", cmbSortingDirection->currentIndex());
    s->setValue("mainwindow/summarySectionSize",tvMain->horizontalHeader()->sectionSize(LS3SUMMARYPROXYMODEL_COLUMN_SUMMARY));
    s->setValue("mainwindow/qf_fields",cmbQuickFindFields->currentIndex());
    s->setValue("mainwindow/qf_term",edtQuickFindTerm->text());
    s->setValue("mainwindow/qf_regexp",chkQuickFindRegExp->isChecked());

    s->setValue("mainwindow/csl_locale",cmbCSLLocales->currentText());
    s->setValue("mainwindow/csl",cmbCSLs->currentText());
    s->setValue("mainwindow/cmbCSLWhich",cmbCSLWhich->currentIndex());

    if (dockReferenceData) {
        //std::cout<<">>>>>> write dockReferenceData/size = ("<<dockReferenceData->size().width()<<", "<<dockReferenceData->size().height()<<")\n";
        s->setValue("dockReferenceData/size", dockReferenceData->size());
        s->setValue("dockReferenceData/pos", dockReferenceData->pos());
    }
    s->sync();
}


void MainWindow::showDBError(QString errorMessage) {
    QMessageBox::critical(this, tr("Database Error"), errorMessage);
}

void MainWindow::changeSorting() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (cmbSortingDirection->currentIndex()==0) {
        //std::cout<<">>>> SORTING: "<<cmbSorting->itemData(cmbSorting->currentIndex()).toInt()<<"  ascending\n";
        tvMainSortProxy->sort(cmbSorting->itemData(cmbSorting->currentIndex()).toInt(), Qt::AscendingOrder);
    } else {
        //std::cout<<">>>> SORTING: "<<cmbSorting->itemData(cmbSorting->currentIndex()).toInt()<<"  descending\n";
        tvMainSortProxy->sort(cmbSorting->itemData(cmbSorting->currentIndex()).toInt(), Qt::DescendingOrder);
    }
    QApplication::restoreOverrideCursor();
}

void MainWindow::currentRecordChanged(int row) {
    dbinsertAct->setEnabled(true);
    dbdeleteAct->setEnabled(true);
    int realRow=row;
    if (tvMain->model()) {
        // map from sorted model
        realRow=tvMainSortProxy->mapFromSource(summaryProxy->index(row, 0)).row();
        if (tvMain->model()->rowCount()!=datastore->recordCount())
            labDBStatusToolBar->setText(tr("dataset %1 / %2 (%3)").arg(realRow+1).arg(tvMain->model()->rowCount()).arg(datastore->recordCount()));
        else
            labDBStatusToolBar->setText(tr("dataset %1 / %2").arg(realRow+1).arg(datastore->recordCount()));
    } else {
        labDBStatusToolBar->setText(tr("dataset -- / --"));
    }
    QString txt=QString("<font face=\"%1\">%2</font>")
                .arg(settings->GetTableFontName())//.arg(settings->GetTableFontSize())
                .arg(settings->GetPreviewStyleManager()->createPreview(-1, datastore->currentRecord(), "en-US"));
    txt=txt.arg(settings->GetPreviewStyleManager()->styles().value(settings->GetCurrentPreviewStyle1()))
        .arg(settings->GetPreviewStyleManager()->createPreview(settings->GetCurrentPreviewStyle1(), datastore->currentRecord(), settings->GetCurrentPreviewLocale1()))
        .arg(settings->GetPreviewStyleManager()->styles().value(settings->GetCurrentPreviewStyle2()))
        .arg(settings->GetPreviewStyleManager()->createPreview(settings->GetCurrentPreviewStyle2(), datastore->currentRecord(), settings->GetCurrentPreviewLocale2()));
    txtPreview->setHtml(txt);
    txtPreview->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    txtPreview->page()->setContentEditable(false);

    disconnecttvMain();
    tvMain->selectionModel()->clear();
    tvMain->selectRow(realRow);
    tvMain->setCurrentIndex(tvMain->model()->index(realRow, 0));
    connecttvMain();
}

void MainWindow::tableRowChanged(const QModelIndex& current, const QModelIndex& /*previous*/) {
    datastore->dbMoveUUID(tvMain->model()->data(tvMain->model()->index(current.row(), LS3SUMMARYPROXYMODEL_COLUMN_UUID)).toString());
}

void MainWindow::tableRowActivted(const QModelIndex & current) {
    datastore->dbMoveUUID(tvMain->model()->data(tvMain->model()->index(current.row(), LS3SUMMARYPROXYMODEL_COLUMN_UUID)).toString());
}

void MainWindow::tableRowDoubleClicked(const QModelIndex & current) {
    int col=current.column();
    if ((col==1)||(col==2)||(col==3)||(col==7)) {
        tableRowActivted(current);
        if (dockReferenceData) {
            dockReferenceData->show();
        } else if (tabReferenceData) {
            int i=tabReferenceData->indexOf(details);
            if (i>=0) tabReferenceData->setCurrentWidget(details);
        }
        /*if (tabMain) {
            tabMain->setCurrentIndex(1);
        }*/
    }
}

void MainWindow::tableDataChanged(int /*firstRow*/, int /*lastRow*/) {
}


void MainWindow::dbDelete() {
    if (datastore->dbIsLoaded()) {
        QModelIndexList sel=tvMain->selectionModel()->selectedRows();
        if (QMessageBox::question(this, tr("Delete record"), tr("Do you really want to delete the selected %1 records?").arg(sel.size()), QMessageBox::Yes | QMessageBox::No)==QMessageBox::Yes) {
            for (int i=sel.size()-1; i>=0; i--) {
                int row=sel[i].row();
                QString uuid=tvMain->model()->data(tvMain->model()->index(row, LS3SUMMARYPROXYMODEL_COLUMN_UUID)).toString();
                datastore->dbDelete(datastore->getRecordByUUID(uuid));
            }

        }
    }
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();

    settings->SetCurrentFile(curFile);
    extended->setBaseDir(QFileInfo(fileName).dir().absolutePath());
    fileView->setBaseDir(QFileInfo(fileName).dir().absolutePath());

    //qDebug()<<"datastore->wasChanged()="<<datastore->wasChanged();
    QString mod="";
    setWindowModified(datastore->wasChanged());
    setWindowTitle(tr("LitSoz %2 - %1 [*]").arg(curFile).arg(QApplication::applicationVersion()));

    // update recent files list in ini file
    recentFilesMenu->addRecentFile(fileName);
    recentFilesMenu->storeSettings(*(settings->GetQSettings()), QString("mainwindow/"));
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


QWidget* MainWindow::GetParentWidget() {
    return this;
};

QSettings* MainWindow::GetQSettings() {
    return settings->GetQSettings();
};

QString MainWindow::GetLanguageID() {
    return settings->GetLanguageID();
};

QString MainWindow::GetDefaultCurrency() {
    return settings->GetDefaultCurrency();
};

QString MainWindow::GetDefaultStatus() {
    return settings->GetDefaultStatus();
};

QString MainWindow::GetIniFilename() {
    return settings->GetIniFilename();
};

QString MainWindow::GetStylesheet() {
    return settings->GetStylesheet();
};

QString MainWindow::GetStyle() {
    return settings->GetStyle();
};

QString MainWindow::GetUsername() {
    return settings->GetUsername();
};

QString MainWindow::GetTableFontName() {
    return settings->GetTableFontName();
};

int MainWindow::GetTableFontSize() {
    return settings->GetTableFontSize();
};

void MainWindow::statusbarShowMessage(const QString & message, int timeout) {
    statusBar()->showMessage(message, timeout);
}

void MainWindow::statusbarShowMessage(const QString & message) {
    statusBar()->showMessage(message);
}

void MainWindow::progressSetRange(int min, int max) {
    progress->setRange(min, max);
}

void MainWindow::progressSetRange(int max) {
    progress->setRange(0, max);
}

void MainWindow::progressSetValue(int value) {
    progress->setValue(value);
}

void MainWindow::progressIncValue(int inc) {
    progress->setValue(progress->value()+inc);
}

void MainWindow::progressIncValue() {
    progress->setValue(progress->value()+1);
}

void MainWindow::registerImporter(LS3Importer* importer) {
    m_importers.append(importer);
    //std::cout<<"added imperter\n";
}

void MainWindow::registerExporter(LS3Exporter* exporter) {
    m_exporters.append(exporter);
}
void MainWindow::deregisterImporter(LS3Importer* importer) {
    m_importers.removeAll(importer);
    //std::cout<<"added imperter\n";
}

void MainWindow::deregisterExporter(LS3Exporter* exporter) {
    m_exporters.removeAll(exporter);
}

QMenu* MainWindow::getMenu(LS3PluginServices::AvailableMenus menu) {
    switch(menu) {
        case LS3PluginServices::FileMenu: return fileMenu;
        case LS3PluginServices::EditMenu: return editMenu;
        case LS3PluginServices::DatabaseMenu: return databaseMenu;
        case LS3PluginServices::ToolsMenu: return toolsMenu;
        case LS3PluginServices::ViewMenu: return viewMenu;
        case LS3PluginServices::HelpMenu: return helpMenu;
        default: return NULL;
    }
}

QToolBar* MainWindow::getToolbar(LS3PluginServices::AvailableToolbars toolbar) {
    switch(toolbar) {
        case LS3PluginServices::FileToolbar: return fileToolBar;
        case LS3PluginServices::EditToolbar: return editToolBar;
        case LS3PluginServices::DatabaseToolbar: return databaseToolBar;
        case LS3PluginServices::ToolsToolbar: return toolsToolBar;
        default: return NULL;
    }
}

void MainWindow::addEditButton(LS3PluginServices::AvailableEditWidgetsForButtons edit, QAction *action) {
    extended->addEditButton(edit, action);
}

QNetworkProxy MainWindow::getNetworkProxy()
{
    QNetworkProxy proxy;
    settings->setQNetworkProxy(proxy);
    return proxy;
}

void MainWindow::insertMenu(QMenu* newMenu, QMenu* before) {
    if (before!=NULL)
        menuBar()->insertMenu(before->menuAction(), newMenu);
    else
        menuBar()->insertMenu(NULL, newMenu);
}

void MainWindow::insertToolBar(QToolBar* newToolbar) {
    addToolBar(newToolbar);
}

void MainWindow::addDockWidget(QDockWidget* dockWidget) {
    dockWidget->setFloating(true);
    QMainWindow::addDockWidget(Qt::AllDockWidgetAreas, dockWidget);
    viewDockMenu->addAction(dockWidget->toggleViewAction());
}

QList<QString> MainWindow::GetAndWords() {
    return settings->GetAndWords();
}

QSet<QString> MainWindow::GetNameAdditions() {
    return settings->GetNameAdditions();
}

QSet<QString> MainWindow::GetNamePrefixes() {
    return settings->GetNamePrefixes();
}

void MainWindow::dbNext() {
    if (!tvMain->model()) return;

    int cur=tvMain->currentIndex().row();
    //std::cout<<"cur="<<cur<<std::endl;
    if (cur+1<tvMain->model()->rowCount()) {
        //tvMain->selectionModel()->clear();
        QString nextUUID=tvMain->model()->data(tvMain->model()->index(cur+1, LS3SUMMARYPROXYMODEL_COLUMN_UUID)).toString();
        //std::cout<<"nextUUID="<<nextUUID.toStdString()<<std::endl;
        //std::cout<<"thisSummary="<<tvMain->model()->data(tvMain->model()->index(cur, LS3SUMMARYPROXYMODEL_COLUMN_SUMMARY)).toString().toStdString()<<std::endl;
        //std::cout<<"nextSummary="<<tvMain->model()->data(tvMain->model()->index(cur+1, LS3SUMMARYPROXYMODEL_COLUMN_SUMMARY)).toString().toStdString()<<std::endl;
        if (!nextUUID.isEmpty()) {
            datastore->dbMoveUUID(nextUUID);
        }
    }

}

void MainWindow::dbPrevious() {
    if (!tvMain->model()) return;

    int cur=tvMain->currentIndex().row();
    //std::cout<<"cur="<<cur<<std::endl;
    if (cur-1>=0) {
        //tvMain->selectionModel()->clear();
        QString nextUUID=tvMain->model()->data(tvMain->model()->index(cur-1, LS3SUMMARYPROXYMODEL_COLUMN_UUID)).toString();
        if (!nextUUID.isEmpty()) {
            datastore->dbMoveUUID(nextUUID);
        }
    }

}

void MainWindow::search() {
    QStringList fields;
    QList<QStringList> fieldToSearch;
    fields<<tr("all fields");
    { fieldToSearch.append(datastore->getFieldNames()); }
    fields<<tr("author/editor names");
    { QStringList fts;  fts<<"authors"<<"editors"; fieldToSearch.append(fts); }
    fields<<tr("title (incl. series,collection)");
    { QStringList fts;  fts<<"title"<<"subtitle"<<"booktitle"<<"series"<<"chapter"; fieldToSearch.append(fts); }
    fields<<tr("keywords/title/topic");
    { QStringList fts;  fts<<"keywords"<<"topic"<<"title"<<"subtitle"<<"booktitle"<<"series"<<"chapter"; fieldToSearch.append(fts); }
    fields<<tr("ID/PMID/DOI/...");
    { QStringList fts;  fts<<"id"<<"doi"<<"pubmed"<<"citeseer"<<"arxiv"<<"librarynum"<<"isbn"<<"issn"; fieldToSearch.append(fts); }
    fields<<tr("abstract/comments");
    { QStringList fts;  fts<<"abstract"<<"comments"; fieldToSearch.append(fts); }
    fields<<tr("keywords");
    { QStringList fts;  fts<<"keywords"; fieldToSearch.append(fts); }
    fields<<tr("topic");
    { QStringList fts;  fts<<"topic"; fieldToSearch.append(fts); }
    fields<<tr("library no.");
    { QStringList fts;  fts<<"librarynum"; fieldToSearch.append(fts); }
    fields<<tr("status");
    { QStringList fts;  fts<<"status"<<"statussince"<<"statuscomment"; fieldToSearch.append(fts); }
    fields<<tr("origin");
    { QStringList fts;  fts<<"origin"; fieldToSearch.append(fts); }
    fields<<tr("publisher");
    { QStringList fts;  fts<<"publisher"; fieldToSearch.append(fts); }
    fields<<tr("places");
    { QStringList fts;  fts<<"places"; fieldToSearch.append(fts); }
    fields<<tr("year");
    { QStringList fts;  fts<<"year"; fieldToSearch.append(fts); }
    fields<<tr("journal");
    { QStringList fts;  fts<<"journal"; fieldToSearch.append(fts); }
    fields<<tr("volume");
    { QStringList fts;  fts<<"volume"; fieldToSearch.append(fts); }
    fields<<tr("number");
    { QStringList fts;  fts<<"number"; fieldToSearch.append(fts); }
    fields<<tr("institution");
    { QStringList fts;  fts<<"institution"; fieldToSearch.append(fts); }
    fields<<tr("ISBN/ISSN");
    { QStringList fts;  fts<<"isbn"<<"issn"; fieldToSearch.append(fts); }
    searchDlg->setFields(fields);
    if (searchDlg->exec()==QDialog::Accepted) {
        QStringList uuids=tvMainSortProxy->getUUIDs();
        QModernProgressDialog pdlg;
        pdlg.setLabelText(tr("Searching database ..."));
        pdlg.setRange(0, uuids.size());
        pdlg.setCancelButtonText(tr("&Cancel"));
        pdlg.progressWidget()->setPercentageMode(QModernProgressWidget::Percent);
        pdlg.progressWidget()->setMinimumSize(QSize(64,64));
        pdlg.setMode(true, true);
        pdlg.open();
        progress->setRange(0, uuids.size());
        statusbarShowMessage(tr("searching database ..."));
        SearchResultModel* sr=lstSearchResults->searchResultModel();
        sr->clearResults();
        QStringList result;

        QString p1=searchDlg->getPhrase1();
        QString p2=searchDlg->getPhrase2();
        QString p3=searchDlg->getPhrase3();

        Qt::CaseSensitivity caseSense=Qt::CaseInsensitive;

        QRegExp rx1(p1);
        rx1.setCaseSensitivity(caseSense);
        rx1.setPatternSyntax(QRegExp::Wildcard);
        QRegExp rx2(p2);
        rx2.setCaseSensitivity(caseSense);
        rx2.setPatternSyntax(QRegExp::Wildcard);
        QRegExp rx3(p3);
        rx3.setCaseSensitivity(caseSense);
        rx3.setPatternSyntax(QRegExp::Wildcard);

        QRegExp reg1(p1);
        reg1.setPatternSyntax(QRegExp::RegExp);
        QRegExp reg2(p2);
        reg2.setPatternSyntax(QRegExp::RegExp);
        QRegExp reg3(p3);
        reg3.setPatternSyntax(QRegExp::RegExp);

        const QStringList& fts1=fieldToSearch[searchDlg->getFields1()];
        const QStringList& fts2=fieldToSearch[searchDlg->getFields2()];
        const QStringList& fts3=fieldToSearch[searchDlg->getFields3()];

        for (int i=0; i<uuids.size(); i++) {
            int fidx=datastore->getRecordByUUID(uuids[i]);
            bool r1=false;
            bool r2=false;
            bool r3=false;
            if (searchDlg->getSearchMode()==SearchDialog::Simple) {
                if (!p1.isEmpty()) for (int f=0; f<fts1.size(); f++) {
                    if (datastore->getField(fidx, fts1[f]).toString().contains(p1, caseSense)) {
                        r1=true;
                        break;
                    }
                }
                if (!p2.isEmpty()) for (int f=0; f<fts2.size(); f++) {
                    if (datastore->getField(fidx, fts2[f]).toString().contains(p2, caseSense)) {
                        r2=true;
                        break;
                    }
                }
                if (!p3.isEmpty()) for (int f=0; f<fts3.size(); f++) {
                    if (datastore->getField(fidx, fts3[f]).toString().contains(p3, caseSense)) {
                        r3=true;
                        break;
                    }
                }

            } else if (searchDlg->getSearchMode()==SearchDialog::Wildcards) {
                if (!p1.isEmpty()) for (int f=0; f<fts1.size(); f++) {
                    if (rx1.indexIn(datastore->getField(fidx, fts1[f]).toString())>-1) {
                        r1=true;
                        break;
                    }
                }
                if (!p2.isEmpty()) for (int f=0; f<fts2.size(); f++) {
                    if (rx2.indexIn(datastore->getField(fidx, fts2[f]).toString())>-1) {
                        r2=true;
                        break;
                    }
                }
                if (!p3.isEmpty()) for (int f=0; f<fts3.size(); f++) {
                    if (rx3.indexIn(datastore->getField(fidx, fts3[f]).toString())>-1) {
                        r3=true;
                        break;
                    }
                }
            } else if (searchDlg->getSearchMode()==SearchDialog::RegExp) {
                if (!p1.isEmpty()) for (int f=0; f<fts1.size(); f++) {
                    if (reg1.indexIn(datastore->getField(fidx, fts1[f]).toString())>-1) {
                        r1=true;
                        break;
                    }
                }
                if (!p2.isEmpty()) for (int f=0; f<fts2.size(); f++) {
                    if (reg2.indexIn(datastore->getField(fidx, fts2[f]).toString())>-1) {
                        r2=true;
                        break;
                    }
                }
                if (!p3.isEmpty()) for (int f=0; f<fts3.size(); f++) {
                    if (reg3.indexIn(datastore->getField(fidx, fts3[f]).toString())>-1) {
                        r3=true;
                        break;
                    }
                }
            }
            if (searchDlg->logicTerm(r1, r2, r3)) {
                result.append(uuids[i]);
            }
            if (i%10) {
                progress->setValue(progress->value()+10);
                pdlg.setValue(i);
                QCoreApplication::processEvents();
            }
            if (pdlg.wasCanceled()) {
                break;
            }
        }
        pdlg.close();
        if (!pdlg.wasCanceled()) {
            sr->addResults(result);
        }
        progress->setValue(uuids.size());
        statusbarShowMessage(tr("searching database ... done!"), 3000);
        progress->setValue(0);
        if (result.size()>0) lstSearchResults->show();
    }
}

void MainWindow::quickfind()
{
    if (actQuickFindEnabled->isChecked()) {
        if (!quickfindToolBar->isVisible()) quickfindToolBar->toggleViewAction()->setChecked(true);
        tvMainSortProxy->setFilterTerm(edtQuickFindTerm->text(), chkQuickFindRegExp->isChecked(), cmbQuickFindFields->itemData(cmbQuickFindFields->currentIndex()).toString());
    } else {
        tvMainSortProxy->setFilterTerm("", false, "");
    }
}

QList<LS3Importer*> MainWindow::getImporters() {
    return m_importers;
}

QList<LS3Exporter*> MainWindow::getExporters() {
    return m_exporters;
}

void MainWindow::exportDatabase() {
    exportDlg->init(this);
    if (exportDlg->exec()==QDialog::Accepted) {
        LS3Exporter* exp=exportDlg->getExporter();
        if (exp) {
            QVector<QMap<QString, QVariant> > data;
            QStringList uuids=tvMainSortProxy->getUUIDs();
            QModernProgressDialog pdlg;
            pdlg.setLabelText(tr("exporting from current database:<br><center>preparing ...</center>"));
            pdlg.setCancelButtonText(tr("&Cancel"));
            pdlg.setHasCancel(false);
            pdlg.openDelayed(500);
            QApplication::setOverrideCursor(Qt::WaitCursor);
            switch(exportDlg->getWhich()) {
                case ExportDialog::Selected:
                    for (int i=0; i<uuids.size(); i++) {
                        if (datastore->isSelected(uuids[i])) data.append(datastore->getRecord(datastore->getRecordByUUID(uuids[i])));
                        if (i%10==0) QApplication::processEvents();
                    }
                    break;
                case ExportDialog::Current:
                    data.append(datastore->currentRecord());
                    break;
                default:
                case ExportDialog::All:
                    for (int i=0; i<uuids.size(); i++) {
                        data.append(datastore->getRecord(datastore->getRecordByUUID(uuids[i])));
                        if (i%10==0) QApplication::processEvents();
                    }
                    break;
            }
            pdlg.setLabelText(tr("exporting from current database:<br><center>exporting ...</center>"));
            QApplication::processEvents();
            if (data.size()>0) {
                pdlg.close();
                exp->execute(data, this);

                QApplication::restoreOverrideCursor();
            }  else {
                pdlg.close();
                QApplication::restoreOverrideCursor();
                QMessageBox::information(this, tr("Export"), tr("No datasets selected for export!"));
            }
        }
    }
}

void MainWindow::importDatabase() {
    importDlg->init(this);
    if (importDlg->exec()==QDialog::Accepted) {
        LS3Importer* importer=importDlg->getImporter();
        if (importer) {
            QVector<QMap<QString, QVariant> > data;
            //QStringList uuids=tvMainSortProxy->getUUIDs();
            QModernProgressDialog pdlg;
            pdlg.setLabelText(tr("importing into current database ..."));
            pdlg.setCancelButtonText(tr("&Cancel"));
            pdlg.setHasCancel(false);
            pdlg.setHasCancel(true);
            pdlg.setMode(true, true);
            QApplication::setOverrideCursor(Qt::WaitCursor);
            QApplication::processEvents();

            data=importer->execute(this);
            pdlg.openDelayed(500);
            pdlg.setRange(0, data.size());

            for (int i=0; i<data.size(); i++) {
                datastore->addRecord(data[i]);

                if (i%10==0) {
                    pdlg.setValue(i);
                    QApplication::processEvents();
                }
                if (pdlg.wasCanceled()) break;
            }
            pdlg.accept();
            QApplication::restoreOverrideCursor();
        }
    }
}

void MainWindow::createMissingIDs() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QStringList uuids=tvMainSortProxy->getUUIDs();
    for (int i=0; i<uuids.size(); i++) {
        int record=datastore->getRecordByUUID(uuids[i]);
        if (datastore->getField(record, "id").toString().isEmpty()) {
            QString id=datastore->createID(record, GetIDType());
            datastore->setField(record, "id", id);
        }
    }
    QApplication::restoreOverrideCursor();
}

void MainWindow::cleanupTopics() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QStringList uuids=tvMainSortProxy->getUUIDs();

    bool preferUpper=QMessageBox::question(this, tr("Clean-Up Topics"), tr("Do you prefer Topics with lower-case letters?\n  [Yes]: Words with a first letter upper-case will be converted to lower-case\n  [No]: Words with a first letter upper-case will be prefered over lower-case"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No)==QMessageBox::No;


    QModernProgressDialog pdlg;
    pdlg.setLabelText(tr("collecting all topics in database ..."));
    pdlg.setRange(0, uuids.size()*2);
    pdlg.setCancelButtonText(tr("&Cancel"));
    pdlg.progressWidget()->setPercentageMode(QModernProgressWidget::Percent);
    pdlg.progressWidget()->setMinimumSize(QSize(64,64));
    pdlg.setMode(true, true);
    pdlg.open();
    progress->setRange(0, uuids.size()*2);
    statusbarShowMessage(tr("collecting all topics in database ..."));

    QHash<QStringList,QStringList> topics; // split by "/", "\" and uppercase-lowercase
    QHash<int, QStringList> topicUCForRecord;
    for (int i=0; i<uuids.size(); i++) {
        int record=datastore->getRecordByUUID(uuids[i]);
        QString topic=datastore->getField(record, "topic").toString();
        QStringList topicsplit=topic.split(QRegExp("[\\\\\\/]"));
        QStringList topicsplitUC;
        for (int j=0; j<topicsplit.size(); j++) {
            topicsplit[j]=topicsplit[j].trimmed().simplified();
            topicsplitUC.append(topicsplit[j].toUpper());
        }

        QHash<QStringList,QStringList>::const_iterator it=topics.find(topicsplitUC);
        if (it==topics.end()) {
            topics[topicsplitUC]=topicsplit;
        } else {
            QStringList oldtopics=it.value();
            for (int j=0; j<qMin(oldtopics.size(), topicsplit.size()); j++) {
                int oldUC=0, oldLC=0, newUC=0, newLC=0;
                countLetterCase(oldtopics[j], oldUC, oldLC);
                countLetterCase(topicsplit[j], newUC, newLC);
                if (newUC>1 && newUC>oldUC) {
                    oldtopics[j]=topicsplit[j];
                } else if (oldUC<=1) {
                    if (preferUpper) {
                        if (newUC>oldUC) {
                            oldtopics[j]=topicsplit[j];
                        }
                    } else {
                        if (newUC<oldUC) {
                            oldtopics[j]=topicsplit[j];
                        }
                    }
                }
            }
            topics[topicsplitUC]=oldtopics;
        }
        topicUCForRecord[record]=topicsplitUC;


        if (i%10==0) {
            progress->setValue(i);
            pdlg.setValue(i);
            QApplication::processEvents();
        }
        if (pdlg.wasCanceled()) break;
    }
    statusbarShowMessage(tr("cleaning-up topics in database ..."));
    pdlg.setLabelText(tr("cleaning-up topics in database ..."));

    if (!pdlg.wasCanceled()) {
        bool wasEmit=datastore->getDoEmitSignals();
        datastore->setDoEmitSignals(false);
        int i=0;
        for (QHash<int, QStringList>::const_iterator it= topicUCForRecord.begin(); it!=topicUCForRecord.end(); ++it) {
            const int record=it.key();
            QString topic=datastore->getField(record, "topic").toString();
            QHash<QStringList,QStringList>::const_iterator itt=topics.find(it.value());
            if (itt!=topics.end()) {
                QString newTopic=itt->join("/");
                if (newTopic!=topic) {
                    datastore->setField(record, "topic", newTopic);
                    pdlg.addLongTextLine(tr("#%3: Replacing '%1' by '%2'!").arg(topic).arg(newTopic).arg(record));
                }
            }

            if (i%10==0) {
                progress->setValue(uuids.size()+i);
                pdlg.setValue(uuids.size()+i);
            }
            QApplication::processEvents();
            if (pdlg.wasCanceled()) break;
            i++;
        }
        datastore->setDoEmitSignals(wasEmit);
        datastore->resetModel();
        pdlg.setCancelButtonText(tr("&Close"));
        pdlg.connect(&pdlg, SIGNAL(cancelClicked()), &pdlg, SLOT(accept()));
        pdlg.exec();
    }
    QApplication::restoreOverrideCursor();
}

QString MainWindow::copyCSL(CSLOutputFormat format, const QString linetemplate)
{
    QString text;
    if (cmbCSLWhich->currentIndex()==0) {
        text=settings->GetPreviewStyleManager()->createPreview(cmbCSLs->currentIndex(), datastore->currentRecord(), cmbCSLLocales->currentText(), format);
    } else {
        QModernProgressDialog pdlg;
        pdlg.setLabelText(tr("exporting records to formatted text ..."));
        pdlg.setCancelButtonText(tr("&Cancel"));
        pdlg.setHasCancel(false);
        pdlg.openDelayed(500);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QStringList uuids=tvMainSortProxy->getUUIDs();
        int cnt=1;
        if (cmbCSLWhich->currentIndex()==1) {
            for (int i=0; i<uuids.size(); i++) {
                if (datastore->isSelected(uuids[i])) {
                    text=text+linetemplate.arg(settings->GetPreviewStyleManager()->createPreview(cmbCSLs->currentIndex(), datastore->getRecord(datastore->getRecordByUUID(uuids[i])), cmbCSLLocales->currentText(), format)).arg(cnt);
                    cnt++;
                }
                if (i%10==0) QApplication::processEvents();
                if (pdlg.wasCanceled()) {
                    return "";
                }
            }
        } else if (cmbCSLWhich->currentIndex()==2) {
            for (int i=0; i<uuids.size(); i++) {
                text=text+linetemplate.arg(settings->GetPreviewStyleManager()->createPreview(cmbCSLs->currentIndex(), datastore->getRecord(datastore->getRecordByUUID(uuids[i])), cmbCSLLocales->currentText(), format)).arg(cnt);
                cnt++;
                if (i%10==0) QApplication::processEvents();
                if (pdlg.wasCanceled()) {
                    return "";
                }
            }
        }
    }
    return text;
}

void MainWindow::copyCSLHTMLTags()
{
    QString text=copyCSL(ofHTML, QString("  <li><b>[%2]:</b> %1</li>\n"));
    if (text.size()>0) {
        text=QString("<ul>\n")+text+QString("</ul>");
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(text);
    }
}

void MainWindow::copyCSLPlainText()
{
    QString text=copyCSL(ofHTML, QString("  [%2]: %1\n"));

    if (text.size()>0) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(text);
    }
}

void MainWindow::copyCSLFormatted()
{
    QString text=copyCSL(ofHTML, QString("  <li><b>[%2]:</b> %1</li>\n"));
    if (text.size()>0) {
        text=QString("<ul>\n")+text+QString("</ul>");

        QTextEdit* edt=new QTextEdit(this);
        edt->setHtml(text);
        edt->selectAll();
        edt->copy();
        delete edt;
    }

}

void MainWindow::previewAnchorClicked(const QUrl& link) {
    QDesktopServices::openUrl(link);
}

QStringList MainWindow::getVisibleRecords() {
    return tvMainSortProxy->getUUIDs();
}

QStringList MainWindow::getFDFIDs() {
    fdfManager* fdf=settings->GetFDFManager();
    QStringList sl;
    for (int i=0; i<fdf->getSize(); i++) {
        sl.append(fdf->get(i)->ID);
    }
    return sl;
}


QStringList MainWindow::getFDFNames() {
    fdfManager* fdf=settings->GetFDFManager();
    QStringList sl;
    for (int i=0; i<fdf->getSize(); i++) {
        sl.append(fdf->get(i)->name[settings->GetLanguageID()]);
    }
    return sl;
}

QIcon MainWindow::getFDFIcon(QString fdfi) {
    fdfManager* fdf=settings->GetFDFManager();
    QStringList sl;
    for (int i=0; i<fdf->getSize(); i++) {
        if (fdf->get(i)->ID==fdfi) {
            return QIcon(fdf->get(i)->icon);
        }
    }
    return QIcon();
}

QString MainWindow::getFDFName(QString fdfi) {
    fdfManager* fdf=settings->GetFDFManager();
    QStringList sl;
    for (int i=0; i<fdf->getSize(); i++) {
        if (fdf->get(i)->ID==fdfi) {
            return fdf->get(i)->name[settings->GetLanguageID()];
        }
    }
    return "";
}

QString MainWindow::getPluginDirectory() {
    return QCoreApplication::applicationDirPath()+"/plugins";
}

LS3Plugin* MainWindow::getPluginByID(QString ID) {
    for (int i=0; i<m_plugins.size(); i++) {
        LS3Plugin* p=m_plugins[i];
        if (p->getName()==ID) return p;
    }
    return NULL;
}

void MainWindow::addProgressThread(LS3ProgressThread *thread) {
    dockThreadOverview->addThread(thread);
}
