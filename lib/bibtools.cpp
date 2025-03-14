﻿/*
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

#include "bibtools.h"
#include <QtCore>
#include <QRegularExpression>
#include <QtGlobal>
#include <QTextDocument>
#include "ls3tools.h"
#include "regextools.h"

QString escapeHTMLString(const QString& input) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return input.toHtmlEscaped();
#else
    return Qt::escape(input);
#endif
}

QString cleanStringForFilename(const QString& text, bool cleanStrongly, bool removeSlashBackslash) {
    QString t=text.simplified();
    QString regexp="";
    t=t.remove(QRegularExpression(regexp+"[^\\w\\d \\_\\(\\)\\.\\/]"));
    t=t.remove(" ");
    t=t.remove('.');
    if (removeSlashBackslash) {
        t.remove('\\');
        t.remove('/');
    }
    QString tt;
    QString tl=t.toLatin1();
    for (int i=0; i<t.size(); i++) {
        if (QString::fromLatin1("ß").contains(t[i])) tt+="ss";
        else if (QString::fromLatin1("äæ").contains(t[i])) tt+="ae";
        else if (QString::fromLatin1("öœ").contains(t[i])) tt+="oe";
        else if (QString::fromLatin1("ü").contains(t[i])) tt+="ue";
        else if (QString::fromLatin1("ÄÆ").contains(t[i])) tt+="AE";
        else if (QString::fromLatin1("ÖŒ").contains(t[i])) tt+="OE";
        else if (QString::fromLatin1("Ü").contains(t[i])) tt+="UE";
        else if (QString::fromLatin1("µ").contains(t[i])) tt+="mu";
        else if (QString::fromLatin1("áàâãåaaa").contains(t[i])) tt+='a';
        else if (QString::fromLatin1("éèêêëeeeee").contains(t[i])) tt+='e';
        else if (QString::fromLatin1("íìîï").contains(t[i])) tt+='i';
        else if (QString::fromLatin1("óòôõöø").contains(t[i])) tt+='o';
        else if (QString::fromLatin1("úùû").contains(t[i])) tt+='u';
        else if (QString::fromLatin1("ÄÁÀÂÃÄÅAAA").contains(t[i])) tt+='A';
        else if (QString::fromLatin1("ÉÈÊÊËEEEEE").contains(t[i])) tt+='E';
        else if (QString::fromLatin1("ÍÌÎÏ").contains(t[i])) tt+='I';
        else if (QString::fromLatin1("ÓÒÔÕÖØ").contains(t[i])) tt+='O';
        else if (QString::fromLatin1("ÚÙÛ").contains(t[i])) tt+='U';
        else if (QString::fromLatin1("çccccc").contains(t[i])) tt+='c';
        else if (QString::fromLatin1("ÇCCCC").contains(t[i])) tt+='C';
        else if (QString::fromLatin1("ðdd").contains(t[i])) tt+='d';
        else if (QString::fromLatin1("ÐDÐ").contains(t[i])) tt+='D';
        else if (QString::fromLatin1("ñ").contains(t[i])) tt+='n';
        else if (QString::fromLatin1("Ñ").contains(t[i])) tt+='N';
        else if (QString::fromLatin1("ýÿý").contains(t[i])) tt+='y';
        else if (QString::fromLatin1("Ý").contains(t[i])) tt+='Y';
        else if (QString::fromLatin1("g").contains(t[i])) tt+='g';
        else if (QString::fromLatin1("G").contains(t[i])) tt+='G';
        else if (QString::fromLatin1("š").contains(t[i])) tt+='s';
        else if (QString::fromLatin1("Š").contains(t[i])) tt+='S';
        else if (tl[i].isLetterOrNumber()) {
            if (tl[i].unicode()<128 || !cleanStrongly) tt+=tl[i];
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

QString shortenGivenName(const QString& names, const QString& shortenWith) {
    QString result=names;
    result.removeIf([](QChar c) { static QString remstr(".,;:-"); return remstr.contains(c);});
    QStringList sl=result.split(QRegularExpression("\\s+"));
    result="";
    for (int i=0; i<sl.size(); i++) {
        QString s=sl[i];
        if (!s.isEmpty())
            result=result+s[0]+shortenWith;
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
    if (n<=26) return QString(QChar(int('a')+n-1));
    return QString(QChar(int('a')+(n/26)-1))+QString(QChar(int('a')+(n%26)-1));
}

QString reformatSingleName(const QString& auth, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions) {
    // split string by whitespaces
    QStringList list = auth.split(QRegularExpression("\\s+"));
    if (list.size()<=0) return "";
    //bool family_name=true;
    QString fname="", gname="", namea="", namep="";

    int state=0; // 0: name additions (sen, jr, ...)
                 // 1: family name
                 // 2: name prefixes (van, den, ...)
                 // 3: given names

    for (int i=list.size()-1; i>=0; i--) {
        QString s=list[i];        // the list entry
        QString st=s.trimmed();   // the trimmed list entry (without whitespaces around it)
        QString lower_nodots=s;   // the list entry converted to lowercase and with '.', ',', ':', ';' removed
        lower_nodots=lower_nodots.removeIf([](QChar c) { static QString remstr(".,;:"); return remstr.contains(c);}).trimmed().toLower();

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
    static QRegularExpression rxAuthorParan(".*\\((.*)\\)\\s*");
    QRegularExpressionMatch rxAuthorParanMatch = rxAuthorParan.match(gname);

    if (rxAuthorParanMatch.hasMatch() && !rxAuthorParanMatch.captured(1).isEmpty()) {
        gname=rxAuthorParanMatch.captured(1);
    }

    bool allfnameupper=true;
    QString newFName="";
    for (int i=0; i<fname.size(); i++) {
        QChar c=fname[i];
        if (c.isLetter()){
            allfnameupper=allfnameupper&&c.isUpper();
            if (c.isUpper()) {
                if (i>0 && fname[i-1].isLetter()) c=c.toLower();
            }
        }
        newFName+=c;
    }
    if (allfnameupper) fname=newFName;

    QString res= QString(namep.trimmed()+((namep.isEmpty())?"":" ")+fname.trimmed()+((namea.isEmpty())?"":" ")+namea.trimmed()+", "+gname.trimmed()).simplified();
    if (namep.trimmed().simplified().isEmpty() && fname.trimmed().simplified().isEmpty() && namea.trimmed().simplified().isEmpty() && gname.trimmed().simplified().isEmpty()) {
        return QString();
    }
    return res;
}

// private tool function that calculates the average wordcount of the texts in auth between separator1
int reformatAuthors_calcAverageWordcount(const QString& auth, QChar separator1, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions) {
    // split input by separator and trimm whitespaces
    QStringList split_by_sep=auth.split(separator1);
    for (int i=0; i<split_by_sep.size(); i++) { split_by_sep[i]=split_by_sep[i].trimmed(); }


    // count average number of words in each split item, thereby ignore name_additions and name_prefixes
    double average_wordcount=0;
    for (int i=0; i<split_by_sep.size(); i++) {
        QStringList list = split_by_sep[i].split(QRegularExpression("\\s+"));
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
    QString auth=authin.simplified().trimmed();
    QChar separator1='\0'; // first separator to appear
    QChar separator2='\0'; // second separator to appear
    int separators=0;   // number of different separators
    int andcount=0;     // number of "and"s in the auth string
    bool sep_is_doubleuse=false; // if this is true, we have <fname>, <gname>, <fname>, <gname>, <fname>, <gname>, ...
    double average_wordcount=0;  // count average number of words in each split item, thereby ignore name_additions and name_prefixes

    //qDebug()<<auth;
    static const QRegularExpression rxAuthorNumItem(QLatin1String("[\\,\\;\\s\\w]{1}[\\d\\#\\§\\$\\+\\~\\*\\?\\%\\!\\²\\³\\@]{1}[\\s\\,\\;]{1}"), QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch rmatch;
    int pos=0;
    while ((pos=auth.indexOf(rxAuthorNumItem, pos, &rmatch))>=0) {
        QString rep=" ";
        if (rmatch.captured(0).contains(',')) rep=", ";
        if (rmatch.captured(0).contains(';')) rep="; ";
        if (rmatch.captured(0).contains('/')) rep="/";
        if (rmatch.captured(0).contains('\\')) rep="\\";
        //qDebug()<<"  "<<pos<<rxAuthorNumItem.cap(0)<<rep;
        auth=auth.replace(rmatch.capturedStart(0), rmatch.capturedLength(0), rep);
        pos=pos+rep.size();//rxAuthorNumItem.matchedLength();
    }
    while (auth.size()>0 && QString::fromLatin1("0123456789*+~#!§$%/?,;-_²³@").contains(auth[auth.size()-1])) {
        auth=auth.remove(auth.size()-1, 1);
    }
    auth=auth.simplified();

    //qDebug()<<auth;

    QString authlc=auth.toLower();
    static const QString rxSAndTemplate="[\\s\\,\\;\\/\\§\\$\\/\\)\\]\\}\\?\\+\\*\\~\\#\\-\\.\\:\\|\\<\\>]{1}%1[\\s\\,\\;\\/\\§\\$\\/\\)\\]\\}\\?\\+\\*\\~\\#\\-\\.\\:\\|\\<\\>]{1}";
    for (int i=0; i<ands.size(); i++) {
        QRegularExpression rxAnd(rxSAndTemplate.arg(ands[i]), QRegularExpression::CaseInsensitiveOption);
        //QString a=" "+ands[i].toLower()+" ";
        //andcount+=authlc.count(a);
        andcount+=authlc.count(rxAnd);
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
    //qDebug()<<"            authin="<<authin;
    //if (separator2!=0) qDebug()<<"            separators="<<separators<<"  separator1="<<separator1<<"  separator2="<<separator2<<" andcount="<<andcount;
    //else qDebug()<<"            separators="<<separators<<"  separator1="<<separator1<<"  separator2="<<0<<" andcount="<<andcount;

    //int separator1count=auth.count(separator1);
    //int separator2count=auth.count(separator2);

    //qDebug()<<"            separator1count="<<separator1count<<"  separator2count="<<separator2count;

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
        //qDebug()<<"            TREATING ANDs:\n";

        if (separators==2) {
            for (int i=0; i<ands.size(); i++) {
                QRegularExpression rxAnd(rxSAndTemplate.arg(ands[i]), QRegularExpression::CaseInsensitiveOption);
                auth=auth.replace(rxAnd, QString(separator2));
                //auth=auth.replace(" "+ands[i]+" ", QString(separator2), Qt::CaseInsensitive);
            }
        } else if (separators==0) {
            for (int i=0; i<ands.size(); i++) {
                QRegularExpression rxAnd(rxSAndTemplate.arg(ands[i]), QRegularExpression::CaseInsensitiveOption);
                auth=auth.replace(rxAnd, QString(";"));
                //auth=auth.replace(" "+ands[i]+" ", QString(";"), Qt::CaseInsensitive);
            }
            separator1=';';
            separators=1;
            sep_is_doubleuse=false;
        } else if (separators==1) {
            if (andcount>1) {
                for (int i=0; i<ands.size(); i++) {
                    QRegularExpression rxAnd(rxSAndTemplate.arg(ands[i]), QRegularExpression::CaseInsensitiveOption);
                    auth=auth.replace(rxAnd, QString("%"));
                    //auth=auth.replace(" "+ands[i]+" ", QString("%"), Qt::CaseInsensitive);
                }
                separator2='%';
                separators=2;
                sep_is_doubleuse=false;
            } else if (andcount==1) {
                int lastseppos=auth.lastIndexOf(separator1);
                int lastandpos=-1;
                //qDebug()<<"              auth="<<auth;
                for (int i=0; i<ands.size(); i++) {
                    QRegularExpression rxAnd(rxSAndTemplate.arg(ands[i]), QRegularExpression::CaseInsensitiveOption);
                    int p=auth.indexOf(rxAnd);
                    //int p=auth.indexOf(" "+ands[i]+" ", Qt::CaseInsensitive);
                    //qDebug()<<"                found '"<<" "+ands[i]+" "<<"' at "<<p;
                    if (p>lastandpos) lastandpos=p;
                }
                for (int i=0; i<ands.size(); i++) {
                    QRegularExpression rxAnd(rxSAndTemplate.arg(ands[i]));
                    auth=auth.replace(rxAnd, QString(separator1));
                    //auth=auth.replace(ands[i], QString(separator1), QRegularExpression::CaseInsensitiveOption);
                }
                sep_is_doubleuse=lastseppos>lastandpos;
                //qDebug()<<"            lastseppos="<<lastseppos<<"  lastandpos="<<lastandpos;
            }
        }
    }

    QString result;

    //if (separator2!=0) qDebug()<<"            separators="<<separators<<"  separator1="<<separator1<<"  separator2="<<separator2<<" andcount="<<andcount;
    //else qDebug()<<"            separators="<<separators<<"  separator1="<<separator1<<"  separator2="<<0<<" andcount="<<andcount;
    if (separators==1) {
        //qDebug()<<"            average_wordcount="<<average_wordcount;
        //qDebug()<<"            sep_is_doubleuse="<<sep_is_doubleuse;
    }

    //int separator1count=auth.count(separator1);
    //int separator2count=auth.count(separator2);

    //qDebug()<<"            separator1count="<<separator1count<<"  separator2count="<<separator2count;

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
                    static const QRegularExpression rxAuthorParan(".*\\((.*)\\)\\s*");
                    QRegularExpressionMatch rmatch;
                    if (split_by_sep[i].indexOf(rxAuthorParan)>=0) {
                        if (!rmatch.captured(1).isEmpty()) {
                            result+=", "+rmatch.captured(1);
                        }
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

    QString res=result.simplified();
    res.replace("; ,;", ";");
    res.replace(";,;", ";");
    res.replace(";, ;", ";");
    res.replace("; ;", ";");
    res.replace(";;", ";");
    return res.simplified();
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
    QStringList caps;
    if (ls3_rxContains(data, "(\\d\\d\\d\\d)", 0, &caps)) {
        if (caps.size()>1) {
            const int y=caps[1].toInt();
            if ((y>999) && (y<QDate::currentDate().year()+10)) return y;
        }
    }
    return 0;
}

int extractNumber(const QString& data) {
    QStringList caps;
    if (ls3_rxContains(data, "(\\d+)", 0, &caps)) {
        if (caps.size()>1) {
            const int y=caps[1].toInt();
            if ((y>999) && (y<QDate::currentDate().year()+10)) return y;
        }
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

QString decodeLanguage(const QString& l) {
    const QString lang=l.toLower();
    if (lang=="en" || lang=="english" || lang=="englicsh" || lang=="anglais") return QString("english");
    if (lang=="de" || lang=="german" || lang=="deutsch" || lang=="allemand") return QString("german");
    if (lang=="fr" || lang=="french" || lang=="französisch" || lang=="francais") return QString("french");
    if (lang=="es" || lang=="spanish" || lang=="spanisch") return QString("spanish");
    return lang;
}

QString cleanText(const QString& data, bool simplify, const QString& specialChars) {
    QString s=data.trimmed();
    if (simplify) s=s.simplified().trimmed();
    while (s.size()>0 && (specialChars.contains(s[0]) || s[0].isSpace())) {
        s=s.remove(0,1);
    }
    //qDebug()<<"s="<<s;
    while (s.size()>0 && (specialChars.contains(s[s.size()-1]) || s[s.size()-1].isSpace())) {
        s=s.remove(s.size()-1,1);
    }

    if (simplify) s=s.simplified();
    return s.trimmed();

}

void countLetterCase(const QString& string, int &upperCase, int &lowerCase)
{
    upperCase=0;
    lowerCase=0;
    for (int i=0; i<string.size(); i++) {
        if (string[i].isUpper()) upperCase++;
        else if (string[i].isLower()) lowerCase++;
    }
}

void removeDuplicatesCaseInseitive(QStringList &sl)
{
    QStringList old=sl;
    QSet<QString> used;
    sl.clear();
    for (int i=0; i<old.size(); i++) {
        const QString uc=old[i].toUpper();
        if (!used.contains(uc)) {
            used.insert(uc);
            sl.append(old[i]);
        }
    }

}
