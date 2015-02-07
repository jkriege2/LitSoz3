#include "filenametools.h"
#include "bibtools.h"
#include <QtCore>


QString cleanFilename(const QString& text, int maxLen, bool removeDot, bool removeSlash) {
    QString t=text.simplified();
    QString regexp="";
    t=t.remove(QRegExp(regexp+"[^\\w\\d \\_\\(\\)\\.\\/]"));
    t=t.replace(" ", "_");
    if (removeDot) t=t.remove('.');
    if (removeSlash) {
        t=t.remove('\\');
        t=t.remove('/');
    }
    if (maxLen>0) {
        if (t.contains('.')) {
            QString ext=QFileInfo(t).suffix();
            t=t.left(maxLen-ext.size()-1)+"."+ext;
        } else {
            t=t.left(maxLen);
        }
    }
    if (t.size()>0 && t[t.size()-1]=='.') t=t.left(t.size()-1);
    return t;
}

#define REPLACE_FROM_DATA(newName, partName, fieldName) \
    newName=newName.replace(partName, cleanFilename(cleanString(data.value(fieldName, "").toString())));

QString createFileName(const QString& scheme, const QMap<QString, QVariant>& data, const QString& filename, int maxLength) {
    QString originalName=QFileInfo(filename).baseName();
    QString extension=QFileInfo(filename).suffix();
    //std::cout<<"filename="<<filename.toStdString()<<"   oname="<<originalName.toStdString()<<"  ext="<<extension.toStdString()<<"\n";
    QString newName=scheme;
    QString authors=data.value("authors", "").toString();
    QString authorsFN=cleanFilename(formatEtalAuthorsFamilyNames(authors, 3, "etal", "_", "_", ""));
    QString editors=data.value("editors", "").toString();
    QString editorsFN=cleanFilename(formatEtalAuthorsFamilyNames(editors, 3, "etal", "_", "_", ""));;
    QString names=cleanFilename(formatEtalAuthorsFamilyNames(authors+"; "+editors, 3, "etal", "_", "_", ""));;
    if (scheme.isEmpty()) return filename;
    newName=newName.replace("%original_name%", originalName);
    newName=newName.replace("%extension%", extension);
    newName=newName.replace("%ext%", extension);
    newName=newName.replace("%authors%", cleanString(authorsFN));
    newName=newName.replace("%editors%", cleanString(editorsFN));
    newName=newName.replace("%names%", cleanString(names));
    REPLACE_FROM_DATA(newName, "%topic%", "topic");
    REPLACE_FROM_DATA(newName, "%title%", "title");
    REPLACE_FROM_DATA(newName, "%year%", "year");
    REPLACE_FROM_DATA(newName, "%publisher%", "publisher");
    REPLACE_FROM_DATA(newName, "%places%", "places");
    REPLACE_FROM_DATA(newName, "%journal%", "journal");
    REPLACE_FROM_DATA(newName, "%number%", "number");
    REPLACE_FROM_DATA(newName, "%volume%", "volume");
    REPLACE_FROM_DATA(newName, "%booktitle%", "booktitle");
    REPLACE_FROM_DATA(newName, "%series%", "series");
    REPLACE_FROM_DATA(newName, "%pages%", "pages");
    REPLACE_FROM_DATA(newName, "%isbn%", "isbn");
    REPLACE_FROM_DATA(newName, "%pmid%", "pmid");
    REPLACE_FROM_DATA(newName, "%doi%", "doi");
    REPLACE_FROM_DATA(newName, "%type%", "type");
    REPLACE_FROM_DATA(newName, "%topic%", "topic");
    REPLACE_FROM_DATA(newName, "%language%", "language");
    REPLACE_FROM_DATA(newName, "%uuid%", "uuid");
    return cleanFilename(newName, maxLength, false, true);
}


QString createFileDir(const QString& scheme, const QMap<QString, QVariant>& data, const QString& dirname, int maxLength) {
    QString originalName=dirname;
    //std::cout<<"filename="<<filename.toStdString()<<"   oname="<<originalName.toStdString()<<"  ext="<<extension.toStdString()<<"\n";
    QString newName=scheme;
    QString authors=data.value("authors", "").toString();
    QString authorsFN=cleanFilename(formatEtalAuthorsFamilyNames(authors, 3, "etal", "_", "_", ""));
    QString editors=data.value("editors", "").toString();
    QString editorsFN=cleanFilename(formatEtalAuthorsFamilyNames(editors, 3, "etal", "_", "_", ""));;
    QString names=cleanFilename(formatEtalAuthorsFamilyNames(authors+"; "+editors, 3, "etal", "_", "_", ""));;
    if (scheme.isEmpty()) return dirname;
    newName=newName.replace("%original_name%", originalName);
    newName=newName.replace("%authors%", cleanString(authorsFN));
    newName=newName.replace("%editors%", cleanString(editorsFN));
    newName=newName.replace("%names%", cleanString(names));
    REPLACE_FROM_DATA(newName, "%topic%", "topic");
    REPLACE_FROM_DATA(newName, "%title%", "title");
    REPLACE_FROM_DATA(newName, "%year%", "year");
    REPLACE_FROM_DATA(newName, "%publisher%", "publisher");
    REPLACE_FROM_DATA(newName, "%places%", "places");
    REPLACE_FROM_DATA(newName, "%journal%", "journal");
    REPLACE_FROM_DATA(newName, "%number%", "number");
    REPLACE_FROM_DATA(newName, "%volume%", "volume");
    REPLACE_FROM_DATA(newName, "%booktitle%", "booktitle");
    REPLACE_FROM_DATA(newName, "%series%", "series");
    REPLACE_FROM_DATA(newName, "%pages%", "pages");
    REPLACE_FROM_DATA(newName, "%isbn%", "isbn");
    REPLACE_FROM_DATA(newName, "%pmid%", "pmid");
    REPLACE_FROM_DATA(newName, "%doi%", "doi");
    REPLACE_FROM_DATA(newName, "%type%", "type");
    REPLACE_FROM_DATA(newName, "%topic%", "topic");
    REPLACE_FROM_DATA(newName, "%language%", "language");
    REPLACE_FROM_DATA(newName, "%uuid%", "uuid");
    //std::cout<<newName.toStdString()<<"\n";
    QString output=cleanFilename(newName, maxLength, false, false);
    return output.replace("//", "/");
}