#include "languagetools.h"
#include <QDomDocument>
#include <QFile>
#include <QDebug>
#include <QDir>
static LanguageRecognizer ___LanguageRecognizerGlobalInstance___LS3;



LanguageRecognizer::LanguageRecognizer()
{
    m_initialized=false;
}

void LanguageRecognizer::init(const QStringList &datafiles)
{
    m_initialized=true;
    for (int i=0; i<datafiles.size(); i++) {
        QFile f(datafiles[i]);
        if (f.open(QFile::ReadOnly)) {
            QDomDocument doc;
            doc.setContent(f.readAll());
            QDomElement e=doc.documentElement();
            if (e.tagName()=="language_ngram_database") {
                QString lang=e.attribute("lang");
                qDebug()<<i<<": loading "<<datafiles[i]<<"  LANG="<<lang;
                LangDB db;
                QDomElement de=e.firstChildElement("ngram");
                while (!de.isNull()) {
                    db.ngramcount.append(qMakePair(de.attribute("ng"), de.attribute("cnt").toInt()));
                    db.ngramposmap.insert(de.attribute("ng"), db.ngramcount.size()-1);
                    de=de.nextSiblingElement("ngram");
                }
                m_dbs.insert(lang, db);
            }
            f.close();
        }
    }

}

void LanguageRecognizer::initFromDirs(const QStringList &datadirs)
{
    QStringList files;
    for (int j=0; j<datadirs.size(); j++) {
        QDir d(datadirs[j]);
        QStringList filts;
        filts<<"*.ldb";
        QStringList fs=d.entryList(filts, QDir::Files);
        for (int i=0; i<fs.size(); i++) {
            files<<d.absoluteFilePath(fs[i]);
        }
    }
    init(files);
}

QString LanguageRecognizer::recognizeLanguage(const QString &text, double* distance)
{
    QHash<QString, int> hash;
    ___LanguageRecognizerGlobalInstance___LS3.splitNGrams(hash, text, 1);
    ___LanguageRecognizerGlobalInstance___LS3.splitNGrams(hash, text, 2);
    ___LanguageRecognizerGlobalInstance___LS3.splitNGrams(hash, text, 3);
    ___LanguageRecognizerGlobalInstance___LS3.splitNGrams(hash, text, 4);
    ___LanguageRecognizerGlobalInstance___LS3.splitNGrams(hash, text, 5);
    QList<QPair<QString, int> > ngramlist=___LanguageRecognizerGlobalInstance___LS3.hashToList(hash);
    QString lang="";
    double minD=0;
    QMapIterator<QString, LangDB> it(m_dbs);
    int i=0;
    while (it.hasNext()) {
        it.next();
        double d=it.value().distance(ngramlist);
        if (d<it.value().getMaxUseDist()) {
            if (i==0) {
                minD=d;
                lang=it.key();
            } else {
                if (d<minD) {
                    minD=d;
                    lang=it.key();
                }
            }
            i++;
        }
    }
    if (distance) *distance=minD;
    return lang;
}

void LanguageRecognizer::clearInit()
{
    m_dbs.clear();
    m_initialized=false;
}

QString LanguageRecognizer::cleanText(const QString &text)
{
    QString txt;
    for (int i=0; i<text.size(); i++) {
        if ((text[i].isSpace() || text[i].category()==QChar::Letter_Lowercase || text[i].category()==QChar::Letter_Uppercase || text[i].category()==QChar::Letter_Other || text[i]=='\'') && !(text[i].isNumber() || text[i].isPunct())) {
            txt+=text[i].toLower();
        }
    }
    return txt.trimmed().simplified();
}

void LanguageRecognizer::splitNGrams(QHash<QString, int> &hash, const QString &text, int length)
{
    QString txt=cleanText(text);
    txt=QString(length-1, ' ')+txt+QString(length-1, ' ');
    for (int i=0; i<txt.size()-length+1; i++) {
        QString ngram=txt.mid(i, length);
        if (ngram.count(' ')<ngram.size()) {
            if (!hash.contains(ngram)) {
                hash.insert(ngram, 1);
            } else {
                hash[ngram]++;
            }
        }
    }
}

bool LanguageRecognizer_notLessThan(const QPair<QString, int>& p1, const QPair<QString, int>& p2) {
    return p1.second>=p2.second;
}

QList<QPair<QString, int> > LanguageRecognizer::hashToList(const QHash<QString, int> &hash, int minOccurence)
{
    QList<QPair<QString, int> > l;

    QHashIterator<QString, int> it(hash);
    while (it.hasNext()) {
        it.next();
        if (minOccurence<0 || it.value()>=minOccurence) l.append(qMakePair(it.key(), it.value()));
    }

    qSort(l.begin(), l.end(), LanguageRecognizer_notLessThan);

    return l;
}

LanguageRecognizer *LanguageRecognizer::globalInstance()
{
    return &___LanguageRecognizerGlobalInstance___LS3;
}

double LanguageRecognizer::getMaxUseDist(int firstNNgrams)
{
    return 15*firstNNgrams;
}


double LanguageRecognizer::LangDB::distance(const QList<QPair<QString, int> > &other, int firstNNgrams) const
{
    long long dist=0;
    const long long MAX_DIST=10000;
    int maxN=qMin(other.size(), firstNNgrams);
    if (firstNNgrams<0) maxN=other.size();
    for (int i=0; i<maxN; i++) {
        QString ngram=other[i].first;
        int ngramIx=ngramposmap.value(ngram, -1);
        if (ngramIx<0) dist+=MAX_DIST;
        else dist+=abs(i-ngramIx);
    }
    return double(dist)/double(maxN);
}

double LanguageRecognizer::LangDB::getMaxUseDist(int firstNNgrams) const
{
    return LanguageRecognizer::getMaxUseDist(firstNNgrams);
}






QString recognizeLanguage(const QString& text, double* distance) {
    QString lang="";

    if (!___LanguageRecognizerGlobalInstance___LS3.isInitialize()) {
        QStringList dirs;
        dirs<<":/language_recognition/";
        dirs<<"./assets/language_recognition/";
        ___LanguageRecognizerGlobalInstance___LS3.initFromDirs(dirs);
    }

    /*QHash<QString, int> hash;
    ___LanguageRecognizerGlobalInstance___LS3.splitNGrams(hash, text, 1);
    ___LanguageRecognizerGlobalInstance___LS3.splitNGrams(hash, text, 2);
    ___LanguageRecognizerGlobalInstance___LS3.splitNGrams(hash, text, 3);
    ___LanguageRecognizerGlobalInstance___LS3.splitNGrams(hash, text, 4);
    ___LanguageRecognizerGlobalInstance___LS3.splitNGrams(hash, text, 5);
    QList<QPair<QString, int> > ngramlist=___LanguageRecognizerGlobalInstance___LS3.hashToList(hash);
    //qDebug()<<"## reg NGRAMS size="<<ngramlist.size();
    QString xml;
    for (int i=0; i<qMin(3000,ngramlist.size()); i++) {
        xml+=QString("<ngram ng=\"%1\" cnt=\"%2\"/>\n").arg(ngramlist[i].first).arg(ngramlist[i].second);
        //if (i<=200) qDebug()<<"  "<<ngramlist[i].first<<": "<<ngramlist[i].second;
    }

    QFile f("./langrec.txt");
    if (f.open(QFile::Append)) {
        f.write(QByteArray("\n\n\n\n\n"));
        f.write(text.left(100).simplified().trimmed().toUtf8());
        f.write(QByteArray("\n\n"));
        f.write(xml.toUtf8());
        f.close();
    }*/

    lang=___LanguageRecognizerGlobalInstance___LS3.recognizeLanguage(text, distance);

    return lang;
}


