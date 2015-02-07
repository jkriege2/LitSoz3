#include "qmoretextobject.h"
#include <QTextFormat>

#include <QtGui>
#include <QtSvg>


QSizeF QSvgTextObject::intrinsicSize(QTextDocument* doc, int posInDocument, const QTextFormat &format) {
    QImage bufferedImage = qVariantValue<QImage>(format.property(1));
    QSize size = bufferedImage.size();

    if (size.width() > doc->textWidth())
        size *= doc->textWidth() / (double) size.width();


    return QSizeF(size);
}


void QSvgTextObject::drawObject(QPainter *painter, const QRectF &rect, QTextDocument* doc, int posInDocument, const QTextFormat &format) {
    QImage bufferedImage = qVariantValue<QImage>(format.property(1));

    painter->drawImage(rect, bufferedImage);
}

bool insertSVG(QTextCursor cursor, int SvgTextFormat, QString fileName) {
     QFile file(fileName);
     if (!file.open(QIODevice::ReadOnly)) {
         return false;
     }

     QByteArray svgData = file.readAll();

     QTextCharFormat svgCharFormat;
     svgCharFormat.setObjectType(SvgTextFormat);
     QSvgRenderer renderer(svgData);

     QImage svgBufferImage(renderer.defaultSize(), QImage::Format_ARGB32);
     QPainter painter(&svgBufferImage);
     renderer.render(&painter, svgBufferImage.rect());

     svgCharFormat.setProperty(1, svgBufferImage);

     cursor.insertText(QString(QChar::ObjectReplacementCharacter), svgCharFormat);
     return true;
}











QSizeF QPictureTextObject::intrinsicSize(QTextDocument* doc, int posInDocument, const QTextFormat &format) {
    QByteArray data = format.property(1).toByteArray();
    QSizeF gsize= format.property(2).toSizeF();
    QPicture pic;
    pic.setData(data.constData(), data.size());
    QSizeF size = gsize;//pic.boundingRect().size();

    if (size.width() > doc->textWidth())
        size *= doc->textWidth() / (double) size.width();

    return QSizeF(size);
}


void QPictureTextObject::drawObject(QPainter *painter, const QRectF &rect, QTextDocument* doc, int posInDocument, const QTextFormat &format) {
    QByteArray data = format.property(1).toByteArray();
    QPicture pic;
    pic.setData(data.constData(), data.size());
    QSizeF size = pic.boundingRect().size();

    painter->save();
        painter->translate(rect.topLeft());
        painter->save();
            double scale=rect.width()/size.width();
            if (scale*size.height()>rect.height()) {
                scale=rect.height()/size.height();
            }
            painter->translate(-1.0*pic.boundingRect().topLeft());
            painter->scale(scale, scale);
            painter->drawPicture(0,0, pic);
        painter->restore();
    painter->restore();
}

bool insertQPicture(QTextCursor cursor, int QPictureTextFormat, const QPicture& picture, QSizeF siz) {


     QByteArray data(picture.data(), picture.size());

     QTextCharFormat qpictureCharFormat;
     qpictureCharFormat.setObjectType(QPictureTextFormat);

     qpictureCharFormat.setProperty(1, data);
     QSizeF size=siz;
     if (siz.isNull()) size=picture.boundingRect().size();
     qpictureCharFormat.setProperty(2, size);

     cursor.insertText(QString(QChar::ObjectReplacementCharacter), qpictureCharFormat);
     return true;
}

bool insertQPicture(QTextCursor cursor, int QPictureTextFormat, const QPicture& picture_in, QPointF translate, QSizeF siz) {
     QPicture picture;
     QPainter* p=new QPainter(&picture);
     p->drawPicture(translate, picture_in);
     p->end();
     delete p;

     QByteArray data(picture.data(), picture.size());

     QTextCharFormat qpictureCharFormat;
     qpictureCharFormat.setObjectType(QPictureTextFormat);

     qpictureCharFormat.setProperty(1, data);
     QSizeF size=siz;
     if (siz.isNull()) size=picture.boundingRect().size();
     qpictureCharFormat.setProperty(2, size);

     cursor.insertText(QString(QChar::ObjectReplacementCharacter), qpictureCharFormat);
     return true;
}
