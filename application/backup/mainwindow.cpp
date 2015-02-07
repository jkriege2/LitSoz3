


#include "mainwindow.h"
#include <iostream>

MainWindow::MainWindow(ProgramOptions* settings):
    QMainWindow(NULL)
{
    this->settings=settings;
    fdf=new fdfManager;
    data=NULL;
    mapper=NULL;
    dataConnectionName="litsoz3_main";
    languageID="en";
    lastOpenDBDirectory="";
    lastNewDBDirectory="";
    configDirectory=QCoreApplication::applicationDirPath()+"/config/";
    IniFilename=QCoreApplication::applicationDirPath()+"/litsoz3.ini";
    init();
    setCurrentFile("");
}

MainWindow::~MainWindow()
{
    delete fdf;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    closeDB();
    writeSettings();
    event->accept();
}

void MainWindow::newFile()
{
    /*MainWindow *other = new MainWindow;
    other->move(x() + 40, y() + 40);
    other->show();*/
    QString fileName = QFileDialog::getSaveFileName(this, tr("Create New Database ..."), lastNewDBDirectory, tr("Literature Databases (*.db)"));
    if (!fileName.isEmpty()) {
        lastNewDBDirectory=QFileInfo(fileName).dir().absolutePath();
        newDB(fileName);
        loadDB(fileName);
    }
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Database ..."), lastOpenDBDirectory, tr("Literature Databases (*.db)"));
    if (!fileName.isEmpty()) {
        lastOpenDBDirectory=QFileInfo(fileName).dir().absolutePath();
        loadDB(fileName);
    }
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        loadDB(action->data().toString());
    }
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About SDI"),
            tr("The <b>SDI</b> example demonstrates how to write single "
               "document interface applications using Qt."));
}

void MainWindow::init()
{
    createWidgets();
    setCentralWidget(tabMain);
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();

    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::connectWidgets() {
    tvMain->setModel(data);
    tvMain->resizeColumnsToContents();
    //tvMain->setVisible(true);

    connect(dbpreviousAct, SIGNAL(triggered()), mapper, SLOT(toPrevious()));
    connect(dbnextAct, SIGNAL(triggered()), mapper, SLOT(toNext()));
    connect(dbfirstAct, SIGNAL(triggered()), mapper, SLOT(toFirst()));
    connect(dblastAct, SIGNAL(triggered()), mapper, SLOT(toLast()));
    //connect(dbsubmitAct, SIGNAL(triggered()), mapper, SLOT(submit()));
    //connect(dbrejectAct, SIGNAL(triggered()), mapper, SLOT(revert()));
    connect(mapper, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDBActions(int)));
    connect(mapper, SIGNAL(currentIndexChanged(int)), tvMain, SLOT(selectRow( int)));
    connect(tvMain->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(tableRowChanged(const QModelIndex&, const QModelIndex&)));
    connect(data, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(tableDataChanged(const QModelIndex&, const QModelIndex&)));
    dbfirstAct->setEnabled(true);
    dbnextAct->setEnabled(true);
    dbpreviousAct->setEnabled(true);
    dblastAct->setEnabled(true);


    emit databaseLoaded(true);
}

void MainWindow::disconnectWidgets() {
    tvMain->setModel(NULL);
    //tvMain->setVisible(false);

    disconnect(dbpreviousAct, SIGNAL(triggered()), mapper, SLOT(toPrevious()));
    disconnect(dbnextAct, SIGNAL(triggered()), mapper, SLOT(toNext()));
    disconnect(dbfirstAct, SIGNAL(triggered()), mapper, SLOT(toFirst()));
    disconnect(dblastAct, SIGNAL(triggered()), mapper, SLOT(toLast()));
    //disconnect(dbsubmitAct, SIGNAL(triggered()), mapper, SLOT(submit()));
    //disconnect(dbrejectAct, SIGNAL(triggered()), mapper, SLOT(revert()));
    disconnect(mapper, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDBActions(int)));
    disconnect(mapper, SIGNAL(currentIndexChanged(int)), tvMain, SLOT(selectRow( int)));
    disconnect(tvMain->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(tableRowChanged(const QModelIndex&, const QModelIndex&)));
    disconnect(data, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(tableDataChanged(const QModelIndex&, const QModelIndex&)));

    emit databaseLoaded(false);
}


void MainWindow::createWidgets() {
    std::cout<<"createWidgets()";
    tabMain = new QTabWidget(this);

    tvMain = new QTableView(tabMain);
    tvMain->setSelectionBehavior(QAbstractItemView::SelectRows);
    tvMain->setSelectionMode(QAbstractItemView::SingleSelection);
    tabMain->addTab(tvMain, tr("Table"));
    std::cout<<std::endl;
}

void MainWindow::createActions()
{
    std::cout<<"createActions()";
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new database"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing database"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    closeAct = new QAction(tr("&Close "), this);
    closeAct->setShortcut(tr("Ctrl+W"));
    closeAct->setStatusTip(tr("Close the current database"));
    connect(closeAct, SIGNAL(triggered()), this, SLOT(closeDB()));
    connect(this, SIGNAL(databaseLoaded(bool)), closeAct, SLOT(setEnabled(bool)));
    closeAct->setEnabled(false);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Alt+F4"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    optionsAct = new QAction(tr("&Settings"), this);
    optionsAct->setStatusTip(tr("Application settings dialog"));
    connect(optionsAct, SIGNAL(triggered()), settings, SLOT(openSettingsDialog()));

/*    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));*/

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    dbnextAct = new QAction(QIcon(":/images/next.png"), tr("&Next"), this);
    dbnextAct->setStatusTip(tr("display next record"));
    dbnextAct->setEnabled(false);

    dbpreviousAct = new QAction(QIcon(":/images/previous.png"), tr("&Previous"), this);
    dbpreviousAct->setStatusTip(tr("display previous record"));
    dbpreviousAct->setEnabled(false);

    dbfirstAct = new QAction(QIcon(":/images/first.png"), tr("&First"), this);
    dbfirstAct->setStatusTip(tr("display first record"));
    dbfirstAct->setEnabled(false);

    dblastAct = new QAction(QIcon(":/images/last.png"), tr("&Last"), this);
    dblastAct->setStatusTip(tr("display last record"));
    dblastAct->setEnabled(false);

    dbrefreshAct = new QAction(QIcon(":/images/refresh.png"), tr("&Refresh"), this);
    dbrefreshAct->setStatusTip(tr("refresh displayed records"));
    connect(dbrefreshAct, SIGNAL(triggered()), this, SLOT(dbRefresh()));
    connect(this, SIGNAL(databaseLoaded(bool)), dbrefreshAct, SLOT(setEnabled(bool)));
    dbrefreshAct->setEnabled(false);

    dbinsertAct = new QAction(QIcon(":/images/add.png"), tr("&Insert"), this);
    dbinsertAct->setStatusTip(tr("insert a new record"));
    connect(dbinsertAct, SIGNAL(triggered()), this, SLOT(dbInsert()));
    connect(this, SIGNAL(databaseLoaded(bool)), dbinsertAct, SLOT(setEnabled(bool)));
    dbinsertAct->setEnabled(false);

    dbdeleteAct = new QAction(QIcon(":/images/delete.png"), tr("&Delete"), this);
    dbdeleteAct->setStatusTip(tr("delete the current record"));
    connect(dbdeleteAct, SIGNAL(triggered()), this, SLOT(dbDelete()));
    connect(this, SIGNAL(databaseLoaded(bool)), dbdeleteAct, SLOT(setEnabled(bool)));
    dbdeleteAct->setEnabled(false);

    dbsubmitAct = new QAction(QIcon(":/images/ok.png"), tr("&Submit"), this);
    dbsubmitAct->setStatusTip(tr("submit changes to the current record"));
    dbsubmitAct->setEnabled(false);
    connect(dbsubmitAct, SIGNAL(triggered()), this, SLOT(dbSubmit()));

    dbrejectAct = new QAction(QIcon(":/images/reject.png"), tr("&Reject"), this);
    dbrejectAct->setStatusTip(tr("reject changes to the current record"));
    dbrejectAct->setEnabled(false);
    connect(dbrejectAct, SIGNAL(triggered()), this, SLOT(dbReject()));

    std::cout<<std::endl;
}

void MainWindow::createMenus()
{
    std::cout<<"createMenus()";
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(closeAct);
    separatorAct = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(recentFileActs[i]);
    fileMenu->addSeparator();
    fileMenu->addAction(optionsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
    updateRecentFileActions();

    editMenu = menuBar()->addMenu(tr("&Edit"));
//    editMenu->addAction(pasteAct);

    databaseMenu = menuBar()->addMenu(tr("&Database"));
    databaseMenu->addAction(dbfirstAct);
    databaseMenu->addAction(dbpreviousAct);
    databaseMenu->addAction(dbnextAct);
    databaseMenu->addAction(dblastAct);
    databaseMenu->addSeparator();
    databaseMenu->addAction(dbinsertAct);
    databaseMenu->addAction(dbdeleteAct);
    databaseMenu->addSeparator();
    databaseMenu->addAction(dbsubmitAct);
    databaseMenu->addAction(dbrejectAct);
    databaseMenu->addSeparator();
    databaseMenu->addAction(dbrefreshAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
    std::cout<<std::endl;
}

void MainWindow::createToolBars() {
    std::cout<<"createToolBars()";
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);

    /*editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);*/

    databaseToolBar = addToolBar(tr("Database"));
    databaseToolBar->addAction(dbfirstAct);
    databaseToolBar->addAction(dbpreviousAct);
    databaseToolBar->addAction(dbnextAct);
    databaseToolBar->addAction(dblastAct);
    databaseToolBar->addSeparator();
    databaseToolBar->addAction(dbinsertAct);
    databaseToolBar->addAction(dbdeleteAct);
    databaseToolBar->addSeparator();
    databaseToolBar->addAction(dbsubmitAct);
    databaseToolBar->addAction(dbrejectAct);
    databaseToolBar->addSeparator();
    databaseToolBar->addAction(dbrefreshAct);
    std::cout<<std::endl;
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
    labDBStatusToolBar=new QLabel(tr("database status"));
    statusBar()->addPermanentWidget(labDBStatusToolBar);
}

void MainWindow::readSettings()
{
    QSettings settings(IniFilename, QSettings::IniFormat);
    settings.sync();
    QPoint pos = settings.value("mainwindow/pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("mainwindow/size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
    lastOpenDBDirectory=settings.value("mainwindow/last_opendb_directory", lastOpenDBDirectory).toString();
    lastNewDBDirectory=settings.value("mainwindow/last_newdb_directory", lastNewDBDirectory).toString();
    languageID=settings.value("litsoz/language", languageID).toString();
    initConfig(settings.value("litsoz/config_directory", configDirectory).toString());
}

void MainWindow::writeSettings()
{
    QSettings settings(IniFilename, QSettings::IniFormat);
    settings.setValue("mainwindow/pos", pos());
    settings.setValue("mainwindow/size", size());
    settings.setValue("mainwindow/last_opendb_directory", lastOpenDBDirectory);
    settings.setValue("mainwindow/last_newdb_directory", lastNewDBDirectory);
    settings.setValue("litsoz/config_directory", configDirectory);
    settings.setValue("litsoz/language", languageID);
}

void MainWindow::initConfig(QString configDir) {
    if (configDirectory!=configDir && configDir!="") {
        configDirectory=configDir;
    }

    // search all .fdf files and load them into the (previously cleared) fdfManager
    fdf->clear();
    QDir dir(configDirectory);
    dir.cd("fdf");
    QStringList filters;
    filters << "*.fdf";
    QStringList sl=dir.entryList(filters, QDir::Files);
    for (int i=0; i<sl.size(); i++) {
        std::cout<<" loading '"<<dir.absoluteFilePath(sl[i]).toStdString()<<"' ... ";
        try {
            if (fdf->loadFile(dir.absoluteFilePath(sl[i]))) {
                std::cout<<"OK\n";
            } else {
                std::cout<<"ERROR: "<<fdf->getLastError().toStdString()<<"\n";
            }
        } catch(std::exception& E) {
            std::cout<<"UNKNOWN ERROR\n";
        }
    }
}

void MainWindow::showDBError(const QSqlError &err)
{
    QMessageBox::critical(this, tr("Database Error"),
                tr("Error during database operation: \n<b>%1</b>").arg(err.text()));
}

void MainWindow::closeDB() {
    disconnectWidgets();

    if (data!=NULL) {
        delete mapper;
        delete data;
        data=NULL;
        mapper=NULL;
    }

    if (QSqlDatabase::contains(dataConnectionName.toLatin1())) {
        QSqlDatabase::database(dataConnectionName.toLatin1()).close();
        QSqlDatabase::removeDatabase(dataConnectionName.toLatin1());
    }

    setCurrentFile("");
}

bool MainWindow::loadDB(const QString &filename)
{
    statusBar()->showMessage(tr("opening database '%1' ...").arg(filename), 2000);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    // if a database is opened: close it!
    closeDB();

    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        QMessageBox::critical(this, tr("Database Error"), tr("LitSoz %1 needs the SQLite driver for Qt").arg(AutoVersion::FULLVERSION_STRING));
        return false;
    }

    // connect to database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", dataConnectionName.toLatin1());
    db.setDatabaseName(filename);

    if (!db.open()) {
        showDBError(db.lastError());
        return false;
    }



    // Create the data model
    data = new QSqlRelationalTableModel(this, db);
    data->setTable("lit");
    data->setEditStrategy(QSqlTableModel::OnManualSubmit);
    mapper = new QDataWidgetMapper(this);
    mapper->setModel(data);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    // TODO: implement mapper completely!!!!!!!!! connect to other widgets (QLineEdit, ...)
    // TODO: implement goto dataset with ID=<value>! (linear search with toNext() from mapper)

    // Remeber the indexes of the columns
    // authorIdx = model->fieldIndex("author");
    // genreIdx = model->fieldIndex("genre");

    // Set the relations to the other database tables
    // model->setRelation(authorIdx, QSqlRelation("authors", "id", "name"));
    // model->setRelation(genreIdx, QSqlRelation("genres", "id", "name"));

    // Set the localized header captions
    /*model->setHeaderData(authorIdx, Qt::Horizontal, tr("Author Name"));
    model->setHeaderData(genreIdx, Qt::Horizontal, tr("Genre"));
    model->setHeaderData(model->fieldIndex("title"), Qt::Horizontal, tr("Title"));
    model->setHeaderData(model->fieldIndex("year"), Qt::Horizontal, tr("Year"));
    model->setHeaderData(model->fieldIndex("rating"), Qt::Horizontal, tr("Rating"));*/

    // Populate the model
    if (!data->select()) {
        showDBError(data->lastError());
        return false;
    }

    setCurrentFile(filename);
    connectWidgets();
    mapper->toFirst();
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("database '%1' opened ...").arg(filename), 2000);
    return true;
}

bool MainWindow::newDB(const QString &filename)
{
    statusBar()->showMessage(tr("creating database '%1' ...").arg(filename), 2000);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QString connectionName="litsoz3_createconnection";

    QSqlDatabase crdb = QSqlDatabase::addDatabase("QSQLITE", connectionName.toLatin1());
    crdb.setDatabaseName(filename);

    if (!crdb.open()) {
        showDBError(crdb.lastError());
        return false;
    }

    QStringList tables = crdb.tables();
    if (tables.contains("lit", Qt::CaseInsensitive)) {
        return true;
    }

    QSqlQuery* q=new QSqlQuery(crdb);
    QString sqlstr="";
    sqlstr ="create table lit(";
    sqlstr+="  num integer primary key";   // unique dataset number inside table
    sqlstr+=", id varchar";                 // item identifier, used to cite this item
    sqlstr+=", type varchar";               // type of publication (book, manula, article ...)
    sqlstr+=", title varchar";              // title of publication
    sqlstr+=", subtitle varchar";           // subtitles
    sqlstr+=", authors varchar";            // authors as "surname, first name; surname, first name; ..."
    sqlstr+=", editors varchar";            // editors as "surname, first name; surname, first name; ..."
    sqlstr+=", year integer";               // year of publication
    sqlstr+=", viewdate varchar";           // when has a webpage been viewed the last time
    sqlstr+=", isbn varchar";               // ISBN, ISSN
    sqlstr+=", publisher varchar";          // publisher
    sqlstr+=", places varchar";             // where is the publisher situated
    sqlstr+=", institution varchar";        // institution of the reference (i.e. university for theses, company for technical reports ...)
    sqlstr+=", journal varchar";            // journal where an article appeared
    sqlstr+=", volume varchar";             // volume of the journal, in a book series, ...
    sqlstr+=", number varchar";             // number of journal inside a volume
    sqlstr+=", booktitle varchar";          // title of the book the reference appeared in
    sqlstr+=", series varchar";             // series, a book appeared in
    sqlstr+=", chapter varchar";            // referenced chapter inside a book
    sqlstr+=", edition integer";            // edition of the book
    sqlstr+=", price float";                // price of the reference
    sqlstr+=", source varchar";             // where is the reference from
    sqlstr+=", abstract varchar";           // abstract, content, ... of the reference
    sqlstr+=", comments varchar";           // comments on the reference
    sqlstr+=", rating integer";             // rate the reference 1..5
    sqlstr+=", doi varchar";                // doi of an article
    sqlstr+=", files varchar";              // file associated with this reference as "file\nfile\n..."
    sqlstr+=", urls varchar";               // urls asoociated with this reference as "URL\nURL\n..."
    sqlstr+=", status integer";             // current status "to be ordered", "has been ordered", ...
    sqlstr+=", origin varchar";             // where did I get the reference from?
    sqlstr+=", librarynum varchar";         // number of reference within a library
    sqlstr+=", owner varchar";              // who added this reference to the database?
    sqlstr+=", keywords varchar";           // keywords for this reference
    sqlstr+=", addeddate varchar";          // date when this reference has been added
    sqlstr+=")";
    if (!q->exec(sqlstr.toLatin1())) {
        showDBError(q->lastError());
        delete q;
        return false;
    }
    delete q;

    // close connection to the database that was used to create a new table
    // this is OK, as the QSqlQuery q has already been deleted.
    QSqlDatabase::removeDatabase(connectionName.toLatin1());

    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Database '%1' created ...").arg(filename), 2000);

    return true;
}

bool MainWindow::isDBOpened() {
    if (data==NULL) return false;
    if (!QSqlDatabase::contains(dataConnectionName.toLatin1())) return false;
    if (!QSqlDatabase::database(dataConnectionName.toLatin1()).isOpen()) return false;
    return true;
}


void MainWindow::dbSubmit() {
    if (isDBOpened()) {
        int row=mapper->currentIndex();
        if ((!mapper->submit()) || (!data->submitAll())) {
            showDBError(data->lastError());
            return ;
        }
        mapper->setCurrentIndex(row);
        dbsubmitAct->setEnabled(false);
        dbrejectAct->setEnabled(false);
        dbinsertAct->setEnabled(true);
    }
}

void MainWindow::dbReject() {
    if (isDBOpened()) {
        int row=mapper->currentIndex();
        mapper->revert();
        data->revertAll();
        mapper->setCurrentIndex(row);
        dbsubmitAct->setEnabled(false);
        dbrejectAct->setEnabled(false);
        dbinsertAct->setEnabled(true);
    }
}

void MainWindow::dbRefresh() {
    if (isDBOpened()) {
        dbSubmit();
        int row=mapper->currentIndex();
        if (!data->select()) {
            showDBError(data->lastError());
            return ;
        }
        mapper->setCurrentIndex(row);
    }
}

void MainWindow::dbInsert() {
    if (isDBOpened()) {
        int row=data->rowCount();
        if (!data->insertRow(row)) {
            showDBError(data->lastError());
            return ;
        }
        mapper->setCurrentIndex(row);
        dbsubmitAct->setEnabled(true);
        dbrejectAct->setEnabled(true);
        dbinsertAct->setEnabled(false);
    }
}

void MainWindow::updateDBActions(int row) {
    dbpreviousAct->setEnabled(row > 0);
    dbfirstAct->setEnabled(row > 0);
    dbnextAct->setEnabled(row < data->rowCount() - 1);
    dblastAct->setEnabled(row < data->rowCount() - 1);
    dbsubmitAct->setEnabled(false);
    dbrejectAct->setEnabled(false);
    dbinsertAct->setEnabled(true);
    dbdeleteAct->setEnabled(true);
    labDBStatusToolBar->setText(tr("dataset %1 / %2").arg(row+1).arg(data->rowCount()));
}

void MainWindow::tableRowChanged(const QModelIndex& current, const QModelIndex& previous) {
    if (isDBOpened()) {
        if (data->isDirty(previous)) {
            if (QMessageBox::question(this, tr("Submit Changes"), tr("Do you want to submit the changes?"), QMessageBox::Yes | QMessageBox::No)==QMessageBox::Yes) {
                dbSubmit();
            } else {
                dbReject();
            }
        }
        mapper->setCurrentIndex(current.row());
    }
}

void MainWindow::tableDataChanged(const QModelIndex& current, const QModelIndex& previous) {
    if (isDBOpened()) {
        dbsubmitAct->setEnabled(true);
        dbrejectAct->setEnabled(true);
        dbinsertAct->setEnabled(false);
    }
}


void MainWindow::dbDelete() {
    if (isDBOpened()) {
        if (QMessageBox::question(this, tr("Delete record"), tr("Do you really want to delete the current record?"), QMessageBox::Yes | QMessageBox::No)==QMessageBox::Yes) {
            std::cout<<"deleting curretn row!\n";
            data->removeRows(mapper->currentIndex(), 1);
            data->select();
            dbsubmitAct->setEnabled(false);
            dbrejectAct->setEnabled(false);
        }
    }
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();

    setWindowTitle(tr("%2 - %1").arg(curFile).arg(tr("LitSoz %1").arg(QApplication::applicationVersion())));

    // update recent files list in ini file
    QSettings settings(IniFilename, QSettings::IniFormat);
    QStringList files = settings.value("mainwindow/recentfilelist").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();
    files.removeDuplicates();
    if (files.size()>0) for (int j = files.size()-1; j >=0 ; j--) {
        if (files[j].trimmed().isEmpty()) {
            files.removeAt(j);
        } else {
            recentFileActs[j]->setVisible(false);
        }
    }
    settings.setValue("mainwindow/recentfilelist", files);
    updateRecentFileActions();

}

void MainWindow::updateRecentFileActions(){
    QSettings settings(IniFilename, QSettings::IniFormat);
    QStringList files = settings.value("mainwindow/recentfilelist").toStringList();
    files.removeDuplicates();
    if (files.size()>0) for (int j = files.size()-1; j >=0 ; j--) {
        if (files[j].trimmed().isEmpty()) {
            files.removeAt(j);
        } else {
            recentFileActs[j]->setVisible(false);
        }
    }
    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);
    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    separatorAct->setVisible(numRecentFiles > 0);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
