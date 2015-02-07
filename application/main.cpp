

#include "mainwindow.h"
#include "../version.h"

#include <QtGui>
#include <QSplashScreen>
#include <QPainter>
int main(int argc, char * argv[])
{
    Q_INIT_RESOURCE(litsoz3);

    QApplication app(argc, argv);
    app.setOrganizationName("");
    app.setApplicationName(QString("LitSoz %1").arg(VERSION_FULL));
    app.setOrganizationDomain(QString(LS3_WEBLINK));
    app.setApplicationVersion(QString("%1 (DATE %2)").arg(VERSION_FULL).arg(COMPILEDATE));
    app.setWindowIcon(QIcon(":/ls3icon.png"));

    QPixmap pixmap(":/splash.png");
    QPainter* painter=new QPainter(&pixmap);
    painter->setFont(QFont("Arial", 9));
    painter->drawText(QPoint(5,290), QString("version %1 (%2)").arg(VERSION_FULL).arg(COMPILEDATE));
    delete painter;
    painter=NULL;
    QSplashScreen splash(pixmap,Qt::WindowStaysOnTopHint);
    splash.showMessage("initializing ...");
    splash.show();
    app.processEvents();
    app.processEvents();
    app.processEvents();


    app.addLibraryPath(QCoreApplication::applicationDirPath()+"/qtplugins");

    /** \brief this object manages program settings */
    ProgramOptions* settings=new ProgramOptions("", &app, &app);

    MainWindow win(settings, &splash);
    win.show();
    splash.finish(&win);
    return app.exec();
}
