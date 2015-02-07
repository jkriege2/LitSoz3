#include "testmain.h"
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "jkstarratingwidget.h"
#include "qenhancedlineedit.h"
#include "qspecialtoolbutton.h"
#include "jkstyledbutton.h"
#include "jkcharacterscreenkeyboard.h"
#include "qmodernprogresswidget.h"
#include "qfilenameslistedit.h"

TestMain::TestMain(QWidget* parent):
    QMainWindow(parent)
{
    progress=0;
    prg_up=true;
    QWidget* w;
    QFormLayout*  l;

    tabMain=new QTabWidget(this);
    setCentralWidget(tabMain);


    w=new QWidget(this);
    l=new QFormLayout(w);
    w->setLayout(l);

    QFilenamesListEdit* fnWidget=new QFilenamesListEdit(w);
    fnWidget->setBaseDir(QApplication::applicationDirPath());
    fnWidget->setFilesFromString("litsoz3.ini\ndownload.pdf");
    fnWidget->addNewFile("../version.h");
    l->addRow("<b>files:</b>", fnWidget);

    fnWidget=new QFilenamesListEdit(w);
    fnWidget->setBaseDir(QApplication::applicationDirPath());
    l->addRow("<b>files (empty):</b>", fnWidget);

    tabMain->addTab(w, "QFilenamesListEdit");

    w=new QWidget(this);
    JKStarRatingWidget* rating=new JKStarRatingWidget(w);
    rating->move(10,10);

    tabMain->addTab(w, "JKStarRatingWidget");

    w=new QWidget(this);
    l=new QFormLayout(w);
    w->setLayout(l);
    l->addRow("<b>with special tool buttons:</b>", new QWidget());
    QEnhancedLineEdit* le=new QEnhancedLineEdit(w);
    QFileSelectToolButton* b1=new QFileSelectToolButton(w);
    b1->setBuddy(le);
    le->addButton(b1);
    QFileExecuteToolButton* b2=new QFileExecuteToolButton(w);
    b2->setBuddy(le);
    le->addButton(b2);
    l->addRow("&File:", le);

    le=new QEnhancedLineEdit(w);
    QWWWToolButton* b3=new QWWWToolButton(w);
    b3->setBuddy(le);
    le->addButton(b3);
    l->addRow("&Webpage:", le);

    l->addRow("<b>Now with JKStyledButton:</b>", new QWidget());

    le=new QEnhancedLineEdit(w);
    QIcon icNetwork(":/jkstyledbutton/network.png");
    icNetwork.addFile(":/jkstyledbutton/network_disabled.png", QSize(), QIcon::Disabled);
    icNetwork.addFile(":/jkstyledbutton/network_hover.png", QSize(), QIcon::Active);
    icNetwork.addFile(":/jkstyledbutton/network_pressed.png", QSize(), QIcon::Selected);
    JKStyledButton* b4=new JKStyledButton(icNetwork, le);
    //b4->setBuddy(le);
    le->addButton(b4);
    l->addRow("&Webpage, no function:", le);

    le=new QEnhancedLineEdit(w);
    JKStyledButton* b5=new JKStyledButton(icNetwork, le);
    //b5->setBuddy(le);
    b5->setEnabled(false);
    le->addButton(b5);
    l->addRow("&Webpage, disabled Icon:", le);

    le=new QEnhancedLineEdit(w);
    JKStyledButton* b6=new JKStyledButton(JKStyledButton::OpenURL, le, le);
    le->addButton(b6);
    le->setText("http://www.jkrieger.de/");
    l->addRow("&Webpage, action:", le);


    le=new QEnhancedLineEdit(w);
    b6=new JKStyledButton(JKStyledButton::SelectFile, le, le);
    le->addButton(b6);
    b6=new JKStyledButton(JKStyledButton::ExecuteFile, le, le);
    le->addButton(b6);
    le->setText("litsoz3widgets_test.exe");
    l->addRow("&File:", le);




    l->addRow("<b>with custom menus:</b>", new QWidget());

    le=new QEnhancedLineEdit(w);
    le->addInsertContextMenuEntry(tr("insert %data%"), tr("%data%"));
    le->addInsertContextMenuEntry(tr("insert %atad%"), tr("%atad%"));
    l->addRow("&Edit:", le);

    tabMain->addTab(w, "QEnhancedLineEdit");


    JKCharacterScreenKeyboard* jkkeyb=new JKCharacterScreenKeyboard(0);
    jkkeyb->show();
    jkkeyb->setFocusPolicy(Qt::NoFocus);
    jkkeyb->setKBFocusWidget(this);
    jkkeyb->setCharacters("ABCÄÖÜäöüß");



    w=new QWidget(this);
    QGridLayout* gl=new QGridLayout(w);
    w->setLayout(gl);
    QModernProgressWidget* prg1=new QModernProgressWidget(w);
    prg1->setSpin(true);
    prg1->setMode(QModernProgressWidget::GradientRing);
    gl->addWidget(prg1, 0,0);

    prg1=new QModernProgressWidget(w);
    prg1->setSpin(true);
    prg1->setMode(QModernProgressWidget::Circles);
    gl->addWidget(prg1, 0,1);
    prg1=new QModernProgressWidget(w);
    prg1->setSpin(true);
    prg1->setMode(QModernProgressWidget::Strokes);
    gl->addWidget(prg1, 1,0);

    prg2=new QModernProgressWidget(w);
    prg2->setMode(QModernProgressWidget::GradientRing);
    prg2->setDisplayPercent(true);
    prg2->setStartColor(prg2->stopColor());
    gl->addWidget(prg2, 2,0);

    prg3=new QModernProgressWidget(w);
    prg3->setMode(QModernProgressWidget::Circles);
    prg3->setDisplayPercent(true);
    prg3->setStartColor(prg3->stopColor());
    gl->addWidget(prg3, 2,1);

    prg4=new QModernProgressWidget(w);
    prg4->setMode(QModernProgressWidget::Strokes);
    prg4->setDisplayPercent(true);
    prg4->setStartColor(prg4->stopColor());
    gl->addWidget(prg4, 3,0);

    tabMain->addTab(w, tr("modern progress"));

    tabMain->setCurrentIndex(2);

    dlg=new QModernProgressDialog("Test label text <b> with some bold</b> text<br>&nbsp;&nbsp;also in different lines!", "Cancel", this);
    dlg->progressWidget()->setPercentageMode(QModernProgressWidget::Value);
    dlg->progressWidget()->setSuffix("°C");
    dlg->progressWidget()->setPrecision(1);
    dlg->setModal(false);
    connect(dlg, SIGNAL(canceled()), dlg, SLOT(hide()));

    QTimer::singleShot(100, this, SLOT(incProgress()));

}

TestMain::~TestMain()
{
    //dtor
}

void TestMain::incProgress() {
    if (prg_up) progress++;
    else progress--;
    if (progress>100) { progress=100; prg_up=false; }
    if (progress<0) { progress=0; prg_up=true; }
    prg2->setValue(progress);
    prg3->setValue(100-progress);
    prg4->setValue(progress);
    dlg->setValue((double)progress*0.37);
    QTimer::singleShot(50, this, SLOT(incProgress()));
}

void TestMain::showEvent(QShowEvent* event) {
    dlg->openDelayed();
}
