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

#include "htmldelegate.h"
#include <QDebug>
#include <QTextEdit>
#include <QVariant>
#include <QTextDocument>
#include <QPainter>
#include <QApplication>
#include <QBrush>
#include <QPixmapCache>

HTMLDelegate::HTMLDelegate(QObject* parent):
    QStyledItemDelegate(parent)
{
    m_displayRichTextEditor=true;
    rxHTML=QRegExp("(<\\s*\\w+.*>)|(<\\s*/\\s*\\w+\\s*>)|(\\&\\w+\\;)");
    rxHTML.setMinimal(true);

}

HTMLDelegate::~HTMLDelegate()
{
    //dtor
}

bool HTMLDelegate::displayRichTextEditor() const {
    return m_displayRichTextEditor;
}

void HTMLDelegate::setDisplayRichTextEditor(bool d) {
    m_displayRichTextEditor=d;
}

void HTMLDelegate::drawCheck(QPainter *painter,
                              const QStyleOptionViewItem &option,
                              const QRect &rect, Qt::CheckState state) const
{
    if (!rect.isValid())
        return;

    QStyleOptionViewItem opt(option);
    opt.rect = rect;
    opt.state = opt.state & ~QStyle::State_HasFocus;

    switch (state) {
    case Qt::Unchecked:
        opt.state |= QStyle::State_Off;
        break;
    case Qt::PartiallyChecked:
        opt.state |= QStyle::State_NoChange;
        break;
    case Qt::Checked:
        opt.state |= QStyle::State_On;
        break;
    }

    QStyle *style = QApplication::style();
    style->drawPrimitive(QStyle::PE_IndicatorViewItemCheck, &opt, painter, NULL);
}


void HTMLDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QVariant data=index.data(Qt::DisplayRole).toString();
    QVariant check=index.data(Qt::CheckStateRole);
    QBrush back=index.data(Qt::BackgroundRole).value<QBrush>();
    QPoint offset=QPoint(0,0);
    //qDebug()<<option.palette.base()<<option.palette.background();
    if (check.isValid()) {
        offset=QPoint(option.rect.height()+2, 0);
    }
    if (index.data(Qt::BackgroundRole).isValid()) {
        painter->fillRect(option.rect, back);
    }
    if ((data.type()==QVariant::String || data.type()==QVariant::StringList)&&data.toString().contains(rxHTML)) {
            QString pre, post;
            pre="<font color=\""+option.palette.text().color().name()+"\">";
            post="</font>";
            if (option.state & QStyle::State_Selected) {
                painter->fillRect(option.rect, option.palette.highlight());
                pre="<font color=\""+option.palette.highlightedText().color().name()+"\">";
                post="</font>";
            }

            QString html=pre+data.toString()+post;
            QRect r(QPoint(0, 0), option.rect.size()-QSize(offset.x(), 0));
            QString id=QString::number(r.width())+"_"+QString::number(r.height())+"_"+html;


            QPixmap pix;
            if (!QPixmapCache::find(id, &pix)) {
                //qDebug()<<"   draw html: create pixmap dT="<<double(et.nsecsElapsed())/1000000.0<<"ms";
                {
                    QImage img=QImage(r.width(), r.height(), QImage::Format_ARGB32_Premultiplied);
                    img.fill(Qt::transparent);
                    QPainter pixp;
                    pixp.begin(&img);
                    QTextDocument doc;
                    doc.setHtml(html);
                    doc.drawContents(&pixp, r);
                    pixp.end();
                    pix=QPixmap::fromImage(img);
                }
                /*bool ok=*/
                QPixmapCache::insert(id, pix);;
                //qDebug()<<" inserted pixmap: "<<ok<<"   cache_limit="<<QPixmapCache::cacheLimit();
            }
            painter->save();
            if (check.isValid()) {
                drawCheck(painter, option, QRect(option.rect.left(),option.rect.top(),option.rect.height(),option.rect.height()), check.toInt()!=0?Qt::Checked:Qt::Unchecked);
            }

            painter->translate(option.rect.topLeft()+offset);
            painter->drawPixmap(0,0, pix);
            painter->restore();





    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize HTMLDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QVariant data=index.data(Qt::DisplayRole).toString();
    if (data.type()==QVariant::String) {
        QTextDocument doc;
        doc.setHtml(data.toString());
        return doc.size().toSize();
    }
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *HTMLDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QVariant data=index.data(Qt::DisplayRole).toString();
    if (m_displayRichTextEditor&&(data.type()==QVariant::String)) {
        return new QTextEdit(parent);
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void HTMLDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QVariant data=index.data(Qt::DisplayRole).toString();
    if (m_displayRichTextEditor&&(data.type()==QVariant::String)) {
        QTextEdit *te=static_cast<QTextEdit*>(editor);
        te->setHtml(data.toString());
        return;
    }
    QStyledItemDelegate::setEditorData(editor, index);
}

void HTMLDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QTextEdit *te=qobject_cast<QTextEdit*>(editor);
    if (m_displayRichTextEditor&&te) {
        model->setData(index, te->toHtml());
        return ;
    }
    QStyledItemDelegate::setModelData(editor, model, index);
}

void HTMLDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QTextEdit *te=qobject_cast<QTextEdit*>(editor);
    QVariant check=index.data(Qt::CheckStateRole);
    QPoint offset=QPoint(0,0);
    if (check.isValid()) {
        offset=QPoint(option.rect.height()+2,0);
    }
    if (m_displayRichTextEditor&&te) {
        editor->setGeometry(QRect(offset+option.rect.topLeft(), QSize(option.rect.width()-offset.x(), option.rect.height())));
        return;
    }
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}
