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

#include "optionsdialog.h"
#include <iostream>
OptionsDialog::OptionsDialog(QWidget* parent):
    QDialog(parent)
{
    setupUi(this);
}

OptionsDialog::~OptionsDialog()
{
    //dtor
}

void OptionsDialog::ensureCurrentStyle()
{
    on_cmbStyle_currentIndexChanged(cmbStyle->currentText());
    on_cmbStylesheet_currentIndexChanged(cmbStylesheet->currentText());
}

void OptionsDialog::on_cmbStylesheet_currentIndexChanged( const QString & text ) {
    QString fn=QString(QCoreApplication::applicationDirPath()+"/stylesheets/%1.qss").arg(text);
    QFile f(fn);
    f.open(QFile::ReadOnly);
    QTextStream s(&f);
    QString qss=s.readAll();
    //std::cout<<qss.toStdString()<<std::endl;
    this->setStyleSheet(qss);
}

void OptionsDialog::on_cmbStylesheet_highlighted( const QString & text ) {
    on_cmbStylesheet_currentIndexChanged(text);
}

void OptionsDialog::on_cmbStyle_currentIndexChanged( const QString & text ) {
//        QStyle* s=QStyleFactory::create(text);
//        if (s!=NULL) this->setStyle(s);
}

void OptionsDialog::on_cmbStyle_highlighted( const QString & text ) {
    on_cmbStyle_currentIndexChanged(text);
}


void OptionsDialog::on_btnDefaultNP_clicked()
{
    QStringList n=QString("da\nde\ndal\ndel\nder\ndi\ndo\ndu\ndos\nla\nle\nvan\nvande\nvon\nden").split("\n");
    QStringList o=edtNamePrefixes->toPlainText().split("\n");
    for (int i=0; i<n.size(); i++) {
        if (!o.contains(n[i])) o.append(n[i]);
    }
    edtNamePrefixes->setPlainText(o.join("\n"));
}

void OptionsDialog::on_btnDefaultNA_clicked()
{
    QStringList n=QString("jr\nsen\ni\nii\niii\niv\njun").split("\n");
    QStringList o=edtNameAdditions->toPlainText().split("\n");
    for (int i=0; i<n.size(); i++) {
        if (!o.contains(n[i])) o.append(n[i]);
    }
    edtNameAdditions->setPlainText(o.join("\n"));
}

void OptionsDialog::on_btnDefaultAND_clicked()
{
    QStringList n=QString("and\nund\n&\net").split("\n");
    QStringList o=edtAndWords->toPlainText().split("\n");
    for (int i=0; i<n.size(); i++) {
        if (!o.contains(n[i])) o.append(n[i]);
    }
    edtAndWords->setPlainText(o.join("\n"));
}

void OptionsDialog::on_btnDefaultSC_clicked()
{
    QString sc=edtSpecialCharacters->text();
    QString dsc=QString("ÀàÁáÂâÃãÄäÅåÆæÇçÈèÉéÊêËëÌìÍíÎîÏïĞğÑñÒòÓóÔôÕõÖöØøÙùÚúÛûÜüİıŞşßÿµ¿¡€¢£¤¥«»‘’·×÷¶±");
    for (int i=0; i<dsc.size(); i++) {
        if (!sc.contains(dsc[i])) sc+=dsc[i];
    }
    edtSpecialCharacters->setText(sc);
}

void OptionsDialog::on_tbutConfigDir_clicked(bool checked) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Config Directory"),
                                                 edtConfigDir->text(),
                                                 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    edtConfigDir->setText(dir);
}


void OptionsDialog::on_tbutInitialFile_clicked(bool checked) {
    QString dir = QFileDialog::getOpenFileName(this, tr("Select Startup File"),
                                                 edtInitialFile->text(),
                                                 tr("Databases (*.xld)"));
    edtInitialFile->setText(dir);
}

