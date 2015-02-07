#ifndef LS3BIBTEXIMPORTER_H
#define LS3BIBTEXIMPORTER_H

#include "ls3importexport.h"
#include <QObject>
#include <QFileDialog>
#include <QProgressDialog>
#include <QString>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QStringList>
#include <QMessageBox>
#include "bibtools.h"

/*! \brief Class to import BiBTeX files/strings
*/
class LS3BibTeXImporter : public LS3Importer {
    public:
        /** Default constructor */
        LS3BibTeXImporter(): LS3Importer() {};
        /** Default destructor */
        virtual ~LS3BibTeXImporter() {};



        /** \copydoc LS3Importer::execute() */
        virtual QVector<QMap<QString, QVariant> > execute(LS3PluginServices* pluginServices);

        /** \copydoc LS3Importer::importsText() */
        virtual bool importsText() const { return true; }

        /** \copydoc LS3Importer::importText() */
        virtual QVector<QMap<QString, QVariant> > importText(const QString& text, LS3PluginServices* pluginServices);



        /** \copydoc LS3Importer::getName() */
        virtual QString getName() { return QObject::tr("BiBTeX Import"); };

        /** \copydoc LS3Importer::getDescription() */
        virtual QString getDescription() { return QObject::tr("import BiBTeX files"); };

        /** \copydoc LS3Importer::getIcon() */
        virtual QIcon getIcon() { return QIcon(":/latextools/bibteximporter.png"); };



};

#endif // LS3BIBTEXIMPORTER_H
