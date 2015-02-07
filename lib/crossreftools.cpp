#include "crossreftools.h"

#include "bibtools.h"
#include "htmltools.h"
#include "latextools.h"

#include <QtXml>
#include <QtCore>

void readPerson(QMap<QString, QVariant>& record, QDomElement element, const QString& authorField=QString("authors")) {
    QString saveInField=authorField;
    QDomElement a=element;
    QString role=a.attribute("contributor_role", "author");
    QString givenName=a.firstChildElement("given_name").text();
    QString surname=a.firstChildElement("surname").text();
    QString suffix=a.firstChildElement("suffix").text();
    QString name=surname+", "+givenName;
    if (!suffix.isEmpty()) name=name+" "+suffix;

    QString oldA=record[saveInField].toString();
    if (!oldA.contains(name)) {
        if (oldA.size()>0) record[saveInField]=oldA+"; "+name;
        else record[saveInField]=name;
    }
}


void readContributors(QMap<QString, QVariant>& record, QDomElement element, const QString& authorField=QString("authors"), bool decode_roles=false, const QString& editorField=QString("editors")) {
    QString saveInField=authorField;
    QDomNodeList nContribs=element.elementsByTagName("person_name");
    for (int i=0; i<nContribs.size(); i++) {
        QDomElement a=nContribs.at(i).toElement();
        QString role=a.attribute("contributor_role", "author");
        QString givenName=a.firstChildElement("given_name").text();
        QString surname=a.firstChildElement("surname").text();
        QString suffix=a.firstChildElement("suffix").text();
        QString name=surname+", "+givenName;
        if (!suffix.isEmpty()) name=name+" "+suffix;
        if (decode_roles) {
            saveInField=authorField;
            if (role.toLower()=="editor") saveInField=editorField;
        }

        QString oldA=record[saveInField].toString();
        if (!oldA.contains(name)) {
            if (oldA.size()>0) record[saveInField]=oldA+"; "+name;
            else record[saveInField]=name;
        }
    }

}



void readTitles(QMap<QString, QVariant>& record, QDomElement element, const QString& titleField=QString("title"), bool has_subtitles=false, const QString& subtitleField=QString("subtitle")) {
    record[titleField]= removeHTML( element.firstChildElement("title").text());
    if (has_subtitles) {
        QDomNodeList nsub=element.elementsByTagName("subtitle");
        for (int i=0; i<nsub.size(); i++) {
            QDomElement a=nsub.at(i).toElement();
            if (!a.text().isEmpty()) addToFieldIfNotContained(record, subtitleField, a.text());
        }
    }
    if (record[titleField].toString().isEmpty())  record[titleField]=element.text();
}
void readPublisher(QMap<QString, QVariant>& record, QDomElement element, const QString& publisherField=QString("publisher"), const QString& placesField=QString("places")) {
    record[publisherField]=removeHTML(element.firstChildElement("publisher_name").text());
    record[placesField]=removeHTML(element.firstChildElement("publisher_place").text());
}

void readPages(QMap<QString, QVariant>& record, QDomElement element, const QString& pagesField=QString("pages")) {
    QString firstpage=element.firstChildElement("first_page").text();
    QString lastpage=element.firstChildElement("last_page").text();
    QString pages=element.text();
    if ((!firstpage.isEmpty()) && (!lastpage.isEmpty())) {
        record[pagesField]=firstpage+"-"+lastpage;
    } else if (!firstpage.isEmpty()) {
        record[pagesField]=firstpage;
    } else {
        record[pagesField]=pages;
    }
}



QMap<QString, QVariant> extractCrossrefUnixrefMetadata(const QString& data) {
    QMap<QString, QVariant> record;
    QDomDocument doc("crossref");
    QString errorMsg="";

    bool error=!(doc.setContent(data, true, &errorMsg));
    if (!error) {
        QDomElement docElem = doc.documentElement();
        if ( docElem.tagName()=="doi_records" ) {
            if (docElem.firstChildElement("doi_record").firstChildElement("crossref").firstChildElement("error").isNull()) {

                bool ok=false;

                QDomElement n = docElem.firstChildElement("doi_record").firstChildElement("crossref").firstChildElement("journal");
                if (!n.isNull()) {
                    record["type"]="article";
                    record["journal"]=n.firstChildElement("journal_metadata").firstChildElement("full_title").text();
                    if (record["journal"].toString().isEmpty()) record["journal"]=n.firstChildElement("journal_metadata").firstChildElement("abbrev_title").text();
                    record["issn"]=n.firstChildElement("journal_metadata").firstChildElement("issn").text();
                    record["year"]=n.firstChildElement("journal_issue").firstChildElement("publication_date").firstChildElement("year").text();
                    if (record["year"].toString().isEmpty()) record["year"]=n.firstChildElement("journal_article").firstChildElement("publication_date").firstChildElement("year").text();
                    record["volume"]=n.firstChildElement("journal_issue").firstChildElement("journal_volume").firstChildElement("volume").text();
                    if (record["volume"].toString().isEmpty())  record["volume"]=n.firstChildElement("journal_issue").firstChildElement("journal_volume").text();
                    record["number"]=n.firstChildElement("journal_issue").firstChildElement("issue").text();

                    readTitles(record, n.firstChildElement("journal_article").firstChildElement("titles"));

                    readContributors(record, n.firstChildElement("journal_article").firstChildElement("contributors"), "authors");

                    record["url"]=n.firstChildElement("journal_article").firstChildElement("doi_data").firstChildElement("resource").text();
                    record["doi"]=n.firstChildElement("book_metadata").firstChildElement("doi_data").firstChildElement("doi").text();

                    readPages(record, n.firstChildElement("journal_article").firstChildElement("pages"), "pages");
                    ok=true;
                }

                n = docElem.firstChildElement("doi_record").firstChildElement("crossref").firstChildElement("book");
                if (!ok && !n.isNull()) {

                    record["type"]="book";
                    QString booktype=n.attribute("book_type").toLower();
                    record["subtype"]=booktype;
                    if (booktype=="edited_book") record["type"]="anthology";
                    readContributors(record, n.firstChildElement("book_metadata").firstChildElement("contributors"), "authors", true, "editors");
                    readTitles(record, n.firstChildElement("book_metadata").firstChildElement("titles"), "title", true, "subtitle");

                    QString ed=n.firstChildElement("book_metadata").firstChildElement("edition_number").text();
                    if (!ed.isEmpty()) {
                        bool edOK=false;
                        int edn=ed.toInt(&edOK);
                        if (edOK && edn>1) {
                            record["edition"]=edn;
                        }
                    }
                    record["volume"]=n.firstChildElement("book_metadata").firstChildElement("volume").text();

                    record["year"]=n.firstChildElement("book_metadata").firstChildElement("publication_date").firstChildElement("year").text();
                    record["isbn"]=n.firstChildElement("book_metadata").firstChildElement("isbn").text();
                    record["howpublished"]=n.firstChildElement("book_metadata").firstChildElement("isbn").attribute("media_type");

                    readPublisher(record, n.firstChildElement("book_metadata").firstChildElement("publisher"));

                    record["url"]=n.firstChildElement("book_metadata").firstChildElement("doi_data").firstChildElement("resource").text();
                    record["doi"]=n.firstChildElement("book_metadata").firstChildElement("doi_data").firstChildElement("doi").text();

                    QDomElement nc=n.firstChildElement("content_item");
                    if (!nc.isNull()) {
                        QString ctype=nc.attribute("component_type");
                        if ((record["type"]=="anthology") || (record["type"]=="collection"))record["type"]="incollection";
                        else record["type"]="inbook";
                        record["subtype"]=ctype;
                        record["booktitle"]=record["title"];

                        readContributors(record, nc.firstChildElement("contributors"), "authors", true, "editors");
                        readTitles(record, nc.firstChildElement("titles"), "title", true, "subtitle");
                        readPages(record, nc.firstChildElement("pages"), "pages");
                        record["year"]=nc.firstChildElement("publication_date").firstChildElement("year").text();
                    }

                    ok=true;
                }


                // test: 10.1117/12.463677
                n = docElem.firstChildElement("doi_record").firstChildElement("crossref").firstChildElement("conference");
                if (!ok && !n.isNull()) {

                    record["type"]="proceedings";

                    if (!n.firstChildElement("proceedings_series_metadata").isNull()) {
                        readContributors(record, n.firstChildElement("proceedings_series_metadata").firstChildElement("contributors"), "authors", true, "editors");
                        record["booktitle"] = removeHTML(n.firstChildElement("proceedings_series_metadata").firstChildElement("proceedings_title").text());
                        record["series"] = removeHTML(n.firstChildElement("proceedings_series_metadata").firstChildElement("series_metadata").firstChildElement("titles").text());
                        record["issn"] = removeHTML(n.firstChildElement("proceedings_series_metadata").firstChildElement("series_metadata").firstChildElement("issn").text());
                        readPublisher(record, n.firstChildElement("proceedings_series_metadata").firstChildElement("publisher"));
                        record["year"]=n.firstChildElement("proceedings_series_metadata").firstChildElement("publication_date").firstChildElement("year").text();
                        record["volume"]=n.firstChildElement("proceedings_series_metadata").firstChildElement("volume").text();
                        record["isbn"]=n.firstChildElement("proceedings_series_metadata").firstChildElement("isbn").text();
                        record["url"]=n.firstChildElement("proceedings_series_metadata").firstChildElement("doi_data").firstChildElement("resource").text();
                        record["doi"]=n.firstChildElement("proceedings_series_metadata").firstChildElement("doi_data").firstChildElement("doi").text();
                    }
                    if (!n.firstChildElement("proceedings_metadata").isNull()) {
                        readContributors(record, n.firstChildElement("proceedings_metadata").firstChildElement("contributors"), "authors", true, "editors");
                        record["booktitle"] = removeHTML(n.firstChildElement("proceedings_metadata").firstChildElement("proceedings_title").text());
                        readPublisher(record, n.firstChildElement("proceedings_metadata").firstChildElement("publisher"));
                        record["year"]=n.firstChildElement("proceedings_metadata").firstChildElement("publication_date").firstChildElement("year").text();
                        record["isbn"]=n.firstChildElement("proceedings_metadata").firstChildElement("isbn").text();
                        record["url"]=n.firstChildElement("proceedings_metadata").firstChildElement("doi_data").firstChildElement("resource").text();
                        record["doi"]=n.firstChildElement("proceedings_metadata").firstChildElement("doi_data").firstChildElement("doi").text();
                    }


                    QDomElement nc=n.firstChildElement("conference_paper");
                    if (!nc.isNull()) {
                        record["type"]="inproceedings";

                        readContributors(record, nc.firstChildElement("contributors"), "authors", true, "editors");
                        readTitles(record, nc.firstChildElement("titles"), "title", true, "subtitle");
                        readPages(record, nc.firstChildElement("pages"), "pages");
                        record["year"]=nc.firstChildElement("publication_date").firstChildElement("year").text();
                    }

                    ok=true;
                }

                n = docElem.firstChildElement("doi_record").firstChildElement("crossref").firstChildElement("dissertation");
                if (!ok && !n.isNull()) {

                    record["type"]="thesis";
                    record["subtype"]=n.firstChildElement("degree").text();
                    if (record["subtype"].toString().isEmpty()) record["subtype"]="dissertation";

                    readPerson(record, n.firstChildElement("person_name"), "authors");
                    readTitles(record, n.firstChildElement("titles"), "title", true, "subtitle");


                    record["institution"] = n.firstChildElement("institution").firstChildElement("institution_name").text()+" "+n.firstChildElement("institution").firstChildElement("institution_department").text();
                    record["places"] = n.firstChildElement("institution").firstChildElement("institution_place").text();
                    record["year"]=n.firstChildElement("approval_date").firstChildElement("year").text();
                    record["isbn"]=n.firstChildElement("isbn").text();
                    record["url"]=n.firstChildElement("doi_data").firstChildElement("resource").text();
                    record["doi"]=n.firstChildElement("doi_data").firstChildElement("doi").text();

                    ok=true;
                }

                if (!ok){
                    qDebug()<<"\nXML ERROR: DIDN'T FIND doi_records/doi_record/crossref/...";
                }
            } else { qDebug()<<"\nXML ERROR: ERROR RETURNED"; }
        } else { qDebug()<<"\nXML ERROR: WRONG DOCUMENT ENTITY"; }
    } else {
        qDebug()<<"\nXML ERROR: "<<errorMsg;
    }

    return record;
}
