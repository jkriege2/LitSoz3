#include "csltools.h"
#include <QFile>
#include <QDomDocument>

bool cslReadMetadata(const QString& filename, QString* name) {
    QDomDocument doc;
    QFile f(filename);
    if (f.open(QFile::ReadOnly|QFile::Text)) {
        doc.setContent(f.readAll());
        f.close();

        QDomElement e=doc.documentElement();
        if (!e.isNull() && e.tagName()=="style") {
            e=e.firstChildElement("info");
            if (e.isNull()) return false;
            QDomElement pe=e.firstChildElement("title-short");
            if (!pe.isNull() && name) *name=pe.text();
            pe=e.firstChildElement("title");
            if (!pe.isNull() && name) *name=pe.text();
        } else {
            return false;
        }

        return true;
    }
    return false;
}


CSLFile::CSLFile(const QString &fn)
{
    m_macros.clear();
    m_bibliography=NULL;
    filename=fn;
    m_name="";
    valid=cslReadMetadata(fn, &m_name);
    if (valid) valid=parse(fn);

}

CSLFile::~CSLFile()
{
    QMapIterator<QString, CSLNode*> it(m_macros);
    while (it.hasNext()) {
        it.next();
        delete it.value();
    }
    m_macros.clear();
    delete m_bibliography;
    m_bibliography=NULL;
}

QString CSLFile::produceHTML(const QMap<QString, QVariant>& data)
{
    QString res;
    res=QString("<b>%1</b><br>").arg(name());
    return res;
}

bool CSLFile::parse(const QString &filename)
{
    QDomDocument doc;
    QFile f(filename);
    if (f.open(QFile::ReadOnly|QFile::Text)) {
        doc.setContent(f.readAll());
        f.close();

        QDomElement e=doc.documentElement();
        if (!e.isNull() && e.tagName()=="style") {
            e=e.firstChildElement("info");
            if (e.isNull()) return false;
            QDomElement pe=e.firstChildElement("title-short");
            if (!pe.isNull() ) m_name=pe.text();
            pe=e.firstChildElement("title");
            if (!pe.isNull() ) m_name=pe.text();
        }

        return true;
    }
    return false;
}


CSLFile::CSLNode::CSLNode(CSLFile::CSLNode *parent)
{
    m_parent=parent;
}

QString CSLFile::CSLNode::produceHTML(const QMap<QString, QVariant> &data)
{
    return QString();
}

QVariant CSLFile::CSLNode::getCSLField(const QString &field, const QMap<QString, QVariant> &data,const QVariant& defaultVal)
{
    if (field==QLatin1String("year-suffix")) {
        return QVariant();
    } else if (field==QLatin1String("version")) {
        return QVariant();
    } else if (field==QLatin1String("title")) {
        return data.value("title", defaultVal);
    } else if (field==QLatin1String("title-short")) {
        return data.value("title", defaultVal);
    } else if (field==QLatin1String("status")) {
        return QVariant();
    } else if (field==QLatin1String("source")) {
        return QVariant();
    } else if (field==QLatin1String("section")) {
        return QVariant();
    } else if (field==QLatin1String("scale")) {
        return QVariant();
    } else if (field==QLatin1String("reviewed-title")) {
        return QVariant();
    } else if (field==QLatin1String("references")) {
        return QVariant();
    } else if (field==QLatin1String("URL")) {
        return data.value("url", defaultVal);
    } else if (field==QLatin1String("publisher")) {
        return data.value("publisher", defaultVal);
    } else if (field==QLatin1String("publisher-place")) {
        return data.value("places", defaultVal);
    } else if (field==QLatin1String("PMID")) {
        return data.value("pubmed", defaultVal);
    } else if (field==QLatin1String("PMCID")) {
        return data.value("pmcid", defaultVal);
    } else if (field==QLatin1String("PMCID")) {
        return data.value("pmcid", defaultVal);
    } else if (field==QLatin1String("page-first")) {
        QString p= data.value("pages", defaultVal).toString();
        int idx=p.indexOf("-");
        if (idx>=0) {
            return p.left(idx);
        } else {
            return p;
        }
    } else if (field==QLatin1String("page")) {
        return data.value("pages", defaultVal);
    } else if (field==QLatin1String("original-title")) {
        return QVariant();
    } else if (field==QLatin1String("original-publisher-place")) {
        return QVariant();
    } else if (field==QLatin1String("original-publisher")) {
        return QVariant();
    } else if (field==QLatin1String("note")) {
        return QVariant();
    } else if (field==QLatin1String("medium")) {
        return QVariant();
    } else if (field==QLatin1String("locator")) {
        return QVariant();
    } else if (field==QLatin1String("keyword")) {
        return data.value("keywords", defaultVal).toString();
    } else if (field==QLatin1String("jurisdiction")) {
        return QVariant();
    } else if (field==QLatin1String("ISSN")) {
        return data.value("issn", defaultVal);
    } else if (field==QLatin1String("ISBN")) {
        return data.value("isbn", defaultVal);
    } else if (field==QLatin1String("genre")) {
        return data.value("subtype", defaultVal);
    } else if (field==QLatin1String("event-place")) {
        return data.value("places", defaultVal);
    } else if (field==QLatin1String("event")) {
        return QVariant();
    } else if (field==QLatin1String("DOI")) {
        return data.value("doi", defaultVal);
    } else if (field==QLatin1String("dimensions")) {
        return QVariant();
    } else if (field==QLatin1String("container-title-short")) {
        return data.value("booktitle", defaultVal);
    } else if (field==QLatin1String("container-title")) {
        return data.value("booktitle", defaultVal);
    } else if (field==QLatin1String("collection-title")) {
        return data.value("booktitle", defaultVal);
    } else if (field==QLatin1String("citation-number")) {
        return QVariant();
    } else if (field==QLatin1String("citation-label")) {
        return QVariant();
    } else if (field==QLatin1String("call-number")) {
        return data.value("librarynum", defaultVal);
    } else if (field==QLatin1String("authority")) {
        return QVariant();
    } else if (field==QLatin1String("archive-place")) {
        return QVariant();
    } else if (field==QLatin1String("archive_location")) {
        return data.value("librarynum", defaultVal);
    } else if (field==QLatin1String("archive")) {
        return data.value("library", defaultVal);
    } else if (field==QLatin1String("annote")) {
        return data.value("comments", defaultVal);
    } else if (field==QLatin1String("abstract")) {
        return data.value("abstract", defaultVal);
    }

    return QVariant();
}
