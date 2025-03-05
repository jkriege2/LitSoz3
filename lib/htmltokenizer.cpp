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

#include "htmltokenizer.h"

HtmlTokenizer::Token::Token() {
    type=HtmlTokenizer::UNKNOWN;
    contents="";
    completeContents="";
    tagName="";
    properties.clear();
    isParsed=false;
}

QString HtmlTokenizer::Token::getProperty(QString pName) {
    for (int i=0; i<properties.size(); i++) {
        if (properties[i].first.toLower()==pName.toLower()) return properties[i].second;
    }

    return "";
}

void HtmlTokenizer::Token::parse() {
    properties.clear();
    isParsed=true;
    int pos=tagName.size();
    QString pName="";
    QString pValue="";
    QChar pDelim='\0';
    bool isPropName=true;
    bool isPropValue=false;
    while (pos<contents.size()) {
        QChar c=contents[pos];
        pos++;
        if (isPropName) {
            if (c.isLetterOrNumber()) {
                pName+=c;
            } else if (!pName.isEmpty()) {
                if (c=='=') {
                    isPropValue=true;
                    isPropName=false;
                } else {
                    isPropValue=false;
                    isPropName=false;
                }
            }
        } else if (!isPropName && !isPropValue) {
            if (c=='=') { // we found PROPNAME=...
                isPropValue=true;
            } else if (!c.isSpace()) { // we found PROPNAME PROPNAME
                isPropName=true;
                isPropValue=false;
                properties.append(qMakePair(pName.trimmed(), QString("")));
                pName="";
                pValue="";
                pDelim='\0';
            }
        } else if (!isPropName && isPropValue) {
            if (c=='\'') {
                if (pDelim==0) {
                    pDelim=c;
                } else {
                    if (c!=pDelim) {
                        pValue+=c;
                    } else { // we found PROPNAME='...'
                        properties.append(qMakePair(pName.trimmed(), pValue));
                        pName="";
                        pValue="";
                        pDelim='\0';
                        isPropName=true;
                        isPropValue=false;
                    }
                }
            } else if (c=='\"') {
                if (pDelim==0) {
                    pDelim=c;
                } else {
                    if (c!=pDelim) {
                        pValue+=c;
                    } else { // we found PROPNAME="..."
                        properties.append(qMakePair(pName.trimmed(), pValue));
                        pName="";
                        pValue="";
                        pDelim='\0';
                        isPropName=true;
                        isPropValue=false;
                    }
                }
            } else {
                if (pDelim==' ' && c.isSpace()) { // we found PROPNAME=PROPVALUE
                    properties.append(qMakePair(pName.trimmed(), pValue.trimmed()));
                    pName="";
                    pValue="";
                    pDelim='\0';
                    isPropName=true;
                    isPropValue=false;
                } else if (pDelim==0 && !c.isSpace() && (c!='\'') && (c!='\"')) {
                    pValue+=c;
                    pDelim=' ';
                } else {
                    if ((c=='\'') || (c=='\"')) {
                        pDelim=c;
                    } else {
                        pValue+=c;
                    }
                }
            }

        }
    }
    if (!pName.isEmpty()) properties.append(qMakePair(pName.trimmed(), pValue));
}



QDebug operator<<(QDebug dbg, const HtmlTokenizer::TokenType &c)
{
    switch(c) {
    case HtmlTokenizer::ENDOFFILE : dbg.nospace() << "ENDOFFILE"; break;
    case HtmlTokenizer::TEXT : dbg.nospace() << "TEXT"; break;
    case HtmlTokenizer::TAG : dbg.nospace() << "TAG"; break;
    case HtmlTokenizer::ENDTAG : dbg.nospace() << "ENDTAG"; break;
    case HtmlTokenizer::SINGLETAG : dbg.nospace() << "SINGLETAG"; break;
    case HtmlTokenizer::COMMENT : dbg.nospace() << "COMMENT"; break;
    case HtmlTokenizer::PROCESSINGINSTRUCTION : dbg.nospace() << "PROCESSINGINSTRUCTION"; break;
    case HtmlTokenizer::DOCTYPE : dbg.nospace() << "DOCTYPE"; break;
    case HtmlTokenizer::UNKNOWN : dbg.nospace() << "UNKNOWN"; break;
    }

    return dbg.space();
}




HtmlTokenizer::HtmlTokenizer(const QString& html, QObject *parent) :
    QObject(parent)
{
    this->html=html;
    pos=0;
}

HtmlTokenizer::Token HtmlTokenizer::getNextToken() {
    HtmlTokenizer::Token token;
    token.type=HtmlTokenizer::UNKNOWN;
    token.contents="";
    if (pos>=html.size()) {
        token.type=HtmlTokenizer::ENDOFFILE;
        token.contents="";
    } else {
        QChar c=html[pos]; pos++;
        /*if (c.isSpace()) {
            token.type=HtmlTokenizer::WHITESPACE;
            token.contents=c;
            while ( (pos<html.size()) && c.isSpace()) {
                c=html[pos];
                token.contents+=c;
                pos++;
            }
        } else */if (c=='<') {
            token.type=HtmlTokenizer::TAG;
            token.completeContents="<";
            int contentsStart=pos;
            QString endTag=">";
            QString start=html.mid(pos-1, 5);
            if (start.startsWith("<!--")) {
                token.type=HtmlTokenizer::COMMENT;
                contentsStart=pos+3;
                token.completeContents="<!--";
                endTag="-->";
            } else if (start.startsWith("<!")) {
                token.type=HtmlTokenizer::DOCTYPE;
                contentsStart=pos+1;
                token.completeContents="<!";
                endTag=">";
            } else if (start.startsWith("</")) {
                token.type=HtmlTokenizer::ENDTAG;
                contentsStart=pos+1;
                token.completeContents="</";
                endTag=">";
            } else if (start.startsWith("<?")) {
                token.type=HtmlTokenizer::PROCESSINGINSTRUCTION;
                contentsStart=pos+1;
                token.completeContents="<?";
                endTag="?>";
            }
            bool isProperty=false;
            QChar endProperty='\0';
            pos=contentsStart;
            bool isTagName=true;
            token.tagName="";
            while (pos<html.size()) {
                c=html[pos];
                pos++;
                token.contents+=c;
                token.completeContents+=c;
                if (isTagName) {
                    if (!c.isSpace()) {
                        token.tagName+=c;
                    } else {
                        isTagName=token.tagName.isEmpty();
                    }
                }
                if (token.type!=HtmlTokenizer::COMMENT) {
                    if (!isProperty) {
                        if (c=='\'') {
                            isProperty=true;
                            endProperty=c;
                        } else if (c=='\"') {
                            isProperty=true;
                            endProperty=c;
                        }
                    } else {
                        isProperty=(c!=endProperty);
                    }
                }
                if ((token.type==HtmlTokenizer::COMMENT) || (!isProperty)) {
                    if (token.completeContents.endsWith(endTag)) {
                        token.contents=token.contents.left(token.contents.size()-endTag.size());
                        if (token.tagName.endsWith(endTag)) {
                            token.tagName=token.tagName.left(token.tagName.size()-endTag.size());
                        }
                        break;
                    }
                }
            }
            if (token.type==HtmlTokenizer::TAG) {
                if (token.completeContents.endsWith("/>")) token.type=HtmlTokenizer::SINGLETAG;
            }
        } else {
            token.type=HtmlTokenizer::TEXT;
            token.contents=c;
            while ( (pos<html.size()) && (c!='<')) {
                c=html[pos];
                if (c!='<') token.contents+=c;
                pos++;
            }
            token.completeContents=token.contents;
            if (c=='<') pos--;
        }
    }

    return token;
}
