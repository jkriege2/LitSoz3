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

#include "htmltools.h"
#include "bibtools.h"
#include "htmltokenizer.h"

#include <QtCore>

QString removeHTMLComments(const QString& data) {
     QRegExp rxComments("<!--(.*)-->", Qt::CaseInsensitive);
     rxComments.setMinimal(true);
     QString data1=data;
     data1.remove(rxComments);
     return data1;
}
QString removeHTML(const QString& data) {
    QRegExp rxMarkup("<[^>]*>", Qt::CaseInsensitive);
    QRegExp rxBR("<\\s*br\\s*[\\/]\\s*>", Qt::CaseInsensitive);
     rxMarkup.setMinimal(true);
     QString data1=data;
     data1=data1.replace(rxBR, "\n");
     data1=data1.remove(rxMarkup);
     return data1;
}

void extractMetaDataWithRegExp(const QString& regex, const QString& data, QMap<QString, QVariant>& output, const QString& field, int capValue, bool minimal=true, bool append=false, const QString& separator=QString("\n"), bool removeHTML=true) {
    QRegExp rx(regex);
    rx.setMinimal(minimal);
    if (rx.indexIn(data)>=0 && !rx.cap(capValue).isEmpty()) {
        QString datacap=rx.cap(capValue);
        if (removeHTML) datacap=datacap.remove(QRegExp("<[^>]*>"));
        if (!append) {
            output[field]=datacap;
        } else if (!output[field].toString().contains(datacap)){
            QString old=output.value(field).toString();
            if (!old.isEmpty()) old=old+separator;
            old=old+datacap;
            output[field]=old;
        }
    }
}

void extractMetaDataWithRegExp(const QString& regex, const QString& data, QMap<QString, QVariant>& output, int capItem, int capValue, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions, const QList<QString>& ands) {

    QRegExp rxDCItem(regex, Qt::CaseInsensitive);

    int pos = 0;

    while ((pos = rxDCItem.indexIn(data, pos)) != -1) {
        QString dcitem=rxDCItem.cap(capItem).toLower();
        QString value=rxDCItem.cap(capValue);
        QString complete_match=rxDCItem.cap(0);

       //qDebug()<<pos<<dcitem<<value;
        if (!value.isEmpty()) {
            if ((dcitem=="dc.creator")||(dcitem=="dc.contributor")||(dcitem=="citation_author")) {
                QString oldA=output["authors"].toString();
                QString oldI=output["institution"].toString();
                QString name=reformatAuthors(value, name_prefixes, name_additions, ands);
                if (complete_match.toLower().contains("\"assignee\"")) {
                    if (!oldI.contains(name)) {
                        if (oldI.size()>0) output["institution"]=oldI+"; "+name;
                        else output["institution"]=name;
                    }
                } else {
                    if (!oldA.contains(name)) {
                        if (oldA.size()>0) output["authors"]=oldA+"; "+name;
                        else output["authors"]=name;
                    }
                }
            } else if ((dcitem=="dc.description")|(dcitem=="description")) {
                output["contents"]=output["contents"].toString()+value+"\n";
            } else if ((dcitem=="keywords")) {
                output["keywords"]=output["keywords"].toString()+value+"\n";
            } else if ((dcitem=="dc.language")) {
                QString l=value.toLower().trimmed();
                if (l=="en") output["language"]="english";
                else if (l=="de") output["language"]="german";
                else if (l=="fr") output["language"]="french";
                else output["language"]=value;
            } else if ((dcitem=="dc.publisher")||(dcitem=="citation_publisher")) {
                output["publisher"]=value;
            } else if ((dcitem=="dc.subject")) {
                output["topic"]=value;
            } else if ((dcitem=="dc.date")) {
                int y=extractYear(value);
                if (y>0) output["year"]=y;
            } else if ((dcitem=="dc.title")) {
                output["title"]=value;
            } else if ((dcitem=="citation_issn")||(dcitem=="prism.issn")) {
                output["issn"]=value;
            } else if ((dcitem=="citation_id")) {
                output["id"]=value;
            } else if ((dcitem=="citation_journal_title")||(dcitem=="prism.publicationName")) {
                output["journal"]=value;
            } else if ((dcitem=="citation_authors")) {
                output["authors"]=value;
            } else if ((dcitem=="citation_pdf_url")||(dcitem=="fulltext_pdf")) {
                output["url"]=value;
                output["pdfurl"]=value;
            } else if ((dcitem=="citation_arxiv_id")) {
                output["arxiv"]=value;
            } else if ((dcitem=="citation_pmid")) {
                output["pubmed"]=value;
            } else if ((dcitem=="citation_doi")) {
                if (value.startsWith("doi:")) value=value.remove("doi:");
                if (!value.trimmed().simplified().isEmpty()) output["doi"]=value;
            } else if ((dcitem=="citation_date")) {
                output["year"]=extractYear(value);
            } else if ((dcitem=="citation_issue")) {
                output["number"]=value;
            } else if ((dcitem=="citation_volume")||(dcitem=="prism.volume")) {
                output["volume"]=value;
            } else if ((dcitem=="citation_number")||(dcitem=="citation_issue")||(dcitem=="prism.number")) {
                output["number"]=value;
            } else if ((dcitem=="citation_firstpage") || (dcitem=="prism.startingpage")) {
                QString p=output["pages"].toString();
                if (p.isEmpty()) output["pages"]=value;
                else output["pages"]=value+"-"+p;
            } else if ((dcitem=="citation_lastpage") || (dcitem=="prism.endingpage")) {
                QString p=output["pages"].toString();
                if (p.isEmpty()) output["pages"]=value;
                else output["pages"]=p+"-"+value;
            } else if ((dcitem=="citation_conference") || (dcitem=="citation_conference_title")) {
                output["publisher"]=value;
                output["type"]="inproceedings";
            } else if ((dcitem=="citation_keyword")) {
                addToFieldIfNotContained(output, "keywords", value);
            } else if ((dcitem=="citation_dissertation_name")) {
                output["title"]=value;
                output["type"]="thesis";
                output["subtype"]="dissertation";
            } else if ((dcitem=="citation_book_title")) {
                //output["title"]=value;
                output["booktitle"]=value;
                if (output.value("type")=="article") output["type"]="book";
            } else if ((dcitem=="citation_isbn")) {
                output["isbn"]=value;
                if (output.value("type")=="article") output["type"]="book";
            } else if ((dcitem=="citation_dissertation_institution")) {
                output["institution"]=value;
                output["type"]="thesis";
                output["subtype"]="dissertation";
            } else if ((dcitem=="citation_title")) {
                //output["title"]=value;
                output["title"]=value;
            } else if ((dcitem=="citation_patent_publication_number")) {
                output["number"]=value;
                output["type"]="patent";
            } else if ((dcitem=="dc.type")) {
                if (value.toLower()=="patent") output["type"]="patent";
            //} else if ((dcitem=="")) {
            //    output[""]=value;

            }
        }

        pos += rxDCItem.matchedLength();
    }

    if (output.contains("chapter") && output.contains("booktitle")) {
        output["title"]=output["chapter"];
        output.remove("chapter");
        output["type"]="inbook";
    }
    if (output.contains("title") && output.contains("booktitle")) {
        output["type"]="inbook";
    }
}


QList<QMap<QString, QVariant> > extractCoins(const QString& data, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions, const QList<QString>& ands) {
    QList<QMap<QString, QVariant> > out;

    HtmlTokenizer tokenizer(data);
    HtmlTokenizer::Token t=tokenizer.getNextToken();
    while (t.type != HtmlTokenizer::ENDOFFILE) {
        if (((t.type==HtmlTokenizer::TAG) || (t.type==HtmlTokenizer::SINGLETAG)) && (t.tagName.toLower()=="span")) {
                t.parse();
                //qDebug()<<t.getProperty("class");
                if (t.getProperty("class")=="Z3988") {
                    QMap<QString, QVariant> coins;
                    QString coinsdata=t.getProperty("title").replace("&amp;", "&");//QUrl::fromPercentEncoding(t.getProperty("title").replace("&amp;", "&").toLatin1());
                    QStringList sl=coinsdata.split("&");
                    //qDebug()<<"\n\n\n---------------------------------------------------------------------------------------------\n"<<sl;
                    bool isBook=false;
                    bool isArticle=false;
                    QString ausuffix="", auinitm="", auinit1="", auinit="", aufirst="", aulast="";
                    for (int i=0; i<sl.size(); i++) {
                        QString item=QUrl::fromPercentEncoding(sl[i].toLatin1()).trimmed().replace('+', ' ');
                        QString value=item.mid(item.indexOf('=')+1);
                        if (item.startsWith("rft_val_fmt")) {
                            if (value=="info:ofi/fmt:kev:mtx:journal") {
                                coins["type"]="article";
                                isArticle=true;
                            } else if (value=="info:ofi/fmt:kev:mtx:book") {
                                coins["type"]="book";
                                isBook=true;
                            } else if (value=="info:ofi/fmt:kev:mtx:patent") {
                                coins["type"]="patent";
                                isBook=true;
                            } else if (value=="info:ofi/fmt:kev:mtx:dissertation") {
                                coins["type"]="thesis";
                                coins["subtype"]="dissertation";
                                isBook=true;
                            }
                        } else if (item.startsWith("rft.atitle")) {
                            coins["title"]=value;
                        } else if (item.startsWith("rft.title")) {
                            coins["journal"]=value;
                            coins["booktitle"]=value;
                        } else if (item.startsWith("rft.jtitle")) {
                            isArticle=true;
                            coins["journal"]=value;
                        } else if (item.startsWith("rft.btitle")) {
                            isArticle=false;
                            coins["booktitle"]=value;
                        } else if (item.startsWith("rft.date")) {
                            coins["published_date"]=value;
                            int y=extractYear(value);
                            if (y>0) coins["year"]=y;
                        } else if (item.startsWith("rft.volume")) {
                            coins["volume"]=value;
                        } else if (item.startsWith("rft.issue")) {
                            coins["number"]=value;
                        } else if (item.startsWith("rft.spage")) {
                            if (coins["pages"].toString().isEmpty()) coins["pages"]=value;
                            else coins["pages"]=value+"-"+coins["pages"].toString();
                        } else if (item.startsWith("rft.epage")) {
                            if (coins["pages"].toString().isEmpty()) coins["pages"]=value;
                            else coins["pages"]=coins["pages"].toString()+"-"+value;
                        } else if (item.startsWith("rft.pages")) {
                            coins["pages"]=value;
                        } else if (item.startsWith("rft.issn")) {
                            coins["issn"]=value;
                        } else if (item.startsWith("rft.artnum")) {
                            coins["article_num"]=value;
                        } else if (item.startsWith("rft.eissn")) {
                            if (coins["issn"].toString().isEmpty()) coins["issn"]=value;
                        } else if (item.startsWith("rft.isbn")) {
                            coins["isbn"]=value;
                        } else if (item.startsWith("rft.genre")) {
                            if (isArticle) {
                                if (value.contains("proceeding")) coins["type"]="inproceedings";
                                else if (value.contains("conference")) coins["type"]="inconference";
                                else coins["howpublished"]=value;
                            } else if (isBook) {
                                if (value.contains("proceeding")) coins["type"]="proceedings";
                                else if (value.contains("conference")) coins["type"]="conference";
                                else if (value.contains("bookitem")) coins["type"]="inbook";
                                else if (value.contains("report")) coins["type"]="techreport";
                                else if (value.contains("document")) coins["type"]="misc";
                                else coins["howpublished"]=value;

                            } else {
                                coins["howpublished"]=value;
                            }
                        } else if (item.startsWith("rft.place")) {
                            if (!coins["places"].toString().isEmpty()) coins["places"]=coins["places"].toString()+"\n";
                            coins["places"]=coins["places"].toString()+value;
                        } else if (item.startsWith("rft.pub")) {
                            coins["publisher"]=value;
                        } else if (item.startsWith("rft.edition")) {
                            coins["edition"]=value;
                        } else if (item.startsWith("rft.series")) {
                            coins["series"]=value;
                        } else if (item.startsWith("rft.tpages")) {
                            if (coins["pages"].toString().isEmpty()) coins["pages"]=value;
                        } else if (item.startsWith("rft.btitle")) {
                            coins["booktitle"]=value;
                        } else if (item.startsWith("rft_id")) {
                            if (value.startsWith("info:doi/")) LS3_SET_IF_NOT_EMPTY(coins["doi"],value.right(value.size()-9));
                            if (value.startsWith("info:pmid/")) LS3_SET_IF_NOT_EMPTY(coins["pubmed"],value.right(value.size()-10));
                            if (value.startsWith("info:arxiv/")) LS3_SET_IF_NOT_EMPTY(coins["arxiv"],value.right(value.size()-10));
                            if (value.startsWith("info:pmcid/")) LS3_SET_IF_NOT_EMPTY(coins["pmcid"],value.right(value.size()-10));
                            if (value.startsWith("info:http://")) LS3_SET_IF_NOT_EMPTY(coins["url"],value.right(value.size()-5));
                            if (value.startsWith("info:https://")) LS3_SET_IF_NOT_EMPTY(coins["url"],value.right(value.size()-5));
                            if (value.startsWith("info:ftp://")) LS3_SET_IF_NOT_EMPTY(coins["url"],value.right(value.size()-5));
                            if (value.startsWith("info:ftps://")) LS3_SET_IF_NOT_EMPTY(coins["url"],value.right(value.size()-5));
                            if (value.startsWith("doi/")) LS3_SET_IF_NOT_EMPTY(coins["doi"],value.right(value.size()-4));
                            if (value.startsWith("pmid/")) LS3_SET_IF_NOT_EMPTY(coins["pubmed"],value.right(value.size()-5));
                            if (value.startsWith("arxiv/")) LS3_SET_IF_NOT_EMPTY(coins["arxiv"],value.right(value.size()-5));
                            if (value.startsWith("pmcid/")) LS3_SET_IF_NOT_EMPTY(coins["pmcid"],value.right(value.size()-5));
                            if (value.startsWith("http://")) LS3_SET_IF_NOT_EMPTY(coins["url"],value);
                            if (value.startsWith("https://")) LS3_SET_IF_NOT_EMPTY(coins["url"],value);
                            if (value.startsWith("ftp://")) LS3_SET_IF_NOT_EMPTY(coins["url"],value);
                            if (value.startsWith("ftps://")) LS3_SET_IF_NOT_EMPTY(coins["url"],value);
                        } else if (item.startsWith("req_ref_fmt")) {
                            if (coins["url"].toString().isEmpty()) LS3_SET_IF_NOT_EMPTY(coins["url"],value);
                        } else if (item.startsWith("req_ref")) {
                            if (coins["url"].toString().isEmpty()) LS3_SET_IF_NOT_EMPTY(coins["url"],value);
                        } else if (item.startsWith("rft.au")) {
                            if (!coins["authors"].toString().isEmpty()) coins["authors"]=coins["authors"].toString()+"; ";
                            coins["authors"]=coins["authors"].toString()+reformatAuthors(value, name_prefixes, name_additions, ands);
                        } else if (item.startsWith("rft.aucorp")) {
                            if (!coins["authors"].toString().isEmpty()) coins["authors"]=coins["authors"].toString()+"; ";
                            coins["authors"]=coins["authors"].toString()+value;
                        } else if (item.startsWith("rft.aulast")) {
                            aulast=value;
                        } else if (item.startsWith("rft.aufirst")) {
                            aufirst=value;
                        } else if (item.startsWith("rft.auinit")) {
                            auinit=value;
                        } else if (item.startsWith("rft.auinit1")) {
                            auinit1=value;
                        } else if (item.startsWith("rft.auinitmt")) {
                            auinitm=value;
                        } else if (item.startsWith("rft.ausuffix")) {
                            ausuffix=value;
                        } else {
                            qDebug()<<"didn't interpret COINS-field: "<<item;
                        }
                    }

                    if (aulast.size()>0) {
                        QString author=aulast;
                        if (ausuffix.size()>0) author+=" "+ausuffix;
                        author+=", ";
                        if (aufirst.size()>0) author+=aufirst;
                        else {
                            if (auinit.size()>0) author+=auinit;
                            else author+=auinit1+" "+auinitm;
                        }
                        if (!coins["authors"].toString().isEmpty()) coins["authors"]=author+"; "+coins["authors"].toString();
                        else coins["authors"]=author;
                    }

                    if (coins["title"].toString().isEmpty() && !coins["booktitle"].toString().isEmpty()) {
                        coins["title"]=coins["booktitle"];
                        coins["booktitle"]="";
                    }

                    out.append(coins);
                }
        }
        t=tokenizer.getNextToken();
    }
    //QString QUrl::fromPercentEncoding ( const QByteArray & input )

    return out;
}

QMap<QString, QVariant> extractMetadataFromHTML(const QString& data, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions, const QList<QString>& ands) {
    QMap<QString, QVariant> output;
    // remove all HTML comments from input data
    QString data_no_comments=removeHTMLComments(data);
    // this regexp should extract any substring of the form <meta ... name="111" ... content="222" ...>

    output=extractHTMLMetadata(data, name_prefixes, name_additions, ands);

    // extract coins metadata from HTML and (assuming it's more reliable) replace other metadata with coins metadata
    QList<QMap<QString, QVariant> > coins=extractCoins(data, name_prefixes, name_additions, ands);
    if (coins.size()>0) {
        QMapIterator<QString, QVariant> i(coins[0]);
        while (i.hasNext()) {
            i.next();
            output[i.key()]=i.value();
        }
    }
    return output;
}


QMap<QString, QVariant> extractHTMLMetadata(const QString& data, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions, const QList<QString>& ands) {
    QMap<QString, QVariant> output;
    // remove all HTML comments from input data
    QString data_no_comments=removeHTMLComments(data);
    // try to extract <title> tag
    QRegExp rxTitle("<title>(.*)</title>", Qt::CaseInsensitive);
    if (rxTitle.indexIn(data) != -1) output["title"]=rxTitle.cap(1);


    // this regexp should extract any substring of the form <meta ... name="111" ... content="222" ...>
    extractMetaDataWithRegExp("<meta\\s*name\\s*=\\s*\"([^\"']*)\"\\s*content\\s*=\\s*\"([^\"']*)\"[^>]*>", data_no_comments, output, 1, 2, name_prefixes, name_additions, ands);
    extractMetaDataWithRegExp("<meta\\s*content\\s*=\\s*\"([^\"']*)\"\\s*name\\s*=\\s*\"([^\"']*)\"[^>]*>", data_no_comments, output, 2, 1, name_prefixes, name_additions, ands);
    extractMetaDataWithRegExp("<tr.*<td.*<abbr.*>\\s*DOI\\s*</abbr>.*<a.*>(10\\..+)</a>.*</td>.*</tr>", data_no_comments, output, "doi", 1 );
    extractMetaDataWithRegExp("PMID:\\s*<a.*href=\"/pubmed/(\\d+)\\\".*title\\s*=\\s*\"PubMed.*of this page.*>\\1</a>", data_no_comments, output, "pubmed", 1 );
    extractMetaDataWithRegExp("<a.*href\\s*=\\s*\"/abs/([^\"]+)\".*>.*arXiv:\\1.*</a>", data_no_comments, output, "arxiv", 1 );
    extractMetaDataWithRegExp("<tr.*>.*<td.*>Subjects:.*</td>.*<td.*>(.+)</td>.*</tr>", data_no_comments, output, "keywords", 1, true, true, "\n", true);
    extractMetaDataWithRegExp("<span.*class\\s*=\\s*[\'\"]Abstract[\'\"].*>.*<p.*>(.+)</p>.*</span>", data_no_comments, output, "abstract", 1);
    extractMetaDataWithRegExp("<div.*class\\s*=\\s*[\'\"](abst|abstract|aps-abstractbox|section abstract)[\'\"].*>.*<[Pp].*>(.+)</[Pp]>.*</div>", data_no_comments, output, "abstract", 1);
    extractMetaDataWithRegExp("<span.*class\\s*=\\s*[\'\"]pb_abstract[\'\"].*><span.*>.*</span>.*(.+)</span>", data_no_comments, output, "abstract", 1);



    return output;
}



QByteArray guessEncoding(const QByteArray& encodingFromReply, const QByteArray& data) {
    if (encodingFromReply.size()>0 && encodingFromReply.toLower()!="gzip") {
        if (QTextCodec::codecForName(encodingFromReply)) return QTextCodec::codecForName(encodingFromReply)->name();
    }
    // test for XML contents:
    QRegExp rxXML("<?xml[^>]*encoding=\"([^\"]*)\"[^>]*\?>");
    rxXML.setCaseSensitivity(Qt::CaseInsensitive);
    QByteArray encoding="";
    if (rxXML.indexIn(QString(data))>=0) {
        QString en=rxXML.cap(1);
        QTextCodec* cod=QTextCodec::codecForName(en.toLocal8Bit());
        if (cod) {
            encoding=cod->name();
        }
    }
    // no try and guess HTML encoding
    if (encoding.isEmpty()){
        QTextCodec* cod=QTextCodec::codecForHtml(data, NULL);
        if (cod) {
            encoding=cod->name();
        }
    }

    if (encoding.isEmpty()){
        encoding="UTF-8";
    }

    return encoding;
}



