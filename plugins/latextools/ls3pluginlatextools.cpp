#include "ls3pluginlatextools.h"

#include <QtGui>
#include <iostream>
#include <QtXml>

LS3PluginLatexTools::LS3PluginLatexTools():
    LS3PluginBase()
{
    bibteximport=new LS3BibTeXImporter();
    bibtexexport=new LS3BibTeXExporter();
}

LS3PluginLatexTools::~LS3PluginLatexTools()
{
    delete bibteximport;
    delete bibtexexport;
}


void LS3PluginLatexTools::init(LS3PluginServices* pluginServices) {
    LS3PluginBase::init(pluginServices);

    actCopyID=new QAction(QIcon(":/latextools/insertcite.png"), tr("Copy \\cite{ID} to Clipboard"), pluginServices->GetParentWidget());
    actCopyIDNoCite=new QAction(QIcon(":/latextools/insertids.png"), tr("Copy ID to Clipboard"), pluginServices->GetParentWidget());
    actPasteBibtex=new QAction(QIcon(":/latextools/pastebibtex.png"), tr("Paste BiBTeX"), pluginServices->GetParentWidget());
    actCopyBibteX=new QAction(QIcon(":/latextools/copybibtex.png"), tr("Copy current record as BibTeX"), pluginServices->GetParentWidget());
    actCopSelectedyBibteX=new QAction(QIcon(":/latextools/copyselectedbibtex.png"), tr("Copy selected records as BibTeX"), pluginServices->GetParentWidget());

    tbLatex=new QToolBar(tr("LaTeX Tools"), pluginServices->GetParentWidget());
    tbLatex->addAction(actCopyID);
    tbLatex->addAction(actCopyIDNoCite);
    tbLatex->addSeparator();
    tbLatex->addAction(actCopyBibteX);
    tbLatex->addAction(actCopSelectedyBibteX);
    tbLatex->addSeparator();
    tbLatex->addAction(actPasteBibtex);
    tbLatex->setObjectName("toolbar_plugin_latextools");
    pluginServices->insertToolBar(tbLatex);

    mLatex=new QMenu(tr("&LaTeX Tools"));
    mLatex->addAction(actCopyID);
    mLatex->addAction(actCopyIDNoCite);
    mLatex->addSeparator();
    mLatex->addAction(actCopyBibteX);
    mLatex->addAction(actCopSelectedyBibteX);
    mLatex->addSeparator();
    mLatex->addAction(actPasteBibtex);
    pluginServices->getMenu(LS3PluginServices::ToolsMenu)->addMenu(mLatex);
    pluginServices->getMenu(LS3PluginServices::EditMenu)->addMenu(mLatex);

    pluginServices->registerImporter(bibteximport);
    pluginServices->registerExporter(bibtexexport);


    connect(actCopyID, SIGNAL(triggered()), this, SLOT(copyID()));
    connect(actCopyIDNoCite, SIGNAL(triggered()), this, SLOT(copyIDNoCite()));
    connect(actPasteBibtex, SIGNAL(triggered()), this, SLOT(pasteBibTex()));
    connect(actCopyBibteX, SIGNAL(triggered()), this, SLOT(copyBibTex()));
    connect(actCopSelectedyBibteX, SIGNAL(triggered()), this, SLOT(copySelectedBibTex()));

}

void LS3PluginLatexTools::deinit(LS3PluginServices* pluginServices) {
    LS3PluginBase::deinit(pluginServices);
}

void LS3PluginLatexTools::openData(LS3Datastore* datastore) {
    LS3PluginBase::openData(datastore);
}

void LS3PluginLatexTools::closeData(LS3Datastore* datastore) {
    LS3PluginBase::closeData(datastore);
}

void LS3PluginLatexTools::copyID() {
    if (datastore()) {
        QString ID=datastore()->getField("id").toString();
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(QString("\\cite{%1}").arg(ID));
    }
}

void LS3PluginLatexTools::copyIDNoCite() {
    if (datastore()) {
        QString ID=datastore()->getField("id").toString();
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(QString("%1").arg(ID));
    }
}

void LS3PluginLatexTools::pasteBibTex() {
    if (datastore()) {
        QClipboard *clipboard = QApplication::clipboard();
        QString text=clipboard->text();

        QVector<QMap<QString, QVariant> > data=bibteximport->importText(text, pluginServices());
        for (int i=0; i<data.size(); i++) {
            datastore()->addRecord(data[i]);
        }
    }
}

void LS3PluginLatexTools::copyBibTex() {
    if (datastore()) {
        QVector<QMap<QString, QVariant> > data;
        data.append(datastore()->currentRecord());
        QString s;
        QTextStream str(&s);
        if (bibtexexport->exportBibTeX(str, data, m_pluginServices)) {
            str.flush();
            QApplication::clipboard()->setText(s);
        } else {
            QMessageBox::critical(NULL, tr("Copy BibTeX"), tr("Error while exporting current record to clipboard!"));
        }
    }
}

void LS3PluginLatexTools::copySelectedBibTex() {
    if (datastore()) {
        QVector<QMap<QString, QVariant> > data;
        if (datastore()->selectionCount()>0) {
            for (int i=0; i<datastore()->selectionCount(); i++) {
                data.append(datastore()->getRecord(datastore()->getRecordByUUID(datastore()->getSelectedUUID(i))));
            }
            QString s;
            QTextStream str(&s);
            if (bibtexexport->exportBibTeX(str, data, m_pluginServices)) {
                str.flush();
                QApplication::clipboard()->setText(s);
            } else {
                QMessageBox::critical(NULL, tr("Copy BibTeX"), tr("Error while exporting selected records to clipboard!"));
            }
        } else {
            QMessageBox::critical(NULL, tr("Copy BibTeX"), tr("Error while exporting selected records to clipboard: No records selected!"));
        }
    }
}

Q_EXPORT_PLUGIN2(latextools, LS3PluginLatexTools)
