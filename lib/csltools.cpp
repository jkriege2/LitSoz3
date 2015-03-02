#include "csltools.h"
#include <QFile>
#include <QDomDocument>
#include <QDebug>
#include "bibtools.h"
#include <QChar>

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
    m_locale=NULL;
    m_macros.clear();
    m_bibliography=NULL;
    m_citation=NULL;
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
    delete m_citation;
    m_citation=NULL;
}

QString CSLFile::produce(const QMap<QString, QVariant>& data, bool citation, OutputFormat outf)
{
    QString res;
    CSLFormatState defFont;
    if (!citation && m_bibliography) res=res+m_bibliography->produce(data, defFont, outf);
    if (citation && m_citation) res=res+m_citation->produce(data, defFont, outf);
    return res;
}



void CSLFile::setLocale(CSLLocale *locale)
{
    m_locale=locale;
}

QString CSLFile::formatDate(QDate date, const QString &form)
{
    if (m_locale) return m_locale->formatDate(this, date, form);
    return date.toString();
}

QString CSLFile::applyTextCase(const QString &text, CSLFile::TextCase tcase)
{
    switch (tcase) {
        case tcLowercase: return text.toLower();
        case tcUppercase: return text.toUpper();
        case tcCapitalizeFirst: {
                QString txt=text.toLower();
                if (txt.size()>0) { txt[0]=txt[0].toUpper(); }
                return txt;
            }
        case tcTitle:
        case tcCapitalizeAll: {
                QString txt=text.toLower();
                bool isFirst=true;
                for (int i=0; i<txt.size(); i++) {
                    if (isFirst) txt[i]=txt[i].toUpper();
                    isFirst=false;
                    if (!txt[i].isLetterOrNumber()) isFirst=true;
                }
                return txt;
            }
        case tcSentence:
            break;
        default:
        case tcNormal:
            return text;
            break;
    }
    return text;
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
        e=doc.documentElement();
        QDomElement me=e.firstChildElement("macro");
        while (!me.isNull()) {
            parseMacro(me);
            me=me.nextSiblingElement("macro");
        }

        e=doc.documentElement();
        me=e.firstChildElement("bibliography");
        if (!me.isNull()) m_bibliography=parseElement(me, NULL);

        e=doc.documentElement();
        me=e.firstChildElement("citation");
        if (!me.isNull()) m_citation=parseElement(me, NULL);

        return true;
    }
    return false;
}

bool CSLFile::parseMacro(const QDomElement &e)
{
    if (e.tagName()=="macro") {
        QString name=e.attribute("name", "");
        qDebug()<<"parsing macro "<<name;
        if (name.size()>0) {
            CSLListNode* node=new CSLListNode(NULL, this);
            QDomElement me=e.firstChildElement();
            while (!me.isNull()) {
                CSLNode* n=parseElement(me, node);
                if (n) node->addChild(n);
                me=me.nextSiblingElement();
            }
            if (m_macros.contains(name) && m_macros[name]) {
                delete m_macros[name];
                m_macros.remove(name);
            }
            if (name.size()>0 && node) {
                m_macros.insert(name, node);
                return true;
            }
        }
    }
    return false;
}

CSLFile::CSLNode* CSLFile::parseElement(const QDomElement &e, CSLFile::CSLNode *parent)
{
    CSLNode* nodeOut=NULL;
    if (e.tagName()=="number") {

    } else if (e.tagName()=="layout") {
        qDebug()<<"parsing layout ...";
        CSLListNode* node=new CSLListNode(parent, this);
        node->parseProperties(e);
        QDomElement me=e.firstChildElement();
        while (!me.isNull()) {
            CSLNode* n=parseElement(me, node);
            if (n) node->addChild(n);
            me=me.nextSiblingElement();
        }
        nodeOut=node;
    } else if (e.tagName()=="group") {
        qDebug()<<"parsing group ...";
        CSLListNode* node=new CSLListNode(parent, this);
        node->parseProperties(e);
        QDomElement me=e.firstChildElement();
        while (!me.isNull()) {
            CSLNode* n=parseElement(me, node);
            if (n) node->addChild(n);
            me=me.nextSiblingElement();
        }
        nodeOut=node;
    } else if (e.tagName()=="text") {
        qDebug()<<"parsing text ...";
        CSLTextNode* node=new CSLTextNode(parent, this);
        node->parseProperties(e);
        nodeOut=node;
    } else if (e.tagName()=="number") {
        qDebug()<<"parsing number ...";
        CSLNumberNode* node=new CSLNumberNode(parent, this);
        node->parseProperties(e);
        nodeOut=node;
    } else if (e.tagName()=="choose") {
        qDebug()<<"parsing choose ...";
        CSLChooseNode* node=new CSLChooseNode(parent, this);
        node->parseProperties(e);
        nodeOut=node;
    } else if (e.tagName()=="names") {
        qDebug()<<"parsing names ...";
        CSLNamesNode* node=new CSLNamesNode(parent, this);
        node->parseProperties(e);
        nodeOut=node;
    } else if (e.tagName()=="label") {
        qDebug()<<"parsing label ...";
        CSLLabelNode* node=new CSLLabelNode(parent, this);
        node->parseProperties(e);
        nodeOut=node;
    } else if (e.tagName()=="bibliography") {
        if (!e.firstChildElement("layout").isNull()) {
            qDebug()<<"parsing bibligraphy.layout ...";
            nodeOut=parseElement(e.firstChildElement("layout"), parent);
        }
    } else if (e.tagName()=="citation") {
        if (!e.firstChildElement("layout").isNull()) {
            qDebug()<<"parsing citation.layout ...";
            nodeOut=parseElement(e.firstChildElement("layout"), parent);
        }
    }

    return nodeOut;
}


CSLFile::CSLNode::CSLNode(CSLFile::CSLNode *parent, CSLFile *file)
{
    m_file=file;
    m_parent=parent;


}

CSLFile::CSLNode::~CSLNode()
{

}


QString CSLFile::CSLNode::produce(const QMap<QString, QVariant> &data, const CSLFormatState& currentFormat, OutputFormat outf)
{
    return QString();
}

QVariant CSLFile::CSLNode::getCSLField(const QString &field, const QMap<QString, QVariant> &data,const QVariant& defaultVal)
{
    QString type=data.value("type", "misc").toString().trimmed().simplified().toLower();
    bool ok=false;

    // TYPE
    if (field=="type") {
        QString csltype=type;
        if (type=="inbook")             csltype="chapter";
        else if (type=="inproceedings") csltype="paper-conference";
        else if (type=="incollection")  csltype="chapter";
        else if (type=="compendium")    csltype="book";
        else if (type=="anthology")     csltype="book";
        else if (type=="booklet")       csltype="book";
        else if (type=="book")          csltype="book";
        else if (type=="article")       csltype="article-journal";
        else if (type=="webpage")       csltype="webpage";
        else if (type=="unpublished")   csltype="manuscript";
        else if (type=="thesis")        csltype="thesis";
        else if (type=="textbook")      csltype="book";
        else if (type=="techreport")    csltype="report";
        else if (type=="techmanual")    csltype="book";
        else if (type=="talk")          csltype="speech";
        else if (type=="proceedings")   csltype="book";
        else if (type=="poster")        csltype="paper-conference";
        else if (type=="patent")        csltype="patent";
        else if (type=="misc")          csltype="book";
        //else if (type=="")            csltype="";


        if (csltype.isEmpty()) return defaultVal;
        else return csltype;
    }


    // STRING FIELDS
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
        if (type=="article") return data.value("journal", defaultVal);
        return data.value("booktitle", defaultVal);
    } else if (field==QLatin1String("collection-title")) {
        return data.value("series", defaultVal);
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


    // NUMBER FIELDS
    } else if (field==QLatin1String("chapter-number")) {
        return data.value("chapter", defaultVal);
    } else if (field==QLatin1String("collection-number")) {
        int i=data.value("volume", defaultVal).toInt(&ok);
        if (ok) return i;
    } else if (field==QLatin1String("edition")) {
        int i=data.value("edition", defaultVal).toInt(&ok);
        if (ok) return i;
    } else if (field==QLatin1String("issue")) {
        int i=data.value("number", defaultVal).toInt(&ok);
        if (ok) return i;
    } else if (field==QLatin1String("number")) {
        int i=data.value("volume", defaultVal).toInt(&ok);
        if (ok) return i;
    } else if (field==QLatin1String("number-of-pages")) {
        //return data.value("", defaultVal);
    } else if (field==QLatin1String("number-of-volumes")) {
        //return data.value("", defaultVal);
    } else if (field==QLatin1String("volume")) {
        int i=data.value("volume", defaultVal).toInt(&ok);
        if (ok) return i;


    // DATE VARIABLES
    } else if (field==QLatin1String("accessed")) {
        return data.value("viewdate", defaultVal);
    } else if (field==QLatin1String("event-date")) {
        return data.value("year", defaultVal);
    } else if (field==QLatin1String("issued")) {
        return data.value("year", defaultVal);
    } else if (field==QLatin1String("original-date")) {
        //return data.value("", defaultVal);
    } else if (field==QLatin1String("submitted")) {
        //return data.value("", defaultVal);


    // NAME VARIABLES
    } else if (field==QLatin1String("author")) {
        return data.value("authors", defaultVal);
    } else if (field==QLatin1String("collection-editor")) {
        return data.value("editors", defaultVal);
    } else if (field==QLatin1String("composer")) {
        return data.value("authors", defaultVal);
    } else if (field==QLatin1String("container-author")) {
        //return data.value("", defaultVal);
    } else if (field==QLatin1String("director")) {
        return data.value("author", defaultVal);
    } else if (field==QLatin1String("editor")) {
        return data.value("editors", defaultVal);
    } else if (field==QLatin1String("editorial-director")) {
        //return data.value("editors", defaultVal);
    } else if (field==QLatin1String("illustrator")) {
        //return data.value("", defaultVal);
    } else if (field==QLatin1String("interviewer")) {
        //return data.value("", defaultVal);
    } else if (field==QLatin1String("original-author")) {
        //return data.value("", defaultVal);
    } else if (field==QLatin1String("recipient")) {
        //return data.value("", defaultVal);
    } else if (field==QLatin1String("reviewed-author")) {
        //return data.value("", defaultVal);
    } else if (field==QLatin1String("translator")) {
        //return data.value("", defaultVal);
    }

    return QVariant();
}

void CSLFile::CSLNode::parseProperties(const QDomElement &e)
{
    parseBasicProperties(e);
}

QString CSLFile::CSLNode::escapeString(CSLFile::OutputFormat outf, const QString &text)
{
    if (outf==ofHTML) return escapeHTMLString(text);
    return text;
}


/*QString CSLFile::CSLNode::getHTMLFormatting(const QString &text, CSLNode* m_parent)
{
    bool setFW=false, resetFW=false;
    if (m_parent) {
        if (fweight!=m_parent->fweight) {
            if (fweight!=fwNormal) {
                setFW=true;
            } else {
                resetFW=true;
            }
        }
    } else {
        if (fweight!=fwNormal) {
            setFW=true;
        }
    }

    bool setFS=false, resetFS=false;
    if (m_parent) {
        if (fstyle!=m_parent->fstyle) {
            if (fstyle!=fsNormal) {
                setFS=true;
            } else {
                resetFS=true;
            }
        }
    } else {
        if (fstyle!=fsNormal) {
            setFS=true;
        }
    }


    bool setFV=false, resetFV=false;
    if (m_parent) {
        if (fvariant!=m_parent->fvariant) {
            if (fvariant!=fvNormal) {
                setFV=true;
            } else {
                resetFV=true;
            }
        }
    } else {
        if (fvariant!=fvNormal) {
            setFV=true;
        }
    }


    bool setTD=false, resetTD=false;
    if (m_parent) {
        if (tdecor!=m_parent->tdecor) {
            if (tdecor!=tdNone) {
                setTD=true;
            } else {
                resetTD=true;
            }
        }
    } else {
        if (tdecor!=tdNone) {
            setTD=true;
        }
    }



    bool setVA=false, resetVA=false;
    if (m_parent) {
        if (valign!=m_parent->valign) {
            if (valign!=vaBaseline) {
                setVA=true;
            } else {
                resetVA=true;
            }
        }
    } else {
        if (valign!=vaBaseline) {
            setVA=true;
        }
    }


    QString pre="";
    QString post="";


    if (setFW) {
        pre=pre+getFontWeightTag(false);
        post=getFontWeightTag(true)+post;
    } else if (resetFW) {
        pre=pre+m_parent->getFontWeightTag(true);
        post=m_parent->getFontWeightTag(false)+post;
    }

    if (setFS) {
        pre=pre+getFontStyleTag(false);
        post=getFontStyleTag(true)+post;
    } else if (resetFS) {
        pre=pre+m_parent->getFontStyleTag(true);
        post=m_parent->getFontStyleTag(false)+post;
    }

    if (setFV) {
        pre=pre+getFontVariantTag(false);
        post=getFontVariantTag(true)+post;
    } else if (resetFV) {
        pre=pre+m_parent->getFontVariantTag(true);
        post=m_parent->getFontVariantTag(false)+post;
    }

    if (setTD) {
        pre=pre+getTextDecorationTag(false);
        post=getTextDecorationTag(true)+post;
    } else if (resetTD) {
        pre=pre+m_parent->getTextDecorationTag(true);
        post=m_parent->getTextDecorationTag(false)+post;
    }

    if (setVA) {
        pre=pre+getVerticalAlignTag(false);
        post=getVerticalAlignTag(true)+post;
    } else if (resetVA) {
        pre=pre+m_parent->getVerticalAlignTag(true);
        post=m_parent->getVerticalAlignTag(false)+post;
    }


    return pre+text+post;
}

QString CSLFile::CSLNode::getFontWeightTag(bool endtag)
{
    if (fweight==fwBold) {
        if (!endtag) return QLatin1String("<b>"); else return QLatin1String("</b>");
    }
    return QString();
}

QString CSLFile::CSLNode::getFontStyleTag(bool endtag)
{
    if (fstyle==fsItalic) {
        if (!endtag) return QLatin1String("<i>"); else return QLatin1String("</i>");
    } else if (fstyle==fsOblique) {
        if (!endtag) return QLatin1String("<i>"); else return QLatin1String("</i>");
    }

    return QString();
}

QString CSLFile::CSLNode::getFontVariantTag(bool endtag)
{
    if (fvariant==fvSmallCaps) {
        if (!endtag) return QLatin1String("<span style=\"font-variant:small-caps\">"); else return QLatin1String("</style>");
    }
    return QString();

}

QString CSLFile::CSLNode::getTextDecorationTag(bool endtag)
{
    if (tdecor==tdUnderline) {
        if (!endtag) return QLatin1String("<u>"); else return QLatin1String("</u>");
    }
    return QString();

}

QString CSLFile::CSLNode::getVerticalAlignTag(bool endtag)
{
    if (valign==vaSuper) {
        if (!endtag) return QLatin1String("<sup>"); else return QLatin1String("</sup>");
    } else if (valign==vaSub) {
        if (!endtag) return QLatin1String("<sub>"); else return QLatin1String("</sub>");
    }
    return QString();

}*/


CSLFile::CSLListNode::CSLListNode(CSLFile::CSLNode *parent, CSLFile *file):
    CSLFile::CSLNode(parent, file)
{

}

CSLFile::CSLListNode::~CSLListNode()
{
    clearChildren();
}


QString CSLFile::CSLListNode::produce(const QMap<QString, QVariant> &data, const CSLFormatState &currentFormat, OutputFormat outf)
{
    CSLFormatState newf=currentFormat;
    modifyStyle(newf);
    QString res;
    for (int i=0; i<m_children.size(); i++) {
        if (i>0) res+=delimiter;
        if (m_children[i]) res+=m_children[i]->produce(data, newf, outf);
    }

    return newf.startFormat(currentFormat, outf)+prefix+res+suffix+newf.endFormat(currentFormat, outf);
}

void CSLFile::CSLListNode::addChild(CSLFile::CSLNode *c)
{
    if (c) {
        m_children.append(c);
        c->setParent(this);
    }
}

void CSLFile::CSLListNode::clearChildren()
{
    for (int i=0; i<m_children.size(); i++) {
        if (m_children[i]) delete m_children[i];
    }
    m_children.clear();
}


CSLFile::CSLTextNode::CSLTextNode(CSLFile::CSLNode *parent, CSLFile *file):
    CSLFile::CSLNode(parent, file)
{
    plural=false;
}

CSLFile::CSLTextNode::~CSLTextNode()
{
    form="";
}


QString CSLFile::CSLTextNode::produce(const QMap<QString, QVariant> &data, const CSLFormatState& currentFormat, OutputFormat outf)
{
    CSLFormatState newf=currentFormat;
    modifyStyle(newf);

    QString res="";
    if (!value.isEmpty()) res=escapeString(outf, applyTextCase(value));
    else if (!variable.isEmpty()) res=escapeString(outf, applyTextCase(getCSLField(variable, data, QString()).toString()));
    else if (!term.isEmpty()) res=escapeString(outf, applyTextCase(m_file->term(term, !plural, form)));
    else if (!macro.isEmpty()) {
        if (m_file && m_file->m_macros.contains(macro)) {
            res+=m_file->m_macros[macro]->produce(data, newf, outf);
        }
    }

    return newf.startFormat(currentFormat, outf)+prefix+res+suffix+newf.endFormat(currentFormat, outf);
}

void CSLFile::CSLTextNode::parseProperties(const QDomElement &e)
{
    CSLFile::CSLNode::parseProperties(e);
    macro=e.attribute("macro");
    variable=e.attribute("variable");
    term=e.attribute("term");
    form=e.attribute("form");
    if (e.attribute("plural ")=="true") plural=true;
    value=e.attribute("value");
}


CSLLocale::CSLLocale(const QString &fn)
{
    QDomDocument doc;
    QFile f(fn);
    if (f.open(QFile::ReadOnly|QFile::Text)) {
        doc.setContent(f.readAll());
        f.close();
        QDomElement e=doc.documentElement();
        if (!e.isNull() && e.tagName()=="locale") {
            QString lang=e.attribute("lang", e.attribute("xml:lang", "en-US"));
            m_name=lang;
            m_locale=QLocale(lang);
            parseLocale(m_terms, e);
            QDomElement de=e.firstChildElement("date");
            while (!de.isNull()) {
                QString form=de.attribute("form");
                QList<CSLDateFormat> l;
                parseDate(l, de);
                m_dates.insert(form, l);
                de=de.nextSiblingElement("date");
            }
        }


    }
}

CSLLocale::~CSLLocale()
{

}

void CSLLocale::parseLocale(QMap<QString, QMap<QString, CSLLocale::CSLLocaleValue> > &m_terms, const QDomElement &e_in)
{
    QDomElement e=e_in;
    e=e.firstChildElement("terms").firstChildElement("term");
    while (!e.isNull()) {
        QString name=e.attribute("name", "");
        QString form=e.attribute("form", "");
        QMap<QString, CSLLocaleValue> t;
        CSLLocaleValue l;
        if (e.hasChildNodes()) {
            l.m_single=e.firstChildElement("single").text();
            l.m_multiple=e.firstChildElement("multiple").text();
        } else {
            l.m_single=e.text();
        }
        t.insert(form, l);
        if (!name.isEmpty()) {
            if (m_terms.contains(name)) {
                if (!m_terms[name].contains(form)) {
                    m_terms[name].insert(form, l);
                }
            } else {
                m_terms.insert(name, t);
            }
        }

        e=e.nextSiblingElement("term");
    }


}

void CSLLocale::parseDate(QList<CSLLocale::CSLDateFormat> &m_terms, const QDomElement &e)
{
    if (e.tagName()=="date") {
        QDomElement dp=e.firstChildElement("date-part");
        while (!dp.isNull()) {
            CSLLocale::CSLDateFormat df;
            df.name=dp.attribute("name");
            df.prefix=dp.attribute("prefix");
            df.suffix=dp.attribute("suffix");
            df.form=dp.attribute("form");
            df.range_delimiter=dp.attribute("range-delimiter");
            m_terms.append(df);
            dp=dp.nextSiblingElement("date-part");
        }
    }
}

QString CSLLocale::formatDate(QDate date, const QString &form)
{
    if (m_dates.contains(form)) {
        QList<CSLDateFormat> l=m_dates[form];
        return formatDate(this, m_locale, l, date, form);
    } else {
        QList<CSLDateFormat> l=m_dates["text"];
        return formatDate(this, m_locale, l, date, form);
    }
}

QString CSLLocale::formatDate(CSLLocaleInterface* localeIntf, const QLocale& locale, const QList<CSLDateFormat>& m_dates, QDate date, const QString& form)
{

    QList<CSLDateFormat> l=m_dates;
    QString res;
    for (int i=0; i<l.size(); i++) {
        res=res+l[i].prefix;
        if (l[i].form=="numeric-leading-zeros") {
            if (l[i].name=="day") res=res+QString("%1").arg(date.day(), 2, 10, QLatin1Char('0'));
            if (l[i].name=="month") res=res+QString("%1").arg(date.month(), 2, 10, QLatin1Char('0'));
            if (l[i].name=="year") res=res+locale.toString(date.year());
        } else if (l[i].form=="numeric") {
            if (l[i].name=="day") res=res+locale.toString(date.day());
            if (l[i].name=="month") res=res+locale.toString(date.month());
            if (l[i].name=="year") res=res+locale.toString(date.year());
        } else if (l[i].form=="ordinal") {
            int val=-1;
            if (l[i].name=="day") val=date.day();
            if (l[i].name=="month") val=date.month();
            if (l[i].name=="year") val=date.year();
            if (val>=0) {
                QString ord=localeIntf->term("ordinal", true);
                if (val==1 && localeIntf->hasTerm("ordinal-01")) ord=localeIntf->term("ordinal-01", true);
                if (val==2 && localeIntf->hasTerm("ordinal-02")) ord=localeIntf->term("ordinal-02", true);
                if (val==3 && localeIntf->hasTerm("ordinal-03")) ord=localeIntf->term("ordinal-03", true);
                if (val==11 && localeIntf->hasTerm("ordinal-11")) ord=localeIntf->term("ordinal-11", true);
                if (val==12 && localeIntf->hasTerm("ordinal-12")) ord=localeIntf->term("ordinal-12", true);
                if (val==13 && localeIntf->hasTerm("ordinal-13")) ord=localeIntf->term("ordinal-13", true);
                res=res+locale.toString(val);
            }
        } else if (l[i].form=="short") {
            if (l[i].name=="day") res=res+locale.dayName(date.day(), QLocale::ShortFormat);
            if (l[i].name=="month") {
                QString t=QString("month-%1").arg(date.month(), 2, 10, QLatin1Char('0'));
                if (localeIntf->hasTerm(t)) res=res+localeIntf->term(t, true, "short");
                else res=res+locale.dayName(date.month(), QLocale::ShortFormat);
            }
            if (l[i].name=="year") res=res+locale.toString(date.year()%100);
        } else { // "long"
            if (l[i].name=="day") res=res+locale.dayName(date.day(), QLocale::LongFormat);
            if (l[i].name=="month") {
                QString t=QString("month-%1").arg(date.month(), 2, 10, QLatin1Char('0'));
                if (localeIntf->hasTerm(t)) res=res+localeIntf->term(t, true, "");
                else res=res+locale.dayName(date.month(), QLocale::ShortFormat);
            }
            if (l[i].name=="year") res=res+locale.toString(date.year());
        }
        res=res+l[i].suffix;
    }
    return res;

}

QString CSLLocale::formatDate(CSLLocaleInterface *localeIntf, QDate date, const QString &form)
{
    QList<CSLDateFormat> l=m_dates[form];
    return formatDate(localeIntf, m_locale, l, date, form);
}




QString CSLFile::CSLFormatState::startFormat(const CSLFile::CSLFormatState &lastFormat, OutputFormat outf) const
{
    QString res;

    if (outf==ofHTML) {
        if (lastFormat.fstyle==fsNormal  && fstyle==fsItalic) res+=QLatin1String("<i>");
        else if (lastFormat.fstyle==fsNormal  && fstyle==fsOblique) res+=QLatin1String("<i>");
        else if (lastFormat.fstyle==fsItalic  && fstyle==fsNormal) res+=QLatin1String("</i>");
        else if (lastFormat.fstyle==fsItalic  && fstyle==fsOblique) res+=QLatin1String("</i><i>");
        else if (lastFormat.fstyle==fsOblique  && fstyle==fsNormal) res+=QLatin1String("</i>");
        else if (lastFormat.fstyle==fsOblique  && fstyle==fsItalic) res+=QLatin1String("</i><i>");

        if (lastFormat.fweight==fwNormal  && fweight==fwBold) res+=QLatin1String("<b>");
        else if (lastFormat.fweight==fwNormal  && fweight==fwLight) res+=QLatin1String("");
        else if (lastFormat.fweight==fwBold  && fweight==fwNormal) res+=QLatin1String("</b>");
        else if (lastFormat.fweight==fwBold  && fweight==fwLight) res+=QLatin1String("</b>");
        else if (lastFormat.fweight==fwLight  && fweight==fwNormal) res+=QLatin1String("");
        else if (lastFormat.fweight==fwLight  && fweight==fwBold) res+=QLatin1String("<b>");

        if (lastFormat.fvariant==fvNormal  && fvariant==fvSmallCaps) res+=QLatin1String("<span style=\"font-variant: small-caps\">");
        else if (lastFormat.fvariant==fvSmallCaps  && fvariant==fvNormal) res+=QLatin1String("</span>");

        if (lastFormat.tdecor==tdNone  && tdecor==tdUnderline) res+=QLatin1String("<u>");
        else if (lastFormat.tdecor==tdUnderline  && tdecor==tdNone) res+=QLatin1String("</u>");


        if (lastFormat.valign==vaBaseline  && valign==vaSub) res+=QLatin1String("<sub>");
        else if (lastFormat.valign==vaBaseline  && valign==vaSuper) res+=QLatin1String("<sup>");
        else if (lastFormat.valign==vaSub  && valign==vaBaseline) res+=QLatin1String("</sub>");
        else if (lastFormat.valign==vaSub  && valign==vaSuper) res+=QLatin1String("</sub><sup>");
        else if (lastFormat.valign==vaSuper  && valign==vaBaseline) res+=QLatin1String("</sup>");
        else if (lastFormat.valign==vaSuper  && valign==vaSub) res+=QLatin1String("</sup><sub>");
    }

    return res;
}

QString CSLFile::CSLFormatState::endFormat(const CSLFile::CSLFormatState &newFormat, OutputFormat outf) const
{
    if (outf==ofHTML) {
        return newFormat.startFormat(*this, outf);
    }
    return QString();
}


CSLFile::CSLNumberNode::CSLNumberNode(CSLFile::CSLNode *parent, CSLFile *file):
    CSLFile::CSLNode(parent, file)
{

}

CSLFile::CSLNumberNode::~CSLNumberNode()
{

}

QString CSLFile::CSLNumberNode::produce(const QMap<QString, QVariant> &data, const CSLFile::CSLFormatState &currentFormat, OutputFormat outf)
{
    CSLFormatState newf=currentFormat;
    modifyStyle(newf);

    QString res="";
    if (!variable.isEmpty()) {
        if (form=="numeric") res=escapeString(outf, QString::number(getCSLField(variable, data, QString()).toInt()));
        else if (form=="ordinal") {
            int i=getCSLField(variable, data, QString()).toInt();
            res=escapeString(outf, QString::number(i));
            if (i==1) res+=escapeString(outf, m_file->term("ordinal-01", true));
            else if (i==2) res+=escapeString(outf, m_file->term("ordinal-02", true));
            else if (i==3) res+=escapeString(outf, m_file->term("ordinal-03", true));
            else if (i==11) res+=escapeString(outf, m_file->term("ordinal-11", true));
            else if (i==12) res+=escapeString(outf, m_file->term("ordinal-12", true));
            else if (i==13) res+=escapeString(outf, m_file->term("ordinal-13", true));
            else res+=escapeString(outf, m_file->term("ordinal", true));

        } else if (form=="long-ordinal") {
            int i=getCSLField(variable, data, QString()).toInt();
            if (i==1) res+=escapeString(outf, m_file->term("long-ordinal-01", true));
            else if (i==2) res+=escapeString(outf, m_file->term("long-ordinal-02", true));
            else if (i==3) res+=escapeString(outf, m_file->term("long-ordinal-03", true));
            else if (i==4) res+=escapeString(outf, m_file->term("long-ordinal-04", true));
            else if (i==5) res+=escapeString(outf, m_file->term("long-ordinal-05", true));
            else if (i==6) res+=escapeString(outf, m_file->term("long-ordinal-06", true));
            else if (i==7) res+=escapeString(outf, m_file->term("long-ordinal-07", true));
            else if (i==8) res+=escapeString(outf, m_file->term("long-ordinal-08", true));
            else if (i==9) res+=escapeString(outf, m_file->term("long-ordinal-09", true));
            else if (i==10) res+=escapeString(outf, m_file->term("long-ordinal-10", true));
            else res+=(escapeString(outf, QString::number(i))+escapeString(outf, m_file->term("ordinal", true)));

        }
        else { //if (form=="roman") {
            res=escapeString(outf, QString::number(getCSLField(variable, data, QString()).toInt()));
        }
    }

    return newf.startFormat(currentFormat, outf)+prefix+res+suffix+newf.endFormat(currentFormat, outf);
}

void CSLFile::CSLNumberNode::parseProperties(const QDomElement &e)
{
    CSLFile::CSLNode::parseProperties(e);
    variable=e.attribute("variable");
    form=e.attribute("form");
}


CSLFile::CSLChooseNode::CSLChooseNode(CSLFile::CSLNode *parent, CSLFile *file):
    CSLFile::CSLNode(parent, file)
{

}

CSLFile::CSLChooseNode::~CSLChooseNode()
{
    for (int i=0; i<ifs.size(); i++) {
        if (ifs[i].node) delete ifs[i].node;
    }
    ifs.clear();
}

QString CSLFile::CSLChooseNode::produce(const QMap<QString, QVariant> &data, const CSLFile::CSLFormatState &currentFormat, OutputFormat outf)
{
    for (int i=0; i<ifs.size(); i++) {
        switch(ifs[i].ifType) {
            case iftType:{
                    QString type=getCSLField("type", data, QString()).toString();
                    QStringList types=ifs[i].type.split(QRegExp("\\s"));
                    qDebug()<<"if-type: "<<type<<types;
                    if (    (ifs[i].match=="any" && types.contains(type))
                            ||(ifs[i].match=="none" && !types.contains(type))
                            ||(ifs[i].match=="" && ifs[i].type==type)
                       )
                    {
                        if (ifs[i].node) return ifs[i].node->produce(data, currentFormat, outf);
                        return QString();
                    }
                }
                break;
            case iftIsNumeric:{
                    QString field=getCSLField(ifs[i].variable, data, QString()).toString();
                    QRegExp rx("[^\\d]*(\\d)+([^\\d]|[\\-\\&\\s\\,])*");
                    qDebug()<<"if-numeric: "<<ifs[i].variable<<field<<rx.indexIn(field);
                    if (rx.indexIn(field)>=0) {
                        if (ifs[i].node) return ifs[i].node->produce(data, currentFormat, outf);
                        return QString();
                    }
                }
                break;
            case iftVariable:{
                    QString field=getCSLField(ifs[i].variable, data, QString()).toString();
                    qDebug()<<"if-variable: "<<ifs[i].variable<<field;
                    if (!field.isEmpty()) {
                        if (ifs[i].node) return ifs[i].node->produce(data, currentFormat, outf);
                        return QString();
                    }
                }
                break;

            case iftElse:
                qDebug()<<"else: ";
                if (ifs[i].node) return ifs[i].node->produce(data, currentFormat, outf);
                return QString();
                break;
            default:
                break;
        }
    }
    return QString();
}

void CSLFile::CSLChooseNode::parseProperties(const QDomElement &e)
{
    if (e.tagName()=="choose") {
        QDomElement ie=e.firstChildElement();
        while (!ie.isNull()) {
            ifData d;
            d.variable=ie.attribute("variable");
            d.match=ie.attribute("match");
            d.type=ie.attribute("type");
            QString isNumeric=ie.attribute("is-numeric");
            if (ie.tagName()=="else") {
                d.ifType=iftElse;
                CSLListNode* node=new CSLListNode(this, m_file);
                QDomElement me=ie.firstChildElement();
                while (!me.isNull()) {
                    CSLNode* n=m_file->parseElement(me, node);
                    if (n) node->addChild(n);
                    me=me.nextSiblingElement();
                }
                d.node=node;
                ifs.append(d);
            } else if (ie.tagName()=="if" || ie.tagName()=="else-if") {
                bool ok=false;
                if (!d.variable.isEmpty()) { ok=true; d.ifType=iftVariable;}
                else if (!d.type.isEmpty()) { ok=true; d.ifType=iftType;}
                else if (!isNumeric.isEmpty()) { ok=true; d.ifType=iftIsNumeric; d.variable=isNumeric; }
                if (ok) {
                    CSLListNode* node=new CSLListNode(this, m_file);
                    QDomElement me=ie.firstChildElement();
                    while (!me.isNull()) {
                        CSLNode* n=m_file->parseElement(me, node);
                        if (n) node->addChild(n);
                        me=me.nextSiblingElement();
                    }
                    d.node=node;
                    ifs.append(d);
                }
            }

            ie=ie.nextSiblingElement();
        }
    }
}


CSLFile::CSLNamesNode::CSLNamesNode(CSLFile::CSLNode *parent, CSLFile *file):
    CSLFile::CSLNode(parent, file)
{
    andSeparator=nasAndTerm;
    delimiter=", ";
    delimiterPrecedesEtAl=dpeaContextual;
    delimiterPrecedesLast=dpeaContextual;
    etalMin=-1;
    etaluseFirst=-1;
    etalUseLast=false;
    form="long";
    initialize=true;
    initializeWith="";
    nameAsSortOrder="";
    sortSeparator=", ";
    etal="et-al";
    m_labelNode=NULL;
}

CSLFile::CSLNamesNode::~CSLNamesNode()
{
    for (int i=0; i<m_substitutes.size(); i++) {
        if (m_substitutes[i]) delete m_substitutes[i];
    }
    m_substitutes.clear();
    if (m_labelNode) delete m_labelNode;
}

QString CSLFile::CSLNamesNode::produce(const QMap<QString, QVariant> &data, const CSLFile::CSLFormatState &currentFormat, OutputFormat outf)
{
    CSLFormatState newf=currentFormat;
    modifyStyle(newf);

    QString res="";

    QStringList familyNames, givenNames, vars;
    for (int i=0; i<variables.size(); i++) {
        QString names=getCSLField(variables[i], data, QString()).toString();
        if (!names.isEmpty()) {
            QStringList gn, fn, v;
            parseAuthors(names, &gn, &fn);
            qDebug()<<" parsedAuthors "<<names<<" => "<<gn<<fn;
            if (gn.size()>0 && fn.size()>0 && gn.size()==fn.size()) {
                for (int j=0; j<gn.size(); j++) v<<variables[i];
                familyNames<<fn;
                givenNames<<gn;
                vars<<v;
            }
        }
    }

    QStringList names;
    if (familyNames.size()>0) {
        CSLFormatState namef=newf;
        nameProps.modifyStyle(namef);
        CSLFormatState fnamef=newf;
        familyNameProps.modifyStyle(fnamef);
        CSLFormatState gnamef=newf;
        givenNameProps.modifyStyle(gnamef);
        CSLFormatState etalf=newf;
        etalProps.modifyStyle(etalf);
        for (int i=0; i<familyNames.size(); i++) {
            QString name;
            QString gn=givenNameProps.applyTextCase(givenNames[i]);
            QString fn=familyNameProps.applyTextCase(familyNames[i]);
            if (initialize) {
                if (initializeWith.endsWith(' ')) gn=shortenGivenName(gn, initializeWith);
                else gn=shortenGivenName(gn, initializeWith+" ");
            }
            if (form=="short") {
                name=namef.startFormat(newf, outf)+escapeString(outf,fn)+namef.endFormat(newf, outf);
            } else { //(form=="long" )
                if (nameAsSortOrder.isEmpty()) {
                    name=gnamef.startFormat(newf, outf)+escapeString(outf,gn)+gnamef.endFormat(newf, outf)+" ";
                    name=name+namef.startFormat(newf, outf)+escapeString(outf,fn)+namef.endFormat(newf, outf);
                } else {
                    name=namef.startFormat(newf, outf)+escapeString(outf,fn)+namef.endFormat(newf, outf);
                    if (gn.size()>0) name=name+sortSeparator+gnamef.startFormat(newf, outf)+escapeString(outf,gn)+gnamef.endFormat(newf, outf);
                }
            }
            if (m_labelNode && !vars.value(i, "").isEmpty()) {
                m_labelNode->setVariable(vars.value(i, ""));
                name=name+" "+m_labelNode->produce(data, newf, outf);
            }
            names<<name;
        }
        if (etalMin<0 || names.size()<=etalMin) {
            for (int i=0; i<names.size(); i++) {
                if (i>0 && i<names.size()-1) res=res+escapeString(outf, delimiter);
                if (i>0 && i==names.size()-1) {
                    if (delimiterPrecedesLast==dpeaAlways) res=res+escapeString(outf, delimiter);
                    else if (delimiterPrecedesLast==dpeaNever) res=res+escapeString(outf, " ");
                    else if (names.size()>1) res=res+escapeString(outf, delimiter);
                    else res=res+" ";
                }
                res=res+names[i];
            }
        } else if (names.size()>etalMin) {
            int count=qMin(etaluseFirst, names.size());
            for (int i=0; i<count; i++) {
                if (i>0 && i<count-1) res=res+escapeString(outf, delimiter);
                res=res+names[i];
            }
            if (delimiterPrecedesEtAl==dpeaAlways) res=res+escapeString(outf, delimiter);
            else if (delimiterPrecedesEtAl==dpeaNever) res=res+escapeString(outf, " ");
            else if (names.size()>1) res=res+escapeString(outf, delimiter);
            else res=res+" ";
            res=res+etalf.startFormat(newf, outf)+etal+etalf.endFormat(newf, outf);
            if (etalUseLast) {
                res=res+" "+names.last();
            }
        }

    } else {
        for (int i=0; i<m_substitutes.size(); i++) {
            if (m_substitutes[i]) {
                //if (dynamic_cast<CSLFile::CSLNamesNode*>(m_substitutes[i])) {

                //} else {
                    QString plain=m_substitutes[i]->produce(data, currentFormat, ofPlaintext);
                    if (plain.size()>0) {
                        res=m_substitutes[i]->produce(data, currentFormat, outf);
                        break;
                    }
                //}
            }
        }
    }
    qDebug()<<"produced names variables="<<variables<<" form="<<form<<" delimiter="<<delimiter<<" etal="<<etal<<" initialize="<<initialize<<" initializeWith="<<initializeWith<<" nameAsSortOrder="<<nameAsSortOrder<<" sortSeparator="<<sortSeparator<<"\n   from: "<<familyNames<<"\n         "<<givenNames<<"\n         "<<vars<<"\n => "<<names;

    return newf.startFormat(currentFormat, outf)+prefix+res+suffix+newf.endFormat(currentFormat, outf);
}

void CSLFile::CSLNamesNode::parseProperties(const QDomElement &e)
{
    CSLFile::CSLNode::parseProperties(e);
    variables=e.attribute("variable").split(' ');
    parseNameProperties(e);

    QDomElement npe=e.firstChildElement("substitute");
    if (!npe.isNull()) {
        QDomElement me=npe.firstChildElement();
        while (!me.isNull()) {
            CSLNode* n=m_file->parseElement(me, this);
            if (dynamic_cast<CSLFile::CSLNamesNode*>(n)) {
                dynamic_cast<CSLFile::CSLNamesNode*>(n)->parseNameProperties(e);
            }
            if (n) m_substitutes.append(n);
            me=me.nextSiblingElement();
        }
    }

    npe=e.firstChildElement("label");
    if (!npe.isNull()) {
        m_labelNode=new CSLLabelNode(this, m_file);
        m_labelNode->parseProperties(e);
    }

}

void CSLFile::CSLNamesNode::parseNameProperties(const QDomElement &e)
{
    qDebug()<<"parseNameProperties("<<e.tagName()<<")";
    parseBasicProperties(e);
    QDomElement le=e.firstChildElement("name");
    qDebug()<<"parseNameProperties("<<e.tagName()<<")  le="<<le.tagName();
    if (!le.isNull()) {
        if (le.attribute("and")=="text") andSeparator=nasAndTerm;
        if (le.attribute("and")=="symbol") andSeparator=nasSymbol;
        form=le.attribute("form", form);
        initializeWith=le.attribute("initialize-with", initializeWith);
        etalMin=le.attribute("et-al-min", "-1").toInt();
        etaluseFirst=le.attribute("et-al-use-first", "-1").toInt();
        nameAsSortOrder=le.attribute("name-as-sort-order", nameAsSortOrder);
        sortSeparator=le.attribute("sort-separator", sortSeparator);
        if (le.attribute("et-al-use-last")=="true") etalUseLast=true;
        if (le.attribute("et-al-use-last")=="false") etalUseLast=false;

        if (le.attribute("initialize")=="true") initialize=true;
        if (le.attribute("initialize")=="false") initialize=false;

        if (le.attribute("delimiter-precedes-et-al")=="contextual") delimiterPrecedesEtAl=dpeaContextual;
        if (le.attribute("delimiter-precedes-et-al")=="after-inverted-name") delimiterPrecedesEtAl=dpeaAfterInverseName;
        if (le.attribute("delimiter-precedes-et-al")=="always") delimiterPrecedesEtAl=dpeaAlways;
        if (le.attribute("delimiter-precedes-et-al")=="never") delimiterPrecedesEtAl=dpeaNever;

        if (le.attribute("delimiter-precedes-et-al")=="contextual") delimiterPrecedesLast=dpeaContextual;
        if (le.attribute("delimiter-precedes-et-al")=="after-inverted-name") delimiterPrecedesLast=dpeaAfterInverseName;
        if (le.attribute("delimiter-precedes-et-al")=="always") delimiterPrecedesLast=dpeaAlways;
        if (le.attribute("delimiter-precedes-et-al")=="never") delimiterPrecedesLast=dpeaNever;
        nameProps.parseBasicProperties(le);
        QDomElement npe=le.firstChildElement("name-part");
        if (!npe.isNull()) {
            if (npe.attribute("name")=="family") familyNameProps.parseBasicProperties(npe);
            else givenNameProps.parseBasicProperties(npe);
            npe=npe.nextSiblingElement("name-part");
        }
        if (!npe.isNull()) {
            if (npe.attribute("name")=="family") familyNameProps.parseBasicProperties(npe);
            else givenNameProps.parseBasicProperties(npe);
        }
    }

    QDomElement npe=e.firstChildElement("et-al");
    if (!npe.isNull()) {
        etalProps.parseBasicProperties(npe);
        etal=npe.attribute("term", etal);
    }
}


CSLFile::CSLBasicProps::CSLBasicProps()
{
    suffix="";
    prefix="";
    delimiter="";
    fstyle=fsNormal;
    fvariant=fvNormal;
    fweight=fwNormal;
    tdecor=tdNone;
    valign=vaBaseline;
    tcase=tcNormal;

    set_fstyle=false;
    set_fvariant=false;
    set_fweight=false;
    set_tdecor=false;
    set_valign=false;
}

void CSLFile::CSLBasicProps::modifyStyle(CSLFile::CSLFormatState &style)
{
    if (set_fstyle) style.fstyle=fstyle;
    if (set_fvariant) style.fvariant=fvariant;
    if (set_fweight) style.fweight=fweight;
    if (set_tdecor) style.tdecor=tdecor;
    if (set_valign) style.valign=valign;}

QString CSLFile::CSLBasicProps::applyTextCase(const QString &text)
{
    return CSLFile::applyTextCase(text, tcase);
}

void CSLFile::CSLBasicProps::parseBasicProperties(const QDomElement &e)
{
    if (e.attribute("font-style")=="italic") { set_fstyle=true; fstyle=fsItalic; }
    if (e.attribute("font-style")=="oblique") { set_fstyle=true; fstyle=fsOblique; }
    if (e.attribute("font-style")=="normal") { set_fstyle=true; fstyle=fsNormal; }

    if (e.attribute("font-variant")=="small-caps") { set_fvariant=true;  fvariant=fvSmallCaps; }
    if (e.attribute("font-variant")=="normal") { set_fvariant=true; fvariant=fvNormal; }

    if (e.attribute("font-weight")=="bold") { set_fweight=true; fweight=fwBold; }
    if (e.attribute("font-weight")=="light") { set_fweight=true; fweight=fwLight; }
    if (e.attribute("font-weight")=="normal") { set_fweight=true; fweight=fwNormal; }

    if (e.attribute("text-decoration")=="none") { set_tdecor=true; tdecor=tdNone; }
    if (e.attribute("text-decoration")=="underline") { set_tdecor=true; tdecor=tdUnderline; }

    if (e.attribute("vertical-align")=="sub") { set_valign=true; valign=vaSub; }
    if (e.attribute("vertical-align")=="sup") { set_valign=true; valign=vaSuper; }
    if (e.attribute("vertical-align")=="baseline") { set_valign=true; valign=vaBaseline; }

    if (e.attribute("text-case")=="lowercase") { tcase=tcLowercase; }
    if (e.attribute("text-case")=="uppercase") { tcase=tcUppercase; }
    if (e.attribute("text-case")=="capitalize-first") { tcase=tcCapitalizeFirst; }
    if (e.attribute("text-case")=="capitalize-all") { tcase=tcCapitalizeAll; }
    if (e.attribute("text-case")=="sentence") { tcase=tcSentence; }
    if (e.attribute("text-case")=="title") { tcase=tcTitle; }

    suffix=e.attribute("suffix", suffix);
    prefix=e.attribute("prefix", prefix);
    delimiter=e.attribute("delimiter", delimiter);
}


CSLFile::CSLLabelNode::CSLLabelNode(CSLFile::CSLNode *parent, CSLFile *file):
    CSLFile::CSLNode(parent, file)
{

}

CSLFile::CSLLabelNode::~CSLLabelNode()
{

}

QString CSLFile::CSLLabelNode::produce(const QMap<QString, QVariant> &data, const CSLFile::CSLFormatState &currentFormat, OutputFormat outf)
{
    CSLFormatState newf=currentFormat;
    modifyStyle(newf);

    QString res="";
    QString dataf=getCSLField(variable, data, QString()).toString();
    bool isSingleNumber=false;
    int dat=dataf.toInt(&isSingleNumber);

    if (plural=="always") res=escapeString(outf, applyTextCase(m_file->term(variable, false, form)));
    if (plural=="never") res=escapeString(outf, applyTextCase(m_file->term(variable, true, form)));
    else res=escapeString(outf, applyTextCase(m_file->term(variable, isSingleNumber, form)));


    return newf.startFormat(currentFormat, outf)+prefix+res+suffix+newf.endFormat(currentFormat, outf);
}

void CSLFile::CSLLabelNode::parseProperties(const QDomElement &e)
{
    CSLFile::CSLNode::parseProperties(e);
    variable=e.attribute("variable");
    form=e.attribute("form");
    plural=e.attribute("plural");
}
