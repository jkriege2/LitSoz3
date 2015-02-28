#include "csltools.h"
#include <QFile>
#include <QDomDocument>
#include <QDebug>


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

QString CSLFile::produceHTML(const QMap<QString, QVariant>& data)
{
    QString res;
    res=QString("<small><b>%1:</b></small><br>").arg(name());
    if (m_bibliography) res=res+m_bibliography->produceHTML(data);
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
    } else if (e.tagName()=="number") {

    }

    return nodeOut;
}


CSLFile::CSLNode::CSLNode(CSLFile::CSLNode *parent, CSLFile *file)
{
    m_file=file;
    m_parent=parent;
    suffix="";
    prefix="";
    delimiter="";
    fstyle=fsNormal;
    fvariant=fvNormal;
    fweight=fwNormal;
    tdecor=tdNone;
    valign=vaBaseline;
    if (parent) {
        fstyle=parent->fstyle;
        fvariant=parent->fvariant;
        fweight=parent->fweight;
        tdecor=parent->tdecor;
        valign=parent->valign;
    }
}

CSLFile::CSLNode::~CSLNode()
{

}

QString CSLFile::CSLNode::produceHTML(const QMap<QString, QVariant> &data)
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
    if (e.attribute("font-style")=="italic") fstyle==fsItalic;
    if (e.attribute("font-style")=="oblique") fstyle==fsOblique;
    if (e.attribute("font-style")=="normal") fstyle==fsNormal;

    if (e.attribute("font-variant")=="small-caps") fvariant==fvSmallCaps;
    if (e.attribute("font-variant")=="normal") fvariant==fvNormal;

    if (e.attribute("font-weight")=="bold") fweight==fwBold;
    if (e.attribute("font-weight")=="light") fweight==fwLight;
    if (e.attribute("font-weight")=="normal") fweight==fwNormal;

    if (e.attribute("text-decoration")=="none") tdecor==tdNone;
    if (e.attribute("text-decoration")=="underline") tdecor==tdUnderline;

    if (e.attribute("vertical-align")=="sub") valign==vaSub;
    if (e.attribute("vertical-align")=="sup") valign==vaSuper;
    if (e.attribute("vertical-align")=="baseline") valign==vaBaseline;
}

QString CSLFile::CSLNode::getHTMLFormatting(const QString &text, CSLNode* m_parent)
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
        if (fstyle!=m_parent->fweight) {
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

}


CSLFile::CSLListNode::CSLListNode(CSLFile::CSLNode *parent, CSLFile *file):
    CSLFile::CSLNode(parent, file)
{

}

CSLFile::CSLListNode::~CSLListNode()
{
    clearChildren();
}

QString CSLFile::CSLListNode::produceHTML(const QMap<QString, QVariant> &data)
{
    QString res=prefix;
    for (int i=0; i<m_children.size(); i++) {
        if (i>0) res+=delimiter;
        if (m_children[i]) res+=m_children[i]->produceHTML(data);
    }
    return getHTMLFormatting(res+suffix, m_parent);
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

}

CSLFile::CSLTextNode::~CSLTextNode()
{

}

QString CSLFile::CSLTextNode::produceHTML(const QMap<QString, QVariant> &data)
{
    QString res="";
    if (!value.isEmpty()) res=value;
    else if (!variable.isEmpty()) res=getCSLField(variable, data, QString()).toString();
    else if (!macro.isEmpty()) {
        if (m_file && m_file->m_macros.contains(macro)) {
            res+=m_file->m_macros[macro]->produceHTML(data);
        }
    }

    return getHTMLFormatting(prefix+res+suffix, m_parent);
}

void CSLFile::CSLTextNode::parseProperties(const QDomElement &e)
{
    CSLFile::CSLNode::parseProperties(e);
    macro=e.attribute("macro");
    variable=e.attribute("variable");
    term=e.attribute("term");
    value=e.attribute("value");
}
