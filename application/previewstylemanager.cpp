#include "previewstylemanager.h"
#include "bibtools.h"
#include <QDir>
#include <QFileInfo>
#include "csltools.h"
#include <QDebug>

PreviewStyleManager::PreviewStyleManager(QObject* parent):
    QObject(parent)
{
}

PreviewStyleManager::~PreviewStyleManager()
{
    for (int i=0; i<cslfiles.size(); i++) {
        delete cslfiles[i];
    }
    cslfiles.clear();
    csls.clear();
    QMapIterator<QString, CSLLocale*> it(csllocales);
    while (it.hasNext()) {
        it.next();
        if (it.value()) delete it.value();
    }
    csllocales.clear();
}

void PreviewStyleManager::searchCSLLocales(const QString &dir)
{
    QDir d(dir);
    QStringList filters;
    filters<<"*.xml";
    filters<<"*.loc";
    filters<<"*.locale";
    QStringList files=d.entryList(filters, QDir::Files);
    csllocales.clear();
    for (int i=0; i<files.size(); i++) {
        QString fn=d.absoluteFilePath(files[i]);
        CSLLocale* f=new CSLLocale(fn);
        if (f) {
            csllocales.insert(f->name(), f);
        }
    }
}

void PreviewStyleManager::searchCSL(const QString &dir)
{
    QDir d(dir);
    QStringList filters;
    filters<<"*.csl";
    QStringList files=d.entryList(filters, QDir::Files);
    csls.clear();
    cslfiles.clear();
    for (int i=0; i<files.size(); i++) {
        QString fn=d.absoluteFilePath(files[i]);
        QString name="";
        qDebug()<<i<<fn;
        if (cslReadMetadata(fn, &name)) {
            qDebug()<<i<<"   "<<name;
            if (!name.isEmpty()) {
                CSLFile* f=new CSLFile(fn);
                if (f->isValid()) {
                    cslfiles.append(f);
                    csls.append(name);
                } else {
                    delete f;
                }
            }
        }
    }
}


QStringList PreviewStyleManager::styles() const {
    QStringList s;
    //s<<tr("Default Preview");
    s<<csls;
    return s;
}

QStringList PreviewStyleManager::locales() const
{
    return csllocales.keys();
}

#define FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, item) QString item=""; if (data.contains(#item)) item=escapeHTMLString(data[#item].toString());

QString PreviewStyleManager::createPreview(int i, const QMap<QString, QVariant>& data, QString locale) const {
    if ((i<-1)||(i>=styles().size())) return "";
    QString result="";
    if (i==-1) {
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, title)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, type); type=type.toLower();
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, authors)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, editors)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, year)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, journal)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, number)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, volume)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, booktitle)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, publisher)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, places)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, edition)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, pages)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, institution)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, subtype)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, abstract)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, comments)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, url)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, urls)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, files)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, id)
        FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, isbn)

        QString names=formatEtalAuthors(authors, -1, "", true, ", ", " &amp; ", "", "");
        if ((!names.isEmpty()) && (!editors.isEmpty())) names+="<br>";
        names+=formatEtalAuthors(editors, -1, "", true, ", ", " &amp; ", "", " (ed.)");
        result+=QString("<small><b><i>%1</i> (%2):</b></small>").arg(type).arg(id);
        result+=QString("<h2>%1</h2>").arg(title);
        if (type=="article") {
            result+=QString("<p>%1</p>").arg(authors);
            result+="<small>";
            QString add="";
            if (!journal.isEmpty()) add+=QString("<i>%1</i>").arg(journal);
            if (!volume.isEmpty()) add+=QString(" <b>%1</b>").arg(volume);
            if (!number.isEmpty()) add+=QString("(%1)").arg(number);
            if (!pages.isEmpty()) add+=((add.isEmpty())?QString(""):QString(", "))+QString("p. %1").arg(pages);
            if (!year.isEmpty()) add+=((add.isEmpty())?QString(""):QString(", "))+QString("<b>%1</b>").arg(year);
            result+="<p>"+add+"</p>";
        } else if ((type=="book")||(type=="anthology")||(type=="compendium")||(type=="collection")||(type=="proceedings")||(type=="textbook")) {
            result+=QString("<p>%1</p>").arg(names);
            result+="<small>";
            QString add="";
            if (!edition.isEmpty()) add+=QString("%1. edition").arg(edition);
            if (!publisher.isEmpty()) add+=((add.isEmpty())?QString(""):QString(", "))+publisher;
            if (!places.isEmpty()) add+=((add.isEmpty())?QString(""):QString(", "))+places;
            if (!isbn.isEmpty()) add+=((add.isEmpty())?QString(""):QString(", "))+tr("ISBN: ")+isbn;
            if (!year.isEmpty()) add+=((add.isEmpty())?QString(""):QString(", "))+QString("<b>%1</b>").arg(year);
            result+="<p>"+add+"</p>";
        } else {
            result+=QString("<p>%1</p>").arg(names);
            result+="<small>";

        }
        result+="</small>";
        QString add;
        if (!url.isEmpty() || !urls.isEmpty()) {
            add+="<p>";
            if (!url.isEmpty()) {
                add+=tr("<a href=\"%1\"><img src=\":/weblink.png\"></a>&nbsp;<a href=\"%1\">%1</a><br>").arg(url);
            }
            if (!urls.isEmpty()) {
                QStringList sl=urls.split('\n');
                for (int i=0; i<sl.size(); i++) {
                    if (sl[i]!=url) {
                        add+=tr("<a href=\"%1\"><img src=\":/weblink.png\"></a>&nbsp;<a href=\"%1\">%1</a><br>").arg(sl[i]);
                    }
                }
            }
            add+="</p>";
        }

        add+="<hr width=\"100%\">";
        add+=tr("<p><b>%1:</b>:<br>%2</p>");
        add+="<hr width=\"100%\">";
        add+=tr("<p><b>%3:</b>:<br>%4</p>");
        add+="<hr width=\"100%\">";

        if (!abstract.isEmpty()) {
            add+=tr("<hr width=\"100%\"><p><b>ABSTRACT:</b>&nbsp;&nbsp;&nbsp;%1</p>").arg(abstract);
        }
        if (!comments.isEmpty()) {
            add+=tr("<hr width=\"100%\"><p><b>COMMENTS:</b>&nbsp;&nbsp;&nbsp;%1</p>").arg(comments);
        }


        if (!files.isEmpty()) {
            add+="<p>";
            QStringList sl=files.split('\n');
            for (int i=0; i<sl.size(); i++) {
                add+=tr("<a href=\"file://%1\"><img src=\"%2\"></a>&nbsp;<a href=\"file://%1\">%1</a><br>").arg(sl[i]).arg(filenameToIconFile(sl[i]));
            }
            add+="</p>";
        }
        if (!add.isEmpty()) {
            result+="<hr width=\"100%\">"+add;
        }

    } else {
        int cslIndex=i;
        if (cslfiles.value(cslIndex, NULL)) {
            cslfiles.value(cslIndex, NULL)->setLocale(csllocales.value(locale, NULL));
            result=cslfiles.value(cslIndex, NULL)->produceHTML(data);
        }
    }

    return result;
}




