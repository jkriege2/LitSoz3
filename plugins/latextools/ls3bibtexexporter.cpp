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

#include "ls3bibtexexporter.h"
#include <QDate>

LS3BibTeXExporter::LS3BibTeXExporter()
{
    //ctor
}

LS3BibTeXExporter::~LS3BibTeXExporter()
{
    //dtor
}


#define OUTPUT_FIELD(fieldname, data) \
    { QString dOUTPUT_FIELD=laTeXify(QVariant(data).toString(), false).simplified(); \
    if (!dOUTPUT_FIELD.isEmpty()) outstring=outstring+ QString("  ")+fieldname+QString(" = {")+ dOUTPUT_FIELD +QString("},\n"); \
    }
#define OUTPUT_FIELD_BRACKETDOUBLE(fieldname, data) \
    { QString dOUTPUT_FIELD=laTeXify(QVariant(data).toString(), true).simplified(); \
    if (!dOUTPUT_FIELD.isEmpty()) outstring=outstring+ QString("  ")+fieldname+QString(" = {")+ dOUTPUT_FIELD +QString("},\n"); \
    }
#define OUTPUT_FIELD_NOTEXIFY(fieldname, data) \
    { QString dOUTPUT_FIELD=QVariant(data).toString().simplified(); \
    if (!dOUTPUT_FIELD.isEmpty()) outstring=outstring+ QString("  ")+fieldname+QString(" = {")+ dOUTPUT_FIELD +QString("},\n"); \
    }

bool LS3BibTeXExporter::execute(const QVector<QMap<QString, QVariant> >& data, LS3PluginServices* pluginServices) {
    bool ok=false;
    QString initDir=pluginServices->GetQSettings()->value("LS3BibTeXExporter/initDir", "").toString();
    QString fileName = QFileDialog::getSaveFileName(
                         pluginServices->GetParentWidget(),
                         QObject::tr("Enter the name of the output BiBTeX file"),
                         initDir,
                         "BiBTeX Files (*.bib)");
    if (!fileName.isEmpty()) {
        QProgressDialog progress(QObject::tr("Exporting to BiBTeX file..."), QObject::tr("&Cancel"), 0, data.size(), pluginServices->GetParentWidget());
        progress.setWindowModality(Qt::WindowModal);
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            initDir=QFileInfo(fileName).absolutePath();
            QTextStream out(&file);
            out.setCodec("ISO-8859-1");
            ok=exportBibTeX(out, data, pluginServices);
            file.close();

        }
        progress.setValue(data.size());
        pluginServices->GetQSettings()->setValue("LS3BibTeXExporter/initDir", initDir);
    }
    return ok;

}

bool LS3BibTeXExporter::exportBibTeX(QTextStream& out, const QVector<QMap<QString, QVariant> > &data, LS3PluginServices *pluginServices) {
    BibTeXExportDialog* dlg=new BibTeXExportDialog(pluginServices, NULL);
    dlg->exec();

    QProgressDialog progress(QObject::tr("Exporting to BiBTeX file..."), QObject::tr("&Cancel"), 0, data.size(), pluginServices->GetParentWidget());
    progress.setWindowModality(Qt::WindowModal);
    bool ok=true;

    int nonIDRecords=0;
    for (int i=0; i<data.size(); i++) {
        QMap<QString, QVariant> record=data[i];

        QString type=record.value("type", "misc").toString().toLower();
        QString outType=type;
        QString subtype=record.value("subtype", "").toString().toLower();
        bool writeSubtype=false;
        if ( (type=="anthology") || (type=="compendium") || (type=="textbook") ) outType="book";
        if ( (type=="techmanual") ) outType="manual";
        if ( (type=="www") || (type=="webpage") ) {
            if (dlg->getExportStyle()==1) {
                outType="www";
            } else if (dlg->getExportStyle()==2) {
                outType="online";
            } else {
                outType="misc";
            }
        }
        if ( (type=="poster") || (type=="talk") ) outType="misc";
        if ( (type=="patent") ) {
            if (dlg->getExportStyle()==2) {
                outType="patent";
            } else {
                outType="misc";
            }
        }
        if (type=="thesis") {
            if ( (subtype=="doktorarbeit") || (subtype.contains("promotion")) || (subtype=="phd thesis") || subtype.contains("phd") || subtype.contains("ph.d") || subtype.contains("ph. d") ) {
                outType="phdthesis";
            } else if (subtype.contains("master")) {
                outType="mastersthesis";
            } else {
                outType="misc";
                writeSubtype=true;
            }
        }
        QString id=record.value("id", "").toString();
        if (id.isEmpty()) {
            nonIDRecords++;
            id=QString("noid%1").arg(nonIDRecords);
        }

        QString outstring="";

        if (outType.toLower()=="article") {
                OUTPUT_FIELD("author",  formatEtalAuthors(record.value("authors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("title", record.value("title", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("journal", record.value("journal", ""));
                OUTPUT_FIELD("year", record.value("year", ""));
                OUTPUT_FIELD("volume", record.value("volume", ""));
                OUTPUT_FIELD("number", record.value("number", ""));
                OUTPUT_FIELD("pages", record.value("pages", "").toString().replace("-", "--"));
        } else if (outType.toLower()=="book") {
                OUTPUT_FIELD("author",  formatEtalAuthors(record.value("authors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD("editor",  formatEtalAuthors(record.value("editors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("title", record.value("title", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("publisher", record.value("publisher", ""));
                OUTPUT_FIELD("address", record.value("places", "").toString().simplified().replace("\r", "").replace("\n", ", "));
                OUTPUT_FIELD("year", record.value("year", ""));
                OUTPUT_FIELD("volume", record.value("volume", ""));
                OUTPUT_FIELD("number", record.value("number", ""));
                OUTPUT_FIELD("isbn", record.value("isbn", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("series", record.value("series", ""));
                bool edok=false;
                int ed=record.value("edition", "0").toInt(&edok);
                if ((edok && ed>0) || (!edok && !record.value("edition", "").toString().isEmpty())) OUTPUT_FIELD("edition", record.value("edition", ""));
        } else if (outType.toLower()=="booklet") {
                OUTPUT_FIELD("author",  formatEtalAuthors(record.value("authors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("title", record.value("title", ""));
                OUTPUT_FIELD("address", record.value("places", "").toString().simplified().replace("\r", "").replace("\n", ", "));
                OUTPUT_FIELD("year", record.value("year", ""));
                OUTPUT_FIELD("howpublished", record.value("howpublished", ""));
        } else if (outType.toLower()=="inbook") {
                OUTPUT_FIELD("author",  formatEtalAuthors(record.value("authors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD("editor",  formatEtalAuthors(record.value("editors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("title", record.value("title", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("chapter", record.value("chapter", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("publisher", record.value("publisher", ""));
                OUTPUT_FIELD("address", record.value("places", "").toString().simplified().replace("\r", "").replace("\n", ", "));
                OUTPUT_FIELD("year", record.value("year", ""));
                OUTPUT_FIELD("volume", record.value("volume", ""));
                OUTPUT_FIELD("number", record.value("number", ""));
                OUTPUT_FIELD("isbn", record.value("isbn", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("series", record.value("series", ""));
                bool edok=false;
                int ed=record.value("edition", "0").toInt(&edok);
                if ((edok && ed>0) || (!edok && !record.value("edition", "").toString().isEmpty())) OUTPUT_FIELD("edition", record.value("edition", ""));
        } else if (outType.toLower()=="inbook") {
                OUTPUT_FIELD("author",  formatEtalAuthors(record.value("authors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD("editor",  formatEtalAuthors(record.value("editors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("title", record.value("booktitle", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("chapter", record.value("title", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("publisher", record.value("publisher", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("address", record.value("places", "").toString().simplified().replace("\r", "").replace("\n", ", "));
                OUTPUT_FIELD("year", record.value("year", ""));
                OUTPUT_FIELD("volume", record.value("volume", ""));
                OUTPUT_FIELD("number", record.value("number", ""));
                OUTPUT_FIELD("isbn", record.value("isbn", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("series", record.value("series", ""));
                bool edok=false;
                int ed=record.value("edition", "0").toInt(&edok);
                if ((edok && ed>0) || (!edok && !record.value("edition", "").toString().isEmpty())) OUTPUT_FIELD("edition", record.value("edition", ""));
                OUTPUT_FIELD("pages", record.value("pages", "").toString().replace("-", "--"));
        } else if (outType.toLower()=="incollection") {
                OUTPUT_FIELD("author",  formatEtalAuthors(record.value("authors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD("editor",  formatEtalAuthors(record.value("editors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("booktitle", record.value("booktitle", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("title", record.value("title", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("publisher", record.value("publisher", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("address", record.value("places", "").toString().simplified().replace("\r", "").replace("\n", ", "));
                OUTPUT_FIELD("year", record.value("year", ""));
                OUTPUT_FIELD("volume", record.value("volume", ""));
                OUTPUT_FIELD("number", record.value("number", ""));
                OUTPUT_FIELD("isbn", record.value("isbn", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("series", record.value("series", ""));
                bool edok=false;
                int ed=record.value("edition", "0").toInt(&edok);
                if ((edok && ed>0) || (!edok && !record.value("edition", "").toString().isEmpty())) OUTPUT_FIELD("edition", record.value("edition", ""));
                OUTPUT_FIELD("pages", record.value("pages", "").toString().replace("-", "--"));
        } else if (outType.toLower()=="inproceedings") {
                OUTPUT_FIELD("author",  formatEtalAuthors(record.value("authors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD("editor",  formatEtalAuthors(record.value("editors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("booktitle", record.value("booktitle", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("title", record.value("title", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("publisher", record.value("publisher", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("address", record.value("places", "").toString().simplified().replace("\r", "").replace("\n", ", "));
                OUTPUT_FIELD("year", record.value("year", ""));
                OUTPUT_FIELD("volume", record.value("volume", ""));
                OUTPUT_FIELD("number", record.value("number", ""));
                OUTPUT_FIELD("isbn", record.value("isbn", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("series", record.value("series", ""));
                bool edok=false;
                int ed=record.value("edition", "0").toInt(&edok);
                if ((edok && ed>0) || (!edok && !record.value("edition", "").toString().isEmpty())) OUTPUT_FIELD("edition", record.value("edition", ""));
                OUTPUT_FIELD("pages", record.value("pages", "").toString().replace("-", "--"));
                OUTPUT_FIELD_BRACKETDOUBLE("organization", record.value("organization", ""));
        } else if (outType.toLower()=="manual") {
                OUTPUT_FIELD("author",  formatEtalAuthors(record.value("authors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD("editor",  formatEtalAuthors(record.value("editors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("title", record.value("title", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("address", record.value("places", "").toString().simplified().replace("\r", "").replace("\n", ", "));
                OUTPUT_FIELD("year", record.value("year", ""));
                bool edok=false;
                int ed=record.value("edition", "0").toInt(&edok);
                if ((edok && ed>0) || (!edok && !record.value("edition", "").toString().isEmpty())) OUTPUT_FIELD("edition", record.value("edition", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("organization", record.value("organization", ""));
        } else if (outType.toLower()=="mastersthesis") {
                OUTPUT_FIELD("author",  formatEtalAuthors(record.value("authors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("title", record.value("title", ""));
                OUTPUT_FIELD("address", record.value("places", "").toString().simplified().replace("\r", "").replace("\n", ", "));
                OUTPUT_FIELD("year", record.value("year", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("type", record.value("subtype", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("school", record.value("institution", ""));
        } else if (outType.toLower()=="phdthesis") {
                OUTPUT_FIELD("author",  formatEtalAuthors(record.value("authors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("title", record.value("title", ""));
                OUTPUT_FIELD("address", record.value("places", "").toString().simplified().replace("\r", "").replace("\n", ", "));
                OUTPUT_FIELD("year", record.value("year", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("type", record.value("subtype", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("school", record.value("institution", ""));
        } else if (outType.toLower()=="misc") {
                OUTPUT_FIELD("author",  formatEtalAuthors(record.value("authors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("title", record.value("title", ""));
                OUTPUT_FIELD("address", record.value("places", "").toString().simplified().replace("\r", "").replace("\n", ", "));
                OUTPUT_FIELD("year", record.value("year", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("howpublished", record.value("howpublished", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("organization", record.value("institution", ""));
                if (type=="poster") {
                    OUTPUT_FIELD_BRACKETDOUBLE("howpublished", QString("poster presented at %1").arg(record.value("conference", "").toString()));
                    OUTPUT_FIELD("type", "conference poster");
                }
                if (type=="talk") {
                    OUTPUT_FIELD_BRACKETDOUBLE("howpublished", QString("talk presented at %1").arg(record.value("conference", "").toString()));
                    OUTPUT_FIELD("type", "conference talk");
                }
                if (type=="patent") {
                    OUTPUT_FIELD("number", QString(record.value("publisher", "").toString()+" "+record.value("number", "").toString()));
                    OUTPUT_FIELD("type", "patent");
                }
                if ((type=="www") || (type=="webpage")) {
                    OUTPUT_FIELD_NOTEXIFY("note", QString("accessed ")+QString(record.value("viewdate", "").toDate().toString("yyyy-MM-dd")));
                    OUTPUT_FIELD("type", "www");
                }
        } else if (outType.toLower()=="patent") {
                OUTPUT_FIELD("author",  formatEtalAuthors(record.value("authors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("title", record.value("title", ""));
                OUTPUT_FIELD("address", record.value("places", "").toString().simplified().replace("\r", "").replace("\n", ", "));
                OUTPUT_FIELD("year", record.value("year", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("howpublished", record.value("howpublished", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("organization", record.value("institution", ""));
                OUTPUT_FIELD("number", QString(record.value("publisher", "").toString()+" "+record.value("number", "").toString()));
        } else if (outType.toLower()=="www" || outType.toLower()=="online") {
                OUTPUT_FIELD("author",  formatEtalAuthors(record.value("authors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("title", record.value("title", ""));
                OUTPUT_FIELD("year", record.value("year", ""));
                OUTPUT_FIELD_NOTEXIFY("urldate", QString(record.value("viewdate", "").toDate().toString("yyyy-MM-dd")));
        } else if (outType.toLower()=="proceedings") {
                OUTPUT_FIELD("editor",  formatEtalAuthors(record.value("editors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("title", record.value("title", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("publisher", record.value("publisher", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("address", record.value("places", "").toString().simplified().replace("\r", "").replace("\n", ", "));
                OUTPUT_FIELD("year", record.value("year", ""));
                OUTPUT_FIELD("volume", record.value("volume", ""));
                OUTPUT_FIELD("number", record.value("number", ""));
                OUTPUT_FIELD("isbn", record.value("isbn", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("series", record.value("series", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("organization", record.value("organization", ""));
        } else if (outType.toLower()=="techreport") {
                OUTPUT_FIELD("author",  formatEtalAuthors(record.value("authors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("title", record.value("title", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("address", record.value("places", "").toString().simplified().replace("\r", "").replace("\n", ", "));
                OUTPUT_FIELD_BRACKETDOUBLE("institution", record.value("institution", ""));
                OUTPUT_FIELD("year", record.value("year", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("howpublished", record.value("howpublished", ""));
                OUTPUT_FIELD("number", record.value("number", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("type", record.value("subtype", ""));
                bool edok=false;
                int ed=record.value("edition", "0").toInt(&edok);
                if ((edok && ed>0) || (!edok && !record.value("edition", "").toString().isEmpty())) OUTPUT_FIELD("edition", record.value("edition", ""));

        } else if (outType.toLower()=="unpublished") {
                OUTPUT_FIELD("author",  formatEtalAuthors(record.value("authors", "").toString(), -1, "", false, " and ", " and ", "", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("title", record.value("title", ""));
                OUTPUT_FIELD("year", record.value("year", ""));
                OUTPUT_FIELD_BRACKETDOUBLE("note", record.value("subtype", ""));
        }
        if (!outstring.isEmpty()) {
            bool doURL=false;
            QString doi=record.value("doi", "").toString();
            QString pmid=record.value("pubmed", "").toString();
            QString pmcid=record.value("pmcid", "").toString();
            QString arxiv=record.value("arxiv", "").toString();
            if (dlg->getExportURL()==0) doURL=true;
            if (dlg->getExportURL()==1 && doi.isEmpty()) doURL=true;

            if (doURL) {
                OUTPUT_FIELD_NOTEXIFY("url", record.value("url", ""));
            }
            OUTPUT_FIELD_NOTEXIFY("doi", record.value("doi", ""));
            if (dlg->getExportStyle()==2) {
                OUTPUT_FIELD_NOTEXIFY("language", record.value("language", ""));
                if (!arxiv.isEmpty()) {
                    OUTPUT_FIELD("eprint", arxiv);
                    OUTPUT_FIELD("eprinttype", QString("arxiv"));
                }
                if (!pmid.isEmpty()) {
                    OUTPUT_FIELD("eprint", pmid);
                    OUTPUT_FIELD("eprinttype", QString("pubmed"));
                }
            }

            if (dlg->getExportAbstract()) {
                QString d=record.value("abstract", "").toString();
                if (!d.isEmpty()) d=d+"\n\n";
                d=d+record.value("contents", "").toString();
                OUTPUT_FIELD_BRACKETDOUBLE("abstract", d);

            }
            if (dlg->getExportPDF()) {
                QStringList pdf=record.value("files", "").toStringList();
                if (!pdf.isEmpty()) {
                    OUTPUT_FIELD_NOTEXIFY("pdf", pdf.first());
                }
            }


            out<<"@"<<outType.toUpper()<<"{"<<id<<",\n";
            out<<outstring;
            out<<"}\n\n";
        }


        if (i%10==0) {
            progress.setValue(i);
            QApplication::processEvents();
            if (progress.wasCanceled()) {
                ok=false;
                break;
            }
        }

    }
    out.flush();
    progress.setValue(data.size());

    delete dlg;

    return ok;
}
