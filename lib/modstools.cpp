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

#include "modstools.h"
#include "bibtools.h"

#include <QtXml>
#include <QtCore>

QMap<QString, QVariant> extractMODSXMLMetadata(const QString& data, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions, const QList<QString>& ands) {
    QMap<QString, QVariant> record;
    QDomDocument doc("modsxml");
    bool error=!(doc.setContent(data, true));
    if (!error) {
        QDomElement docElem = doc.documentElement();
        if ( docElem.tagName()=="modsCollection" ) {
            QDomElement n=docElem.firstChildElement("mods");
            if (!n.isNull()) {
                record["type"]="book";
                QDomElement nt=n.firstChildElement("titleInfo");
                record["title"]=nt.firstChildElement("title").text();
                QDomNodeList nlst=nt.elementsByTagName("subTitle");
                for (int i=0; i<nlst.size(); i++) {
                    if (!nlst.at(i).toElement().text().isEmpty()) addToFieldIfNotContained(record, "subtitle", nlst.at(i).toElement().text());
                }

                QString author;
                QDomNodeList nla=n.elementsByTagName("name");
                for (int i=0; i<nla.size(); i++) {
                    QDomElement a=nla.at(i).toElement();
                    if (!author.isEmpty()) author=author+"; ";
                    author+=a.firstChildElement("namePart").text();
                }
                record["authors"]=reformatAuthors(author, name_prefixes, name_additions, ands);
                record["language"]=nt.firstChildElement("language").firstChildElement("languageTerm").text().toLower();
                record["isbn"]=nt.firstChildElement("language").firstChildElement("languageTerm").text().toLower();
                QDomNodeList nli=n.elementsByTagName("identifier");
                for (int i=0; i<nli.size(); i++) {
                    QDomElement a=nli.at(i).toElement();
                    QString t=a.attribute("type").toLower().trimmed();
                    if (t=="isbn") record["isbn"]=a.text();
                    if (t=="doi" && !a.text().simplified().trimmed().isEmpty()) record["doi"]=a.text();
                    if (t=="issn") record["issn"]=a.text();
                    if (t=="issue number") record["volume"]=a.text();
                }
                addToFieldIfNotContained(record, "abstract", nt.firstChildElement("abstract").text());
                addToFieldIfNotContained(record, "abstract", QString("\n\n\ntable of contents:\n")+nt.firstChildElement("tableOfContents").text());
                addToFieldIfNotContained(record, "notes", nt.firstChildElement("note").text());
                QDomNodeList nlo=n.elementsByTagName("originInfo");
                for (int i=0; i<nlo.size(); i++) {
                    QDomElement a=nlo.at(i).toElement();
                    QDomNodeList nlp=a.elementsByTagName("place");
                    for (int j=0; j<nlp.size(); j++) {
                        QDomElement aa=nlp.at(j).toElement();
                        addToFieldIfNotContained(record, "places", aa.text());
                    }
                    record["publisher"]=a.firstChildElement("publisher").text();
                    record["year"]=extractYear(a.firstChildElement("dateIssued").text());
                    record["edition"]=a.firstChildElement("edition").text();
                }

            }
        }
    }

    return record;

}
