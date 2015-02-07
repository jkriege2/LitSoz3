#include "pdftools.h"
#include "bibtools.h"


/////////////////////////////////////////////////////////
// specially recognize data from OSA journals
/////////////////////////////////////////////////////////
bool recognizeOSA(const QString& data, QMap<QString, QString>& props, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes, QString pluginDir) {
    QStringList lines=data.trimmed().split('\n');
    QRegExp rxOSA("©\\s*(\\d\\d\\d\\d)\\s*Optical\\sSociety", Qt::CaseInsensitive);
    if (rxOSA.indexIn(data)>=0) {
        props["year"]=rxOSA.cap(1);

       /* QRegExp rxTitle("([ \\d\\w\\-\\:\\;\\,\\.\\?\\(\\)\\[\\]\\%\\&\\\"\\']*)\\n", Qt::CaseInsensitive);
        rxTitle.setMinimal(true);
        if (rxTitle.indexIn(data) != -1) {
            props["title"]=rxTitle.cap(1);
        }*/

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

        QRegExp rxJournal("Vol[\\.\\:\\s]*(\\d*).*No[\\.\\:\\s]*(\\d*)\\s*\\/\\s*([ a-zA-Z]*)\\s*(\\d*)\\s*\\n", Qt::CaseInsensitive);
        rxJournal.setMinimal(true);
        int count = 0;
        int pos = 0;
        QString firstPages="", lastPages="";
        while ((pos = rxJournal.indexIn(data, pos)) != -1) {
            props["volume"]=rxJournal.cap(1);
            props["number"]=rxJournal.cap(2);
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
    QRegExp rxOSA("©\\s*(\\d\\d\\d\\d)\\s*Bentham\\sScience\\sPublishers", Qt::CaseInsensitive);
    if (rxOSA.indexIn(data)>=0) {
        props["year"]=rxOSA.cap(1);

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
    QRegExp rxCheck("©\\s+(\\d\\d\\d\\d)\\s+by\\s+The\\s+National\\s+Academy\\s+of\\s+Sciences\\s+of\\s+the\\s+USA", Qt::CaseInsensitive);

    if (rxCheck.indexIn(data)>=0) {
        props["year"]=rxCheck.cap(1);
        props["journal"]="Proceedings of the National Academy of Sciences of the USA";

        QRegExp rxPNAS=QRegExp("PNAS\\s+.+,\\s+(\\d\\d\\d\\d).+vol.+(\\d+).+no.+(\\d+)\\s+(\\d+)\\s", Qt::CaseInsensitive);
        rxPNAS.setMinimal(true);
        int start=-1;
        if ((start=rxPNAS.indexIn(data))>=0) {
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
        QRegExp rxDOI=QRegExp("doi\\s+(10\\..*)\\n", Qt::CaseInsensitive);
        rxDOI.setMinimal(true);
        if (rxDOI.indexIn(data)>=0) {
            props["doi"]=rxDOI.cap(1).simplified().replace(' ', '/');
        } else {
            if (lines.size()>0) props["title"]=lines[0];
            if (lines.size()>1) {
                QString auth=lines[1].simplified();
                auth.remove(QRegExp("[\\d\\*\\#\\+\\~\\!\\$\\&\\§\\?]"));
                QString a=reformatAuthors(auth, namePrefixes, nameAdditions, andWords);
                props["authors"]=a;
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
    QStringList lines=data.trimmed().split('\n');
    QRegExp rxCheck("©\\s+(\\d+)\\s+The\\s+American\\s+Physical\\s+Society", Qt::CaseInsensitive);

    if (rxCheck.indexIn(data)>=0) {
        props["year"]=rxCheck.cap(1);

        QRegExp rxAPS("(.+)\\s+vol(ume)?\\s+(\\d+).+(no|number)\\s+(\\d+).+(\\d\\d\\d\\d)\\s+([a-z]+.+)\\n([a-z]+.+)\\n", Qt::CaseInsensitive);
        rxAPS.setMinimal(true);

        if (rxAPS.indexIn(data)>=0) {
            props["journal"]=rxAPS.cap(1);
            props["volume"]=rxAPS.cap(3);
            props["issue"]=rxAPS.cap(5);
            props["year"]=rxAPS.cap(6);
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
    QRegExp rxCheck("[©c]{1}\\s+(\\d\\d\\d\\d)\\s+Kluwer\\s+Academic\\s+Publisher", Qt::CaseInsensitive);

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


        rxData=QRegExp("[©c]{1}\\s+(\\d\\d\\d\\d)\\s+Kluwer\\s+Academic\\s+Publisher", Qt::CaseInsensitive);
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
#ifdef Q_OS_WIN32

    qDebug()<<"running "<<pd.absoluteFilePath("pdftools/pdftotext.exe")+params;
    QProcess::execute(pd.absoluteFilePath("pdftools/pdftotext.exe")+params);

#else
    qDebug()<<"running "<<pd.absoluteFilePath("pdftotext.exe")+params;
    QProcess::execute("pdftotext"+params);
#endif

    if (QFile::exists(tempFile)) {
        QFile f(tempFile);
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QString contents=f.readAll();

            //qDebug()<<contents;

            /////////////////////////////////////////////////////////
            // common metadata:
            /////////////////////////////////////////////////////////
            QRegExp rxDoi("doi[\\:]?[\\s]*(10.[a-zA-Z\\d\\.\\/\\(\\)\\-\\_]*)", Qt::CaseInsensitive);
            if (rxDoi.indexIn(contents)>=0) {
                QString doi=rxDoi.cap(1);
                QChar last=doi[doi.size()-1];
                //qDebug()<<doi;
                while ((doi.size()>4)&&(!(last.isLetterOrNumber() || QString("./()-_").contains(last)))) {
                    doi=doi.left(doi.size()-1);
                    last=doi[doi.size()-1];
                }
                //qDebug()<<doi;

                if (doi.size()>0) props["doi"]=doi;
            }

            QRegExp rxYear("\\s(\\d\\d\\d\\d)\\s");
            if (rxYear.indexIn(contents)>=0) {
                //qDebug()<<"found year @ "<<rxYear.indexIn(contents)<<"   ="<<rxYear.cap(0).trimmed();
                bool ok=true;
                int y=rxYear.cap(1).toInt(&ok);
                if (ok && (y>999) && (y<=QDate::currentDate().year()+100)) props["year"]=rxYear.cap(1);
            }

            QRegExp rxIntro("abstract\\s([^\\n©]*)", Qt::CaseInsensitive);
            if (rxIntro.indexIn(contents)>=0) {
                props["abstract"]=rxIntro.cap(1);
            } else {
                QRegExp rxIntro("introduction\\s([^\\n©]*)", Qt::CaseInsensitive);
                if (rxIntro.indexIn(contents)>=0) {
                    props["abstract"]=rxIntro.cap(1);
                }

            }

            QRegExp rxkw("(keywords|ocis codes|index terms)[\\:\\-Ð]?[\\s]*([^\\n]*)\\n", Qt::CaseInsensitive);
            if (rxkw.indexIn(contents)>=0) {
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
                props["keywords"]=kw.simplified();
            }

            /////////////////////////////////////////////////////////
            // specially recognize data from some journals
            /////////////////////////////////////////////////////////
            bool ok=false;
            ok=recognizeOSA(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            if (!ok) ok=recognizeElsevier(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            if (!ok) ok=recognizeBentham(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            if (!ok) ok=recognizeSPIE(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            if (!ok) ok=recognizeBiophysJ(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            if (!ok) ok=recognizePNAS(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            if (!ok) ok=recognizeIEEE(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            if (!ok) ok=recognizeAPS(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);
            if (!ok) ok=recognizeKluwer(contents, props, andWords, nameAdditions, namePrefixes, pluginDir);


            f.close();
            f.remove();

        }

        //QFile::remove(tempFile);
    }

    return props;
}




















