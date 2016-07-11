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
#include "../version.h"

#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QSplashScreen>
#include <QPainter>
int main(int argc, char * argv[])
{
#ifdef Q_OS_WIN
    qDebug()<<"setting Qt lib paths: "<<"./qtplugins"<<(QFileInfo(argv[0]).absolutePath()+"/qtplugins");
    QCoreApplication::addLibraryPath("./qtplugins");
    QCoreApplication::addLibraryPath(QFileInfo(argv[0]).absolutePath()+"/qtplugins");
#endif

    Q_INIT_RESOURCE(litsoz3);

    QApplication app(argc, argv);

#ifdef Q_OS_WIN
    QApplication::addLibraryPath(QCoreApplication::applicationDirPath()+"/qtplugins");
#endif
    app.setOrganizationName("");
    app.setApplicationName(QString("LitSoz %1").arg(VERSION_FULL));
    app.setOrganizationDomain(QString(LS3_WEBLINK));
    app.setApplicationVersion(QString("%1 (DATE %2)").arg(VERSION_FULL).arg(COMPILEDATE));
    app.setWindowIcon(QIcon(":/ls3icon.png"));



    QPixmap pixmap(":/splash.png");
    QPainter* painter=new QPainter(&pixmap);
    painter->setFont(QFont("Arial", 9));
    painter->drawText(QPoint(5,290), QString("version %1 (GITCC: %3 DATE: %2)").arg(VERSION_FULL).arg(COMPILEDATE).arg(GITCOMMITCOUNT));
    delete painter;
    painter=NULL;
    QSplashScreen splash(pixmap,Qt::WindowStaysOnTopHint);
    splash.showMessage("initializing ...");
    splash.show();
    app.processEvents();
    app.processEvents();
    app.processEvents();


    app.processEvents();
    app.processEvents();
    app.processEvents();
    app.processEvents();

    ProgramOptions* settings=new ProgramOptions("", &app, &app);

    MainWindow win(settings, &splash);
    win.show();
    splash.finish(&win);
    return app.exec();
}
