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

#include "programoptions.h"
#include <iostream>
#include "fdf.h"
#include <QNetworkProxy>
#include "languagetools.h"

ProgramOptions *ProgramOptions::m_instance=NULL;

ProgramOptions *ProgramOptions::instance()
{
    return m_instance;
}

ProgramOptions::ProgramOptions( QString ini, QObject * parent, QApplication* app  ):
    QObject(parent)
{
    if (!m_instance) m_instance=this;
    this->app=app;
    QFileInfo fi(QCoreApplication::applicationFilePath());
    configDirectory=QDir::homePath()+"/."+fi.completeBaseName()+"/";

    QDir d=fi.absoluteDir();
    d.mkpath(configDirectory);
    d.mkpath(configDirectory+"/completers/");

    readConfig=true;
    iniFilename=ini;
    if (iniFilename.isEmpty()) {
        iniFilename= configDirectory+"/"+fi.completeBaseName()+".ini";
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
    configDirectory=QDir::homePath()+"/."+fi.completeBaseName()+"/";



    assetsDirectory=QCoreApplication::applicationDirPath()+"/assets/";

    LanguageRecognizer::globalInstance()->clearInit();
    QStringList langDirs;
    langDirs<<QString(assetsDirectory+"/language_recognition/");
    langDirs<<QString(":/language_recognition/");
    LanguageRecognizer::globalInstance()->initFromDirs(langDirs);

    // copy initial assets files
    {
        QDir dass(assetsDirectory);
        QStringList assfiles=dass.entryList(QDir::Files);
        for (int i=0; i<assfiles.size(); i++) {
            if (!QFile::exists(configDirectory+assfiles[i])) QFile::copy(dass.absoluteFilePath(assfiles[i]), configDirectory+assfiles[i]);
        }

    }
    {
        QDir dass(assetsDirectory+"/completers/");
        QStringList assfiles=dass.entryList(QDir::Files);
        for (int i=0; i<assfiles.size(); i++) {
            if (!QFile::exists(configDirectory+"/completers/"+assfiles[i])) QFile::copy(dass.absoluteFilePath(assfiles[i]), configDirectory+"/completers/"+assfiles[i]);
        }

    }
    username="unknown";
    tableFontName="Arial";
    tableFontSize=8;
    extendedShortWidth=450;
    extendedLongWidth=-1;
    howDisplayReferenceDetails=0;
    defaultCurrency="EUR";
    defaultStatus=tr("present");
    startupFile="";
    specialCharacters=QString::fromUtf8("ÀàÁáÂâÃãÄäÅåÆæÇçÈèÉéÊêËëÌìÍíÎîÏïÐðÑñÒòÓóÔôÕõÖöØøÙùÚúÛûÜüÝýÞþßÿµ¿¡`^¢£¤¥«»·×÷¶±");
    currentPreviewStyle1=0;
    currentPreviewStyle2=1;
    currentPreviewLocale1="en-US";
    currentPreviewLocale2="en-US";
    proxyHost=QNetworkProxy::applicationProxy().hostName();
    proxyType=QNetworkProxy::applicationProxy().type();
    proxyPort=QNetworkProxy::applicationProxy().port();

    previewStyles->searchCSL(assetsDirectory+"/csl/");
    previewStyles->searchCSLLocales(assetsDirectory+"/csl_locales/");


    readSettings();
}

ProgramOptions::~ProgramOptions()
{
    writeSettings();
}

QString ProgramOptions::GetConfigDirectory()
{
    return QDir(configDirectory).absolutePath ()+"/";
}

QString ProgramOptions::GetAssetsDirectory()
{
    return QDir(assetsDirectory).absolutePath ()+"/";
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
    settingsDlg->cmbPreviewStyles2->clear();
    settingsDlg->cmbPreviewStyles->addItems(previewStyles->styles());
    settingsDlg->cmbPreviewStyles2->addItems(previewStyles->styles());
    settingsDlg->cmbPreviewLocales->clear();
    settingsDlg->cmbPreviewLocales2->clear();
    settingsDlg->cmbPreviewLocales->addItems(previewStyles->locales());
    settingsDlg->cmbPreviewLocales2->addItems(previewStyles->locales());
    settingsDlg->cmbPreviewStyles->setCurrentIndex(currentPreviewStyle1);
    settingsDlg->cmbPreviewStyles2->setCurrentIndex(currentPreviewStyle2);
    settingsDlg->cmbPreviewLocales->setCurrentIndex(settingsDlg->cmbPreviewLocales->findText(currentPreviewLocale1));
    settingsDlg->cmbPreviewLocales2->setCurrentIndex(settingsDlg->cmbPreviewLocales2->findText(currentPreviewLocale2));

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
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        username=settingsDlg->edtUserName->text();
        languageID=settingsDlg->cmbLanguage->currentText();
        stylesheet=settingsDlg->cmbStylesheet->currentText();
        style=settingsDlg->cmbStyle->currentText();
        //configDirectory=settingsDlg->edtConfigDir->text();
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
        currentPreviewStyle1=settingsDlg->cmbPreviewStyles->currentIndex();
        currentPreviewStyle2=settingsDlg->cmbPreviewStyles2->currentIndex();
        currentPreviewLocale1=settingsDlg->cmbPreviewLocales->currentText();
        currentPreviewLocale2=settingsDlg->cmbPreviewLocales2->currentText();
        howDisplayReferenceDetails=settingsDlg->cmbReferenceDetails->currentIndex();
        setProxyHost(settingsDlg->edtProxyHost->text());
        setProxyPort(settingsDlg->spinProxyPort->value());
        setProxyType(settingsDlg->cmbProxyType->currentIndex());

        writeSettings();
        readSettings();
        QApplication::restoreOverrideCursor();
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
    settings->setValue("mainwindow/currentPreviewStyle", currentPreviewStyle1);
    settings->setValue("mainwindow/currentPreviewStyle2", currentPreviewStyle2);
    settings->setValue("mainwindow/currentPreviewLocale", currentPreviewLocale1);
    settings->setValue("mainwindow/currentPreviewLocale2", currentPreviewLocale2);
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
    currentPreviewStyle1= settings->value("mainwindow/currentPreviewStyle", 0).toInt();
    currentPreviewStyle2= settings->value("mainwindow/currentPreviewStyle2", 1).toInt();
    currentPreviewLocale1= settings->value("mainwindow/currentPreviewLocale", "en-US").toString();
    currentPreviewLocale2= settings->value("mainwindow/currentPreviewLocale2", "en-US").toString();

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
    QDir dir(assetsDirectory);
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
