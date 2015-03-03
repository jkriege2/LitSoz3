#include "fdf.h"

#include "qmappablecombobox.h"
#include "qcompleterfromfile.h"
#include "jkstyledbutton.h"
#include "qenhancedlineedit.h"
#include "qcompleterplaintextedit.h"


#include <iostream>


bool fdfManager::loadFile(QString filename) {
    fdfFile* fdf= new fdfFile;
    int num=files.size();

    QDomDocument doc("fdf");
    fdf->filename=filename;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        lastError=tr("Could not open '%1'!").arg(filename);
        delete fdf;
        return false;
    }
    if (!doc.setContent(&file)) {
        file.close();
        lastError=tr("Could not parse '%1'!").arg(filename);
        delete fdf;
        return false;
    }
    file.close();

    // get document main element and check whether it is "<fdf>...</fdf>"
    QDomElement docElem = doc.documentElement();
    if ( docElem.tagName()!="fdf" ) {
        lastError=tr("File '%1' is not a valid FDF file, as it does not contain a <fdf> tag!").arg(filename);
        delete fdf;
        return false;
    }

    // get first child element and afterwards iterate through all child elements
    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            //std::cout<<"      <"<<e.tagName().toStdString()<<"> ?\n";
            bool ok=true;
            fdfWidgetProperty* wp = new fdfWidgetProperty;
            // read default widget properties
            wp->readonly=QVariant(e.attribute("readonly", "false")).toBool();
            wp->casesensitive=QVariant(e.attribute("casesensitive", "true")).toBool();
            wp->editable=QVariant(e.attribute("editable", "true")).toBool();
            wp->captionTextDefault=e.attribute("caption");
            wp->completerList=e.attribute("completer");
            wp->dataField=e.attribute("datafield");
            wp->digits=e.attribute("digits", "7").toInt();
            wp->maskDefault=e.attribute("mask");
            wp->regexpDefault=e.attribute("regexp");
            wp->formatDefault=e.attribute("format");
            wp->precision=e.attribute("precision", "2").toInt();
            wp->minVal=e.attribute("min", "0").toDouble();
            wp->maxVal=e.attribute("max", "32765").toDouble();
            wp->widgetWidth=e.attribute("widgetwidth", "-1").toInt();
            wp->widgetHeight=e.attribute("widgetheight", "-1").toInt();
            QDomElement ee=e.firstChildElement("item");
            while (!ee.isNull()) {
                QString l=ee.attribute("lang", "en");
                if (!wp->items.contains(l)) wp->items.insert(l, QStringList());
                wp->items[l].append(ee.text());
                wp->itemIDs.append(ee.attribute("id."));
                ee=ee.nextSiblingElement("item");
            }
            //wp=e.attribute("");
            // find attributes of the form caption.language-id
            QDomNamedNodeMap m=e.attributes();
            for (int i=0; i<m.size(); i++) {
                QDomAttr a=m.item(i).toAttr();
                if (!a.isNull()) {
                    if (a.name().startsWith("caption.")) {
                        //std::cout<<"    found "<<a.name().toStdString()<<" ("<<a.name().remove("caption.").toStdString()<<") = '"<<a.value().toStdString()<<"'\n";
                        wp->captionText[a.name().remove("caption.")]=a.value();
                    } else if (a.name().startsWith("mask.")) {
                        //std::cout<<"    found "<<a.name().toStdString()<<" ("<<a.name().remove("mask.").toStdString()<<") = '"<<a.value().toStdString()<<"'\n";
                        wp->mask[a.name().remove("mask.")]=a.value();
                    } else if (a.name().startsWith("regexp.")) {
                        //std::cout<<"    found "<<a.name().toStdString()<<" ("<<a.name().remove("mask.").toStdString()<<") = '"<<a.value().toStdString()<<"'\n";
                        wp->mask[a.name().remove("regexp.")]=a.value();
                    } else if (a.name().startsWith("format.")) {
                        //std::cout<<"    found "<<a.name().toStdString()<<" ("<<a.name().remove("format.").toStdString()<<") = '"<<a.value().toStdString()<<"'\n";
                        wp->format[a.name().remove("format.")]=a.value();
                    }

                }
            }

            if (e.tagName()=="options") {           // options section
                // parse <ID> subtag of <options>
                QDomNodeList nl=e.elementsByTagName("ID");
                if (nl.count()>0) {
                    fdf->ID=nl.item(0).toElement().text();
                } else {
                    lastError=tr("FDF file '%1' does not contain the <ID> tag!").arg(filename);
                    delete fdf;
                    return false;
                }
                // parse <name> subtag of <options>
                nl=e.elementsByTagName("name");
                for (int i=0; i<nl.count(); i++) {
                    QDomElement ne = nl.item(i).toElement();
                    if (ne.hasAttribute("language")) {
                        fdf->name[ne.attribute("language")]=ne.text();
                    }
                }
                // parse <icon> subtag of <options>
                QDomElement ni=e.firstChildElement("icon");
                if (!ni.isNull()) {
                    QString iconFile=QFileInfo(filename).absolutePath()+"/"+ni.text();
                    if (QFile::exists(iconFile))
                        fdf->icon=QPixmap(iconFile);
                    else
                        fdf->icon=QPixmap(":/emptyfdffile.png");
                } else fdf->icon=QPixmap(":/emptyfdffile.png");

                // parse <description> subtag of <options>
                nl=e.elementsByTagName("description");
                if (nl.count()>0) for (int i=0; i<nl.count(); i++) {
                    QDomElement ne = nl.item(i).toElement();
                    if (ne.hasAttribute("language")) {
                        fdf->description[ne.attribute("language")]=ne.text();
                    }
                }
                ok=false;
            } else if (e.tagName()=="edit") {       // simple string edit
                wp->type=fdfManager::fdfWidgetEdit;
            } else if (e.tagName()=="regexpedit") {       // simple string edit
                wp->type=fdfManager::fdfWidgetRegexpEdit;
            } else if (e.tagName()=="memo") {       // multiline string edit
                wp->type=fdfManager::fdfWidgetMemo;
            } else if (e.tagName()=="separator") {       // separator
                wp->type=fdfManager::fdfWidgetSeparator;
            } else if (e.tagName()=="combobox") {   // combo box
                wp->type=fdfManager::fdfWidgetComboBox;
            } else if (e.tagName()=="wwwedit") {    // WWW URL edit
                wp->type=fdfManager::fdfWidgetWWWEdit;
            } else if (e.tagName()=="authors") {    // authors widget
                wp->type=fdfManager::fdfWidgetAuthors;
            } else if (e.tagName()=="dateedit") {   // date editing widget
                wp->type=fdfManager::fdfWidgetDateEdit;
            } else if (e.tagName()=="intedit") {    // integer number editing widget
                wp->type=fdfManager::fdfWidgetIntEdit;
            } else if (e.tagName()=="floatedit") {  // float number editing widget
                wp->type=fdfManager::fdfWidgetFloatEdit;
            } else if (e.tagName()=="intspinbox") {    // integer number editing widget (spinbox)
                wp->type=fdfManager::fdfWidgetIntSpinbox;
            } else if (e.tagName()=="floatspinbox") {  // float number editing widget (spinbox)
                wp->type=fdfManager::fdfWidgetFloatSpinbox;
            } else {
                ok=false;
            }
            //std::cout<<"      <"<<e.tagName().toStdString()<<"> adding\n";
            if (ok) { // if a valid widget tag was recognized ... add the widget struct to the list
                      // note that the options tag is NOT a widget tag and this section will bes skipped!
                 fdf->widget.push_back(wp);
            } else {
                delete wp;
            }
            //std::cout<<"      <"<<e.tagName().toStdString()<<"> OK\n";
        }
        n = n.nextSibling();
    }

    files.push_back(fdf);
    idmap[fdf->ID]=num;
    return true;
}


QWidget* fdfManager::createWidgets(QString ID, QString language, QString configDir, QWidget* parent, LS3Datastore* datastore, QVector<QPointer<QWidget> >* widgetvec) {
    int row=0;
    int iID=-1;
    QWidget* wret=new QWidget(parent);
    QFormLayout* layout=new QFormLayout();
    layout->setLabelAlignment(Qt::AlignTop|Qt::AlignRight);
    wret->setLayout(layout);
    if (widgetvec!=NULL) widgetvec->clear();

    if (idmap.contains(ID)) {
        iID=idmap[ID];

        fdfFile* f=files[iID];
        for (int i=0; i<f->widget.size(); i++) {
            fdfWidgetProperty* wp=f->widget[i];

            QString l=wp->captionText.value(language, wp->captionTextDefault);
            QWidget* w=NULL;
            if (wp->type==fdfWidgetAuthors) {
                QCompleterPlainTextEdit* e=new QCompleterPlainTextEdit(wret);
                e->setAppendContents(false);
                e->setTabChangesFocus(true);
                w=e;
                if (datastore->getNamesModel()) e->setCompleter(authorsdata);
                e->setReadOnly(wp->readonly);
                datastore->addMapping(e, wp->dataField);
            } else if (wp->type==fdfWidgetComboBox) {
                QMappableComboBox* e=new QMappableComboBox(wret);
                w=e;
                e->setEditable(wp->editable);
                if (wp->items.size()>0) {
                    for (int ii=0; ii<qMax(wp->items[language].size(), wp->itemIDs.size()); ii++) {
                        e->addItem(wp->items[language].value(i, wp->itemIDs.value(i, "")), wp->itemIDs.value(i, wp->items[language].value(i, "")));
                    }
                }
                else if (wp->completerList.size()>0) e->setFilename(configDir+"/completers/"+wp->completerList+".lst");
                if (wp->items.size()>0) {
                    datastore->addMapping(e, wp->dataField, "data");
                } else {
                    datastore->addMapping(e, wp->dataField, "text");
                }
            } else if (wp->type==fdfWidgetSeparator) {
                QFrame* f=new QFrame(wret);
                f->setFrameShape(QFrame::HLine);
                f->setMidLineWidth(1);
                f->setLineWidth(0);
                f->setMaximumHeight(5);
                f->setFrameShadow(QFrame::Raised);
                layout->addRow(f);
            } else if (wp->type==fdfWidgetDateEdit) {
                QDateEdit* e=new QDateEdit(wret);
                w=e;
                e->setDisplayFormat(wp->format.value(language, wp->formatDefault));
                e->setReadOnly(wp->readonly);
                datastore->addMapping(e, wp->dataField, "date");
            } else if (wp->type==fdfWidgetEdit) {
                QLineEdit* e=new QEnhancedLineEdit(wret);
                w=e;
                e->setInputMask(wp->mask.value(language, wp->maskDefault));
                e->setReadOnly(wp->readonly);
                if (wp->completerList.size()>0) {
                    QCompleterFromFile* c=new QCompleterFromFile(e);
                    c->setFilename(configDir+"/completers/"+wp->completerList+".lst");
                    e->setCompleter(c);
                }
                datastore->addMapping(e, wp->dataField);
            } else if (wp->type==fdfWidgetRegexpEdit) {
                QLineEdit* e=new QEnhancedLineEdit(wret);
                w=e;
                if (wp->casesensitive) {
                    e->setValidator(new QRegExpValidator(QRegExp(wp->regexp.value(language, wp->regexpDefault), Qt::CaseSensitive, QRegExp::RegExp2), e));
                } else {
                    e->setValidator(new QRegExpValidator(QRegExp(wp->regexp.value(language, wp->regexpDefault), Qt::CaseInsensitive, QRegExp::RegExp2), e));
                }
                if (wp->completerList.size()>0) {
                    QCompleterFromFile* c=new QCompleterFromFile(e);
                    c->setFilename(configDir+"/completers/"+wp->completerList+".lst");
                    e->setCompleter(c);
                }
                e->setReadOnly(wp->readonly);
                datastore->addMapping(e, wp->dataField);
            } else if (wp->type==fdfWidgetFloatEdit) {
                QLineEdit* e=new QLineEdit(wret);
                w=e;
                e->setValidator(new QDoubleValidator(wp->minVal, wp->maxVal, wp->precision, e));
                e->setReadOnly(wp->readonly);
                datastore->addMapping(e, wp->dataField);
            } else if (wp->type==fdfWidgetIntEdit) {
                QLineEdit* e=new QLineEdit(wret);
                w=e;
                e->setValidator(new QIntValidator(wp->minVal, wp->maxVal, e));
                e->setReadOnly(wp->readonly);
                datastore->addMapping(e, wp->dataField);
            } else if (wp->type==fdfWidgetFloatSpinbox) {
                QDoubleSpinBox* e=new QDoubleSpinBox(wret);
                w=e;
                e->setRange(wp->minVal, wp->maxVal);
                e->setReadOnly(wp->readonly);
                e->setDecimals(wp->precision);
                datastore->addMapping(e, wp->dataField);
            } else if (wp->type==fdfWidgetIntSpinbox) {
                QSpinBox* e=new QSpinBox(wret);
                w=e;
                e->setRange(wp->minVal, wp->maxVal);
                e->setReadOnly(wp->readonly);
                datastore->addMapping(e, wp->dataField);
            } else if (wp->type==fdfWidgetMemo) {
                QCompleterPlainTextEdit* e=new QCompleterPlainTextEdit(wret);
                e->setTabChangesFocus(true);
                w=e;
                e->setReadOnly(wp->readonly);
                if (wp->completerList.size()>0) {
                    QCompleterFromFile* c=new QCompleterFromFile(e);
                    c->setFilename(configDir+"/completers/"+wp->completerList+".lst");
                    e->setCompleter(c);
                }
                datastore->addMapping(e, wp->dataField);
            } else if (wp->type==fdfWidgetWWWEdit) {
                QEnhancedLineEdit* e=new QEnhancedLineEdit(wret);
                JKStyledButton* tb= new JKStyledButton(JKStyledButton::OpenURL, e, e);
                e->addButton(tb);
                e->setReadOnly(wp->readonly);
                datastore->addMapping(e, wp->dataField);
                if (wp->completerList.size()>0) {
                    QCompleterFromFile* c=new QCompleterFromFile(e);
                    c->setFilename(configDir+"/completers/"+wp->completerList+".lst");
                    e->setCompleter(c);
                }
                w=e;
            }
            if (w!=NULL) {
                if (widgetvec!=NULL) widgetvec->append(w);
                layout->addRow(l, w);
                //layout->addWidget(w, row, 2);
                QSize s=w->size();
                if (wp->widgetHeight>-1) s.setHeight(wp->widgetHeight);
                if (wp->widgetWidth>-1) s.setWidth(wp->widgetWidth);
                w->resize(s);
                if (wp->widgetHeight>-1) w->setMaximumHeight(wp->widgetHeight);
                if (wp->widgetWidth>-1) w->setMaximumWidth(wp->widgetWidth);
            }
            row++;
        }

    }

    return wret;
}





