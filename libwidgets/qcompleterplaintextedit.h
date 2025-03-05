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

#ifndef QCOMPLETERPLAINTEXTEDIT_H
#define QCOMPLETERPLAINTEXTEDIT_H

#include <QPlainTextEdit>
#include <QCompleter>
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "litsoz3widgets_export.h"

class LITSOZ3WIDGETS_EXPORT QCompleterPlainTextEdit : public QPlainTextEdit {
    Q_OBJECT
public:
    /** Default constructor */
    QCompleterPlainTextEdit(QWidget* parent = 0);
    /** Default destructor */
    virtual ~QCompleterPlainTextEdit();
    void setCompleter(QCompleter *c);
    QCompleter *completer() const { return c; }
    void setAppendContents(bool app) { appendContents=app; }
    //void setSingleLine(bool app) { singleLine=app; }

protected:
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent ( QFocusEvent * event ) ;

private slots:
    void insertCompletion(const QString &completion);

private:
    QString textUnderCursor() const;

private:
    QCompleter *c;
    bool appendContents;
    //bool singleLine;
};

#endif // QCOMPLETERPLAINTEXTEDIT_H
