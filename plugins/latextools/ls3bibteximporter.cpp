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

#include "ls3bibteximporter.h"
#include "latextools.h"
#include <iostream>


QVector<QMap<QString, QVariant> > LS3BibTeXImporter::execute(LS3PluginServices* pluginServices) {
    QVector<QMap<QString, QVariant> > r;
    QString initDir=pluginServices->GetQSettings()->value("LS3BibTeXImporter/initDir", "").toString();
    QStringList files = QFileDialog::getOpenFileNames(
                         pluginServices->GetParentWidget(),
                         QObject::tr("Select one or more BiBTeX files to import"),
                         initDir,
                         "BiBTeX Files (*.bib)");
    if (files.size()>0) {
        QProgressDialog progress(QObject::tr("Importing BiBTeX files..."), QObject::tr("&Cancel"), 0, files.size(), pluginServices->GetParentWidget());
        progress.setWindowModality(Qt::WindowModal);
        for (int i=0; i<files.size(); i++) {
            progress.setValue(i);
            QFile data(files[i]);
            if (data.open(QIODevice::ReadOnly | QIODevice::Text)) {
                initDir=QFileInfo(files[i]).absolutePath();
                QTextStream in(&data);
                QString text=in.readAll();

                QVector<QMap<QString, QVariant> > r1=importText(text, pluginServices);
                r+=r1;
            }
            QApplication::processEvents();
            if (progress.wasCanceled())
                break;
        }
        progress.setValue(files.size());
        pluginServices->GetQSettings()->setValue("LS3BibTeXImporter/initDir", initDir);
    }
    return r;
}

QVector<QMap<QString, QVariant> > LS3BibTeXImporter::importText(const QString& text, LS3PluginServices* pluginServices) {
    QVector<QMap<QString, QVariant> > r;

    BiBTeXParser parser;
    QString error;
        //std::cout<<"PARSE: OK!\n";
    if (parser.parse(text, &error)) {
    } else {
        //std::cout<<"PARSE: ERROR ["<<error.toStdString()<<"]!\n";
        QMessageBox::critical(pluginServices->GetParentWidget(), QObject::tr("BiBTeX import Error"), QObject::tr("An error occured while importing from BiBTeX:\n%1").arg(error));
        return r;
    }

    for (int i=0; i<parser.getRecordCount(); i++) {
        BiBTeXParser::Record rec=parser.getRecord(i);
        QMap<QString, QVariant> m;
        //std::cout<<"parsed fields: "<<QStringList(rec.fields.keys()).join(", ").toStdString()<<"\n";
        QMapIterator<QString, QString> f(rec.fields);
        while (f.hasNext()) {
            f.next();
            //std::cout<<"parsed field: "<<f.key().toStdString()<<" = '"<<f.value().toStdString()<<"'\n";
            QString field=f.key().toLower();
            if (!field.isEmpty()) {
                if (field=="author") {
                    m["authors"]=f.value().trimmed();
                } else if (field=="editor") {
                    m["editors"]=f.value().trimmed();
                } else if (field=="note") {
                    m["abstract"]=(m["abstract"].toString()+"\n"+f.value()).trimmed();
                } else {
                    m[field]=f.value().trimmed();
                }
            }
        }

        if (rec.fields.contains("howpublished")) {
            QString d=rec.fields["howpublished"];
            if (d.startsWith("url:", Qt::CaseInsensitive)) d=d.right(d.size()-4).trimmed();
            if (m.contains("url")) {
                if (m["urls"].toString().isEmpty()) m["urls"]=d;
                else m["urls"]=(m["urls"].toString()+"\n"+d).trimmed();
            } else {
                m["url"]=d;
                if (m["urls"].toString().isEmpty()) m["urls"]=d;
                else m["urls"]=(m["urls"].toString()+"\n"+d).trimmed();
            }
        }

        if (m.contains("authors")) {
            m["authors"]=reformatAuthors(m["authors"].toString(), pluginServices->GetNamePrefixes(), pluginServices->GetNameAdditions(), pluginServices->GetAndWords());
        }
        if (m.contains("editors")) {
            m["editors"]=reformatAuthors(m["editors"].toString(), pluginServices->GetNamePrefixes(), pluginServices->GetNameAdditions(), pluginServices->GetAndWords());
        }

        QString type=rec.type.toLower().trimmed();
        m["id"]=rec.ID;
        if (type=="manual") {
            m["type"]="techmanual";
        } else if (type=="mastersthesis") {
        } else if (type.toUpper()=="WISSENSCHAFTLICHE ARBEIT") {
            m["type"]="thesis";
            m["subtype"]=type;
        } else if (type=="phdthesis") {
            m["type"]="thesis";
            m["subtype"]="PhD thesis";
        } else if (type=="misc") {
            m["type"]=type;
            if (!m["url"].toString().isEmpty()) m["type"]="webpage";
        } else {
            m["type"]=type;
        }
        r.append(m);
    }

    for (int i=0; i<r.size(); i++) {
        //std::cout<<"RECORD "<<i<<":\n";
        QMapIterator<QString, QVariant> j(r[i]);
        while (j.hasNext()) {
            j.next();
            //std::cout<<"  FIELD "<<j.key().toStdString()<<" = '"<<j.value().toString().toStdString()<<"'\n";
        }
        //std::cout<<"\n";
    }
    return r;
}
