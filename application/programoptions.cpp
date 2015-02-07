#include "programoptions.h"
#include <iostream>
#include "fdf.h"
#include <QNetworkProxy>

ProgramOptions::ProgramOptions( QString ini, QObject * parent, QApplication* app  ):
    QObject(parent)
{
    this->app=app;
    readConfig=true;
    iniFilename=ini;
    if (iniFilename.isEmpty()) {
        QFileInfo fi(QCoreApplication::applicationFilePath());
        iniFilename= fi.absolutePath()+"/"+fi.completeBaseName()+".ini";
    }
    //std::cout<<"config file is: "<<iniFilename.toStdString()<<std::endl;
    settings=NULL;
    fdf=new fdfManager(this);
    previewStyles=new PreviewStyleManager(this);

    // default values
    style= QApplication::style()->metaObject()->className();
    stylesheet="";
    languageID="en";
    currentFile="";
    lastOpenDBDirectory="";
    lastNewDBDirectory="";
    configDirectory=QCoreApplication::applicationDirPath()+"/config/";
    username="unknown";
    tableFontName="Arial";
    tableFontSize=8;
    extendedShortWidth=450;
    extendedLongWidth=-1;
    howDisplayReferenceDetails=0;
    defaultCurrency="EUR";
    defaultStatus=tr("present");
    startupFile="";
    specialCharacters="�����������������������������������������������������������������`^��������������";
    currentPreviewStyle=0;
    proxyHost=QNetworkProxy::applicationProxy().hostName();
    proxyType=QNetworkProxy::applicationProxy().type();
    proxyPort=QNetworkProxy::applicationProxy().port();

    readSettings();
}

ProgramOptions::~ProgramOptions()
{
    writeSettings();
}

void ProgramOptions::openSettingsDialog() {
    OptionsDialog* settingsDlg=new OptionsDialog(NULL);
    settingsDlg->edtUserName->setText(username);
    settingsDlg->cmbTableFont->setCurrentFont(QFont(tableFontName));
    settingsDlg->spnTableFont->setValue(tableFontSize);
    settingsDlg->edtConfigDir->setText(configDirectory);
    settingsDlg->edtInitialFile->setText(startupFile);
    settingsDlg->edtSpecialCharacters->setText(specialCharacters);
    settingsDlg->edtAndWords->setPlainText(QStringList(andWords).join("\n"));
    settingsDlg->edtNameAdditions->setPlainText(QStringList(nameAdditions.toList()).join("\n"));
    settingsDlg->edtNamePrefixes->setPlainText(QStringList(namePrefixes.toList()).join("\n"));
    settingsDlg->cmbPreviewStyles->clear();
    settingsDlg->cmbPreviewStyles->addItems(previewStyles->styles());
    settingsDlg->cmbPreviewStyles->setCurrentIndex(currentPreviewStyle);
    settingsDlg->cmbReferenceDetails->setCurrentIndex(howDisplayReferenceDetails);
    settingsDlg->spinProxyPort->setValue(getProxyPort());
    settingsDlg->edtProxyHost->setText(getProxyHost());
    settingsDlg->cmbProxyType->setCurrentIndex(qBound(0,getProxyType(),2));

    // find all available translations ... program is already in english, so add "en" by default (there is not en.qm file!)
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cd("translations");
    QStringList filters;
    filters << "*.qm";
    settingsDlg->cmbLanguage->clear();
    settingsDlg->cmbLanguage->addItem("en");
    QStringList sl=dir.entryList(filters, QDir::Files);
    for (int i=0; i<sl.size(); i++) {
        QString s=sl[i];
        int idx=s.indexOf(".");
        if (settingsDlg->cmbLanguage->findText(s.left(idx))<0) {
            settingsDlg->cmbLanguage->addItem(s.left(idx), Qt::CaseInsensitive);
        }
        //settingsDlg->cmbLanguage->addItem(sl[i].remove(".qm"), Qt::CaseInsensitive);
    }
    settingsDlg->cmbLanguage->setCurrentIndex( settingsDlg->cmbLanguage->findText(languageID));
    settingsDlg->cmbStyle->addItems(QStyleFactory::keys());
    settingsDlg->cmbStyle->setCurrentIndex(settingsDlg->cmbStyle->findText(style, Qt::MatchContains));


    {
        QFile f(GetConfigDirectory()+"/completers/currency.lst");
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            //std::cout<<"OK\n";
            QString s=QString::fromUtf8(f.readAll());
            QStringList sl=s.split('\n', QString::SkipEmptyParts);
            settingsDlg->cmbDefaultCurrency->clear();
            settingsDlg->cmbDefaultCurrency->addItems(sl);
        } //else std::cout<<"ERROR\n";
        int c=settingsDlg->cmbDefaultCurrency->findText(defaultCurrency, Qt::MatchContains);
        if (c==-1) {
            settingsDlg->cmbDefaultCurrency->addItem(defaultCurrency);
            c=settingsDlg->cmbDefaultCurrency->count()-1;
        }
        settingsDlg->cmbDefaultCurrency->setCurrentIndex(c);;
    }

    {
        QFile f(GetConfigDirectory()+"/completers/status.lst");
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            //std::cout<<"OK\n";
            QString s=QString::fromUtf8(f.readAll());
            QStringList sl=s.split('\n', QString::SkipEmptyParts);
            settingsDlg->cmbDefaultStatus->clear();
            settingsDlg->cmbDefaultStatus->addItems(sl);
        } //else std::cout<<"ERROR\n";
        int c=settingsDlg->cmbDefaultStatus->findText(defaultStatus, Qt::MatchContains);
        if (c==-1) {
            settingsDlg->cmbDefaultStatus->addItem(defaultStatus);
            c=settingsDlg->cmbDefaultStatus->count()-1;
        }
        settingsDlg->cmbDefaultStatus->setCurrentIndex(c);;
    }


    // find all available stylesheets
    dir.cd("../stylesheets");
    filters.clear();
    filters << "*.qss";
    settingsDlg->cmbStylesheet->clear();
    sl=dir.entryList(filters, QDir::Files);
    for (int i=0; i<sl.size(); i++) {
        settingsDlg->cmbStylesheet->addItem(sl[i].remove(".qss"), Qt::CaseInsensitive);
    }
    settingsDlg->cmbStylesheet->setCurrentIndex( settingsDlg->cmbStylesheet->findText(stylesheet));

    if (settingsDlg->exec() == QDialog::Accepted ){
        username=settingsDlg->edtUserName->text();
        languageID=settingsDlg->cmbLanguage->currentText();
        stylesheet=settingsDlg->cmbStylesheet->currentText();
        style=settingsDlg->cmbStyle->currentText();
        configDirectory=settingsDlg->edtConfigDir->text();
        tableFontName=settingsDlg->cmbTableFont->currentText();
        tableFontSize=settingsDlg->spnTableFont->value();
        defaultCurrency=settingsDlg->cmbDefaultCurrency->currentText();
        defaultStatus=settingsDlg->cmbDefaultStatus->currentText();
        startupFile=settingsDlg->edtInitialFile->text();
        specialCharacters=settingsDlg->edtSpecialCharacters->text();
        andWords=settingsDlg->edtAndWords->toPlainText().simplified().split(" ");
        for (int i=0; i<andWords.size(); i++) andWords[i]=andWords[i].toLower().trimmed();
        nameAdditions=QSet<QString>::fromList(settingsDlg->edtNameAdditions->toPlainText().simplified().split(" "));
        namePrefixes=QSet<QString>::fromList(settingsDlg->edtNamePrefixes->toPlainText().simplified().split(" "));
        currentPreviewStyle=settingsDlg->cmbPreviewStyles->currentIndex();
        howDisplayReferenceDetails=settingsDlg->cmbReferenceDetails->currentIndex();
        setProxyHost(settingsDlg->edtProxyHost->text());
        setProxyPort(settingsDlg->spinProxyPort->value());
        setProxyType(settingsDlg->cmbProxyType->currentIndex());

        writeSettings();
        readSettings();
    }

}

void ProgramOptions::writeSettings() {
    emit storeSettings();
    settings->setValue("mainwindow/last_opendb_directory", lastOpenDBDirectory);
    settings->setValue("mainwindow/last_newdb_directory", lastNewDBDirectory);
    settings->setValue("mainwindow/last_opened_db", currentFile);
    settings->setValue("mainwindow/table_font_name", tableFontName);
    settings->setValue("mainwindow/table_font_size", tableFontSize);
    settings->setValue("mainwindow/extendedShortWidth", extendedShortWidth);
    settings->setValue("mainwindow/extendedLongWidth", extendedLongWidth);
    settings->setValue("mainwindow/currentPreviewStyle", currentPreviewStyle);
    settings->setValue("mainwindow/howDisplayReferenceDetails", howDisplayReferenceDetails);
    settings->setValue("litsoz/config_directory", configDirectory);
    settings->setValue("litsoz/language", languageID);
    settings->setValue("litsoz/stylesheet", stylesheet);
    settings->setValue("litsoz/style", style);
    settings->setValue("litsoz/username", username);
    settings->setValue("litsoz/defaultCurrency", defaultCurrency);
    settings->setValue("litsoz/defaultStatus", defaultStatus);
    settings->setValue("litsoz/startupFile", startupFile);
    settings->setValue("litsoz/osk_specialCharacters", specialCharacters);
    settings->setValue("litsoz/proxy/host", proxyHost);
    settings->setValue("litsoz/proxy/port", proxyPort);
    settings->setValue("litsoz/proxy/type", proxyType);
    saveConfigFiles();
}


void ProgramOptions::readSettings() {
    if (!settings) {
        if (iniFilename=="native") { // on windows: registry, on Linux/MacOS: default
            settings= new QSettings(this);
        } else if (iniFilename=="native_inifile") { // ensures a INI file at the default location, even on windows
        } else if (iniFilename=="native_inifile") { // ensures a INI file at the default location, even on windows
            settings= new QSettings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName(), this);
        } else {
            settings= new QSettings(iniFilename, QSettings::IniFormat, this);
        }
    }
    lastOpenDBDirectory=settings->value("mainwindow/last_opendb_directory", lastOpenDBDirectory).toString();
    lastNewDBDirectory=settings->value("mainwindow/last_newdb_directory", lastNewDBDirectory).toString();
    tableFontName=settings->value("mainwindow/table_font_name", tableFontName).toString();
    tableFontSize=settings->value("mainwindow/table_font_size", tableFontSize).toInt();
    emit tableFontChanged(tableFontName, tableFontSize);
    currentPreviewStyle= settings->value("mainwindow/currentPreviewStyle", 0).toInt();
    howDisplayReferenceDetails = settings->value("mainwindow/howDisplayReferenceDetails", 0).toInt();
    username=settings->value("litsoz/username", username).toString();
    startupFile=settings->value("litsoz/startupFile", startupFile).toString();
    defaultCurrency=settings->value("litsoz/defaultCurrency", defaultCurrency).toString();
    defaultStatus=settings->value("litsoz/defaultStatus", defaultStatus).toString();
    extendedShortWidth=settings->value("mainwindow/extendedShortWidth", extendedShortWidth).toInt();
    extendedLongWidth=settings->value("mainwindow/extendedLongWidth", extendedLongWidth).toInt();
    specialCharacters=settings->value("litsoz/osk_specialCharacters", specialCharacters).toString();
    proxyHost=(settings->value("litsoz/proxy/host", proxyHost).toString());
    proxyPort=(settings->value("litsoz/proxy/port", proxyPort).toUInt());
    proxyType=((QNetworkProxy::ProxyType)settings->value("litsoz/proxy/type", proxyType).toInt());

    QString tmp=settings->value("litsoz/config_directory", configDirectory).toString();
    if (tmp!=configDirectory || readConfig) {
        configDirectory=tmp;
        initConfig();
        emit configDirReread();
    }

    languageID=settings->value("litsoz/language", languageID).toString();
    if (languageID != "en") { // english is default
        QDir d(QCoreApplication::applicationDirPath()+"/translations");
        QStringList filters;
        filters << "*.qm";
        QStringList sl=d.entryList(filters, QDir::Files);
        for (int i=0; i<sl.size(); i++) {
            QString s=sl[i];
            if (s.startsWith(languageID+".")) {
                QString fn=d.absoluteFilePath(s);
                //std::cout<<"loading translation '"<<fn.toStdString()<<"' ... \n";
                QTranslator* translator=new QTranslator(this);
                if (app && translator->load(fn)) {
                    app->installTranslator(translator);
                    //std::cout<<"OK\n";
                } else {
                    //std::cout<<"ERROR\n";
                }
            }
        }
        emit languageChanged(languageID);

    }

    /*if (languageID != "en") { // english is default
        QString fn=QString(QCoreApplication::applicationDirPath()+"/translations/%1.qm").arg(languageID);
        std::cout<<"loading translation '"<<fn.toStdString()<<"' ... ";
        QTranslator* translator=new QTranslator(this);
        if (app && translator->load(fn)) {
            app->installTranslator(translator);
            std::cout<<"OK\n";
            emit languageChanged(languageID);
        } else {
            std::cout<<"ERROR\n";
        }
    }*/
    style=settings->value("litsoz/style", style).toString();
    if (app) {
        app->setStyle(style);
    }
    emit styleChanged(style);
    stylesheet=settings->value("litsoz/stylesheet", stylesheet).toString();
    if (app) {
        QString fn=QString(QCoreApplication::applicationDirPath()+"/stylesheets/%1.qss").arg(stylesheet);
        //std::cout<<"loading stylesheet '"<<fn.toStdString()<<"' ... ";
        QFile f(fn);
        f.open(QFile::ReadOnly);
        QTextStream s(&f);
        QString qss=s.readAll();
        //std::cout<<qss.toStdString()<<std::endl;
        app->setStyleSheet(qss);
        //std::cout<<"OK\n";
    }
    emit stylesheetChanged(stylesheet);


    emit configDataChanged();
    loadConfigFiles();
}

void ProgramOptions::saveConfigFiles() {
    QDir cd(configDirectory);

    {   QFile f(cd.absoluteFilePath("and_words.txt"));
        if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
            f.write(QStringList(andWords).join("\n").toUtf8());
        }
        f.close();
    }
    {   QFile f(cd.absoluteFilePath("name_prefix.txt"));
        if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
            f.write(QStringList(namePrefixes.toList()).join("\n").toUtf8());
        }
    }
    {   QFile f(cd.absoluteFilePath("name_additions.txt"));
        if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
            f.write(QStringList(nameAdditions.toList()).join("\n").toUtf8());
        }
    }
}

void ProgramOptions::loadConfigFiles() {
    QDir cd(configDirectory);

    {   QFile f(cd.absoluteFilePath("and_words.txt"));
        andWords.clear();
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QString s=QString::fromUtf8(f.readAll());
            andWords=s.split('\n', QString::SkipEmptyParts);
        }
        f.close();
    }
    {   QFile f(cd.absoluteFilePath("name_prefix.txt"));
        namePrefixes.clear();
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QString s=QString::fromUtf8(f.readAll());
            namePrefixes=QSet<QString>::fromList(s.split('\n', QString::SkipEmptyParts));
        }
        f.close();
    }
    {   QFile f(cd.absoluteFilePath("name_additions.txt"));
        nameAdditions.clear();
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QString s=QString::fromUtf8(f.readAll());
            nameAdditions=QSet<QString>::fromList(s.split('\n', QString::SkipEmptyParts));
        }
        f.close();
    }
}


void ProgramOptions::initConfig() {
    //std::cout<<"reading config dir '"<<configDirectory.toStdString()<<"' ... \n";
    readConfig=false;
    // search all .fdf files and load them into the (previously cleared) fdfManager
    fdf->clear();
    QDir dir(configDirectory);
    dir.cd("fdf");
    QStringList filters;
    filters << "*.fdf";
    QStringList sl=dir.entryList(filters, QDir::Files);
    for (int i=0; i<sl.size(); i++) {
        //std::cout<<" loading '"<<dir.absoluteFilePath(sl[i]).toStdString()<<"' ... ";
        try {
            if (fdf->loadFile(dir.absoluteFilePath(sl[i]))) {
                //std::cout<<"OK\n";
            } else {
                //std::cout<<"ERROR: "<<fdf->getLastError().toStdString()<<"\n";
            }
        } catch(std::exception& E) {
            //std::cout<<"UNKNOWN ERROR\n";
        }
    }

}


void ProgramOptions::setProxyHost(const QString &host)
{
    //proxy.setHostName(host);
    proxyHost=host;
}

void ProgramOptions::setProxyPort(quint16 port)
{
    //proxy.setPort(port);
    proxyPort=port;
}

quint16 ProgramOptions::getProxyPort() const
{
    //return proxy.port();
    return proxyPort;
}

QString ProgramOptions::getProxyHost() const
{
    //return proxy.hostName();
    return proxyHost;
}

int ProgramOptions::getProxyType() const
{
    //return proxy.type();
    return proxyType;
}

void ProgramOptions::setProxyType(int type)
{
    if (type<0) proxyType=0; //proxy.setType(QNetworkProxy::DefaultProxy);
    else proxyType=type; //proxy.setType(type);
}

void ProgramOptions::setQNetworkProxy(QNetworkProxy &proxy)
{
    proxy.setHostName(getProxyHost());
    proxy.setPort(getProxyPort());
    proxy.setType((QNetworkProxy::ProxyType)getProxyType());

}