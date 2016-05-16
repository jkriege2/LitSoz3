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

#ifndef LS3PLUGINLATEXTOOLS_H
#define LS3PLUGINLATEXTOOLS_H

#include "ls3pluginbase.h"
#include <QWidget>
#include <QComboBox>
#include <QProgressDialog>
#include <QLineEdit>
#include <QToolBar>
#include <QMenu>
#include <QClipboard>
#include <QApplication>
#include "jkstyledbutton.h"
#include "qenhancedlineedit.h"
#include "bibtools.h"
#include "ls3bibteximporter.h"
#include "ls3bibtexexporter.h"

/*! \brief Tools for interplay with LaTeX/BiBTeX
*/
class LS3PluginLatexTools : public LS3PluginBase {
        Q_OBJECT
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.jkrieger.de.LitSoz3.Plugins.LS3PluginLatexTools")
#endif
    public:
        LS3PluginLatexTools();
        virtual ~LS3PluginLatexTools();


        /** \brief name for the plugin */
        virtual QString getName() { return tr("LaTeX Tools"); }

        /** \brief short description for the plugin */
        virtual QString getDescription() { return tr("tools for interplay with LaTeX/BiBTeX"); }

        /** \brief author the plugin */
        virtual QString getAuthor() { return tr("J. Krieger"); }

        /** \brief copyright information the plugin */
        virtual QString getCopyright() { return tr("(c) 2011-2015 by J. Krieger"); }

        /** \brief weblink for the plugin */
        virtual QString getWeblink() { return tr("http://www.jkrieger.de/"); }

        /** \brief icon for the plugin */
        virtual QString getIconFilename() { return QString(":/latextools/logo.png"); }

        /** \brief initialize the plugin (create buttons, menu entries, register import/export objects ...) */
        virtual void init(LS3PluginServices* pluginServices);

        /** \brief de-initialize the plugin (clean up when program is closed) */
        virtual void deinit(LS3PluginServices* pluginServices) ;

        /** \brief this function is called when a datastore is closed */
        virtual void openData(LS3Datastore* datastore);

        /** \brief this function is called when a datastore is closed */
        virtual void closeData(LS3Datastore* datastore);
    protected:
        QAction* actCopyID;
        QAction* actCopyIDNoCite;
        QAction* actPasteBibtex;
        QAction* actPasteBibtexCurrent;
        QAction* actCopyBibteX;
        QAction* actCopSelectedyBibteX;
        QToolBar* tbLatex;
        QMenu* mLatex;
        LS3BibTeXImporter* bibteximport;
        LS3BibTeXExporter* bibtexexport;
    protected slots:
        void copyID();
        void copyIDNoCite();
        void pasteBibTex();
        void copyBibTex();
        void copySelectedBibTex();
        void pasteBibTexCurrent();
    private:
};

#endif // LS3PLUGINLATEXTOOLS_H
