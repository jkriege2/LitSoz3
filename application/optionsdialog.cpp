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
    QString dsc=QString::fromUtf8("\u00C0\u00E0\u00C1\u00E1\u00C2\u00E2\u00C3\u00E3\u00C4\u00E4\u00C5\u00E5\u00C6\u00E6\u00C7\u00E7\u00C8\u00E8\u00C9\u00E9\u00CA\u00EA\u00CB\u00EB\u00CC\u00EC\u00CD\u00ED\u00CE\u00EE\u00CF\u00EF\u00D0\u00F0\u00D1\u00F1\u00D2\u00F2\u00D3\u00F3\u00D4\u00F4\u00D5\u00F5\u00D6\u00F6\u00D8\u00F8\u00D9\u00F9\u00DA\u00FA\u00DB\u00FB\u00DC\u00FC\u00DD\u00FD\u00DE\u00FE\u00DF\u00FF\u00B5\u00BF\u00A1\u20AC\u00A2\u00A3\u00A4\u00A5\u00AB\u00BB\u2018\u2019\u00B7\u00D7\u00F7\u00B6\u00B1");
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

