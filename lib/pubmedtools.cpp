#include "pubmedtools.h"

#include "bibtools.h"

#include <QtXml>
#include <QtCore>

QMap<QString, QVariant> extractPubmedMetadata(const QString& data) {
    QMap<QString, QVariant> record;
    QDomDocument doc("crossref");
    //qDebug()<<"parsing: \n"<<data;

    bool error=!(doc.setContent(data, true));
    if (!error) {
        QDomElement docElem = doc.documentElement();
        if ( docElem.tagName()=="PubmedArticleSet" ) {

                QDomElement n = docElem.firstChildElement("PubmedArticle").firstChildElement("MedlineCitation").firstChildElement("Article");
                if (!n.isNull()) {
                    record["journal"]=n.firstChildElement("Journal").firstChildElement("Title").text();
                    if (record["journal"].toString().isEmpty()) record["journal"]=n.firstChildElement("Journal").firstChildElement("ISOAbbreviation").text();
                    record["issn"]=n.firstChildElement("Journal").firstChildElement("ISSN").text();
                    record["year"]=n.firstChildElement("Journal").firstChildElement("JournalIssue").firstChildElement("PubDate").firstChildElement("Year").text();
                    if (record["year"].toString().isEmpty()) record["year"]=n.firstChildElement("Journal").firstChildElement("JournalIssue").firstChildElement("PubDate").text();
                    record["volume"]=n.firstChildElement("Journal").firstChildElement("JournalIssue").firstChildElement("Volume").text();
                    record["number"]=n.firstChildElement("Journal").firstChildElement("JournalIssue").firstChildElement("Issue").text();

                    record["title"]=n.firstChildElement("ArticleTitle").text();
                    record["abstract"]=n.firstChildElement("Abstract").text();
                    record["pages"]=n.firstChildElement("Pagination").text();
                    record["language"]=n.firstChildElement("Language").text();
                    if (n.firstChildElement("Language").text().toLower().trimmed()=="eng") record["language"]="english";
                    if (n.firstChildElement("Language").text().toLower().trimmed()=="ger") record["language"]="german";
                    if (n.firstChildElement("Language").text().toLower().trimmed()=="fr") record["language"]="french";
                    if (n.firstChildElement("Language").text().toLower().trimmed()=="de") record["language"]="german";

                    QString pubtype=n.firstChildElement("PublicationTypeList").firstChildElement("PublicationType").text();
                    if (pubtype=="Academic Dissertations")  record["type"]="thesis";
                    else if (pubtype=="Almanacs")  record["type"]="compendium";
                    else if (pubtype=="Autobiography") { addToFieldIfNotContained(record, "comments", pubtype, "\n\n"); }
                    else if (pubtype=="Biography") { addToFieldIfNotContained(record, "comments", pubtype, "\n\n"); }
                    else if (pubtype=="Atlases")  record["type"]="compendium";
                    else if (pubtype=="Dictionary")  record["type"]="compendium";
                    else if (pubtype=="Encyclopedias")  record["type"]="compendium";
                    else if (pubtype=="Annual Reports")  record["type"]="techreport";
                    else if (pubtype=="Technical Report")  record["type"]="techreport";
                    else if (pubtype=="Collections")  record["type"]="anthology";
                    else if (pubtype=="Guidebooks")  record["type"]="book";
                    else if (pubtype=="Handbooks")  record["type"]="compendium";
                    else if (pubtype=="Monograph")  record["type"]="book";
                    else if (pubtype=="Posters")  record["type"]="poster";
                    else if (pubtype=="Textbooks")  record["type"]="textbook";
                    else if (pubtype=="Unpublished Works")  record["type"]="unpublished";
                    else if (pubtype=="Case Report")  {
                        record["articletype"]="case report";
                        addToFieldIfNotContained(record, "keywords", "case report");
                    } else if (pubtype=="Review")  record["articletype"]="review";
                    else if (pubtype=="In Vitro")  {
                        addToFieldIfNotContained(record, "keywords", "in vitro study");
                    } else if (pubtype.startsWith("Research Support")) {
                        addToFieldIfNotContained(record, "comments", pubtype, "\n\n");
                    } else if (pubtype.startsWith("Clinical Trial")) {
                        addToFieldIfNotContained(record, "comments", pubtype, "\n\n");
                    } else {
                        addToFieldIfNotContained(record, "comments", QObject::tr("Publication Type: %1").arg(pubtype), "\n\n");
                    }



                    QDomElement nd=n.firstChildElement("ELocationID");
                    while (!nd.isNull()) {
                        QString EIdType=nd.attribute("EIdType", "");
                        QString ValidYN=nd.attribute("ValidYN", "Y").toUpper();
                        QString doitxt=nd.text();
                        if (EIdType=="doi" && ValidYN=="Y" && !doitxt.isEmpty() && !record.contains("doi")) record["doi"]=doitxt;
                        if (EIdType=="pmc" && ValidYN=="Y" && !doitxt.isEmpty() && !record.contains("pmcid")) record["pmcid"]=doitxt;
                        nd=nd.nextSiblingElement("ELocationID");
                    }

                    QDomNodeList nContribs=n.firstChildElement("AuthorList").elementsByTagName("Author");
                    for (int i=0; i<nContribs.size(); i++) {
                        QDomElement a=nContribs.at(i).toElement();
                        QString givenName=a.firstChildElement("ForeName").text();
                        QString surname=a.firstChildElement("LastName").text();
                        QString initials=a.firstChildElement("Initials").text();
                        QString name=surname+", "+givenName;
                        if (givenName.isEmpty() && (!initials.isEmpty())) name=surname+", "+initials;
                        else name=surname+", "+givenName;
                        QString oldA=record["authors"].toString();
                        if (!oldA.contains(name)) {
                            if (oldA.size()>0) record["authors"]=oldA+"; "+name;
                            else record["authors"]=name;
                        }
                    }
                }

                n = docElem.firstChildElement("PubmedArticle").firstChildElement("MedlineCitation");
                if (!n.isNull()) {
                    QDomNodeList nMesh=n.firstChildElement("MeshHeadingList").elementsByTagName("MeshHeading");
                    for (int i=0; i<nMesh.size(); i++) {
                        QDomElement a=nMesh.at(i).toElement();
                        QString descriptor=a.firstChildElement("DescriptorName").text();
                        QString qualifier=a.firstChildElement("QualifierName").text();
                        QString keyword=descriptor;
                        if (!qualifier.isEmpty()) keyword=keyword+"/"+qualifier;
                        a=a.nextSiblingElement("QualifierName");
                        while (!a.isNull()) {
                            qualifier=a.firstChildElement("QualifierName").text();
                            if (!qualifier.isEmpty()) keyword="\n"+descriptor+"/"+qualifier;
                            a=a.nextSiblingElement("QualifierName");
                        }

                        QString oldA=record["keywords"].toString();
                        if (!oldA.contains(keyword)) {
                            if (oldA.size()>0) record["keywords"]=oldA+"\n"+keyword;
                            else record["keywords"]=keyword;
                        }
                    }

                    QDomNodeList nChemicals=n.firstChildElement("ChemicalList").elementsByTagName("Chemical");
                    for (int i=0; i<nChemicals.size(); i++) {
                        QDomElement a=nChemicals.at(i).toElement();
                        QString regnum=a.firstChildElement("RegistryNumber").text();
                        QString name=a.firstChildElement("NameOfSubstance").text();
                        QString chem=name;
                        if ((!regnum.isEmpty()) && (regnum!="0")) chem=chem+" ("+regnum+")";

                        QString oldA=record["keywords"].toString();
                        if (!oldA.contains(chem)) {
                            if (oldA.size()>0) record["keywords"]=oldA+"\n"+chem;
                            else record["keywords"]=chem;
                        }
                    }

                    QDomNodeList nOtherID=n.elementsByTagName("OtherID");
                    for (int i=0; i<nOtherID.size(); i++) {
                        QDomElement a=nOtherID.at(i).toElement();
                        QString source=a.attribute("Source");
                        QString id=a.text();
                        if (source=="NLM" && id.startsWith("PMC") && !record.value("pmcid").toString().isEmpty()) {
                            record["pmcid"]=id;
                        }
                    }

                    if (!n.firstChildElement("PMID").isNull() && !n.firstChildElement("PMID").text().isEmpty() && !record.value("pubmed").toString().isEmpty()) {
                        if (n.firstChildElement("PMID").text().startsWith("PMC")) record["pmcid"]=n.firstChildElement("PMID").text();
                        else record["pubmed"]=n.firstChildElement("PMID").text();
                    }

                }

                n = docElem.firstChildElement("PubmedArticle").firstChildElement("PubmedData");
                if (!n.isNull()) {
                    QDomNodeList nID=n.firstChildElement("ArticleIdList").elementsByTagName("ArticleId");
                    for (int i=0; i<nID.size(); i++) {
                        QDomElement a=nID.at(i).toElement();
                        QString type=a.attribute("IdType");
                        QString id=a.text();
                        //qDebug()<<type<<id;

                        if (type.toLower()=="doi") record["doi"]=id;
                        if (type.toLower()=="pubmed") record["pubmed"]=id;
                        if (type.toLower()=="pmc") record["pmcid"]=id;
                    }
                }
        }
    }

    return record;

}


QList<QString> extractPMIDList(const QString& data) {
    QList<QString> res;
    QDomDocument doc("esearch");

    bool error=!(doc.setContent(data, true));
    if (!error) {
        QDomElement docElem = doc.documentElement();
        if ( docElem.tagName()=="eSearchResult" ) {
            QDomElement n = docElem.firstChildElement("IdList").firstChildElement("Id");
            while (!n.isNull()) {
                res.append(n.text());
                n=n.nextSiblingElement("Id");
            }
        }
    }

    return res;
}


 QString extractPubmedBookIDList(const QString& data) {
     QString res;
     QDomDocument doc("esearch");

     //qDebug()<<"extractPubmedBookIDList: input: "<<data;

     bool error=!(doc.setContent(data, true));
     if (!error) {
         QDomElement docElem = doc.documentElement();
         //qDebug()<<"extractPubmedBookIDList: docElem: "<<docElem.tagName();
         if ( docElem.tagName()=="eSummaryResult" ) {
             QDomElement n = docElem.firstChildElement("DocSum").firstChildElement("Item");
             //qDebug()<<"extractPubmedBookIDList: docElem.Item: "<<n.tagName();
             while (!n.isNull()) {
                 //qDebug()<<"extractPubmedBookIDList: docElem.Item >>> Name="<<n.attribute("Name")<<" txt="<<n.text();
                 //<Item Name="RID" Type="String">NBK143561</Item>
                 if (n.attribute("Name").toUpper()=="RID") {
                     QString id=n.text();
                     int idxSlash=id.indexOf('/');
                     if (idxSlash>0) {
                         id=id.left(idxSlash);
                     }
                     if (!id.isEmpty()) return id;
                 }
                 n=n.nextSiblingElement("Item");
             }
         }
     }

     return res;

}

void extractPubmedIDConvertIDs(QMap<QString, QVariant>& record, const QString& data) {
    QDomDocument doc("esearch");

    bool error=!(doc.setContent(data, true));
    if (!error) {
        QDomElement docElem = doc.documentElement();
        if ( docElem.tagName()=="pmcids" && docElem.attribute("status").toLower()=="ok" ) {
            QDomElement n = docElem.firstChildElement("record");
            if (!n.isNull()) {
                QString pmcid=n.attribute("pmcid");
                QString pmid=n.attribute("pmid");
                QString doi=n.attribute("doi");
                if (!pmcid.isEmpty()) record["pmcid"]=pmcid;
                if (!pmid.isEmpty()) record["pubmed"]=pmid;
                if (!doi.isEmpty()) record["doi"]=doi;
            }
        }
    }
}
