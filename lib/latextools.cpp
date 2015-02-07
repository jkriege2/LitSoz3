#include "latextools.h"
#include <QDebug>

QString chartoprintablestr(QChar data){
  QString ost;
  switch(data.toLatin1()) {
    case 0: ost="[NUL]"; break;
    case 1: ost="[SOH]"; break;
    case 2: ost="[STX]"; break;
    case 3: ost="[ETX]"; break;
    case 4: ost="[EOT]"; break;
    case 5: ost="[ENQ]"; break;
    case 6: ost="[ACK]"; break;
    case 7: ost="[BEL]"; break;
    case 8: ost="[BS]"; break;
    case 9: ost="[TAB]"; break;
    case 10: ost="[LF]"; break;
    case 11: ost="[VT]"; break;
    case 12: ost="[FF]"; break;
    case 13: ost="[CR]"; break;
    case 14: ost="[SO]"; break;
    case 15: ost="[SI]"; break;
    case 16: ost="[DLE]"; break;
    case 17: ost="[DC1]"; break;
    case 18: ost="[DC2]"; break;
    case 19: ost="[DC3]"; break;
    case 20: ost="[DC4]"; break;
    case 21: ost="[NAK]"; break;
    case 22: ost="[SYN]"; break;
    case 23: ost="[ETB]"; break;
    case 24: ost="[CAN]"; break;
    case 25: ost="[EM]"; break;
    case 26: ost="[SUB]"; break;
    case 27: ost="[ESC]"; break;
    case 28: ost="[FS]"; break;
    case 29: ost="[GS]"; break;
    case 30: ost="[RS]"; break;
    case 31: ost="[US]"; break;
    case 32: ost="[SP]"; break;
    case 127: ost="[DEL]"; break;
//    case : ost=""; break;
    default:
      ost=data;
  }
  return ost;
}

BiBTeXParser::BiBTeXParser(const QString& input) {
    this->input="";
    pos=-1;
    line=0;
    lastError="";
    records.clear();
    errorOccured=false;
    if (input.size()>0) parse(input);
}

bool BiBTeXParser::parse(const QString& input, QString* errorMessage) {
    //std::cout<<"PARSING:\n"<<input.toStdString()<<"\n\n";
    this->input=input;
    pos=0;
    line=0;
    lastError="";
    records.clear();
    errorOccured=false;
    QString recType="";
    QString recID="";
    if (input.size()<=0) {
        errorOccured=true;
        lastError=QObject::tr("input was empty");
    } else {
        BiBTeXParser::Record rec;
        while (findRecord(&rec) && (!errorOccured)) {
            if (rec.type.toLower()=="string") {
                QMapIterator<QString, QString> it(rec.fields);
                while (it.hasNext()) {
                    it.next();
                    strings[it.key()]=it.value();
                    //qDebug()<<"added @string '"<<it.key()<<"' = '"<<it.value()<<"'";
                }
                rec.type="";
                rec.ID="";
                rec.fields.clear();
            } else if (rec.type.toLower()=="preamble") {
            } else if (rec.type.toLower()=="comment") {
            } else {
                records.append(rec);
                rec.type="";
                rec.ID="";
                rec.fields.clear();
            }
        }
    }
    if (errorMessage) *errorMessage=lastError;
    return !errorOccured;
}

bool BiBTeXParser::findRecord(Record* record) {
    BiBTeXParser::TokenType tt=getNextToken();
    if (tt==BiBTeXParser::ttAt) {
        tt=getNextToken();
        if (tt==BiBTeXParser::ttName) {
            record->type=currentToken.value.trimmed().toLower();
            tt=getNextToken();
            if (tt==BiBTeXParser::ttOpenBrace) {
                tt=getNextToken();
                BiBTeXParser::Token ttemp=currentToken;
                tt=getNextToken();
                switch(ttemp.type) {
                    case BiBTeXParser::ttName: {
                        if (tt==BiBTeXParser::ttComma) {
                            record->ID=ttemp.value;
                        } else if (tt==BiBTeXParser::ttEquals) {
                            parseProperty(record, false, ttemp.value);
                        } else if (tt==BiBTeXParser::ttCloseBrace) {
                            return true;
                        } else {
                            errorOccured=true;
                            lastError=QObject::tr("',', '}' or '=' expected in line %1").arg(line);
                            return false;
                        }
                        break;
                    }
                    case BiBTeXParser::ttComma: break;
                    case BiBTeXParser::ttCloseBrace:
                        return !errorOccured;
                        break;
                    default:
                        errorOccured=true;
                        lastError=QObject::tr("unexpected token in line %1").arg(line);
                        return false;
                        break;
                }
                while ((currentToken.type!=BiBTeXParser::ttCloseBrace) && (currentToken.type!=BiBTeXParser::ttEOF) && (!errorOccured)) {
                    parseProperty(record, true);
                }
            } else {
                errorOccured=true;
                lastError=QObject::tr("'{' expected at start of record in line %1").arg(line);
            }
        } else {
            errorOccured=true;
            lastError=QObject::tr("type expected after '@' in line %1").arg(line);
        }
    } else if (tt==BiBTeXParser::ttEOF) {
        return false;
    } else {
        errorOccured=true;
        lastError=QObject::tr("unexpected token in line %1, '@' expected").arg(line);
    }
    return !errorOccured;
}

void BiBTeXParser::parseProperty(Record* record, bool getFull, QString name) {
    //std::cout<<"entering parseProperty()\n";
    QString pname=name;
    QString pvalue="";
    if (getFull) {
        getNextToken();
        if (currentToken.type==BiBTeXParser::ttName) {
            pname=currentToken.value;
            if (getNextToken()==BiBTeXParser::ttEquals) {

            } else {
                errorOccured=true;
                lastError=QObject::tr("'=' token expected after <NAME> at line %1").arg(line);
            }
        } else if (currentToken.type==BiBTeXParser::ttCloseBrace) {
            return;
        } else {
            errorOccured=true;
            lastError=QObject::tr("<NAME> token expected at line %1").arg(line);
        }
    }
    if (!errorOccured) {
        bool isfirst=true;
        bool endreached=false;
        QChar endc='\0';
        QChar lastc='\0';
        QChar c=input[pos];
        int obcount=0;
        QString outsideEndCs;
        while ((pos<input.size())&&(!errorOccured)) {
            bool add=true;
            lastc=c;
            c=input[pos];
            if (endc=='\0') {
                if (c=='"') { endc='"'; add=false; } //std::cout<<"endc='"<<endc.toLatin1()<<"'\n"; }
                else if ((c=='{')&&(lastc!='\\')) { endc='}'; add=false; } //std::cout<<"endc='"<<endc.toLatin1()<<"'\n"; }
                //else if (!c.isSpace()) { endc=','; } //std::cout<<"endc='"<<endc.toLatin1()<<"'\n"; }
            } else {
                if ((c=='{')&&(lastc!='\\')) {
                    obcount++;
                }
            }
            if (endc!='\0') {
                if (c==endc) {

                    if (endc=='"') {
                        if (lastc!='\\') {
                            //endreached=true;
                            endc='\0';
                            add=false;
                            isfirst=true;
                            //break;
                        } else {
                            pvalue += c;
                        }
                    } else if (endc=='}') {
                        if (lastc!='\\') {
                            if (obcount==0) {
                                //endreached=true;
                                endc='\0';
                                add=false;
                                isfirst=true;
                                //break;
                            } else {
                                obcount--;
                                pvalue += c;
                            }
                        } else {
                            pvalue += c;
                        }
                    } else {
                        //endreached=true;
                        endc='\0';
                        add=false;
                        isfirst=true;
                        //break;
                    }
                }
            } else {
                if (c==',') {
                    endreached=true;
                    break;
                } else if (c=='}') {
                    endreached=true;
                    pos-=1;
                    break;
                } else if (c=='#') {
                    add=false;
                }
            }

            //qDebug()<<"pos="<<pos<<"   c='"<<chartoprintablestr(c)<<"'  lastc='"<<chartoprintablestr(lastc)<<"'  endc='"<<chartoprintablestr(endc)<<"'  obcount="<<obcount<<"  pvalue="<<pvalue<<"  outsideEndCs="<<outsideEndCs<<"  add="<<add<<"  isfirst="<<isfirst<<"  endreached="<<endreached;

            if (add) {
                if (!c.isSpace()) {
                    isfirst=false;
                    if (endc=='\0') {
                        outsideEndCs+=c;
                    } else {
                        pvalue += c;
                    }
                } else {
                    if (!isfirst) {
                        pvalue += c;
                    }
                }
            } else {
                if (endc!='\0') {
                    pvalue += replaceStrings(outsideEndCs);
                    outsideEndCs="";
                }
            }

            pos++;
        }
        //qDebug()<<"DONE: pos="<<pos<<"   input[pos]='"<<chartoprintablestr(input[pos])<<"'   c='"<<chartoprintablestr(c)<<"'  lastc='"<<chartoprintablestr(lastc)<<"'  endc='"<<chartoprintablestr(endc)<<"'  obcount="<<obcount<<"  pvalue="<<pvalue<<"  isfirst="<<isfirst<<"  endreached="<<endreached;
        if ((pos>=input.size()) && (endc!=',') && (endc!='\0') && (!endreached)) {
            errorOccured=true;
            lastError=QObject::tr("<NAME>=<VALUE> pair unfinished at line %1").arg(line);
            //qDebug()<<"error in parseProperty(): "<<lastError;
        } else {
            if (outsideEndCs.size()>0) pvalue += replaceStrings(outsideEndCs);
            record->fields[pname]=removeLaTeX(pvalue.trimmed());
            //if ((endreached)&&(c!='}')) pos++;
            pos++;
            if ((endc=='}') || (endc=='"')) getNextToken();
        }
    }
    //std::cout<<"leaving parseProperty()\n";
}

QString BiBTeXParser::replaceStrings(const QString& data) {
    QString result=data;
    QMapIterator<QString, QString> it(strings);
    while (it.hasNext()) {
        it.next();
        result.replace(it.key(), it.value());
    }
    return result;
}



BiBTeXParser::TokenType BiBTeXParser::getNextToken() {
    if (pos>=input.size()) {
        currentToken.type=BiBTeXParser::ttEOF;
        //std::cout<<"TOKEN: eof\n";
        return currentToken.type;
    }
    QChar c=input[pos];
    while (c.isSpace() || (c=='%')) {
        if (pos>=input.size()) {
            currentToken.type=BiBTeXParser::ttEOF;
            //std::cout<<"TOKEN: eof\n";
            return currentToken.type;
        } else if (c=='\n') {
            line++;
        } else if (c=='%') {
            while ((pos<input.size())&&(input[pos]!='\n')) {
                c=input[pos];
                pos++;
            };
            if (c=='\n') line++;
            if (pos>=input.size()) {
                currentToken.type=BiBTeXParser::ttEOF;
                //std::cout<<"TOKEN: eof\n";
                return currentToken.type;
            }
        }
        pos++;
        c=input[pos];
    }

    if (pos>=input.size()) {
        currentToken.type=BiBTeXParser::ttEOF;
        //std::cout<<"TOKEN: eof\n";
        return currentToken.type;
    }

    if (c=='@') {
        //std::cout<<"TOKEN: @\n";
        pos++;
        currentToken.type=BiBTeXParser::ttAt;
        return currentToken.type;
    }
    if (c=='{') {
        //std::cout<<"TOKEN: {\n";
        pos++;
        currentToken.type=BiBTeXParser::ttOpenBrace;
        currentToken.value=c;
        return currentToken.type;
    }
    if (c=='}') {
        //std::cout<<"TOKEN: }\n";
        currentToken.type=BiBTeXParser::ttCloseBrace;
        pos++;
        currentToken.value=c;
        return currentToken.type;
    }
    if (c=='=') {
        //std::cout<<"TOKEN: =\n";
        pos++;
        currentToken.type=BiBTeXParser::ttEquals;
        return currentToken.type;
    }
    if (c==',') {
        //std::cout<<"TOKEN: ,\n";
        pos++;
        currentToken.type=BiBTeXParser::ttComma;
        return currentToken.type;
    }
    if (c.isLetterOrNumber()) {
        currentToken.type=ttName;
        currentToken.value="";
        while ((pos<input.size()) && (!c.isSpace()) && (c!=',') && (c!='=') && (c!='}') && (c!='{') && (c!='\"') && (c!='\'')) {//(c.isLetterOrNumber())) {
            currentToken.value+=c;
            pos++;
            c=input[pos];
        }
        if (pos<input.size()) {
            //pos--;
        }
        //std::cout<<"TOKEN: NAME '"<<currentToken.value.toStdString()<<"'\n";
        return currentToken.type;
    }
    //std::cout<<"TOKEN: unknown  c='"<<c.toLatin1()<<"'\n";

    currentToken.type=BiBTeXParser::ttUnknown;
    return currentToken.type;
}


QString laTeXify(const QString& input, bool brackedDoubleUppercase) {
    QString out;

    if (brackedDoubleUppercase) {
        QStringList sl=input.split(' ');
        for (int i=0; i<sl.size(); i++) {
            if (!out.isEmpty()) out.append(' ');

            int cnt=sl[i].count(QRegExp("[A-ZÄÖÜ]"));
            if (cnt>1) {
                out.append("{"+sl[i]+"}");
            } else {
                out.append(sl[i]);
            }
        }
    } else {
        out=input;
    }

    out.replace("$", "\\$");
    out.replace("\\", "$\\backslash$");
    out.replace("\n", "\\\\");
    out.replace("\"", "\\dq");
    out.replace("_", "\\_");
    out.replace("&", "\\&");
    out.replace("%", "\\%");
    out.replace("#", "\\#");
    out.replace("‘", "\\glq");
    out.replace("’", "\\grq");
    out.replace("“", "\\glqq");
    out.replace("”", "\\grqq");

    out.replace("ä", "\\\"{a}");
    out.replace("ö", "\\\"{o}");
    out.replace("ü", "\\\"{u}");
    out.replace("Ä", "\\\"{A}");
    out.replace("Ö", "\\\"{O}");
    out.replace("Ü", "\\\"{U}");
    out.replace("µ", "$\\mu$");
    out.replace("ß", "{\\ss}");
    out.replace("»", "{\\frqq}");
    out.replace("«", "{\\flqq}");
    out.replace("§", "{\\S}");
    out.replace("£", "{\\pounds}");
    out.replace("<", "$<$");
    out.replace(">", "$>$");
    out.replace("¿", "?`");
    out.replace("¡", "!`");
    out.replace("æ", "{\\ae}");
    out.replace("Æ", "{\\AE}");
    out.replace("å", "{\\aa}");
    out.replace("Å", "{\\AA}");
    out.replace("ø", "{\\o}");
    out.replace("Ø", "{\\O}");
    out.replace("ç", "\\c{c}");
    out.replace("Ç", "\\c{C}");
    out.replace("à", "\\`{a}");
    out.replace("À", "\\`{A}");
    out.replace("á", "\\'{a}");
    out.replace("Á", "\\'{A}");
    out.replace("â", "\\^{a}");
    out.replace("Â", "\\^{A}");
    out.replace("ã", "\\~{a}");
    out.replace("Ã", "\\~{A}");
    out.replace("è", "\\`{e}");
    out.replace("È", "\\`{E}");
    out.replace("é", "\\'{e}");
    out.replace("É", "\\'{E}");
    out.replace("ê", "\\^{e}");
    out.replace("Ê", "\\^{E}");
    out.replace("ë", "\\\"{e}");
    out.replace("Ë", "\\\"{E}");
    out.replace("ì", "\\`{i}");
    out.replace("Ì", "\\`{I}");
    out.replace("í", "\\'{i}");
    out.replace("Í", "\\'{I}");
    out.replace("î", "\\^{i}");
    out.replace("Î", "\\^{I}");
    out.replace("ï", "\\\"{i}");
    out.replace("Ï", "\\\"{I}");
    out.replace("ñ", "\\~{n}");
    out.replace("Ñ", "\\~{N}");
    out.replace("ò", "\\`{o}");
    out.replace("Ò", "\\`{O}");
    out.replace("ó","\\'{o}" );
    out.replace("Ó", "\\'{O}");
    out.replace("ô", "\\^{o}");
    out.replace("Ô", "\\^{O}");
    out.replace("õ", "\\~{o}");
    out.replace("Õ", "\\~{O}");
    out.replace("ù", "\\`{u}");
    out.replace("Ù", "\\`{U}");
    out.replace("ú", "\\'{u}");
    out.replace("Ú", "\\'{U}");
    out.replace("û", "\\^{u}");
    out.replace("Û", "\\^{U}");
    out.replace("ý", "\\'{y}");
    out.replace("Ý", "\\'{Y}");
    out.replace("ÿ", "\\\"{y}");

    out.replace("×", "$\\times$");
    out.replace("÷", "$\\div$");
    out.replace("±", "$\\pm$");
    out.replace("©", "{\\copyright}");
    out.replace("¬", "$\\neg$");
    out.replace("·", "$\\cdot$");
    out.replace("¶", "{\\P}");
    out.replace("|", "$\\mid$");
    out.replace("…", "$\\ldots$");
    out.replace("Œ", "\\OE");
    out.replace("Š", "\\v{S}");
    out.replace("Ž", "\\v{Z}");
    out.replace("š", "\\v{s}");
    out.replace("ž", "\\v{z}");





    out.replace( QChar((int)0x03B1), "$\\alpha$");
    out.replace( QChar((int)0x03B2), "$\\beta$");
    out.replace( QChar((int)0x03B3), "$\\gamma$");
    out.replace( QChar((int)0x03B4), "$\\delta$");
    out.replace( QChar((int)0x03B5), "$\\epsilon$");
    out.replace( QChar((int)0x03B6), "$\\zeta$");
    out.replace( QChar((int)0x03B7), "$\\eta$");
    out.replace( QChar((int)0x03B8), "$\\theta$");
    out.replace( QChar((int)0x03b9), "$\\iota$");
    out.replace( QChar((int)0x03ba), "$\\kappa$");
    out.replace( QChar((int)0x03bb), "$\\lambda$");
    out.replace( QChar((int)0x03bc), "$\\mu$");
    out.replace( QChar((int)0x03bd), "$\\nu$");
    out.replace( QChar((int)0x03be), "$\\xi$");
    out.replace( QChar((int)0x03bf), "$\\omikron$");
    out.replace( QChar((int)0x03c0), "$\\pi$");
    out.replace( QChar((int)0x03c1), "$\\rho$");
    out.replace( QChar((int)0x03C3), "$\\sigma$");
    out.replace( QChar((int)0x03c4), "$\\tau$");
    out.replace( QChar((int)0x03c5), "$\\upsilon$");
    out.replace( QChar((int)0x03c6), "$\\varphi$");
    out.replace( QChar((int)0x03c7), "$\\chi$");
    out.replace( QChar((int)0x03c8), "$\\psi$");
    out.replace( QChar((int)0x03c9), "$\\omega$");
    out.replace( QChar((int)0x03D5), "$\\phi$");
    out.replace( QChar((int)0x0391), "$\\Alpha$");
    out.replace( QChar((int)0x0392), "$\\Beta$");
    out.replace( QChar((int)0x0393), "$\\Gamma$");
    out.replace( QChar((int)0x0394), "$\\Delta$");
    out.replace( QChar((int)0x0395), "$\\Epsilon$");
    out.replace( QChar((int)0x0396), "$\\Zeta$");
    out.replace( QChar((int)0x0397), "$\\Eta$");
    out.replace( QChar((int)0x0398), "$\\Theta$");
    out.replace( QChar((int)0x0399), "$\\Iota$");
    out.replace( QChar((int)0x039a), "$\\Kappa$");
    out.replace( QChar((int)0x039b), "$\\Lambda$");
    out.replace( QChar((int)0x039c), "$\\Mu$");
    out.replace( QChar((int)0x039d), "$\\Nu$");
    out.replace( QChar((int)0x039e), "$\\Xi$");
    out.replace( QChar((int)0x039f), "$\\Omikron$");
    out.replace( QChar((int)0x03a0), "$\\Pi$");
    out.replace( QChar((int)0x03a1), "$\\Rho$");
    out.replace( QChar((int)0x03a2), "$\\Sigma$");
    out.replace( QChar((int)0x03a3), "$\\Tau$");
    out.replace( QChar((int)0x03a4), "$\\Upsilon$");
    out.replace( QChar((int)0x03a5), "$\\Phi$");
    out.replace( QChar((int)0x03a6), "$\\Chi$");
    out.replace( QChar((int)0x03a7), "$\\Psi$");
    out.replace( QChar((int)0x03a8), "$\\Omega$");





    return out;
}

QString removeLaTeX(const QString& input) {
    QString out=input;

    //qDebug()<<"removeLaTeX_input "<<out;

    // first replace some simple LaTeX markup
    out.replace("\\\"{a}", "ä");
    out.replace("\\\"{o}", "ö");
    out.replace("\\\"{u}", "ü");
    out.replace("\\\"{A}", "Ä");
    out.replace("\\\"{O}", "Ö");
    out.replace("\\\"{U}", "Ü");
    out.replace("\\\"a", "ä");
    out.replace("\\\"o", "ö");
    out.replace("\\\"u", "ü");
    out.replace("\\\"A", "Ä");
    out.replace("\\\"O", "Ö");
    out.replace("\\\"U", "Ü");
    out.replace("\\\\", "\n");
    out.replace("\\par", "\n");
    out.replace("\\mu", "µ");
    out.replace("\\ss", "ß");
    out.replace("\\dq", "\"");
    out.replace("\\frqq", "»");
    out.replace("\\flqq", "«");
    out.replace("\\_", "_");
    out.replace("\\&", "&");
    out.replace("\\%", "%");
    out.replace("\\#", "#");
    out.replace("\\LaTeX", "LaTeX");
    out.replace("\\TeX", "TeX");
    out.replace("\\S", "§");
    out.replace("\\pounds", "£");
    out.replace("$<$", "<");
    out.replace("$>$", ">");
    out.replace("?`", "¿");
    out.replace("!`", "¡");
    out.replace("\\ae", "æ");
    out.replace("\\AE", "Æ");
    out.replace("\\aa", "å");
    out.replace("\\AA", "Å");
    out.replace("\\o", "ø");
    out.replace("\\O", "Ø");
    out.replace("\\c{c}", "ç");
    out.replace("\\c{C}", "Ç");
    out.replace("\\`{a}", "à");
    out.replace("\\`{A}", "À");
    out.replace("\\'{a}", "á");
    out.replace("\\'{A}", "Á");
    out.replace("\\^{a}", "â");
    out.replace("\\^{A}", "Â");
    out.replace("\\~{a}", "ã");
    out.replace("\\~{A}", "Ã");
    out.replace("\\`{e}", "è");
    out.replace("\\`{E}", "È");
    out.replace("\\'{e}", "é");
    out.replace("\\'{E}", "É");
    out.replace("\\^{e}", "ê");
    out.replace("\\^{E}", "Ê");
    out.replace("\\\"{e}", "ë");
    out.replace("\\\"{E}", "Ë");
    out.replace("\\`{i}", "ì");
    out.replace("\\`{I}", "Ì");
    out.replace("\\'{i}", "í");
    out.replace("\\'{I}", "Í");
    out.replace("\\^{i}", "î");
    out.replace("\\^{I}", "Î");
    out.replace("\\\"{i}", "ï");
    out.replace("\\\"{I}", "Ï");
    out.replace("\\~{n}", "ñ");
    out.replace("\\~{N}", "Ñ");
    out.replace("\\`{o}", "ò");
    out.replace("\\`{O}", "Ò");
    out.replace("\\'{o}", "ó");
    out.replace("\\'{O}", "Ó");
    out.replace("\\^{o}", "ô");
    out.replace("\\^{O}", "Ô");
    out.replace("\\~{o}", "õ");
    out.replace("\\~{O}", "Õ");
    out.replace("\\`{u}", "ù");
    out.replace("\\`{U}", "Ù");
    out.replace("\\'{u}", "ú");
    out.replace("\\'{U}", "Ú");
    out.replace("\\^{u}", "û");
    out.replace("\\^{U}", "Û");
    out.replace("\\'{y}", "ý");
    out.replace("\\'{Y}", "Ý");
    out.replace("\\\"{y}", "ÿ");

    out.replace("\\`a", "à");
    out.replace("\\`A", "À");
    out.replace("\\'a", "á");
    out.replace("\\'A", "Á");
    out.replace("\\^a", "â");
    out.replace("\\^A", "Â");
    out.replace("\\~a", "ã");
    out.replace("\\~A", "Ã");
    out.replace("\\`e", "è");
    out.replace("\\`E", "È");
    out.replace("\\'e", "é");
    out.replace("\\'E", "É");
    out.replace("\\^e", "ê");
    out.replace("\\^E", "Ê");
    out.replace("\\\"e", "ë");
    out.replace("\\\"E", "Ë");
    out.replace("\\`i", "ì");
    out.replace("\\`I", "Ì");
    out.replace("\\'i", "í");
    out.replace("\\'I", "Í");
    out.replace("\\^i", "î");
    out.replace("\\^I", "Î");
    out.replace("\\\"i", "ï");
    out.replace("\\\"I", "Ï");
    out.replace("\\~n", "ñ");
    out.replace("\\~N", "Ñ");
    out.replace("\\`o", "ò");
    out.replace("\\`O", "Ò");
    out.replace("\\'o", "ó");
    out.replace("\\'O", "Ó");
    out.replace("\\^o", "ô");
    out.replace("\\^O", "Ô");
    out.replace("\\~o", "õ");
    out.replace("\\~O", "Õ");
    out.replace("\\`u", "ù");
    out.replace("\\`U", "Ù");
    out.replace("\\'u", "ú");
    out.replace("\\'U", "Ú");
    out.replace("\\^u", "û");
    out.replace("\\^U", "Û");
    out.replace("\\'y", "ý");
    out.replace("\\'Y", "Ý");
    out.replace("\\\"y", "ÿ");

    out.replace("\\times", "×");
    out.replace("\\div", "÷");
    out.replace("\\pm", "±");
    out.replace("\\copyright", "©");
    out.replace("\\neg", "¬");
    out.replace("\\cdot", "·");
    out.replace("\\P", "¶");
    out.replace("\\mid", "|");
    out.replace("\\ldots", "…");
    out.replace("\\glq", "‘");
    out.replace("\\grq", "’");
    out.replace("\\glqq", "“");
    out.replace("\\grqq", "”");
    out.replace("\\OE", "Œ");
    out.replace("\\v{S}", "Š");
    out.replace("\\v{Z}", "Ž");
    out.replace("\\v{s}", "š");
    out.replace("\\v{z}", "ž");
    out.replace("---", "-");
    out.replace("--", "-");
    out.replace("\"`", "”");
    out.replace("\"<", "«");
    out.replace("\">", "»");
    out.replace("\"´", "„");
    out.replace("\"-", "");
    out.replace("\"\"", "");
    out.replace("\"=", "");
    out.replace("\"ck", "ck");
    out.replace("\"tt", "tt");
    out.replace("\"ff", "ff");
    out.replace("\"ll", "ll");
    out.replace("\"mm", "mm");
    out.replace("\"nn", "nn");
    out.replace("\"pp", "p");
    out.replace("\"rr", "rr");
    out.replace("\"|", "");
    out.replace("\"~", "");
    out.replace("\\ ", " ");
    out.replace("\\,", " ");
    out.replace("\\-", " ");
    out.replace("\\/", "");
    out.replace("\\:", " ");
    out.replace("\\;", " ");
    out.replace("\\@", " ");

    out.replace("\\alpha", QChar((int)0x03B1));
    out.replace("\\beta", QChar((int)0x03B2));
    out.replace("\\gamma", QChar((int)0x03B3));
    out.replace("\\delta", QChar((int)0x03B4));
    out.replace("\\epsilon", QChar((int)0x03B5));
    out.replace("\\zeta", QChar((int)0x03B6));
    out.replace("\\eta", QChar((int)0x03B7));
    out.replace("\\theta", QChar((int)0x03B8));
    out.replace("\\iota", QChar((int)0x03b9));
    out.replace("\\kappa", QChar((int)0x03ba));
    out.replace("\\lambda", QChar((int)0x03bb));
    out.replace("\\mu", QChar((int)0x03bc));
    out.replace("\\nu", QChar((int)0x03bd));
    out.replace("\\xi", QChar((int)0x03be));
    out.replace("\\omikron", QChar((int)0x03bf));
    out.replace("\\pi", QChar((int)0x03c0));
    out.replace("\\rho", QChar((int)0x03c1));
    out.replace("\\sigma", QChar((int)0x03C3));
    out.replace("\\tau", QChar((int)0x03c4));
    out.replace("\\upsilon", QChar((int)0x03c5));
    out.replace("\\varphi", QChar((int)0x03c6));
    out.replace("\\chi", QChar((int)0x03c7));
    out.replace("\\psi", QChar((int)0x03c8));
    out.replace("\\omega", QChar((int)0x03c9));
    out.replace("\\phi", QChar((int)0x03D5));
    out.replace("\\Alpha", QChar((int)0x0391));
    out.replace("\\Beta", QChar((int)0x0392));
    out.replace("\\Gamma", QChar((int)0x0393));
    out.replace("\\Delta", QChar((int)0x0394));
    out.replace("\\Epsilon", QChar((int)0x0395));
    out.replace("\\Zeta", QChar((int)0x0396));
    out.replace("\\Eta", QChar((int)0x0397));
    out.replace("\\Theta", QChar((int)0x0398));
    out.replace("\\Iota", QChar((int)0x0399));
    out.replace("\\Kappa", QChar((int)0x039a));
    out.replace("\\Lambda", QChar((int)0x039b));
    out.replace("\\Mu", QChar((int)0x039c));
    out.replace("\\Nu", QChar((int)0x039d));
    out.replace("\\Xi", QChar((int)0x039e));
    out.replace("\\Omikron", QChar((int)0x039f));
    out.replace("\\Pi", QChar((int)0x03a0));
    out.replace("\\Rho", QChar((int)0x03a1));
    out.replace("\\Sigma", QChar((int)0x03a2));
    out.replace("\\Tau", QChar((int)0x03a3));
    out.replace("\\Upsilon", QChar((int)0x03a4));
    out.replace("\\Phi", QChar((int)0x03a5));
    out.replace("\\Chi", QChar((int)0x03a6));
    out.replace("\\Psi", QChar((int)0x03a7));
    out.replace("\\Omega", QChar((int)0x03a8));
    out.replace("\\sum", QChar((int)0x03a2));
    out.replace("\\prod", QChar((int)0x03a0));

    //qDebug()<<"removeLaTeX_stag1 "<<out;


    // now remove all braces { [ ] } and $ if they are not precedet by a backslash
    if (out.size()>0) {
        QChar c, c_last;
        c_last='\0';
        c='\0';
        int i=0;
        QString out1="";
        QString inst="";
        bool instruction=false;
        bool isMath=false;
        while (i<out.size()) {
            c_last=c;
            c=out[i];
            if (c=='\\') {
                instruction=true;
                inst="";
            } else {
                if (instruction) {
                    if (c=='{')      { if (inst.size()==0) { out1=out1+'{'; } else { i--; } instruction=false; }
                    else if (c=='}') { if (inst.size()==0) { out1=out1+'}'; } else { i--; } instruction=false; }
                    else if (c=='[') { if (inst.size()==0) { out1=out1+'['; } else { i--; } instruction=false; }
                    else if (c==']') { if (inst.size()==0) { out1=out1+']'; } else { i--; } instruction=false; }
                    else if (c=='$') { if (inst.size()==0) { out1=out1+'$'; } else { i--; } instruction=false; }
                    /*else if (c=='}') { out1=out1+'}'; instruction=false; }
                    else if (c=='[') { out1=out1+'['; instruction=false; }
                    else if (c==']') { out1=out1+']'; instruction=false; }
                    else if (c=='$') { out1=out1+'$'; instruction=false; }*/
                    else if (!c.isLetter()) {
                        instruction=false;
                        if (inst=="backslash") out1+='\\';
                    } else {
                        inst+=c;
                    }
                } else {
                    bool mayOutput=true;
                    if (isMath) {
                        if (c=='_') { mayOutput=false; }
                        else if (c=='^') { mayOutput=false; }
                        else if ((c=='2')&&(c_last=='^')) { mayOutput=false; out1+='²'; }
                        else if ((c=='3')&&(c_last=='^')) { mayOutput=false; out1+='³'; }
                    }
                    if (c=='$') { isMath=!isMath; mayOutput=false; }
                    else if (c=='{') { mayOutput=false; }
                    else if (c=='}') { mayOutput=false; }
                    else if (c=='[') { mayOutput=false; }
                    else if (c==']') { mayOutput=false; }
                    if (mayOutput) { out1+=c; }
                }
            }
            /*if (!(((c=='{') || (c=='}') || (c=='[') || (c==']') || (c=='$')) && (c_last!='\\'))) {
                out1+=c;
            }*/
            i++;
        }
        out=out1;
    }

    //qDebug()<<"removeLaTeX_stag2 "<<out;

    out.replace("~", " ");

    //qDebug()<<"removeLaTeX_out   "<<out.simplified();

    return out.simplified();
}
