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

#include "xmptools.h"

#include <QtCore>
#include <QtXml>
#include <QtDebug>



void parseNode(QDomNode node, QMap<QString, QString>& props, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes) {
    if (node.nodeName().toLower()=="dc:title") {
        props["title"]=node.toElement().text();
    } else if (node.nodeName().toLower()=="dc:subject") {
        props["topic"]=node.toElement().text();
    } else if (node.nodeName().toLower()=="dc:description") {
        props["abstract"]=node.toElement().text();
    } else if (node.nodeName().toLower()=="dc:publisher") {
        props["publisher"]=node.toElement().text();
    } else if (node.nodeName().toLower()=="dc:language") {
        props["language"]=node.toElement().text();
    } else if (node.nodeName().toLower()=="dc:creator") {
        QString author=reformatSingleName(node.toElement().text(), namePrefixes, nameAdditions);
        if (props.value("authors", "").isEmpty()) {
            props["authors"]=props["authors"]+"; ";
        }
        props["authors"]=props["authors"]+author;
    } else {
        QDomNode n = node.firstChild();
         while (!n.isNull()) {
             if (n.isElement()) {
                parseNode(n, props, andWords, nameAdditions, namePrefixes);
             }
             n = n.nextSibling();
         }
    }

}



QMap<QString, QString> extractXMP(const QString& filename, QList<QString> andWords, QSet<QString> nameAdditions, QSet<QString> namePrefixes) {
    QMap<QString, QString> props;
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray contents=file.readAll();

        // 1. search for header in UTF-8, UTF-16 or UCS-4
        QString header="<?xpacket begin=";
        QString endPI="<?xpacket end=";
        int startPos=contents.indexOf(header.toUtf8());
        int endPos=0;
        if (startPos>=0) {
            // UTF-8
            endPos=contents.indexOf(endPI.toUtf8(), startPos)-1;
        } else {
            /*startPos=contents.indexOf(QByteArray(header.data(), header.size()));
            if (startPos>=0) {
                // UTF-16
                endPos=contents.indexOf(endPI.data(), startPos)-1;
            }*/
            /* else {
                startPos=contents.indexOf(header.toUcs4());
                if (startPos>=0) {
                    // UCS-4
                    endPos=contents.indexOf(endPI.toUcs4(), startPos)-1;
                }
            }*/
        }

        if ((startPos>=0) && (endPos>startPos)) {
            char* data=contents.data();
            QByteArray xmpData=QByteArray(&(data[startPos]), endPos-startPos);
            QDomDocument doc;
            QString errorMsg;
            int errorLine, errorColumn;

            // try to find Dublin Core entries:
            if (doc.setContent(xmpData, false, &errorMsg, &errorLine, &errorColumn)) {
                //qDebug()<<doc.toByteArray();
                parseNode(doc, props, andWords, nameAdditions, namePrefixes);
            } else {
                //qDebug()<<"XMP parse error: "<<errorMsg<<" line="<<errorLine<<" columne="<<errorColumn;
            }


        }
        file.close();
    }
    return props;
}


