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

#include "jkcharacterscreenkeyboard.h"

#include <QKeyEvent>
#include <QApplication>
#include <QPushButton>

JKCharacterScreenKeyboard::JKCharacterScreenKeyboard(QWidget* parent):
    QWidget(parent)
{
    m_characters="ABC";
    m_generateApplicationEvent=true;
    QFontMetrics fm(font());
    int s=(double)fm.height()*1.5;
    m_buttonSize=QSize(s,s);
    m_focus=parent;
    setFocusPolicy(Qt::NoFocus);
    lay=new FlowLayout(this, 2, 0, 0);
    setLayout(lay);
    populateLayout();
}

JKCharacterScreenKeyboard::~JKCharacterScreenKeyboard()
{
    //dtor
}

QSize JKCharacterScreenKeyboard::buttonSize() const {
    return m_buttonSize;
}

void JKCharacterScreenKeyboard::setButtonSize(QSize s) {
    m_buttonSize=s;
    for (int i=0; i<m_buttons.size(); i++) {
        QAbstractButton* but=m_buttons[i];
        but->setMaximumSize(s);
        but->setMinimumSize(s);
    }
}


void JKCharacterScreenKeyboard::setKBFocusWidget(QWidget* focus) {
    m_focus=focus;
}

QWidget* JKCharacterScreenKeyboard::KBFocusWidget() {
    return m_focus;
}

void JKCharacterScreenKeyboard::setCharacters(const QString& characters) {
    m_characters=characters;
    populateLayout();
}

QString JKCharacterScreenKeyboard::characters() const {
    return m_characters;
}

void JKCharacterScreenKeyboard::setGenerateFocusEvent(bool enable) {
    m_generateApplicationEvent=enable;
}

bool JKCharacterScreenKeyboard::generateFocusEvent() const {
    return m_generateApplicationEvent;
}

void JKCharacterScreenKeyboard::buttonClicked() {
    int idx=-1;
    if (sender()) {
        QAbstractButton* but=qobject_cast<QAbstractButton*>(sender());
        if (but) {
            idx=m_buttons.indexOf(but);
        }
    }
    if ((idx>-1)&&(idx<m_characters.size())) {
        QChar character=m_characters[idx];
        emit characterClicked(character);
        if (m_generateApplicationEvent && m_focus) {
            QKeyEvent keyEvent(QEvent::KeyPress, 0, Qt::NoModifier, QString(character));
            QApplication::sendEvent(m_focus->focusWidget(), &keyEvent);
        }
    }
}

void JKCharacterScreenKeyboard::populateLayout() {
    bool upEn=updatesEnabled();
    if (upEn) setUpdatesEnabled(false);
    // remove all buttons from layout and delete them
    for (int i=0; i<m_buttons.size(); i++) {
        disconnect(m_buttons[i], SIGNAL(clicked()), this, SLOT(buttonClicked()));
        lay->removeWidget(m_buttons[i]);
        delete m_buttons[i];
    }
    m_buttons.clear();

    for (int i=0; i<m_characters.size(); i++) {
        QChar character=m_characters[i];
        if (character.isPrint()){
            QPushButton* but=new QPushButton(QString(character), this);
            m_buttons.append(but);
            but->setFocusPolicy(Qt::NoFocus);
            but->setMaximumSize(m_buttonSize);
            but->setMinimumSize(m_buttonSize);
            connect(but, SIGNAL(clicked()), this, SLOT(buttonClicked()));
            lay->addWidget(but);
        }
    }
    if (upEn) setUpdatesEnabled(true);
}


