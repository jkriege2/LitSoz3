#include "bibtools.h"
#include <QtCore>
#include <QRegExp>
#include <iostream>


QString cleanString(const QString& text, bool cleanStrongly) {
    QString t=text.simplified();
    QString regexp="";
    t=t.remove(QRegExp(regexp+"[^\\w\\d \\_\\(\\)\\.\\/]"));
    t=t.remove(" ");
    t=t.remove('.');
    t.remove('\\');
    t.remove('/');
    QString tt;
    QString tl=t.toLatin1();
    for (int i=0; i<t.size(); i++) {
        if (t[i]=='ß') tt+="ss";
        else if (QString("äæ").contains(t[i])) tt+="ae";
        else if (QString("öœ").contains(t[i])) tt+="oe";
        else if (QString("ü").contains(t[i])) tt+="ue";
        else if (QString("ÄÆ").contains(t[i])) tt+="AE";
        else if (QString("ÖŒ").contains(t[i])) tt+="OE";
        else if (QString("Ü").contains(t[i])) tt+="UE";
        else if (QString("µ").contains(t[i])) tt+="mu";
        else if (QString("áàâãåaaa").contains(t[i])) tt+='a';
        else if (QString("éèêêëeeeee").contains(t[i])) tt+='e';
        else if (QString("íìîï").contains(t[i])) tt+='i';
        else if (QString("óòôõöø").contains(t[i])) tt+='o';
        else if (QString("úùû").contains(t[i])) tt+='u';
        else if (QString("ÄÁÀÂÃÄÅAAA").contains(t[i])) tt+='A';
        else if (QString("ÉÈÊÊËEEEEE").contains(t[i])) tt+='E';
        else if (QString("ÍÌÎÏ").contains(t[i])) tt+='I';
        else if (QString("ÓÒÔÕÖØ").contains(t[i])) tt+='O';
        else if (QString("ÚÙÛ").contains(t[i])) tt+='U';
        else if (QString("çccccc").contains(t[i])) tt+='c';
        else if (QString("ÇCCCC").contains(t[i])) tt+='C';
        else if (QString("ðdd").contains(t[i])) tt+='d';
        else if (QString("ÐDÐ").contains(t[i])) tt+='D';
        else if (QString("ñ").contains(t[i])) tt+='n';
        else if (QString("Ñ").contains(t[i])) tt+='N';
        else if (QString("ýÿý").contains(t[i])) tt+='y';
        else if (QString("Ý").contains(t[i])) tt+='Y';
        else if (QString("g").contains(t[i])) tt+='g';
        else if (QString("G").contains(t[i])) tt+='G';
        else if (QString("š").contains(t[i])) tt+='s';
        else if (QString("Š").contains(t[i])) tt+='S';
        else if (tl[i].isLetterOrNumber()) {
            if (tl[i]<128 || !cleanStrongly) tt+=tl[i];
        }
    }

    return tt;
}

void parseAuthors(const QString& authors, QStringList* givenNames, QStringList* familyNames) {
    QStringList al=authors.split(';');
    givenNames->clear();
    familyNames->clear();
    for (int i=0; i<al.size(); i++ ) {
        QStringList a=al[i].split(',');
        if (a.size()==1) { // author is just a name
            if (!a[0].trimmed().isEmpty()) {
                givenNames->append("");
                familyNames->append(a[0].trimmed());
            }
        } else if (a.size()==2) { // author is "familyname, givenname"
            if ((!a[0].trimmed().isEmpty()) || (!a[1].trimmed().isEmpty())) {
                givenNames->append(a[1].trimmed());
                familyNames->append(a[0].trimmed());
            }
        } else if (a.size()==3) { // author is "familyname, suffix, givenname" -> familyname+" "+suffix and givenName
            if ((!a[0].trimmed().isEmpty()) || (!a[1].trimmed().isEmpty()) || (!a[2].trimmed().isEmpty())) {
                givenNames->append(a[2].trimmed());
                familyNames->append(a[0].trimmed()+" "+a[1].trimmed());
            }
        }
    }
}

QString shortenGivenName(const QString& names) {
    QString result=names;
    QRegExp rx("[.,;:]");
    rx.setPatternSyntax(QRegExp::Wildcard);
    result.remove(rx);
    QStringList sl=result.split(QRegExp("\\s+"));
    result="";
    for (int i=0; i<sl.size(); i++) {
        QString s=sl[i];
        if (!s.isEmpty())
            result=result+s[0]+". ";
    }
    return result.trimmed();
}

QString formatEtalAuthorsFamilyNames(const QString& authors, int maxAuthors, QString etal, QString separator, QString lastSeparator, QString nameAddition) {
    QStringList givenNames, familyNames;
    parseAuthors(authors, &givenNames, &familyNames);
    QString result;
    int maxFor=qMin(maxAuthors, givenNames.size());
    if (maxAuthors<0) maxFor=givenNames.size();

    for (int i=0; i<maxFor; i++) {
        if (!result.isEmpty()) {
            if (i==maxFor-1) result=result+lastSeparator;
            else result=result+separator;
        }
        result=result+familyNames[i]+nameAddition;
    }
    if (givenNames.size()>maxFor) result=result+etal;
    return result;

}

QString formatEtalAuthors(const QString& authors, int maxAuthors, QString etal, bool shortGivenName, QString separator, QString lastSeparator, QString nameSeparator, QString nameAddition) {
    QStringList givenNames, familyNames;
    parseAuthors(authors, &givenNames, &familyNames);
    QString result;
    int maxFor=qMin(maxAuthors, givenNames.size());
    if (maxAuthors<0) maxFor=givenNames.size();

    for (int i=0; i<maxFor; i++) {
        if (!result.isEmpty()) {
            if (i==maxFor-1) result=result+lastSeparator;
            else result=result+separator;
        }
        if (nameSeparator.isEmpty()) {
            if (shortGivenName) result=result+shortenGivenName(givenNames[i])+" "+familyNames[i]+nameAddition;
            else result=result+givenNames[i]+" "+familyNames[i]+nameAddition;
        } else {
            if (shortGivenName) result=result+familyNames[i]+nameAddition+nameSeparator+shortenGivenName(givenNames[i]);
            else result=result+familyNames[i]+nameAddition+nameSeparator+givenNames[i];
        }
    }
    if (givenNames.size()>maxFor) result=result+etal;
    /*for (int i=0; i<qMin(maxAuthors, givenNames.size()); i++) {
        if (!result.isEmpty()) result=result+", ";
        if (shortGivenName) result=result+shortenGivenName(givenNames[i])+" "+familyNames[i];
        else result=result+givenNames[i]+" "+familyNames[i];
    }
    if (maxAuthors<givenNames.size()) result=result+" "+etal;*/
    return result;

}

QString intToLetterCode(unsigned int n) {
    if (n==0) return "";
    if (n<=26) return QString('a'+n-1);
    return QString('a'+(n/26)-1)+QString('a'+(n%26)-1);
}

QString reformatSingleName(const QString& auth, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions) {
    // split string by whitespaces
    QStringList list = auth.split(QRegExp("\\s+"));
    if (list.size()<=0) return "";
    //bool family_name=true;
    QString fname="", gname="", namea="", namep="";

    int state=0; // 0: name additions (sen, jr, ...)
                 // 1: family name
                 // 2: name prefixes (van, den, ...)
                 // 3: given names

    for (int i=list.size()-1; i>=0; i--) {
        QRegExp rx("[.,;:]");
        rx.setPatternSyntax(QRegExp::Wildcard);
        QString s=list[i];        // the list entry
        QString st=s.trimmed();   // the trimmed list entry (without whitespaces around it)
        QString lower_nodots=s;   // the list entry converted to lowercase and with '.', ',', ':', ';' removed
        lower_nodots=lower_nodots.remove(rx).trimmed().toLower();

        //int next_state=state;     // which state should we be in next

        if (state==0) {
            if (!name_additions.contains(lower_nodots)) state=1;
        } else if (state==1) {
            if (!fname.isEmpty()) { // we already found a family name, now we have to watch out for name prefixes
                if (name_prefixes.contains(lower_nodots)) state=2;
                else state=3;
            }
        } else if (state==2) {
            if (!name_prefixes.contains(lower_nodots)) state=3;
            // stay in name prefixes state until we found something that is NOT a prefix, i.e. it has to be the given name
        } else if (state==3) {
        }

        switch (state) {
            case 0: namea=st+" "+namea; break;
            case 1: fname=st+" "+fname; break;
            case 2: namep=st+" "+namep; break;
            case 3: gname=st+" "+gname; break;
        }
        //state=next_state;
    }
    QRegExp rxAuthorParan(".*\\((.*)\\)\\s*");
    if (rxAuthorParan.indexIn(gname)>=0 && !rxAuthorParan.cap(1).isEmpty()) {
        gname=rxAuthorParan.cap(1);
    }

    return QString(namep.trimmed()+((namep.isEmpty())?"":" ")+fname.trimmed()+((namea.isEmpty())?"":" ")+namea.trimmed()+", "+gname.trimmed()).simplified();
}

// private tool function that calculates the average wordcount of the texts in auth between separator1
int reformatAuthors_calcAverageWordcount(const QString& auth, QChar separator1, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions) {
    // split input by separator and trimm whitespaces
    QStringList split_by_sep=auth.split(separator1);
    for (int i=0; i<split_by_sep.size(); i++) { split_by_sep[i]=split_by_sep[i].trimmed(); }


    // count average number of words in each split item, thereby ignore name_additions and name_prefixes
    double average_wordcount=0;
    for (int i=0; i<split_by_sep.size(); i++) {
        QStringList list = split_by_sep[i].split(QRegExp("\\s+"));
        //average_wordcount += list.size();
        for (int j=0; j<list.size(); j++) {
            QString item=list[j].toLower();
            item=item.remove('.').remove(',').remove(';').remove(':').remove('(').remove(')');
            if ((!name_additions.contains(item))&&(!name_prefixes.contains(item))) average_wordcount++;
        }
    }
    //std::cout<<"            wordcount="<<average_wordcount<<"\n";
    average_wordcount/=(double)split_by_sep.size();
    //std::cout<<"            average_wordcount="<<average_wordcount<<"\n";
    //if (average_wordcount<2) sep_is_doubleuse=true;

    //std::cout<<"            sep_is_doubleuse="<<sep_is_doubleuse<<"\n";
    return average_wordcount;
}

QString reformatAuthors(const QString& authin, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions, const QList<QString>& ands, QString separatorlist) {
    QString auth=authin.simplified();
    QChar separator1=0; // first separator to appear
    QChar separator2=0; // second separator to appear
    int separators=0;   // number of different separators
    int andcount=0;     // number of "and"s in the auth string
    bool sep_is_doubleuse=false; // if this is true, we have <fname>, <gname>, <fname>, <gname>, <fname>, <gname>, ...
    double average_wordcount=0;  // count average number of words in each split item, thereby ignore name_additions and name_prefixes

    QString authlc=auth.toLower();
    for (int i=0; i<ands.size(); i++) {
        QString a=" "+ands[i].toLower()+" ";
        andcount+=authlc.count(a);
    }

    // find the separators in the names string:
    for (int i=0; i<auth.size(); i++) {
        QChar c=auth[i];
        //if ((c==',')||(c==';')||(c=='/')||(c=='\\')) { // found a separator
        if (separatorlist.contains(c)) { // found a separator
            if (separator1==0) {
                separator1=c;
                separators++;
            } else if ((c!=separator1)&&(separator2==0)) {
                separator2=c;
                separators++;
                break;
            }
        }
    }
    //std::cout<<"            separators="<<separators<<"  separator1="<<QString(separator1).toStdString()<<"  separator2="<<QString(separator2).toStdString()<<" andcount="<<andcount<<"\n";

    //int separator1count=auth.count(separator1);
    //int separator2count=auth.count(separator2);

    //std::cout<<"            separator1count="<<separator1count<<"  separator2count="<<separator2count<<"\n";

    /*
        for two separators <sep1> and <sep2> we have a format:
            <family_name><sep1><given_name> <sep2> <family_name><sep1><given_name> <sep2> ...
          -> the splitting is clear! basically we only have to replace <sep1> by ',' and <sep2> by ';'

        for one separator we have one of two possible formats:
           1. <family_name><sep1><given_name> <sep1> <family_name><sep1><given_name> <sep1> ...
              -> separate every first <sep1> by ',' and every second <sep1> by ';'
           2. <given_name> <family_name> <sep1> <given_name> <family_name>  <sep1> ...
              -> Best we can do here is: replace <sep1> by ';' and take the last word from the substrings as family name.
                 The rest is added behind the family name, separated by ','.
           Here the question arises how to distinguish between 1 and 2. This seems to be possible only by some type of heuristics:#
             - Count number of words in the single parts. in case 2 the wordcount should be >=2, as there is always family+given name.
             - Look for name additions like Jr. jun. II. III. I. sen. Sen. ...
             - Look for given name abbreviations (A. B. AB. ...)
             - compare to a database with common given names

        Finally we have to care for the case of "and", "und", "et" in the authors string.
             - If we already identified two separators then the and should be replaced by the second separator
             - If only one separator has been identified, we have to care for these cases:
                * either there is only one and then we may have two cases:
                   # <gname> <fname>, <gname> <fname> and <gname> <fname>
                     -> replace and by , and separator2='' separators=1 sep_is_doubleuse=false
                   # <fname>, <gname> and <fname>, <gname>
                     -> replace and by , and separators=1 sep_is_doubleuse=true
                   # <fname>, <gname>, <fname>, <gname>  and <fname>, <gname>
                     -> replace and by , and separator2='' separators=1 sep_is_doubleuse=true
                   -> here the last two cases may be distinguished from the first by looking whether there is a separator BEHIND the AND
                * if there are multiple ands:
                   # <fname>, <gname> and <fname>, <gname> and <fname>, <gname> and <fname>, <gname>
                     -> replace and by % and separator2=% separators=2
             - If no separator has been identified, the and's may be replaced by any "dummy" separator which is then taken as
               separator1 and sep_is_doubleuse=false
    */

    // calculate average wordcount
    if (separators==1) {
        // count average number of words in each split item, thereby ignore name_additions and name_prefixes
        average_wordcount=reformatAuthors_calcAverageWordcount(auth, separator1, name_prefixes, name_additions);
        if (average_wordcount<2) sep_is_doubleuse=true;
    }

    // treat "and"s
    if (andcount>0) {
        //std::cout<<"            TREATING ANDs:\n";

        if (separators==2) {
            for (int i=0; i<ands.size(); i++) {
                auth=auth.replace(" "+ands[i]+" ", QString(separator2), Qt::CaseInsensitive);
            }
        } else if (separators==0) {
            for (int i=0; i<ands.size(); i++) {
                auth=auth.replace(" "+ands[i]+" ", QString(";"), Qt::CaseInsensitive);
            }
            separator1=';';
            separators=1;
            sep_is_doubleuse=false;
        } else if (separators==1) {
            if (andcount>1) {
                for (int i=0; i<ands.size(); i++) {
                    auth=auth.replace(" "+ands[i]+" ", QString("%"), Qt::CaseInsensitive);
                }
                separator2='%';
                separators=2;
                sep_is_doubleuse=false;
            } else if (andcount==1) {
                int lastseppos=auth.lastIndexOf(separator1);
                int lastandpos=-1;
                //std::cout<<"              auth="<<auth.toStdString()<<"\n";
                for (int i=0; i<ands.size(); i++) {
                    int p=auth.indexOf(" "+ands[i]+" ", Qt::CaseInsensitive);
                    //std::cout<<"                found '"<<" "+ands[i].toStdString()+" "<<"' at "<<p<<"\n";
                    if (p>lastandpos) lastandpos=p;
                }
                for (int i=0; i<ands.size(); i++) {
                    auth=auth.replace(ands[i], QString(separator1), Qt::CaseInsensitive);
                }
                sep_is_doubleuse=lastseppos>lastandpos;
                //std::cout<<"            lastseppos="<<lastseppos<<"  lastandpos="<<lastandpos<<"\n";
            }
        }
    }

    QString result;

    //std::cout<<"            separators="<<separators<<"  separator1="<<QString(separator1).toStdString()<<"  separator2="<<QString(separator2).toStdString()<<" andcount="<<andcount<<"\n";
    if (separators==1) {
        //std::cout<<"            average_wordcount="<<average_wordcount<<"\n";
        //std::cout<<"            sep_is_doubleuse="<<sep_is_doubleuse<<"\n";
    }

    //int separator1count=auth.count(separator1);
    //int separator2count=auth.count(separator2);

    //std::cout<<"            separator1count="<<separator1count<<"  separator2count="<<separator2count<<"\n";

    if ( (separators==2) /*&& (!((separator1==',')&&(separator2==';'))) */ ) {
        for (int i=0; i<auth.size(); i++) {
            QChar c=auth[i];
            bool ignore_whitespace=false;
            if (c==separator1) {
                result=result.trimmed()+", ";
                ignore_whitespace=true;
            } else if (c==separator2) {
                result=result.trimmed()+"; ";
                ignore_whitespace=true;
            } else {
                if (ignore_whitespace && c.isSpace()) {
                } else {
                    result+=c;
                    ignore_whitespace=false;
                }
            }
        }
    } else if (separators==1) {
        // split input by separator and trimm whitespaces
        QStringList split_by_sep=auth.split(separator1);
        for (int i=0; i<split_by_sep.size(); i++) { split_by_sep[i]=split_by_sep[i].trimmed(); }

        if (sep_is_doubleuse) { // auth = <family_name><sep1><given_name> <sep1> <family_name><sep1><given_name> <sep1> ...
                                // i =           0                 1                    2                 3
            for (int i=0; i<split_by_sep.size(); i++) {
                if (i%2==0) { // this is a family name
                    if (!result.isEmpty()) result+="; ";
                    result+=split_by_sep[i];
                } else { // this is a given name
                    QRegExp rxAuthorParan(".*\\((.*)\\)\\s*");
                    if (rxAuthorParan.indexIn(split_by_sep[i])>=0 && !rxAuthorParan.cap(1).isEmpty()) {
                        result+=", "+rxAuthorParan.cap(1);
                    } else {
                        result+=", "+split_by_sep[i];
                    }
                }

            }
        } else { // <given_name> <family_name> <sep1> <given_name> <family_name>  <sep1> ...
            for (int i=0; i<split_by_sep.size(); i++) {
                if (!result.isEmpty()) result+="; ";
                result+=reformatSingleName(split_by_sep[i], name_prefixes, name_additions);
            }
        }
    } else {
        //We have a single name and have to extract the family name
        result=reformatSingleName(auth, name_prefixes, name_additions);
    }

    return result.simplified();
}

#define FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, item) QString item=""; if (data.contains(#item)) item=data[#item].toString();

QString formatShortReferenceSummary(const QMap<QString, QVariant>& data) {
    FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, title)
    FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, authors)
    FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, editors)
    FORMATREFERENCESUMMARY_MAP_READ_ITEM(data, year)


    QString result="";

    QString a=authors;
    if (a.isEmpty()) a=editors;
    if (a.isEmpty()) a="NA";
    QString y=year;
    if (y.isEmpty()) y="?";
    result=QString("%1 (%2): <b>%3</b>").arg(a).arg(y).arg(title);

    return result;
}


QString formatReferenceSummary(const QMap<QString, QVariant>& data) {
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


    QString result="";
    if (type=="article") {
        QString a=formatEtalAuthors(authors);
        if (authors.isEmpty()) a=formatEtalAuthors(editors);
        if (authors.isEmpty() && editors.isEmpty()) a="NA";
        QString y=year;
        if (y.isEmpty()) y="?";
        QString j=journal;
        if (j.isEmpty()) j="?";
        QString w="";
        if (!volume.isEmpty()) {
            w=w+volume;
        }
        if (!number.isEmpty()) {
            w=w+"("+number+")";
        }
        if (!pages.isEmpty()) {
            w=w+", "+pages;
        }
        result=QString("%1 (%2): <b>%3</b>, in: <i>%4</i>, %5").arg(a).arg(y).arg(title).arg(j).arg(w);
    } else if ((type=="inbook")||(type=="incollection")||(type=="inproceedings")) {
        QString a=formatEtalAuthors(authors);
        if (a.isEmpty()) a="NA";
        QString ed=formatEtalAuthors(editors);
        if (!ed.isEmpty()) ed=ed+"(eds.) , ";
        QString y=year;
        if (y.isEmpty()) y="?";
        QString bt=booktitle;
        if (bt.isEmpty()) bt="?";
        QString pub="";
        if (!publisher.isEmpty()) {
            pub=", "+publisher;
            if (!places.isEmpty()) pub=pub+", "+places;
        }
        QString pg="";
        if (!pages.isEmpty()) {
            pg=pg+", "+pages;
        }
        result=QString("%1 (%2): <b>%3</b>, in: %7<i>'%4'</i>%5%6").arg(a).arg(y).arg(title).arg(bt).arg(pub).arg(pg).arg(ed);
    } else if ((type=="thesis")) {
        QString a=formatEtalAuthors(authors);
        if (authors.isEmpty()) a=formatEtalAuthors(editors);
        if (authors.isEmpty() && editors.isEmpty()) a="NA";
        QString y=year;
        if (y.isEmpty()) y="?";
        QString org=institution;
        if (!org.isEmpty()) org=", "+org;
        QString typ=subtype;
        if (!typ.isEmpty()) typ=", "+typ;
        QString pl=places;
        if (!pl.isEmpty()) pl=", "+pl;
        result=QString("%1 (%2): <b>%3</b>%4%5%6").arg(a).arg(y).arg(title).arg(org).arg(pl).arg(typ);
    } else if ((type=="book")||(type=="anthology")||(type=="compendium")||(type=="collection")||(type=="proceedings")||(type=="textbook")) {
        QString a=formatEtalAuthors(authors);
        if (authors.isEmpty()) a=formatEtalAuthors(editors);
        if (authors.isEmpty() && editors.isEmpty()) a="NA";
        QString y=year;
        if (y.isEmpty()) y="?";
        QString ed="";
        if (!edition.isEmpty()) ed=", "+edition+".ed";
        QString pub="";
        if (!publisher.isEmpty()) {
            pub=", "+publisher;
            if (!places.isEmpty()) pub=pub+", "+places;
        }
        result=QString("%1 (%2): <b>%3</b>%4%5").arg(a).arg(y).arg(title).arg(ed).arg(pub);
    } else {
        QString a=formatEtalAuthors(authors);
        if (authors.isEmpty()) a=formatEtalAuthors(editors);
        if (authors.isEmpty() && editors.isEmpty()) a="NA";
        QString y=year;
        if (y.isEmpty()) y="?";
        result=QString("%1 (%2): <b>%3</b>").arg(a).arg(y).arg(title);
    }
    return result;
}

QString filenameToIconFile(const QString& file) {
    QString icon=":/filetypes/empty.png";
    if (!file.isEmpty()) {
        QFileInfo info(file);
        QString ext=info.suffix().toLower();

        if ((ext=="pdf")) icon=":/filetypes/pdf.png";
        else if ((ext=="ps")||(ext=="eps")) icon=":/filetypes/ps.png";
        else if ((ext=="zip")||(ext=="tar")||(ext=="gz")||(ext=="gz2")||(ext=="bz2")||(ext=="bz")||(ext=="rar")||(ext=="cab")||(ext=="7z")) icon=":/filetypes/zip.png";
        else if ((ext=="doc")||(ext=="docx")||(ext=="odf")) icon=":/filetypes/doc.png";
        else if ((ext=="txt")||(ext=="dat")) icon=":/filetypes/txt.png";
        else if ((ext=="avi")||(ext=="mpg")||(ext=="mpeg")||(ext=="ogg")) icon=":/filetypes/video.png";
        else if ((ext=="mp3")||(ext=="mid")||(ext=="midi")) icon=":/filetypes/music.png";
        else if ((ext=="tex")) icon=":/filetypes/tex.png";
        else if ((ext=="htm")||(ext=="html")||(ext=="xhtml")) icon=":/filetypes/html.png";
        else if ((ext=="png")||(ext=="jpg")||(ext=="jpeg")||(ext=="bmp")||(ext=="tif")||(ext=="tiff")||(ext=="jp2")) icon=":/filetypes/image.png";

    }
    return icon;
}




int extractYear(const QString& data) {
    QRegExp rxYear("(\\d\\d\\d\\d)");
    if (rxYear.indexIn(data)!=-1) {
        int y=rxYear.cap(1).toInt();
        if ((y>999) && (y<QDate::currentDate().year()+10)) return y;
    }
    return 0;
}

int extractNumber(const QString& data) {
    QRegExp rxYear("(\\d+)");
    if (rxYear.indexIn(data)!=-1) {
        int y=rxYear.cap(1).toInt();
    }
    return 0;
}

void addToFieldIfNotContained(QMap<QString, QVariant>& data, const QString& field, const QString& newdata, const QString& separator) {
    if (newdata.isEmpty()) return;
    QString oldval=data.value(field, "").toString();
    if (oldval.isEmpty()) {
        data[field]=newdata;
    } else if (!oldval.contains(newdata, Qt::CaseInsensitive)) {
        if (!oldval.isEmpty()) oldval=oldval+separator;
        oldval=oldval+newdata;
        data[field]=oldval;
    }
}

QString decodeLanguage(const QString& lang) {
    QString l=lang.toLower();
    if (lang=="en" || lang=="english" || lang=="englicsh" || lang=="anglais") return QString("english");
    if (lang=="de" || lang=="german" || lang=="deutsch" || lang=="allemand") return QString("german");
    if (lang=="fr" || lang=="french" || lang=="französisch" || lang=="francais") return QString("french");
    if (lang=="es" || lang=="spanish" || lang=="spanisch") return QString("spanish");
    return lang;
}
