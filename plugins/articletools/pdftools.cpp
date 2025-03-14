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

#include "pdftools.h"
#include "bibtools.h"
#include "languagetools.h"

#define DEBUG_PDF

/////////////////////////////////////////////////////////
// specially recognize data from OSA journals
/////////////////////////////////////////////////////////
bool recognizeOSA(const QString& data, QMap<QString, QString>& props, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes, QString pluginDir) {
    QStringList lines=data.trimmed().split('\n');
    QRegularExpressionMatch rxOSAMatch;
    static const QRegularExpression rxOSA("(\\d\\d\\d\\d)\\s*Optical\\sSociety", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch rxOSA2Match;
    static const QRegularExpression rxOSA2("journal\\s*of\\s*the\\s*\\s*optical\\s*society\\s*of\\s*america", QRegularExpression::CaseInsensitiveOption);
    if (data.indexOf(rxOSA,0,&rxOSAMatch)>=0 || data.indexOf(rxOSA2,0,&rxOSA2Match)>=0) {
        props["year"]=rxOSAMatch.captured(1);

       /* QRegularExpressionMatch rxTitleMatch;
       static const QRegularExpression rxTitle("([ \\d\\w\\-\\:\\;\\,\\.\\?\\(\\)\\[\\]\\%\\&\\\"\\']*)\\n", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
        if (data.indexOf(rxTitle,0,&rxTitleMatch) != -1) {
            props["title"]=rxTitleMatch.captured(1);
        }*/

        if (data.indexOf(rxOSA2,0,&rxOSA2Match)>0) props["journal"]="Journal of the Optical Society of America";

        int i=0;
        while ((i<lines.size()) && (lines[i].simplified().isEmpty())) {
            i++;
        }
        if (i<lines.size()) props["title"]=lines[i].simplified();
        i++;
        while ((i<lines.size()) && (lines[i].simplified().isEmpty())) {
            i++;
        }
        if (i<lines.size()) {
            QString auth=lines[i].simplified();
            auth.remove(QRegularExpression("[\\d\\*\\#\\+\\~\\!\\$\\&\\\xA7\\?]"));
            QString a=reformatAuthors(auth, namePrefixes, nameAdditions, andWords);
            props["authors"]=a;
        }

        static const QRegularExpression rxJournal("Vol[\\.\\:\\s]*(\\d+).*No[\\.\\:\\s]*(\\d+)\\s*\\/\\s*([ a-zA-Z]*)\\s*(\\d*)\\s*\\n", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
        QRegularExpressionMatch rxJournalMatch;
        int count = 0;
        int pos = 0;
        QString firstPages="", lastPages="";
        while ((pos = data.indexOf(rxJournal,pos,&rxJournalMatch)) != -1) {
            props["volume"]=rxJournalMatch.captured(1);
            props["issue"]=rxJournalMatch.captured(2);
            props["journal"]=rxJournalMatch.captured(3);
            if (firstPages.isEmpty()) firstPages=rxJournalMatch.captured(4);
            else lastPages=rxJournalMatch.captured(4);
            ++count;
            pos += rxJournalMatch.capturedLength();
        }
        if (!firstPages.isEmpty() && !lastPages.isEmpty()) {
            props["pages"]=firstPages+"-"+lastPages;
        } else {
            props["pages"]=firstPages+lastPages;
        }

        bool hasYear=false;
        QRegularExpressionMatch rxYearMatch;
        static const QRegularExpression rxYear("(jan|january|feb|february|march|mar|apr|april|may|jun|june|jul|july|aug|august|sep|sept|september|oct|october|nov|november|dec|december)\\s*(\\d+)\\s*\\n", QRegularExpression::CaseInsensitiveOption);
        bool hasVol=false;
        QRegularExpressionMatch rxVolMatch;
        static const QRegularExpression rxVol("vol[^\\s]*\\s+(\\d+)\\s*", QRegularExpression::CaseInsensitiveOption);
        bool hasNumber=false;
        QRegularExpressionMatch rxNumberMatch;
        static const QRegularExpression rxNumber("num[^\\s]*\\s+(\\d+)\\s*", QRegularExpression::CaseInsensitiveOption);
        for (int i=0; i<lines.size(); i++) {
            if (!hasYear && lines[i].indexOf(rxYear,0,&rxYearMatch)>=0) {
                props["year"]=rxYearMatch.captured(2);
                hasYear=true;
            }
            if (!hasVol && lines[i].indexOf(rxVol,0,&rxVolMatch)>=0) {
                props["volume"]=rxVolMatch.captured(1);
                hasVol=true;
            }
            if (!hasNumber && lines[i].indexOf(rxNumber,0,&rxNumberMatch)>=0) {
                props["issue"]=rxNumberMatch.captured(1);
                hasNumber=true;
            }
            if (lines[i].toLower().contains("index") && lines[i].toLower().contains("head")) break;
        }


        return true;
    }
    return false;
}




/////////////////////////////////////////////////////////
// specially recognize data from Elsevier journals
/////////////////////////////////////////////////////////
bool recognizeElsevier(const QString& data, QMap<QString, QString>& props, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes, QString pluginDir) {
    QStringList lines=data.trimmed().split('\n');
    if (lines.size()<1) return false;
    QRegularExpressionMatch rxElsevierMatch;
    static const QRegularExpression rxElsevier("(.+)[\\s]+([\\d]+)[\\s]+\\((\\d\\d\\d\\d)\\)[\\s]+(.+).*www.elsevier.com", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
    if (lines[0].indexOf(rxElsevier,0,&rxElsevierMatch)>=0) {
        props["journal"]=rxElsevierMatch.captured(1);
        props["volume"]=rxElsevierMatch.captured(2);
        props["year"]=QString::number(rxElsevierMatch.captured(3).toInt());
        props["pages"]=rxElsevierMatch.captured(4);
        int i=1;
        while ((i<lines.size()) && (lines[i].simplified().isEmpty())) {
            i++;
        }
        if (i<lines.size()) props["title"]=lines[i].simplified();
        i++;
        while ((i<lines.size()) && (lines[i].simplified().isEmpty())) {
            i++;
        }
        if (i<lines.size()) {
            QString auth=lines[i].simplified();
            auth.remove(QRegularExpression("[\\d\\*\\#\\+\\~\\!\\$\\&\\\xA7\\?]"));
            QString a=reformatAuthors(auth, namePrefixes, nameAdditions, andWords);
            props["authors"]=a;
        }
        return true;
    }
    return false;
}



/////////////////////////////////////////////////////////
// specially recognize data from Bentham journals
/////////////////////////////////////////////////////////
bool recognizeBentham(const QString& data, QMap<QString, QString>& props, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes, QString pluginDir) {
    QStringList lines=data.trimmed().split('\n');
    QRegularExpressionMatch rxBenthamMatch;
    static const QRegularExpression rxBentham("\\s*(\\d\\d\\d\\d)\\s*Bentham\\sScience\\sPublishers", QRegularExpression::CaseInsensitiveOption);
    if (data.indexOf(rxBentham,0,&rxBenthamMatch)>=0) {
        props["year"]=rxBenthamMatch.captured(1);

        int i=0;
        bool isTitle=false;
        QRegularExpressionMatch rxJournalMatch;
        static const QRegularExpression rxJournal("(.+)\\s*,\\s+(\\d\\d\\d\\d)\\s*,\\s+(\\d+)\\s*,\\s+(.+)", QRegularExpression::CaseInsensitiveOption);
        while ((i<lines.size()) && (lines[i].simplified().isEmpty()) && (!isTitle)) {
            if (lines[i].indexOf(rxJournal,0,&rxJournalMatch)>=0) {
                props["pages"]=rxJournalMatch.captured(4);
                props["volume"]=rxJournalMatch.captured(3);
                props["year"]=rxJournalMatch.captured(2);
                props["journal"]=rxJournalMatch.captured(1);
            } else {
                if (lines[i].simplified().contains(QRegularExpression("[A-Za-z]",QRegularExpression::CaseInsensitiveOption))) isTitle=true;
            }
            if (!isTitle) i++;
        }
        if (i<lines.size()) props["title"]=lines[i].simplified();
        i++;
        while ((i<lines.size()) && (lines[i].simplified().isEmpty())) {
            i++;
        }
        if (i<lines.size()) {
            QString auth=lines[i].simplified();
            auth.remove(QRegularExpression("[\\d\\*\\#\\+\\~\\!\\$\\&\\\xA7\\?]"));
            QString a=reformatAuthors(auth, namePrefixes, nameAdditions, andWords);
            props["authors"]=a;
        }

        return true;
    }
    return false;
}



/////////////////////////////////////////////////////////
// specially recognize data from SPIE
/////////////////////////////////////////////////////////
bool recognizeSPIE(const QString& data, QMap<QString, QString>& props, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes, QString pluginDir) {
    QStringList lines=data.trimmed().split('\n');
    static const QRegularExpression rxProcSPIEFull("Proceedings\\s+of\\s+SPIE\\s+Vol.\\s*(\\d+)\\s+(.+),\\s+edited\\s+by\\s+(.*)\\s+\\(.+,\\s+(\\d\\d\\d\\d)\\)[^\\d]*([a-z\\d]{4,4}-[a-z\\d]{4,4}).*(\\d)+\\n", QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression rxProcSPIESmall("Proceedings\\s+of\\s+SPIE\\s+Vol.\\s*(\\d+)\\s+(.+),\\s+edited\\s+by\\s+(.*)\\s+\\(.+,\\s+(\\d\\d\\d\\d)\\)[^\\d]*([a-z\\d]{4,4}-[a-z\\d]{4,4}).*(\\d)+\\n", QRegularExpression::CaseInsensitiveOption);
    const QRegularExpression* spieUse=&rxProcSPIEFull;
    QRegularExpressionMatch spieUseMatch;
    bool ok=false;
    int start=-1;
    if ((start=data.indexOf(rxProcSPIEFull,0,&spieUseMatch))<0) {
        spieUse=&rxProcSPIESmall;
        if ((start=data.indexOf(rxProcSPIESmall,0,&spieUseMatch))<0) {

        } else ok=true;
    } else ok=true;

    if (ok) {
        start+=spieUseMatch.capturedLength();
        props["title"]=lines[0];
        props["year"]=spieUseMatch.captured(4);
        props["issn"]=spieUseMatch.captured(5);
        props["type"]="inproceedings";
        props["volume"]=spieUseMatch.captured(1);
        props["places"]="Bellingham";
        props["series"]="Proceedings of the SPIE";
        props["institution"]="SPIE";
        props["booktitle"]=spieUseMatch.captured(2);
        props["editors"]=reformatAuthors(spieUseMatch.captured(3), namePrefixes, nameAdditions, andWords);
        QString firstpage=spieUseMatch.captured(6);
        QString lastPage="";
        if (!firstpage.isEmpty()) {
            while ((start=data.indexOf(*spieUse, start, &spieUseMatch))!=-1) {
                lastPage=spieUseMatch.captured(6);
                start+=spieUseMatch.capturedLength();
            }
        }
        if (!firstpage.isEmpty()) {
            props["pages="]=firstpage;
            if (!lastPage.isEmpty() && (firstpage!=lastPage)) {
                props["pages="]=firstpage+"-"+lastPage;
            }
        }
        return true;
    }
    return false;
}




/////////////////////////////////////////////////////////
// specially recognize data from Biopphysical Journal
/////////////////////////////////////////////////////////
bool recognizeBiophysJ(const QString& data, QMap<QString, QString>& props, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes, QString pluginDir) {
    QStringList lines=data.trimmed().split('\n');
    QRegularExpressionMatch rxFullMatch;
    static const QRegularExpression rxFull("Biophysical\\s+Journal\\s+Volume\\s+(\\d+).+(\\d\\d\\d\\d)\\s+(\\d+)[^\\d]+(\\d+)\\s+.*([a-z]+.*)\\s*\\n\\s*([a-z]+.*)\\n", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
    bool ok=false;
    int start=-1;
    if ((start=data.indexOf(rxFull,0,&rxFullMatch))<0) {
    } else ok=true;

    if (ok) {
        start+=rxFullMatch.capturedLength();
        props["journal"]="Biophysical Journal";
        props["volume"]=rxFullMatch.captured(1);
        props["year"]=rxFullMatch.captured(2);
        props["pages"]=rxFullMatch.captured(3)+"-"+rxFullMatch.captured(4);
        props["title"]=rxFullMatch.captured(5);
        props["authors"]=reformatAuthors(rxFullMatch.captured(6), namePrefixes, nameAdditions, andWords);
        return true;
    }
    return false;
}



/////////////////////////////////////////////////////////
// specially recognize data from PNAS
/////////////////////////////////////////////////////////
bool recognizePNAS(const QString& data, QMap<QString, QString>& props, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes, QString pluginDir) {
    QStringList lines=data.trimmed().split('\n');
    QRegularExpressionMatch rxCheckMatch;
    static const QRegularExpression rxCheck("\\s+(\\d\\d\\d\\d)\\s+by\\s+The\\s+National\\s+Academy\\s+of\\s+Sciences\\s+of\\s+the\\s+USA", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch rxCheck2Match;
    static const QRegularExpression rxCheck2("PNAS\\s+.+\\s+(\\d\\d\\d\\d)[^\\d].+vol.+(\\d+)[^\\d].+no.+(\\d+)[^\\d].*(\\d+)", QRegularExpression::CaseInsensitiveOption);

    if (data.indexOf(rxCheck,0,&rxCheckMatch)>=0 || data.indexOf(rxCheck2,0,&rxCheck2Match)>=0) {
        if (rxCheckMatch.captured(1).size()>0) props["year"]=rxCheckMatch.captured(1);
        if (rxCheck2Match.captured(1).size()>0) props["year"]=rxCheck2Match.captured(1);
        props["journal"]="Proceedings of the National Academy of Sciences of the USA";

        QRegularExpressionMatch rxPNASMatch;
        static const QRegularExpression rxPNAS=QRegularExpression("PNAS\\s+.+\\s+(\\d\\d\\d\\d)[^\\d].+vol.+(\\d+)[^\\d].+no.+(\\d+)[^\\d].*(\\d+)", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
        int start=-1;
        if ((start=data.indexOf(rxPNAS,0,&rxPNASMatch))>=0) {
            props["year"]=rxPNASMatch.captured(1);
            props["volume"]=rxPNASMatch.captured(2);
            props["issue"]=rxPNASMatch.captured(3);
            QString firstpage=rxPNASMatch.captured(4);
            QString lastPage="";
            if (!firstpage.isEmpty()) {
                while ((start=data.indexOf(rxPNAS, start,&rxPNASMatch))!=-1) {
                    lastPage=rxPNASMatch.captured(4);
                    start+=rxPNASMatch.capturedLength();
                }
            }
            if (!firstpage.isEmpty()) {
                props["pages="]=firstpage;
                if (!lastPage.isEmpty() && (firstpage!=lastPage)) {
                    props["pages="]=firstpage+"-"+lastPage;
                }
            }
        }
        QRegularExpressionMatch rxDOIMatch;
        QRegularExpression rxDOI=QRegularExpression("doi\\s*(10\\..*)\\n", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
        if (data.indexOf(rxDOI,0,&rxDOIMatch)>=0) {
            props["doi"]=rxDOIMatch.captured(1).simplified().replace(' ', '/');
        } else {
            QString data1=data;
            data1=data1.replace('?', '/');
            rxDOI=QRegularExpression("doi\\s*(10\\..*)\\n", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
            if (data.indexOf(rxDOI,0,&rxDOIMatch)>=0) {
                props["doi"]=rxDOIMatch.captured(1).simplified().replace(' ', '/');
            } else {
                rxDOI=QRegularExpression("www.pnas.orgcgi[?\\/]doi[?\\/](10\\..*)\\n", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
                if (data.indexOf(rxDOI,0,&rxDOIMatch)>=0) {
                    props["doi"]=rxDOIMatch.captured(1).simplified().replace(' ', '/');
                } else {
                    if (lines.size()>0) props["title"]=lines[0];
                    if (lines.size()>1) {
                        int spcnt=0;
                        spcnt+=lines[1].count('*');
                        spcnt+=lines[1].count('#');
                        spcnt+=lines[1].count('+');
                        spcnt+=lines[1].count('~');
                        spcnt+=lines[1].count('$');
                        spcnt+=lines[1].count('\xA7');
                        spcnt+=lines[1].count('?');
                        spcnt+=lines[1].count('?');
                        QString aline=lines[1];
                        if (spcnt>1) {
                            props["title"]=props["title"]+lines[1];
                            aline=lines.value(2, "");
                        }
                        QString auth=aline.simplified();
                        auth.remove(QRegularExpression("[\\d\\*\\#\\+\\~\\!\\$\\&\\\xA7\\?]"));
                        QString a=reformatAuthors(auth, namePrefixes, nameAdditions, andWords);
                        props["authors"]=a;
                    }
                }
            }

        }
        return true;
    }
    return false;

}




/////////////////////////////////////////////////////////
// specially recognize data from IEEE
/////////////////////////////////////////////////////////
bool recognizeIEEE(const QString& data, QMap<QString, QString>& props, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes, QString pluginDir) {
    QStringList lines=data.trimmed().split('\n');
    QRegularExpressionMatch rxCheckMatch;
    static const QRegularExpression rxCheck("(IEEE[^,]*).+vol.+(\\d+).+no.+(\\d+).+(\\d\\d\\d\\d)\\s+(\\d+)\\s+([a-z]+.*)\\n.+([a-z]+.*)\\n", QRegularExpression::CaseInsensitiveOption);

    if (data.indexOf(rxCheck,0,&rxCheckMatch)>=0) {
        props["journal"]=rxCheckMatch.captured(1);
        props["volume"]=rxCheckMatch.captured(2);
        props["issue"]=rxCheckMatch.captured(3);
        props["year"]=rxCheckMatch.captured(4);
        props["pages"]=rxCheckMatch.captured(5)+"ff";
        props["title"]=rxCheckMatch.captured(6);

        QString auth=rxCheckMatch.captured(7).simplified();
        auth.remove(QRegularExpression("[\\d\\*\\#\\+\\~\\!\\$\\&\\\xA7\\?]"));
        QString a=reformatAuthors(auth.simplified(), namePrefixes, nameAdditions, andWords);
        props["authors"]=a;

        QRegularExpressionMatch rxDOIMatch;
        static const QRegularExpression rxDOI=QRegularExpression("doi\\s+(10\\..*)\\n", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
        if (data.indexOf(rxDOI,0,&rxDOIMatch)>=0) {
            props["doi"]=rxDOIMatch.captured(1).simplified().replace(' ', '/');
        }
        return true;
    }
    return false;

}





/////////////////////////////////////////////////////////
// specially recognize data from APS
/////////////////////////////////////////////////////////
bool recognizeAPS(const QString& data, QMap<QString, QString>& props, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes, QString pluginDir) {
    //QStringList lines=data.trimmed().split('\n');
    QRegularExpressionMatch rxCheckMatch;
    static const QRegularExpression rxCheck("(\\d\\d\\d\\d).*\\sAmerican\\s*Physical\\s*Society", QRegularExpression::CaseInsensitiveOption);

    if (data.indexOf(rxCheck,0,&rxCheckMatch)>=0) {
        props["year"]=rxCheckMatch.captured(1);

        QRegularExpressionMatch rxAPSMatch;
        static const QRegularExpression rxAPS("(.+)\\s+vol(ume)?[\\.\\:\\,]?\\s*(\\d+).+(no|number)[\\.\\:\\,]?\\s*(\\d+).+(\\d\\d\\d\\d)\\s+([a-z]+.+)\\n([a-z]+.+)\\n", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);

        if (data.indexOf(rxAPS,0,&rxAPSMatch)>=0) {
            props["journal"]=rxAPSMatch.captured(1);
            props["volume"]=rxAPSMatch.captured(3);
            props["issue"]=rxAPSMatch.captured(5);
            props["year"]=rxAPSMatch.captured(6);
            props["title"]=rxAPSMatch.captured(7);
            QString auth=rxCheckMatch.captured(8).simplified();
            if (auth.size()<8192) {
                auth.remove(QRegularExpression("[\\d\\*\\#\\+\\~\\!\\$\\&\\\xA7\\?]"));
                QString a=reformatAuthors(auth.simplified(), namePrefixes, nameAdditions, andWords);
                props["authors"]=a;
            }
        }

        return true;
    }
    return false;

}


/////////////////////////////////////////////////////////
// specially recognize data from ACS
/////////////////////////////////////////////////////////
bool recognizeACS(const QString& data, QMap<QString, QString>& props, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes, QString pluginDir) {
    QStringList lines=data.trimmed().split('\n');
    QRegularExpressionMatch rxCheckMatch;
    static const QRegularExpression rxCheck("(\\d\\d\\d\\d).*\\sAmerican\\s*Chemical\\s*Society", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
    if (data.indexOf(rxCheck,0,&rxCheckMatch)>=0) {
        props["year"]=rxCheckMatch.captured(1);

        QRegularExpressionMatch rxAPSMatch;
        static const QRegularExpression rxAPS("(.+)\\s+vol(ume)?[\\.\\:\\,]?\\s*(\\d+).+(no|number)[\\.\\:\\,]?\\s*(\\d+).+(\\d\\d\\d\\d)\\n", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);

        if (data.indexOf(rxAPS,0,&rxAPSMatch)>=0) {
            props["journal"]=rxAPSMatch.captured(1);
            props["volume"]=rxAPSMatch.captured(3);
            props["issue"]=rxAPSMatch.captured(5);
            props["year"]=rxAPSMatch.captured(6);

            for (int i=0; i<lines.size(); i++) {
                if (lines.size()>0) props["title"]=lines[0];
                if (lines.size()>1) {
                    int spcnt=0;
                    spcnt+=lines[i].count('*');
                    spcnt+=lines[i].count('#');
                    spcnt+=lines[i].count('+');
                    spcnt+=lines[i].count('~');
                    spcnt+=lines[i].count('$');
                    spcnt+=lines[i].count('\xA7');
                    spcnt+=lines[i].count('?');
                    spcnt+=lines[i].count('?');
                    QString aline=lines[i];
                    if (spcnt>1) {
                        QString auth=aline.simplified();
                        auth.remove(QRegularExpression("[\\d\\*\\#\\+\\~\\!\\$\\&\\\xA7\\?]"));
                        QString a=reformatAuthors(auth, namePrefixes, nameAdditions, andWords);
                        props["authors"]=a;
                        break;
                    }
                }
            }
            props["title"]=rxAPSMatch.captured(7);
            QString auth=rxCheckMatch.captured(8).simplified();
            auth.remove(QRegularExpression("[\\d\\*\\#\\+\\~\\!\\$\\&\\\xA7\\?]"));
            QString a=reformatAuthors(auth.simplified(), namePrefixes, nameAdditions, andWords);
            props["authors"]=a;
        }

        return true;
    }
    return false;

}

/////////////////////////////////////////////////////////
// specially recognize data from Kluwer
/////////////////////////////////////////////////////////
bool recognizeKluwer(const QString& data, QMap<QString, QString>& props, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes, QString pluginDir) {
    QStringList lines=data.trimmed().split('\n');
    QRegularExpressionMatch rxCheckMatch;
    static const QRegularExpression rxCheck("\\s+(\\d\\d\\d\\d)\\s+Kluwer\\s+Academic\\s+Publisher", QRegularExpression::CaseInsensitiveOption);

    if (data.indexOf(rxCheck,0,&rxCheckMatch)>=0) {
        props["year"]=rxCheckMatch.captured(1);

        QRegularExpressionMatch rxDataMatch;
        QRegularExpression rxData("([a-z\\s]+)\\s+(\\d+)\\((\\d+)\\)[,;]?\\s+(\\d+)[^\\d]+(\\d+)[,;]?\\s+(\\d\\d\\d\\d)", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
        if (data.indexOf(rxData,0,&rxDataMatch)>=0) {
            props["journal"]=rxDataMatch.captured(1);
            props["volume"]=rxDataMatch.captured(2);
            props["issue"]=rxDataMatch.captured(3);
            props["pages"]=rxDataMatch.captured(4)+"-"+rxDataMatch.captured(5);

        }


        rxData=QRegularExpression("\\s+(\\d\\d\\d\\d)\\s+Kluwer\\s+Academic\\s+Publisher", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);

        if (data.indexOf(rxData,0,&rxDataMatch)>=0) {
            props["year"]=rxCheckMatch.captured(1);

            QString auth=rxCheckMatch.captured(8).simplified();
            auth.remove(QRegularExpression("[\\d\\*\\#\\+\\~\\!\\$\\&\\\xA7\\?]"));
            QString a=reformatAuthors(auth.simplified(), namePrefixes, nameAdditions, andWords);
            props["authors"]=a;
        }


        return true;
    }
    return false;

}




/////////////////////////////////////////////////////////
// specially recognize data from Rockefeller University Press
/////////////////////////////////////////////////////////
bool recognizeRockefeller(const QString& data, QMap<QString, QString>& props, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes, QString pluginDir) {
    QStringList lines=data.trimmed().split('\n');
    QRegularExpressionMatch rxCheckMatch;
    static const QRegularExpression rxCheck("\\s*Rockefeller\\s*University\\s*Press\\s*", QRegularExpression::CaseInsensitiveOption);

    if (data.indexOf(rxCheck,0,&rxCheckMatch)>=0) {
        //props["year"]=rxCheckMatch.captured(1);

        //The Journal of Cell Biology, Volume 115, Number 1, October 1991 67-73
        QRegularExpressionMatch rxDataMatch;
        QRegularExpression rxData("([a-z\\s]+).*\\s*vol\\w*\\s*(\\d+).*num\\w*\\s*(\\d+).*\\w+\\s*(\\d\\d\\d\\d).* (\\d+)\\s*[\\-]*\\s*(\\d+)", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
        if (data.indexOf(rxData,0,&rxDataMatch)>=0) {
            props["journal"]=rxDataMatch.captured(1);
            props["volume"]=rxDataMatch.captured(2);
            props["issue"]=rxDataMatch.captured(3);
            props["pages"]=rxDataMatch.captured(4)+"-"+rxDataMatch.captured(5);

        }


        rxData=QRegularExpression("\\s+(\\d\\d\\d\\d)\\s+Kluwer\\s+Academic\\s+Publisher", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);

        if (data.indexOf(rxData,0,&rxDataMatch)>=0) {
            props["journal"]=rxDataMatch.captured(1);
            props["volume"]=rxDataMatch.captured(2);
            props["issue"]=rxDataMatch.captured(3);
            props["pages"]=rxDataMatch.captured(4)+"-"+rxDataMatch.captured(5);

            QString auth=rxCheckMatch.captured(8).simplified();
            auth.remove(QRegularExpression("[\\d\\*\\#\\+\\~\\!\\$\\&\\\xA7\\?]"));
            QString a=reformatAuthors(auth.simplified(), namePrefixes, nameAdditions, andWords);
            props["authors"]=a;
        }


        return true;
    }
    return false;

}




/////////////////////////////////////////////////////////
// specially recognize data in a US Patent
/////////////////////////////////////////////////////////
bool recognizePatent(const QString& data, QMap<QString, QString>& props, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes, QString pluginDir) {
    QRegularExpressionMatch rxCheckMatch;
    static const QRegularExpression rxCheck("\\s*(Patent\\s*Application|Patentet\\s*Application|Offenlegungsschrift|Patentschrift|United\\s*States\\s*Patent)\\s*", QRegularExpression::CaseInsensitiveOption);

    QString cleanData=data;
    cleanData=cleanData.replace("\n\n", "\n").simplified();

    if (cleanData.indexOf(rxCheck,0,&rxCheckMatch)>=0) {
        //props["year"]=rxCheckMatch.captured(1);
        props["type"]="patent";

        QMap<int, QString> databook;

        //find (dd)TEXT, which occurs on the first page of the patent quite often!
        //QRegularExpressionMatch rxDataMatch;
        static const QRegularExpression rxData("\\n[\\t \\r]*\\(\\s*(\\d+)\\s*\\)\\s*([\\w\\d\\s]+[^\\s])(\\s+(\\n[\\t \\r]*)+)", QRegularExpression::CaseInsensitiveOption);
        if (false) {
            QRegularExpressionMatch rxDataMatch;
            static const QRegularExpression rxData("\\n[\\t \\r]*\\(\\s*(\\d\\d?)\\s*\\)\\s*([^(\\n\\n)]+[^\\s]{1})(\\s*(\\n[\\t \\r]*)+)", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
            int pos=0;
            int iter=0;
            while ((pos=cleanData.indexOf(rxData,pos,&rxDataMatch))>=0 && iter<1000) {
                int id=rxDataMatch.captured(1).toInt();
                QString text=rxDataMatch.captured(2);
                text=text.replace('\n', ' ');
                text=text.replace('\r', ' ');
                text=text.simplified().trimmed();
                //qDebug()<<"PATENT: (1."<<id<<"): "<<text<<"\nPATENT:      match="<<rxDataMatch.captured(0)<<"\nPATENT:      cap3="<<rxDataMatch.captured(3);
                if (text.size()>0 && !databook.contains(id)) {
                    databook[id]=text;
                    //qDebug()<<"PATENT:      STORED!!!";
                }
                if (rxDataMatch.capturedLength()-rxDataMatch.captured(3).length()<=0) pos+=rxDataMatch.capturedLength();
                else pos+=(rxDataMatch.capturedLength()-rxDataMatch.captured(3).length());
                iter++;
            }
        }

        if (true) {
            QRegularExpressionMatch rxDataMatch;
            static const QRegularExpression rxData("\\s*\\(\\s*(\\d\\d?)\\s*\\)\\s*(.*)(\\s*\\(\\s*\\d\\d?\\s*\\))", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
            int pos=0;
            int iter=0;
            while ((pos=cleanData.indexOf(rxData, pos,&rxDataMatch))>=0 && iter<1000) {
                int id=rxDataMatch.captured(1).toInt();
                QString text=rxDataMatch.captured(2);
                text=text.replace('\n', ' ');
                text=text.replace('\r', ' ');
                text=text.simplified().trimmed();
                //qDebug()<<"PATENT: (2."<<id<<"): "<<text<<"\nPATENT:      match="<<rxDataMatch.captured(0)<<"\nPATENT:      cap3="<<rxDataMatch.captured(3);
                if (text.size()>0 && !databook.contains(id)) {
                    databook[id]=text;
                    //qDebug()<<"PATENT:      STORED!!!";
                }
                if (rxDataMatch.capturedLength()-rxDataMatch.captured(3).length()<=0) pos+=rxDataMatch.capturedLength();
                else pos+=(rxDataMatch.capturedLength()-rxDataMatch.captured(3).length());
                iter++;
            }
        }

        for (auto it=databook.begin(); it!=databook.end(); ++it) {
            const int id=it.key();
            QString text=it.value();
            if (id==54 && props["title"].isEmpty()) {
                //qDebug()<<"PATENT: TITLE.before = "<<text;
                text=text.remove(QRegularExpression("\\s*(Title|Bezeichnung)\\s*\\:?\\s*", QRegularExpression::CaseInsensitiveOption));
                //qDebug()<<"PATENT: TITLE.after = "<<text;
                props["title"]=text;
            }
            else if (id==12) props["subtype"]=text;
            else if ((id==11||id==10) && props["number"].isEmpty()) {
                //qDebug()<<"PATENT: NUMBER.before = "<<text;
                {
                    QString datalocal=text;
                    //datalocal.remove(QRegularExpression("\\s"));
                    QRegularExpressionMatch rxDataMatch;
                    static const QRegularExpression rxData("[A-Z]{2}[\\d\\,\\.]+[A-Z]\\s*\\d", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
                    if (datalocal.indexOf(rxData,0,&rxDataMatch)) {
                        props["number"]=rxDataMatch.captured(0).simplified();
                    }
                }
                if (props["number"].isEmpty()) {
                    QString datalocal=text;
                    //datalocal.remove(QRegularExpression("\\s"));
                    QRegularExpressionMatch rxDataMatch;
                    static const QRegularExpression rxData("[A-Z]{2}[\\d\\,\\. ]+[A-Z]\\s*\\d", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
                    if (datalocal.indexOf(rxData,0,&rxDataMatch)) {
                        props["number"]=rxDataMatch.captured(0).simplified();
                    }
                } else {
                    text=text.remove(QRegularExpression("\\s*(Pub\\s*\\.?\\s*No\\s*\\.?|Appl\\s*\\.?\\s*No\\s*\\.?|Anmeldenummer|Aktenzeichen)\\s*\\:?\\s*", QRegularExpression::CaseInsensitiveOption));
                    //qDebug()<<"PATENT: NUMBER.after1 = "<<text;
                    text=text.remove(QRegularExpression("\\s", QRegularExpression::CaseInsensitiveOption));
                    //qDebug()<<"PATENT: NUMBER.after3 = "<<text;
                    props["number"]=text.simplified();
                }
                props["publisher"]=props["number"].left(2);
            }
            else if (id==21 && props["volume"].isEmpty()) {
                //qDebug()<<"PATENT: VOLUME.before = "<<text;
                text=text.remove(QRegularExpression("\\s*(Pub\\s*\\.?\\s*No\\s*\\.?|Appl\\s*\\.?\\s*No\\s*\\.?|Anmeldenummer|Aktenzeichen)\\s*\\:?\\s*", QRegularExpression::CaseInsensitiveOption));
                //qDebug()<<"PATENT: VOLUME.after1 = "<<text;
                text=text.remove(QRegularExpression("[^\\d\\.\\/\\\\\\,]{3,}", QRegularExpression::CaseInsensitiveOption));
                //qDebug()<<"PATENT: VOLUME.after2 = "<<text;
                text=text.remove(QRegularExpression("\\s", QRegularExpression::CaseInsensitiveOption));
                //qDebug()<<"PATENT: VOLUME.after3 = "<<text;
                props["volume"]=text;
            }
            else if (id==71 && props["institution"].isEmpty()) {
                //qDebug()<<"PATENT: INSTITUTION.before = "<<text;
                text=text.remove(QRegularExpression("\\s*(Assignee|Applicant|Anmelder|Erfinder|Patentinhaber|Vertreter|Inventors?)\\s*\\:?\\s*", QRegularExpression::CaseInsensitiveOption));
                //qDebug()<<"PATENT: INSTITUTION.after1 = "<<text;
                text=text.remove(QRegularExpression("\\,.*", QRegularExpression::CaseInsensitiveOption));
                //qDebug()<<"PATENT: INSTITUTION.after2 = "<<text;
                props["institution"]=text;
            }
            else if (id==73 && props["institution"].isEmpty())  {
                //qDebug()<<"PATENT: INSTITUTION.before = "<<text;
                text=text.remove(QRegularExpression("\\s*(Assignee|Applicant|Anmelder|Erfinder|Patentinhaber|Vertreter|Inventors?)\\s*\\:?\\s*", QRegularExpression::CaseInsensitiveOption));
                //qDebug()<<"PATENT: INSTITUTION.after1 = "<<text;
                text=text.remove(QRegularExpression("\\,.*", QRegularExpression::CaseInsensitiveOption));
                //qDebug()<<"PATENT: INSTITUTION.after2 = "<<text;
                props["institution"]=text;
            }
            else if (id==57 && props["abstract"].isEmpty()) {
                //qDebug()<<"PATENT: ABSTRACT.before = "<<text;
                text=text.remove(QRegularExpression("\\s*(Zusammenfassung|Summary)\\s*\\:?\\s*", QRegularExpression::CaseInsensitiveOption));
                //qDebug()<<"PATENT: ABSTRACT.after = "<<text;
                props["abstract"]=text;
            }
            else if (id==22 && props["year"].isEmpty()){
                //qDebug()<<"PATENT: YEAR.before = "<<text;
                props["year"]=QString::number(extractYear(text));
            }
            else if (id==72 && props["authors"].isEmpty()) {
                //qDebug()<<"PATENT: AUTHORS.before = "<<text;
                QString authors;
                { // names listing type 1: Family, First, BLA,BLA...;Family, First, BLA,BLA...
                    QRegularExpressionMatch rxDataMatch;
                    static const QRegularExpression rxData("([\\w\\s\\.]+,[\\w\\s\\.]+),[^;]+(;|$)", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
                    int pos=0;
                    int iter=0;
                    while ((pos=text.indexOf(rxData, pos,&rxDataMatch))>=0 && iter<1000) {
                        if (!authors.isEmpty()) authors+="; ";
                        authors+=rxDataMatch.captured(1);
                        //qDebug()<<"PATENT: AUTHOR: "<<rxDataMatch.captured(1);
                        pos+=rxDataMatch.capturedLength();
                        iter++;
                    }
                }
                if (authors.isEmpty()){ // names listing type 1: First Family, BLA...;First Family, BLA...
                    QRegularExpressionMatch rxDataMatch;
                    static const QRegularExpression rxData("([\\w\\s\\.]+),[^;]+(;|$)", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
                    int pos=0;
                    int iter=0;
                    while ((pos=text.indexOf(rxData, pos,&rxDataMatch))>=0 && iter<1000) {
                        if (!authors.isEmpty()) authors+="; ";
                        authors+=rxDataMatch.captured(1);
                        //qDebug()<<"PATENT: AUTHOR: "<<rxDataMatch.captured(1);
                        pos+=rxDataMatch.capturedLength();
                        iter++;
                    }
                }
                props["authors"]=reformatAuthors(authors.simplified(), namePrefixes, nameAdditions, andWords);
            }

        }

        if (props["number"].isEmpty()) {
            QString datalocal=data;
            //datalocal.remove(QRegularExpression("\\s"));
            QRegularExpressionMatch rxDataMatch;
            static const QRegularExpression rxData("[A-Z]{2}[\\d\\,\\.\\s]+[A-Z]\\s*\\d+", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
            if (datalocal.indexOf(rxData,0,&rxDataMatch)) {
                props["number"]=rxDataMatch.captured(0);
                //qDebug()<<"PATENT: NUMBER: "<<props["number"];
            }
        }
        if (props["number"].isEmpty()) {
            QString datalocal=data;
            //datalocal.remove(QRegularExpression("\\s"));
            QRegularExpressionMatch rxDataMatch;
            static const QRegularExpression rxData("[A-Z]{2}[\\d\\,\\.\\s]+", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
            if (datalocal.indexOf(rxData,0,&rxDataMatch)) {
                props["number"]=rxDataMatch.captured(0);
                //qDebug()<<"PATENT: NUMBER: "<<props["number"];
            }
        }

        return true;
    }
    return false;

}









/////////////////////////////////////////////////////////
// specially recognize data from Physik Journal
/////////////////////////////////////////////////////////
bool recognizePhysikJournal(const QString& data, QMap<QString, QString>& props, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes, QString pluginDir) {
    QRegularExpressionMatch rxCheck1Match;
    static const QRegularExpression rxCheck1("Physik\\s*Journal", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch rxCheck2Match;
    static const QRegularExpression rxCheck2("Wiley\\s*\\-\\s*VCH\\s*Verlag\\s*GmbH", QRegularExpression::CaseInsensitiveOption);

    if (data.indexOf(rxCheck1,0,&rxCheck1Match)>=0 && data.indexOf(rxCheck2,0,&rxCheck2Match)>=0) {

        QRegularExpressionMatch rxData1Match;
        static const QRegularExpression rxData1("\\s+(\\d+)\\s+Physik\\s*Journal\\s+(\\d+)\\s+\\((\\d\\d\\d\\d)\\)\\s+Nr\\s*\\.\\s*(\\d+)", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
        QRegularExpressionMatch rxData2Match;
        static const QRegularExpression rxData2("Physik\\s*Journal\\s+(\\d+)\\s+\\((\\d\\d\\d\\d)\\)\\s+Nr\\s*\\.\\s*(\\d+)\\s+(\\d+)\\s+", QRegularExpression::CaseInsensitiveOption|QRegularExpression::InvertedGreedinessOption);
        int pos1=0, pos2=0;
        int pmin=std::numeric_limits<int>::max(), pmax=-1;
        bool hasBasiscs=false;
        props["journal"]="Physik Journal";

        while ((pos1=data.indexOf(rxData1, pos1,&rxData1Match))>=0) {
            if (!hasBasiscs) {
                props["volume"]=rxData1Match.captured(2);
                props["issue"]=rxData1Match.captured(4);
                props["year"]=rxData1Match.captured(3);
                hasBasiscs=true;
            }
            bool ok=false;
            const int p=rxData1Match.captured(1).toInt(&ok);
            if (ok && p>0) {
                pmin=qMin(pmin, p);
                pmax=qMax(pmax, p);
            }
            pos1 += rxData1Match.capturedLength();
        }

        while ((pos2=data.indexOf(rxData2, pos2,&rxData2Match))>=0) {
            if (!hasBasiscs) {
                props["volume"]=rxData2Match.captured(1);
                props["issue"]=rxData2Match.captured(3);
                props["year"]=rxData2Match.captured(2);
                hasBasiscs=true;
            }
            bool ok=false;
            const int p=rxData2Match.captured(4).toInt(&ok);
            if (ok && p>0) {
                pmin=qMin(pmin, p);
                pmax=qMax(pmax, p);
            }
            pos2 += rxData2Match.capturedLength();
        }

        if (pmin>0 && pmax>0) {
            if (pmin==pmax) {
                props["pages"]=QString::number(pmin);
            } else {
                props["pages"]=QString::number(pmin)+"-"+QString::number(pmax);
            }
        }

        return true;
    }
    return false;

}








QMap<QString, QString> extractFromPDF(const QString& filename, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes, QString pluginDir) {
    QMap<QString, QString> props;

    QString tempFile="pdftools_temp.txt";
    {
        QTemporaryFile tf;
        tf.open();
        tempFile=tf.fileName();
        tf.close();
    }

    tempFile=filename+".pdf2txt.litsoz3.txt";

    if (QFile::exists(tempFile)) QFile::remove(tempFile);


    const QString params=" -enc UTF-8 -layout \""+filename+"\" \""+tempFile+"\"";
    QDir pd(pluginDir);
    pd.cd("pdftools");
    if (!QFile::exists(pd.absoluteFilePath("pdftotext.exe"))) {
        pd=QDir(QApplication::applicationDirPath());
    }
#ifdef Q_OS_WIN32

    qDebug()<<"running "<<pd.absoluteFilePath("pdftotext.exe")+params;
    QProcess::execute(pd.absoluteFilePath("pdftotext.exe")+params);

#else
    qDebug()<<"running "<<pd.absoluteFilePath("pdftotext")+params;
    QProcess::execute("pdftotext"+params);
#endif

#ifdef DEBUG_PDF
    QElapsedTimer timerdbg;
    timerdbg.start();
    QFile testout(filename+".pdf2txt.debug.txt");
    testout.open(QFile::WriteOnly|QFile::Text);
    QTextStream dbgtxt(&testout);
#endif

    if (QFile::exists(tempFile)) {
        QFile f(tempFile);
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QString contents=f.readAll();
            props["language"]=recognizeLanguage(contents);
            if (props["language"].isEmpty()) props.remove("language");

#ifdef DEBUG_PDF
            dbgtxt<<"CONTENTS:\n"<<contents<<"\n\n\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n\n\n";
            dbgtxt.flush();
            timerdbg.start();

#endif

            //qDebug()<<contents;

            /////////////////////////////////////////////////////////
            // common metadata:
            /////////////////////////////////////////////////////////
            QRegularExpressionMatch rxDoiMatch;
            static const QRegularExpression rxDoi("doi[\\:]?\\s*(10.[a-zA-Z\\d\\.\\/\\(\\)\\-\\_]*)", QRegularExpression::CaseInsensitiveOption);
            if (contents.indexOf(rxDoi,0,&rxDoiMatch)>=0) {
                QString doi=rxDoiMatch.captured(1);
                QChar last=doi[doi.size()-1];
                //qDebug()<<doi;
                while ((doi.size()>4)&&(!(last.isLetterOrNumber() || QString("./()-_").contains(last)))) {
                    doi=doi.left(doi.size()-1);
                    last=doi[doi.size()-1];
                }
                //qDebug()<<doi;

                if (doi.simplified().trimmed().size()>0) props["doi"]=doi;
            }
            #ifdef DEBUG_PDF
            dbgtxt<<"search DOI : "<<props.value("doi", "")<<"\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n";
            dbgtxt.flush();
            timerdbg.start();
            #endif
            QRegularExpression rxYear;
            QRegularExpressionMatch rxYearMatch;
            bool hasYear=false;
            rxYear=QRegularExpression("Available[^\\n]*\\s(\\d\\d\\d\\d)\\s");
            if (!hasYear && contents.indexOf(rxYear,0,&rxYearMatch)>=0) {
                //qDebug()<<"found year @ "<<contents.indexOf(rxYear,0,&rxYearMatch)<<"   ="<<rxYearMatch.captured(0).trimmed();
                bool ok=true;
                int y=rxYearMatch.captured(1).toInt(&ok);
                if (ok && (y>1900) && (y<=QDate::currentDate().year()+100)) {
                    props["year"]=rxYearMatch.captured(1);
                    hasYear=true;
                }
            }
            rxYear=QRegularExpression("accepted[^\\n]*\\s(\\d\\d\\d\\d)\\s");
            if (!hasYear && contents.indexOf(rxYear,0,&rxYearMatch)>=0) {
                //qDebug()<<"found year @ "<<contents.indexOf(rxYear,0,&rxYearMatch)<<"   ="<<rxYearMatch.captured(0).trimmed();
                bool ok=true;
                int y=rxYearMatch.captured(1).toInt(&ok);
                if (ok && (y>1900) && (y<=QDate::currentDate().year()+100)) {
                    props["year"]=rxYearMatch.captured(1);
                    hasYear=true;
                }
            }
            rxYear=QRegularExpression("\\s(\\d\\d\\d\\d)\\s");
            if (!hasYear && contents.indexOf(rxYear,0,&rxYearMatch)>=0) {
                //qDebug()<<"found year @ "<<contents.indexOf(rxYear,0,&rxYearMatch)<<"   ="<<rxYearMatch.captured(0).trimmed();
                bool ok=true;
                int y=rxYearMatch.captured(1).toInt(&ok);
                if (ok && (y>1900) && (y<=QDate::currentDate().year()+100)) {
                    props["year"]=rxYearMatch.captured(1);
                    hasYear=true;
                }
            }

            #ifdef DEBUG_PDF
            dbgtxt<<"search year : "<<props.value("year", "")<<"\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n";
            dbgtxt.flush();
            timerdbg.start();

            #endif
            bool hasAbstract=false;
            QRegularExpressionMatch rxIntroMatch;
            static const QRegularExpression rxIntro("a\\s*b\\s*s\\s*t\\s*r\\s*a\\s*c\\s*t\\s([\xA9]*)\\n\\n", QRegularExpression::CaseInsensitiveOption);
            if (contents.indexOf(rxIntro,0,&rxIntroMatch)>=0) {
                if (rxIntroMatch.captured(1).size()<=contents.size()/10){
                    props["abstract"]=rxIntroMatch.captured(1);
                    hasAbstract=true;
                }
            }
            if (!hasAbstract) {
                QRegularExpressionMatch rxIntroMatch;
                static const QRegularExpression rxIntro("i\\s*n\\s*t\\s*r\\s*o\\s*d\\s*u\\s*c\\s*t\\s*i\\s*o\\s*n\\s([^\xA9]*)\\n\\n", QRegularExpression::CaseInsensitiveOption);
                if (contents.indexOf(rxIntro,0,&rxIntroMatch)>=0) {
                    if (rxIntroMatch.captured(1).size()<=contents.size()/10){
                        props["abstract"]=rxIntroMatch.captured(1);
                        hasAbstract=true;
                    }
                }

            }
            if (!hasAbstract) {
                QRegularExpressionMatch rxIntroMatch;
                static const QRegularExpression rxIntro("i\\s*n\\s*t\\s*r\\s*o\\s*d\\s*u\\s*c\\s*t\\s*i\\s*o\\s*n\\s([^\xA9\\n]*)\\n", QRegularExpression::CaseInsensitiveOption);
                if (contents.indexOf(rxIntro,0,&rxIntroMatch)>=0) {
                    if (rxIntroMatch.captured(1).size()<=contents.size()/10){
                        props["abstract"]=rxIntroMatch.captured(1);
                        hasAbstract=true;
                    }
                }

            }
            #ifdef DEBUG_PDF
            dbgtxt<<"search abstract : "<<props.value("abstract", "")<<"\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n";
            dbgtxt.flush();
            timerdbg.start();

            #endif

            bool haskeywords=false;
            QRegularExpression rxkw;
            QRegularExpressionMatch rxkwMatch;
            rxkw=QRegularExpression("(k\\s*e\\s*y\\s*w\\s*o\\s*r\\s*d\\s*s|o\\s*c\\s*i\\s*s\\s*c\\s*o\\s*d\\s*e\\s*s|i\\s*n\\s*d\\s*e\\s*x\\s*t\\s*e\\s*r\\s*m\\s*s|i\\s*n\\s*d\\s*e\\s*x\\s*h\\s*e\\s*a\\s*d\\s*i\\s*n\\s*g\\s*s)[\\:\\-\xD0]?[\\s]*(.*)\\n\\n", QRegularExpression::CaseInsensitiveOption);
            if (!haskeywords && contents.indexOf(rxkw,0,&rxkwMatch)>=0) {
                QString kw=rxkwMatch.captured(2).simplified();
                if (kw.size()<contents.size()/20) {
                    if (kw.contains(",") && kw.contains(";")) {
                        kw.replace("; ", "\n");
                        kw.replace(";", "\n");
                    } else {
                        kw.replace("; ", "\n");
                        kw.replace(", ", "\n");
                        kw.replace(';', '\n');
                        kw.replace(',', '\n');
                        kw.replace('\xB7', '\n');
                    }
                    if (kw.simplified().size()>0) {
                        props["keywords"]=kw.simplified();
                        haskeywords=true;
                    }
                }
            }
            rxkw=QRegularExpression("(k\\s*e\\s*y\\s*w\\s*o\\s*r\\s*d\\s*s|o\\s*c\\s*i\\s*s\\s*c\\s*o\\s*d\\s*e\\s*s|i\\s*n\\s*d\\s*e\\s*x\\s*t\\s*e\\s*r\\s*m\\s*s|i\\s*n\\s*d\\s*e\\s*x\\s*h\\s*e\\s*a\\s*d\\s*i\\s*n\\s*g\\s*s)[\\:\\-\xD0]?[\\s]*([^\\n]*)\\n", QRegularExpression::CaseInsensitiveOption);
            if (!haskeywords && contents.indexOf(rxkw,0,&rxkwMatch)>=0) {
                QString kw=rxkwMatch.captured(2).simplified();
                if (kw.contains(",") && kw.contains(";")) {
                    kw.replace("; ", "\n");
                    kw.replace(";", "\n");
                } else {
                    kw.replace("; ", "\n");
                    kw.replace(", ", "\n");
                    kw.replace(';', '\n');
                    kw.replace(',', '\n');
                    kw.replace('\xB7', '\n');
                }
                if (kw.simplified().size()>0) {
                    props["keywords"]=kw.simplified();
                    haskeywords=true;
                }
            }
            rxkw=QRegularExpression("(k\\s*e\\s*y\\s*w\\s*o\\s*r\\s*d\\s*s|o\\s*c\\s*i\\s*s\\s*c\\s*o\\s*d\\s*e\\s*s|i\\s*n\\s*d\\s*e\\s*x\\s*t\\s*e\\s*r\\s*m\\s*s|i\\s*n\\s*d\\s*e\\s*x\\s*h\\s*e\\s*a\\s*d\\s*i\\s*n\\s*g\\s*s)[\\:\\-\xD0]?[\\s]*([^\\n]*)\\n", QRegularExpression::CaseInsensitiveOption);
            if (!haskeywords && contents.indexOf(rxkw,0,&rxkwMatch)>=0) {
                QString kw=rxkwMatch.captured(2).simplified();
                if (kw.contains(",") && kw.contains(";")) {
                    kw.replace("; ", "\n");
                    kw.replace(";", "\n");
                } else {
                    kw.replace("; ", "\n");
                    kw.replace(", ", "\n");
                    kw.replace(';', '\n');
                    kw.replace(',', '\n');
                    kw.replace('\xB7', '\n');
                }
                if (kw.simplified().size()>0) {
                    props["keywords"]=kw.simplified();
                    haskeywords=true;
                }
            }
            #ifdef DEBUG_PDF
            dbgtxt<<"search keywords : "<<props.value("keywords", "")<<"\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n";
            dbgtxt.flush();
            timerdbg.start();
            #endif

            /////////////////////////////////////////////////////////
            // specially recognize data from some journals
            /////////////////////////////////////////////////////////
            bool ok=false;
            ok=recognizeOSA(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            #ifdef DEBUG_PDF
            dbgtxt<<"recognizeOSA : "<<ok<<"\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n";
            dbgtxt.flush();
            timerdbg.start();
            #endif
            if (!ok) ok=recognizeElsevier(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            #ifdef DEBUG_PDF
            dbgtxt<<"recognizeElsevier : "<<ok<<"\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n";
            dbgtxt.flush();
            timerdbg.start();
            #endif
                        if (!ok) ok=recognizeBentham(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            #ifdef DEBUG_PDF
            dbgtxt<<"recognizeBentham : "<<ok<<"\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n";
            dbgtxt.flush();
            timerdbg.start();
            #endif
                        if (!ok) ok=recognizeSPIE(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            #ifdef DEBUG_PDF
            dbgtxt<<"recognizeSPIE : "<<ok<<"\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n";
            dbgtxt.flush();
            timerdbg.start();
            #endif
                        if (!ok) ok=recognizeBiophysJ(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            #ifdef DEBUG_PDF
            dbgtxt<<"recognizeBiophysJ : "<<ok<<"\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n";
            dbgtxt.flush();
            timerdbg.start();
            #endif
                        if (!ok) ok=recognizePNAS(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            #ifdef DEBUG_PDF
            dbgtxt<<"recognizePNAS : "<<ok<<"\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n";
            dbgtxt.flush();
            timerdbg.start();
            #endif
                        if (!ok) ok=recognizeIEEE(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            #ifdef DEBUG_PDF
            dbgtxt<<"recognizeIEEE : "<<ok<<"\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n";
            dbgtxt.flush();
            timerdbg.start();
            #endif
                        if (!ok) ok=recognizeAPS(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            #ifdef DEBUG_PDF
            dbgtxt<<"recognizeAPS : "<<ok<<"\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n";
            dbgtxt.flush();
            timerdbg.start();
            #endif
                        if (!ok) ok=recognizeACS(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            #ifdef DEBUG_PDF
            dbgtxt<<"recognizeACS : "<<ok<<"\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n";
            dbgtxt.flush();
            timerdbg.start();
            #endif
                        if (!ok) ok=recognizeKluwer(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            #ifdef DEBUG_PDF
            dbgtxt<<"recognizeKluwer : "<<ok<<"\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n";
            dbgtxt.flush();
            timerdbg.start();
            #endif
            if (!ok) ok=recognizeRockefeller(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            #ifdef DEBUG_PDF
            dbgtxt<<"recognizeRockefeller : "<<ok<<"\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n";
            dbgtxt.flush();
            timerdbg.start();
            #endif

            if (!ok) ok=recognizePatent(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            #ifdef DEBUG_PDF
            dbgtxt<<"recognizeUSPatent : "<<ok<<"\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n";
            dbgtxt.flush();
            timerdbg.start();
            #endif

            if (!ok) ok=recognizePhysikJournal(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
#ifdef DEBUG_PDF
            dbgtxt<<"recognizePhysikJournal : "<<ok<<"\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n";
            dbgtxt.flush();
            timerdbg.start();
#endif


            f.close();
            f.remove();

        }

        //QFile::remove(tempFile);
    }
#ifdef DEBUG_PDF
    dbgtxt<<"\n\n\nOUTPUT:\n";
    QMapIterator<QString, QString> it(props);
    while (it.hasNext()) {
        it.next();
        dbgtxt<<"  "<<it.key()<<" = "<<it.value()<<"\n";
    }
    testout.close();
#endif
    return props;
}




















