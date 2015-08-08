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

//#define DEBUG_PDF

/////////////////////////////////////////////////////////
// specially recognize data from OSA journals
/////////////////////////////////////////////////////////
bool recognizeOSA(const QString& data, QMap<QString, QString>& props, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes, QString pluginDir) {
    QStringList lines=data.trimmed().split('\n');
    QRegExp rxOSA("(\\d\\d\\d\\d)\\s*Optical\\sSociety", Qt::CaseInsensitive);
    QRegExp rxOSA2("journal\\s*of\\s*the\\s*\\s*optical\\s*society\\s*of\\s*america", Qt::CaseInsensitive);
    if (rxOSA.indexIn(data)>=0 || rxOSA2.indexIn(data)>=0) {
        props["year"]=rxOSA.cap(1);

       /* QRegExp rxTitle("([ \\d\\w\\-\\:\\;\\,\\.\\?\\(\\)\\[\\]\\%\\&\\\"\\']*)\\n", Qt::CaseInsensitive);
        rxTitle.setMinimal(true);
        if (rxTitle.indexIn(data) != -1) {
            props["title"]=rxTitle.cap(1);
        }*/

        if (rxOSA2.indexIn(data)>0) props["journal"]="Journal of the Optical Society of America";

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
            auth.remove(QRegExp("[\\d\\*\\#\\+\\~\\!\\$\\&\\§\\?]"));
            QString a=reformatAuthors(auth, namePrefixes, nameAdditions, andWords);
            props["authors"]=a;
        }

        QRegExp rxJournal("Vol[\\.\\:\\s]*(\\d+).*No[\\.\\:\\s]*(\\d+)\\s*\\/\\s*([ a-zA-Z]*)\\s*(\\d*)\\s*\\n", Qt::CaseInsensitive);
        rxJournal.setMinimal(true);
        int count = 0;
        int pos = 0;
        QString firstPages="", lastPages="";
        while ((pos = rxJournal.indexIn(data, pos)) != -1) {
            props["volume"]=rxJournal.cap(1);
            props["issue"]=rxJournal.cap(2);
            props["journal"]=rxJournal.cap(3);
            if (firstPages.isEmpty()) firstPages=rxJournal.cap(4);
            else lastPages=rxJournal.cap(4);
            ++count;
            pos += rxJournal.matchedLength();
        }
        if (!firstPages.isEmpty() && !lastPages.isEmpty()) {
            props["pages"]=firstPages+"-"+lastPages;
        } else {
            props["pages"]=firstPages+lastPages;
        }

        bool hasYear=false;
        QRegExp rxYear("(jan|january|feb|february|march|mar|apr|april|may|jun|june|jul|july|aug|august|sep|sept|september|oct|october|nov|november|dec|december)\\s*(\\d+)\\s*\\n", Qt::CaseInsensitive);
        bool hasVol=false;
        QRegExp rxVol("vol[^\\s]*\\s+(\\d+)\\s*", Qt::CaseInsensitive);
        bool hasNumber=false;
        QRegExp rxNumber("num[^\\s]*\\s+(\\d+)\\s*", Qt::CaseInsensitive);
        for (int i=0; i<lines.size(); i++) {
            if (!hasYear && rxYear.indexIn(lines[i])>=0) {
                props["year"]=rxYear.cap(2);
                hasYear=true;
            }
            if (!hasVol && rxVol.indexIn(lines[i])>=0) {
                props["volume"]=rxVol.cap(1);
                hasVol=true;
            }
            if (!hasNumber && rxNumber.indexIn(lines[i])>=0) {
                props["issue"]=rxNumber.cap(1);
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
    QRegExp rxElsevier("(.+)[\\s]+([\\d]+)[\\s]+\\((\\d\\d\\d\\d)\\)[\\s]+(.+).*www.elsevier.com", Qt::CaseInsensitive);
    rxElsevier.setMinimal(true);
    if (rxElsevier.indexIn(lines[0])>=0) {
        props["journal"]=rxElsevier.cap(1);
        props["volume"]=rxElsevier.cap(2);
        props["year"]=rxElsevier.cap(3).toInt();
        props["pages"]=rxElsevier.cap(4);
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
            auth.remove(QRegExp("[\\d\\*\\#\\+\\~\\!\\$\\&\\§\\?]"));
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
    QRegExp rxBentham("\\s*(\\d\\d\\d\\d)\\s*Bentham\\sScience\\sPublishers", Qt::CaseInsensitive);
    if (rxBentham.indexIn(data)>=0) {
        props["year"]=rxBentham.cap(1);

        int i=0;
        bool isTitle=false;
        QRegExp rxJournal("(.+)\\s*,\\s+(\\d\\d\\d\\d)\\s*,\\s+(\\d+)\\s*,\\s+(.+)", Qt::CaseInsensitive);
        while ((i<lines.size()) && (lines[i].simplified().isEmpty()) && (!isTitle)) {
            if (rxJournal.indexIn(lines[i])>=0) {
                props["pages"]=rxJournal.cap(4);
                props["volume"]=rxJournal.cap(3);
                props["year"]=rxJournal.cap(2);
                props["journal"]=rxJournal.cap(1);
            } else {
                if (lines[i].simplified().contains(QRegExp("[A-Za-z]",Qt::CaseInsensitive))) isTitle=true;
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
            auth.remove(QRegExp("[\\d\\*\\#\\+\\~\\!\\$\\&\\§\\?]"));
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
    QRegExp rxProcSPIEFull("Proceedings\\s+of\\s+SPIE\\s+Vol.\\s*(\\d+)\\s+(.+),\\s+edited\\s+by\\s+(.*)\\s+\\(.+,\\s+(\\d\\d\\d\\d)\\)[^\\d]*([a-z\\d]{4,4}-[a-z\\d]{4,4}).*(\\d)+\\n", Qt::CaseInsensitive);
    QRegExp rxProcSPIESmall("Proceedings\\s+of\\s+SPIE\\s+Vol.\\s*(\\d+)\\s+(.+),\\s+edited\\s+by\\s+(.*)\\s+\\(.+,\\s+(\\d\\d\\d\\d)\\)[^\\d]*([a-z\\d]{4,4}-[a-z\\d]{4,4}).*(\\d)+\\n", Qt::CaseInsensitive);
    QRegExp& spieUse=rxProcSPIEFull;
    bool ok=false;
    int start=-1;
    if ((start=rxProcSPIEFull.indexIn(data))<0) {
        spieUse=rxProcSPIESmall;
        if ((start=rxProcSPIESmall.indexIn(data))<0) {

        } else ok=true;
    } else ok=true;

    if (ok) {
        start+=spieUse.matchedLength();
        props["title"]=lines[0];
        props["year"]=spieUse.cap(4);
        props["issn"]=spieUse.cap(5);
        props["type"]="inproceedings";
        props["volume"]=spieUse.cap(1);
        props["places"]="Bellingham";
        props["series"]="Proceedings of the SPIE";
        props["institution"]="SPIE";
        props["booktitle"]=spieUse.cap(2);
        props["editors"]=reformatAuthors(spieUse.cap(3), namePrefixes, nameAdditions, andWords);
        QString firstpage=spieUse.cap(6);
        QString lastPage="";
        if (!firstpage.isEmpty()) {
            while ((start=spieUse.indexIn(data, start))!=-1) {
                lastPage=spieUse.cap(6);
                start+=spieUse.matchedLength();
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
    QRegExp rxFull("Biophysical\\s+Journal\\s+Volume\\s+(\\d+).+(\\d\\d\\d\\d)\\s+(\\d+)[^\\d]+(\\d+)\\s+.*([a-z]+.*)\\s*\\n\\s*([a-z]+.*)\\n", Qt::CaseInsensitive);
    rxFull.setMinimal(true);
    QRegExp& rxUse=rxFull;
    bool ok=false;
    int start=-1;
    if ((start=rxUse.indexIn(data))<0) {
        /*rxUse=rxProcSPIESmall;
        if ((start=rxUse.indexIn(data))<0) {

        } else ok=true;*/
    } else ok=true;

    if (ok) {
        start+=rxUse.matchedLength();
        props["journal"]="Biophysical Journal";
        props["volume"]=rxUse.cap(1);
        props["year"]=rxUse.cap(2);
        props["pages"]=rxUse.cap(3)+"-"+rxUse.cap(4);
        props["title"]=rxUse.cap(5);
        props["authors"]=reformatAuthors(rxUse.cap(6), namePrefixes, nameAdditions, andWords);
        return true;
    }
    return false;
}



/////////////////////////////////////////////////////////
// specially recognize data from PNAS
/////////////////////////////////////////////////////////
bool recognizePNAS(const QString& data, QMap<QString, QString>& props, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes, QString pluginDir) {
    QStringList lines=data.trimmed().split('\n');
    QRegExp rxCheck("\\s+(\\d\\d\\d\\d)\\s+by\\s+The\\s+National\\s+Academy\\s+of\\s+Sciences\\s+of\\s+the\\s+USA", Qt::CaseInsensitive);
    QRegExp rxCheck2("PNAS\\s+.+\\s+(\\d\\d\\d\\d)[^\\d].+vol.+(\\d+)[^\\d].+no.+(\\d+)[^\\d].*(\\d+)", Qt::CaseInsensitive);

    if (rxCheck.indexIn(data)>=0 || rxCheck2.indexIn(data)>=0) {
        if (rxCheck.cap(1).size()>0) props["year"]=rxCheck.cap(1);
        if (rxCheck2.cap(1).size()>0) props["year"]=rxCheck2.cap(1);
        props["journal"]="Proceedings of the National Academy of Sciences of the USA";

        QRegExp rxPNAS=QRegExp("PNAS\\s+.+\\s+(\\d\\d\\d\\d)[^\\d].+vol.+(\\d+)[^\\d].+no.+(\\d+)[^\\d].*(\\d+)", Qt::CaseInsensitive);
        rxPNAS.setMinimal(true);
        int start=-1;
        if ((start=rxPNAS.indexIn(data))>=0) {
            props["year"]=rxPNAS.cap(1);
            props["volume"]=rxPNAS.cap(2);
            props["issue"]=rxPNAS.cap(3);
            QString firstpage=rxPNAS.cap(4);
            QString lastPage="";
            if (!firstpage.isEmpty()) {
                while ((start=rxPNAS.indexIn(data, start))!=-1) {
                    lastPage=rxPNAS.cap(4);
                    start+=rxPNAS.matchedLength();
                }
            }
            if (!firstpage.isEmpty()) {
                props["pages="]=firstpage;
                if (!lastPage.isEmpty() && (firstpage!=lastPage)) {
                    props["pages="]=firstpage+"-"+lastPage;
                }
            }
        }
        QRegExp rxDOI=QRegExp("doi\\s*(10\\..*)\\n", Qt::CaseInsensitive);
        rxDOI.setMinimal(true);
        if (rxDOI.indexIn(data)>=0) {
            props["doi"]=rxDOI.cap(1).simplified().replace(' ', '/');
        } else {
            QString data1=data;
            data1=data1.replace('?', '/');
            rxDOI=QRegExp("doi\\s*(10\\..*)\\n", Qt::CaseInsensitive);
            rxDOI.setMinimal(true);
            if (rxDOI.indexIn(data)>=0) {
                props["doi"]=rxDOI.cap(1).simplified().replace(' ', '/');
            } else {
                rxDOI=QRegExp("www.pnas.orgcgi[?\\/]doi[?\\/](10\\..*)\\n", Qt::CaseInsensitive);
                rxDOI.setMinimal(true);
                if (rxDOI.indexIn(data)>=0) {
                    props["doi"]=rxDOI.cap(1).simplified().replace(' ', '/');
                } else {
                    if (lines.size()>0) props["title"]=lines[0];
                    if (lines.size()>1) {
                        int spcnt=0;
                        spcnt+=lines[1].count('*');
                        spcnt+=lines[1].count('#');
                        spcnt+=lines[1].count('+');
                        spcnt+=lines[1].count('~');
                        spcnt+=lines[1].count('$');
                        spcnt+=lines[1].count('§');
                        spcnt+=lines[1].count('?');
                        spcnt+=lines[1].count('?');
                        QString aline=lines[1];
                        if (spcnt>1) {
                            props["title"]=props["title"]+lines[1];
                            aline=lines.value(2, "");
                        }
                        QString auth=aline.simplified();
                        auth.remove(QRegExp("[\\d\\*\\#\\+\\~\\!\\$\\&\\§\\?]"));
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
    QRegExp rxCheck("(IEEE[^,]*).+vol.+(\\d+).+no.+(\\d+).+(\\d\\d\\d\\d)\\s+(\\d+)\\s+([a-z]+.*)\\n.+([a-z]+.*)\\n", Qt::CaseInsensitive);

    if (rxCheck.indexIn(data)>=0) {
        props["journal"]=rxCheck.cap(1);
        props["volume"]=rxCheck.cap(2);
        props["issue"]=rxCheck.cap(3);
        props["year"]=rxCheck.cap(4);
        props["pages"]=rxCheck.cap(5)+"ff";
        props["title"]=rxCheck.cap(6);

        QString auth=rxCheck.cap(7).simplified();
        auth.remove(QRegExp("[\\d\\*\\#\\+\\~\\!\\$\\&\\§\\?]"));
        QString a=reformatAuthors(auth.simplified(), namePrefixes, nameAdditions, andWords);
        props["authors"]=a;

        QRegExp rxDOI=QRegExp("doi\\s+(10\\..*)\\n", Qt::CaseInsensitive);
        rxDOI.setMinimal(true);
        if (rxDOI.indexIn(data)>=0) {
            props["doi"]=rxDOI.cap(1).simplified().replace(' ', '/');
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
    QRegExp rxCheck("(\\d\\d\\d\\d).*\\sAmerican\\s*Physical\\s*Society", Qt::CaseInsensitive);

    if (rxCheck.indexIn(data)>=0) {
        props["year"]=rxCheck.cap(1);

        QRegExp rxAPS("(.+)\\s+vol(ume)?[\\.\\:\\,]?\\s*(\\d+).+(no|number)[\\.\\:\\,]?\\s*(\\d+).+(\\d\\d\\d\\d)\\s+([a-z]+.+)\\n([a-z]+.+)\\n", Qt::CaseInsensitive);
        rxAPS.setMinimal(true);

        if (rxAPS.indexIn(data)>=0) {
            props["journal"]=rxAPS.cap(1);
            props["volume"]=rxAPS.cap(3);
            props["issue"]=rxAPS.cap(5);
            props["year"]=rxAPS.cap(6);
            props["title"]=rxAPS.cap(7);
            QString auth=rxCheck.cap(8).simplified();
            if (auth.size()<8192) {
                auth.remove(QRegExp("[\\d\\*\\#\\+\\~\\!\\$\\&\\§\\?]"));
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
    QRegExp rxCheck("(\\d\\d\\d\\d).*\\sAmerican\\s*Chemical\\s*Society", Qt::CaseInsensitive);
    rxCheck.setMinimal(true);
    if (rxCheck.indexIn(data)>=0) {
        props["year"]=rxCheck.cap(1);

        QRegExp rxAPS("(.+)\\s+vol(ume)?[\\.\\:\\,]?\\s*(\\d+).+(no|number)[\\.\\:\\,]?\\s*(\\d+).+(\\d\\d\\d\\d)\\n", Qt::CaseInsensitive);
        rxAPS.setMinimal(true);

        if (rxAPS.indexIn(data)>=0) {
            props["journal"]=rxAPS.cap(1);
            props["volume"]=rxAPS.cap(3);
            props["issue"]=rxAPS.cap(5);
            props["year"]=rxAPS.cap(6);

            for (int i=0; i<lines.size(); i++) {
                if (lines.size()>0) props["title"]=lines[0];
                if (lines.size()>1) {
                    int spcnt=0;
                    spcnt+=lines[i].count('*');
                    spcnt+=lines[i].count('#');
                    spcnt+=lines[i].count('+');
                    spcnt+=lines[i].count('~');
                    spcnt+=lines[i].count('$');
                    spcnt+=lines[i].count('§');
                    spcnt+=lines[i].count('?');
                    spcnt+=lines[i].count('?');
                    QString aline=lines[i];
                    if (spcnt>1) {
                        QString auth=aline.simplified();
                        auth.remove(QRegExp("[\\d\\*\\#\\+\\~\\!\\$\\&\\§\\?]"));
                        QString a=reformatAuthors(auth, namePrefixes, nameAdditions, andWords);
                        props["authors"]=a;
                        break;
                    }
                }
            }
            props["title"]=rxAPS.cap(7);
            QString auth=rxCheck.cap(8).simplified();
            auth.remove(QRegExp("[\\d\\*\\#\\+\\~\\!\\$\\&\\§\\?]"));
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
    QRegExp rxCheck("\\s+(\\d\\d\\d\\d)\\s+Kluwer\\s+Academic\\s+Publisher", Qt::CaseInsensitive);

    if (rxCheck.indexIn(data)>=0) {
        props["year"]=rxCheck.cap(1);

        QRegExp rxData("([a-z\\s]+)\\s+(\\d+)\\((\\d+)\\)[,;]?\\s+(\\d+)[^\\d]+(\\d+)[,;]?\\s+(\\d\\d\\d\\d)", Qt::CaseInsensitive);
        rxData.setMinimal(true);
        if (rxData.indexIn(data)>=0) {
            props["journal"]=rxData.cap(1);
            props["volume"]=rxData.cap(2);
            props["issue"]=rxData.cap(3);
            props["pages"]=rxData.cap(4)+"-"+rxData.cap(5);

        }


        rxData=QRegExp("\\s+(\\d\\d\\d\\d)\\s+Kluwer\\s+Academic\\s+Publisher", Qt::CaseInsensitive);
        rxData.setMinimal(true);

        if (rxData.indexIn(data)>=0) {
            props["year"]=rxCheck.cap(1);

            QString auth=rxCheck.cap(8).simplified();
            auth.remove(QRegExp("[\\d\\*\\#\\+\\~\\!\\$\\&\\§\\?]"));
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
    QRegExp rxCheck("\\s*Rockefeller\\s*University\\s*Press\\s*", Qt::CaseInsensitive);

    if (rxCheck.indexIn(data)>=0) {
        //props["year"]=rxCheck.cap(1);

        //The Journal of Cell Biology, Volume 115, Number 1, October 1991 67-73
        QRegExp rxData("([a-z\\s]+).*\\s*vol\\w*\\s*(\\d+).*num\\w*\\s*(\\d+).*\\w+\\s*(\\d\\d\\d\\d).* (\\d+)\\s*[\\-]*\\s*(\\d+)", Qt::CaseInsensitive);
        rxData.setMinimal(true);
        if (rxData.indexIn(data)>=0) {
            props["journal"]=rxData.cap(1);
            props["volume"]=rxData.cap(2);
            props["issue"]=rxData.cap(3);
            props["pages"]=rxData.cap(4)+"-"+rxData.cap(5);

        }


        rxData=QRegExp("\\s+(\\d\\d\\d\\d)\\s+Kluwer\\s+Academic\\s+Publisher", Qt::CaseInsensitive);
        rxData.setMinimal(true);

        if (rxData.indexIn(data)>=0) {
            props["journal"]=rxData.cap(1);
            props["volume"]=rxData.cap(2);
            props["issue"]=rxData.cap(3);
            props["pages"]=rxData.cap(4)+"-"+rxData.cap(5);

            QString auth=rxCheck.cap(8).simplified();
            auth.remove(QRegExp("[\\d\\*\\#\\+\\~\\!\\$\\&\\§\\?]"));
            QString a=reformatAuthors(auth.simplified(), namePrefixes, nameAdditions, andWords);
            props["authors"]=a;
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


    QString params=" \""+filename+"\" \""+tempFile+"\"";
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
            QRegExp rxDoi("doi[\\:]?\\s*(10.[a-zA-Z\\d\\.\\/\\(\\)\\-\\_]*)", Qt::CaseInsensitive);
            if (rxDoi.indexIn(contents)>=0) {
                QString doi=rxDoi.cap(1);
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
            QRegExp rxYear;
            bool hasYear=false;
            rxYear=QRegExp("Available[^\\n]*\\s(\\d\\d\\d\\d)\\s");
            if (!hasYear && rxYear.indexIn(contents)>=0) {
                //qDebug()<<"found year @ "<<rxYear.indexIn(contents)<<"   ="<<rxYear.cap(0).trimmed();
                bool ok=true;
                int y=rxYear.cap(1).toInt(&ok);
                if (ok && (y>1900) && (y<=QDate::currentDate().year()+100)) {
                    props["year"]=rxYear.cap(1);
                    hasYear=true;
                }
            }
            rxYear=QRegExp("accepted[^\\n]*\\s(\\d\\d\\d\\d)\\s");
            if (!hasYear && rxYear.indexIn(contents)>=0) {
                //qDebug()<<"found year @ "<<rxYear.indexIn(contents)<<"   ="<<rxYear.cap(0).trimmed();
                bool ok=true;
                int y=rxYear.cap(1).toInt(&ok);
                if (ok && (y>1900) && (y<=QDate::currentDate().year()+100)) {
                    props["year"]=rxYear.cap(1);
                    hasYear=true;
                }
            }
            rxYear=QRegExp("\\s(\\d\\d\\d\\d)\\s");
            if (!hasYear && rxYear.indexIn(contents)>=0) {
                //qDebug()<<"found year @ "<<rxYear.indexIn(contents)<<"   ="<<rxYear.cap(0).trimmed();
                bool ok=true;
                int y=rxYear.cap(1).toInt(&ok);
                if (ok && (y>1900) && (y<=QDate::currentDate().year()+100)) {
                    props["year"]=rxYear.cap(1);
                    hasYear=true;
                }
            }

            #ifdef DEBUG_PDF
            dbgtxt<<"search year : "<<props.value("year", "")<<"\n=== elapsed: "<<timerdbg.elapsed()<<"ms\n";
            dbgtxt.flush();
            timerdbg.start();

            #endif
            bool hasAbstract=false;
            QRegExp rxIntro("a\\s*b\\s*s\\s*t\\s*r\\s*a\\s*c\\s*t\\s([\xA9]*)\\n\\n", Qt::CaseInsensitive);
            if (rxIntro.indexIn(contents)>=0) {
                if (rxIntro.cap(1).size()<=contents.size()/10){
                    props["abstract"]=rxIntro.cap(1);
                    hasAbstract=true;
                }
            }
            if (!hasAbstract) {
                QRegExp rxIntro("i\\s*n\\s*t\\s*r\\s*o\\s*d\\s*u\\s*c\\s*t\\s*i\\s*o\\s*n\\s([^\xA9]*)\\n\\n", Qt::CaseInsensitive);
                if (rxIntro.indexIn(contents)>=0) {
                    if (rxIntro.cap(1).size()<=contents.size()/10){
                        props["abstract"]=rxIntro.cap(1);
                        hasAbstract=true;
                    }
                }

            }
            if (!hasAbstract) {
                QRegExp rxIntro("i\\s*n\\s*t\\s*r\\s*o\\s*d\\s*u\\s*c\\s*t\\s*i\\s*o\\s*n\\s([^\xA9\\n]*)\\n", Qt::CaseInsensitive);
                if (rxIntro.indexIn(contents)>=0) {
                    if (rxIntro.cap(1).size()<=contents.size()/10){
                        props["abstract"]=rxIntro.cap(1);
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
            QRegExp rxkw;
            rxkw=QRegExp("(k\\s*e\\s*y\\s*w\\s*o\\s*r\\s*d\\s*s|o\\s*c\\s*i\\s*s\\s*c\\s*o\\s*d\\s*e\\s*s|i\\s*n\\s*d\\s*e\\s*x\\s*t\\s*e\\s*r\\s*m\\s*s|i\\s*n\\s*d\\s*e\\s*x\\s*h\\s*e\\s*a\\s*d\\s*i\\s*n\\s*g\\s*s)[\\:\\-Ð]?[\\s]*(.*)\\n\\n", Qt::CaseInsensitive);
            if (!haskeywords && rxkw.indexIn(contents)>=0) {
                QString kw=rxkw.cap(2).simplified();
                if (kw.size()<contents.size()/20) {
                    if (kw.contains(",") && kw.contains(";")) {
                        kw.replace("; ", "\n");
                        kw.replace(";", "\n");
                    } else {
                        kw.replace("; ", "\n");
                        kw.replace(", ", "\n");
                        kw.replace(';', '\n');
                        kw.replace(',', '\n');
                        kw.replace('·', '\n');
                    }
                    if (kw.simplified().size()>0) {
                        props["keywords"]=kw.simplified();
                        haskeywords=true;
                    }
                }
            }
            rxkw=QRegExp("(k\\s*e\\s*y\\s*w\\s*o\\s*r\\s*d\\s*s|o\\s*c\\s*i\\s*s\\s*c\\s*o\\s*d\\s*e\\s*s|i\\s*n\\s*d\\s*e\\s*x\\s*t\\s*e\\s*r\\s*m\\s*s|i\\s*n\\s*d\\s*e\\s*x\\s*h\\s*e\\s*a\\s*d\\s*i\\s*n\\s*g\\s*s)[\\:\\-Ð]?[\\s]*([^\\n]*)\\n", Qt::CaseInsensitive);
            if (!haskeywords && rxkw.indexIn(contents)>=0) {
                QString kw=rxkw.cap(2).simplified();
                if (kw.contains(",") && kw.contains(";")) {
                    kw.replace("; ", "\n");
                    kw.replace(";", "\n");
                } else {
                    kw.replace("; ", "\n");
                    kw.replace(", ", "\n");
                    kw.replace(';', '\n');
                    kw.replace(',', '\n');
                    kw.replace('·', '\n');
                }
                if (kw.simplified().size()>0) {
                    props["keywords"]=kw.simplified();
                    haskeywords=true;
                }
            }
            rxkw=QRegExp("(k\\s*e\\s*y\\s*w\\s*o\\s*r\\s*d\\s*s|o\\s*c\\s*i\\s*s\\s*c\\s*o\\s*d\\s*e\\s*s|i\\s*n\\s*d\\s*e\\s*x\\s*t\\s*e\\s*r\\s*m\\s*s|i\\s*n\\s*d\\s*e\\s*x\\s*h\\s*e\\s*a\\s*d\\s*i\\s*n\\s*g\\s*s)[\\:\\-Ð]?[\\s]*([^\\n]*)\\n", Qt::CaseInsensitive);
            if (!haskeywords && rxkw.indexIn(contents)>=0) {
                QString kw=rxkw.cap(2).simplified();
                if (kw.contains(",") && kw.contains(";")) {
                    kw.replace("; ", "\n");
                    kw.replace(";", "\n");
                } else {
                    kw.replace("; ", "\n");
                    kw.replace(", ", "\n");
                    kw.replace(';', '\n');
                    kw.replace(',', '\n');
                    kw.replace('·', '\n');
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




















