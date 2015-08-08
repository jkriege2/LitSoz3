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

#include "../bibtools.h"
#include "../htmltokenizer.h"
#include "../htmltools.h"
#include "../languagetools.h"

#include <iostream>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QElapsedTimer>

#define TEST_REFORMATAUHTORS(pattern) { QString t=pattern; qDebug()<<"  pattern = '"<<pattern<<"'\n    -> '"<<reformatAuthors(t, name_prefixes, name_additions, ands)<<"'\n\n"; };
#define TEST_BUILDLANGNGRAMBOOK(FILENM) {\
        filename=FILENM;\
        txt="";\
        qDebug()<<"reading data "<<filename;\
        QFile file(filename);\
        QHash<QString, int> hash;\
        if (file.open(QIODevice::ReadOnly)) { \
          while (!file.atEnd()) {\
              txt+=QString::fromUtf8(file.readLine()); \
              if (txt.size()>4096) {\
                std::cout<<"."; \
                LanguageRecognizer::splitNGrams(hash, txt, 1);\
                LanguageRecognizer::splitNGrams(hash, txt, 2);\
                LanguageRecognizer::splitNGrams(hash, txt, 3);\
                LanguageRecognizer::splitNGrams(hash, txt, 4);\
                LanguageRecognizer::splitNGrams(hash, txt, 5);\
                txt="";\
              } \
          }\
          file.close(); \
        }\
        std::cout<<"\nsorting N-gram histogram:\n"; \
        QList<QPair<QString, int> > ngramlist=LanguageRecognizer::hashToList(hash, 10); \
        QString xml;\
        std::cout<<"\nwriting N-gram XML-data:\n"; \
        for (int i=0; i<qMin(3000,ngramlist.size()); i++) {\
            xml+=QString("<ngram ng=\"%1\" cnt=\"%2\"/>\n").arg(ngramlist[i].first).arg(ngramlist[i].second);\
        }\
        QFile f("./langrec.txt");\
        if (f.open(QFile::Append)) {\
            f.write(QByteArray("\n\n\n\n\n"));\
            f.write(QByteArray(FILENM));\
            f.write(QByteArray("\n\n"));\
            f.write(xml.toUtf8());\
            f.close();\
        }\
    }

#define MULTITEST_RECOGNIZELANG(FILENM) {\
    filename=FILENM;\
    txt="";\
    QFile file(filename);\
    if (file.open(QIODevice::ReadOnly)) { \
        while (!file.atEnd()) {\
            txt+=QString::fromUtf8(file.readLine()); \
        }\
        /*txt=QString::fromUtf8(file.readAll());*/ \
        QStringList lst=txt.split("\n\n"); \
        QMap<QString, int> langCnt; \
        double t=0, cnt=0, l=0, d=0; \
        for (int ii=0; ii<lst.size(); ii++) { \
            if (LanguageRecognizer::cleanText(lst[ii]).size()>5) {\
                QElapsedTimer time; \
                double dist=0; \
                time.start(); \
                QString lang=recognizeLanguage(lst[ii], &dist); \
                t=t+time.nsecsElapsed()/1e6; cnt++;\
                l=l+lst[ii].size(); \
                d=d+dist; \
                if (!langCnt.contains(lang)) langCnt.insert(lang, 1); \
                else langCnt[lang]++; \
            } \
        } \
        file.close(); \
        QMapIterator<QString, int> it(langCnt); \
        qDebug()<<"recognizeLanguages("<<filename<<")  ~ "<<t/cnt<<"ms / recognition,   avg. length: "<<l/cnt<<",   avg. distance: "<<d/cnt<<",   avg. distance/500: "<<d/cnt/500.0<<",   cnt="<<cnt; \
        while (it.hasNext()) { \
            it.next(); \
            qDebug()<<it.key()<<": "<<double(it.value())/cnt*100.0<<"%"; \
        }\
    }\
}

int main(int argc, char * argv[]) {

    bool testTokenizer=false;
    bool testCOINS=false;
    bool testAuthors=false;
    bool testLangRec=true;
    bool buildLangRec=false;

    QSet<QString> name_additions, name_prefixes;
    QList<QString> ands;
    name_additions<<"jr"<<"sen"<<"i"<<"ii"<<"iii"<<"iv"<<"jun";
    name_prefixes<<"da"<<"de"<<"dal"<<"del"<<"der"<<"di"<<"do"<<"du"<<"dos"<<"la"<<"le"<<"van"<<"vande"<<"von"<<"den";
    ands<<"and"<<"und"<<"&"<<"et";

    QFile file("../lib/test/test1.html");
    QString html="";
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) { html=file.readAll(); file.close(); }

    if (testTokenizer) {
        qDebug()<<"----------------------------------------------------------------------";
        qDebug()<<"-- Test HTML TOKENIZER                                              --";
        qDebug()<<"----------------------------------------------------------------------\n";
        HtmlTokenizer tokenizer(html);
        HtmlTokenizer::Token t=tokenizer.getNextToken();
        //qDebug()<<(int)t.type<<t.contents;
        while (t.type != HtmlTokenizer::ENDOFFILE) {
            if (t.type!=HtmlTokenizer::TEXT && t.type!=HtmlTokenizer::COMMENT) t.parse();
            qDebug()<<t.type<<t.tagName<<t.properties<<t.contents<<t.completeContents;

            t=tokenizer.getNextToken();
        }
    }

    if (testCOINS) {
        qDebug()<<"----------------------------------------------------------------------";
        qDebug()<<"-- COinS extraction                                                 --";
        qDebug()<<"----------------------------------------------------------------------\n";
        QList<QMap<QString, QVariant> > c=extractCoins(html, name_prefixes, name_additions, ands);
        for (int i=0; i<c.size(); i++) qDebug()<<c[i]<<"\n\n";
    }


    if (testAuthors) {
        qDebug()<<"----------------------------------------------------------------------";
        qDebug()<<"-- Test reformatAuthors()                                           --";
        qDebug()<<"----------------------------------------------------------------------\n";

        TEST_REFORMATAUHTORS("Krieger, J.")
        TEST_REFORMATAUHTORS("J. Krieger")
        TEST_REFORMATAUHTORS("I. Krieger I.")
        TEST_REFORMATAUHTORS("Krieger I., I.")
        TEST_REFORMATAUHTORS("Jan Krieger III. sen.")
        TEST_REFORMATAUHTORS("Krieger, Jan")
        TEST_REFORMATAUHTORS("Jan Krieger")
        TEST_REFORMATAUHTORS("Krieger III. sen., Jan")
        TEST_REFORMATAUHTORS("Bush Jr., George W.")
        TEST_REFORMATAUHTORS("George W. Bush Jr.")
        TEST_REFORMATAUHTORS("Bush Jr., George W.")
        TEST_REFORMATAUHTORS("Frank Maria Schuster")
        TEST_REFORMATAUHTORS("Schuster, Frank Maria")
        TEST_REFORMATAUHTORS("Jan Krieger; Wolfgang Krieger; Ute Kort-Krieger; Frank Maria Schuster; George W. Bush Jr.")
        TEST_REFORMATAUHTORS("Jan Krieger, Wolfgang Krieger, Ute Kort-Krieger, Frank Maria Schuster, George W. Bush Jr.")
        TEST_REFORMATAUHTORS("Jan Krieger / Wolfgang Krieger / Ute Kort-Krieger / Frank Maria Schuster / George W. Bush Jr.")
        TEST_REFORMATAUHTORS("Jan Krieger/Wolfgang Krieger/Ute Kort-Krieger/Frank Maria Schuster/George W. Bush Jr.")
        TEST_REFORMATAUHTORS("Jan Krieger \\ Wolfgang Krieger \\ Ute Kort-Krieger \\ Frank Maria Schuster \\ George W. Bush Jr.")
        TEST_REFORMATAUHTORS("Jan Krieger\\Wolfgang Krieger\\Ute Kort-Krieger\\Frank Maria Schuster\\George W. Bush Jr.")
        TEST_REFORMATAUHTORS("Krieger, Jan; Krieger, Wolfgang; Kort-Krieger, Ute; Schuster, Frank Maria; Bush Jr., George W.")
        TEST_REFORMATAUHTORS("Krieger, Jan, Krieger, Wolfgang, Kort-Krieger, Ute, Schuster, Frank Maria, Bush Jr., George W.")

        TEST_REFORMATAUHTORS("Jan Krieger; Wolfgang Krieger; Ute Kort-Krieger; Frank Maria Schuster; George W. Bush")
        TEST_REFORMATAUHTORS("Jan Krieger, Wolfgang Krieger, Ute Kort-Krieger, Frank Maria Schuster, George W. Bush")
        TEST_REFORMATAUHTORS("Jan Krieger / Wolfgang Krieger / Ute Kort-Krieger / Frank Maria Schuster / George W. Bush")
        TEST_REFORMATAUHTORS("Jan Krieger/Wolfgang Krieger/Ute Kort-Krieger/Frank Maria Schuster/George W. Bush")
        TEST_REFORMATAUHTORS("Jan Krieger \\ Wolfgang Krieger \\ Ute Kort-Krieger \\ Frank Maria Schuster \\ George W. Bush")
        TEST_REFORMATAUHTORS("Jan Krieger\\Wolfgang Krieger\\Ute Kort-Krieger\\Frank Maria Schuster\\George W. Bush")
        TEST_REFORMATAUHTORS("Krieger, Jan; Krieger, Wolfgang; Kort-Krieger, Ute; Schuster, Frank Maria; Bush, George W.")
        TEST_REFORMATAUHTORS("Krieger, Jan, Krieger, Wolfgang, Kort-Krieger, Ute, Schuster, Frank Maria, Bush, George W.")
        TEST_REFORMATAUHTORS("J. Krieger; W. Krieger; U. Kort-Krieger; F. M. Schuster; GW. Bush")
        TEST_REFORMATAUHTORS("J. Krieger, W. Krieger, U. Kort-Krieger, F. M. Schuster, GW. Bush")
        TEST_REFORMATAUHTORS("J. Krieger / W. Krieger / U. Kort-Krieger / F. M. Schuster / GW. Bush")
        TEST_REFORMATAUHTORS("J. Krieger/W. Krieger/U. Kort-Krieger/F. M. Schuster/GW. Bush Jr.")
        TEST_REFORMATAUHTORS("J. Krieger \\ W. Krieger \\ U. Kort-Krieger \\ F. M. Schuster \\ GW. Bush Jr.")
        TEST_REFORMATAUHTORS("J. Krieger\\W. Krieger\\U. Kort-Krieger\\F. M. Schuster\\GW. Bush Jr.")
        TEST_REFORMATAUHTORS("Krieger, J.; Krieger, W.; Kort-Krieger, U.; Schuster, F. M.; Bush Jr., GW.")
        TEST_REFORMATAUHTORS("Krieger, J., Krieger, W., Kort-Krieger, U., Schuster, F. M., Bush Jr., GW.")

        TEST_REFORMATAUHTORS("van Krieger, J.")
        TEST_REFORMATAUHTORS("J. van Krieger")
        TEST_REFORMATAUHTORS("I. van Krieger I.")
        TEST_REFORMATAUHTORS("van Krieger I., I.")
        TEST_REFORMATAUHTORS("Jan van den Krieger III. sen.")
        TEST_REFORMATAUHTORS("van den Krieger, Jan")
        TEST_REFORMATAUHTORS("Jan van den Krieger")
        TEST_REFORMATAUHTORS("van den Krieger III. sen., Jan")

        TEST_REFORMATAUHTORS("Jan Krieger and Wolfgang Krieger and Ute Kort-Krieger and Frank Maria Schuster and George W. Bush")
        TEST_REFORMATAUHTORS("Jan Krieger; Wolfgang Krieger; Ute Kort-Krieger; Frank Maria Schuster and George W. Bush")
        TEST_REFORMATAUHTORS("Krieger, Jan, Krieger, Wolfgang, Kort-Krieger, Ute, Schuster, Frank Maria and Bush Jr., George W.")
        TEST_REFORMATAUHTORS("Krieger, Jan and Krieger, Wolfgang and Kort-Krieger, Ute and Schuster, Frank Maria and Bush Jr., George W.")
        TEST_REFORMATAUHTORS("Jan Krieger and Wolfgang Krieger")
        TEST_REFORMATAUHTORS("J. Krieger and W. Krieger")
        TEST_REFORMATAUHTORS("J. Krieger and W. G. Krieger")
        TEST_REFORMATAUHTORS("J. KRIEGER and W. G. MAYER")
        TEST_REFORMATAUHTORS("J. W. KRIEGER, U.E. Majer, and W. G. MAYER")
        TEST_REFORMATAUHTORS("J KRIEGER and W G MAYER")
        TEST_REFORMATAUHTORS("J KRIEGER and WG MAYER")
        TEST_REFORMATAUHTORS("E. P. Petrov, T. Ohrt, R. G. Winkler, and P. Schwille")
        TEST_REFORMATAUHTORS("E. P. Petrov, T. Ohrt, R. G. Winkler and P. Schwille")
        TEST_REFORMATAUHTORS("E. P. Petrov and P. Schwille")
        TEST_REFORMATAUHTORS("E. P. Petrov, T. Ohrt")
        TEST_REFORMATAUHTORS("E. P. Petrov, T. Ohrt, R. G. Winkler")
        TEST_REFORMATAUHTORS("A. CORANA, M. MARCHESI, C. MARTINI, and S. RIDELLA")
    }

    if (testLangRec) {
        qDebug()<<"----------------------------------------------------------------------";
        qDebug()<<"-- Test recognizeLanguage()                                         --";
        qDebug()<<"----------------------------------------------------------------------\n";
        QFile::remove("./langrec.txt");
        QString txt="";
        QString filename="";
        if (buildLangRec) {
            TEST_BUILDLANGNGRAMBOOK("../lib/test/langreference_de.txt")
            TEST_BUILDLANGNGRAMBOOK("../lib/test/langreference_en.txt")
            TEST_BUILDLANGNGRAMBOOK("../lib/test/langreference_fr.txt")
            TEST_BUILDLANGNGRAMBOOK("../lib/test/langreference_es.txt")
            TEST_BUILDLANGNGRAMBOOK("../lib/test/langreference_it.txt")
            TEST_BUILDLANGNGRAMBOOK("../lib/test/langreference_ru.txt")
            TEST_BUILDLANGNGRAMBOOK("../lib/test/langreference_cn.txt")
        }

        MULTITEST_RECOGNIZELANG("../lib/test/langtest_de.txt")
        MULTITEST_RECOGNIZELANG("../lib/test/langtest_en.txt")
        MULTITEST_RECOGNIZELANG("../lib/test/langtest_fr.txt")
        MULTITEST_RECOGNIZELANG("../lib/test/langtest_es.txt")
        MULTITEST_RECOGNIZELANG("../lib/test/langtest_it.txt")
        MULTITEST_RECOGNIZELANG("../lib/test/langtest_ru.txt")
        MULTITEST_RECOGNIZELANG("../lib/test/langtest_cn.txt")
        MULTITEST_RECOGNIZELANG("../lib/test/chuvash-utf.txt")
        MULTITEST_RECOGNIZELANG("../lib/test/czech-utf.txt")
        MULTITEST_RECOGNIZELANG("../lib/test/danish-utf.txt")
        MULTITEST_RECOGNIZELANG("../lib/test/finnish-utf.txt")
        MULTITEST_RECOGNIZELANG("../lib/test/greek-utf.txt")

    }

    return 0;
}
