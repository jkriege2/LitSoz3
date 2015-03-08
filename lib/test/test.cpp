#include "../bibtools.h"
#include "../htmltokenizer.h"
#include "../htmltools.h"

#include <iostream>
#include <QDebug>
#include <QFile>
#include <QTextStream>


#define TEST_REFORMATAUHTORS(pattern) { QString t=pattern; qDebug()<<"  pattern = '"<<pattern<<"'\n    -> '"<<reformatAuthors(t, name_prefixes, name_additions, ands)<<"'\n\n"; };

int main(int argc, char * argv[]) {


    QSet<QString> name_additions, name_prefixes;
    QList<QString> ands;
    name_additions<<"jr"<<"sen"<<"i"<<"ii"<<"iii"<<"iv"<<"jun";
    name_prefixes<<"da"<<"de"<<"dal"<<"del"<<"der"<<"di"<<"do"<<"du"<<"dos"<<"la"<<"le"<<"van"<<"vande"<<"von"<<"den";
    ands<<"and"<<"und"<<"&"<<"et";


    qDebug()<<"----------------------------------------------------------------------";
    qDebug()<<"-- Test HTML TOKENIZER                                              --";
    qDebug()<<"----------------------------------------------------------------------\n";
    QFile file("../lib/test/test1.html");
    QString html="";
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) html=file.readAll();
    HtmlTokenizer tokenizer(html);
    HtmlTokenizer::Token t=tokenizer.getNextToken();
    //qDebug()<<(int)t.type<<t.contents;
    while (t.type != HtmlTokenizer::ENDOFFILE) {
        if (t.type!=HtmlTokenizer::TEXT && t.type!=HtmlTokenizer::COMMENT) t.parse();
        qDebug()<<t.type<<t.tagName<<t.properties<<t.contents<<t.completeContents;

        t=tokenizer.getNextToken();
    }

    qDebug()<<"----------------------------------------------------------------------";
    qDebug()<<"-- COinS extraction                                                 --";
    qDebug()<<"----------------------------------------------------------------------\n";
    QList<QMap<QString, QVariant> > c=extractCoins(html, name_prefixes, name_additions, ands);
    for (int i=0; i<c.size(); i++) qDebug()<<c[i]<<"\n\n";


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

    return 0;
}
