#ifndef QMORETEXTOBJECT_H
#define QMORETEXTOBJECT_H

#include <QTextObjectInterface>

class QTextDocument;
class QTextFormat;
class QPainter;
class QRectF;
class QSizeF;
class QPicture;
class QTextCursor;
#include "../lib_imexport.h"

#include <QObject>


/*! \defgroup tools_qt_QTextObjects Additional QTextObjectInterface implementations
    \ingroup tools_qt

*/

/** \brief QTextObjectInterface to plot a SVG file (taken from Qt help)
 *  \ingroup tools_qt
 *
 */
class LIB_EXPORT QSvgTextObject : public QObject, public QTextObjectInterface {
        Q_OBJECT
        Q_INTERFACES(QTextObjectInterface)

     public:
         QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format);
         void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format);

     protected:
};

bool insertSVG(QTextCursor cursor, int SvgTextFormat, QString fileName);



/** \brief QTextObjectInterface to plot a QPicture
 *  \ingroup tools_qt
 *
 */
class LIB_EXPORT QPictureTextObject : public QObject, public QTextObjectInterface {
        Q_OBJECT
        Q_INTERFACES(QTextObjectInterface)

     public:
         QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format);
         void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format);

     protected:
};

LIB_EXPORT bool insertQPicture(QTextCursor cursor, int SvgTextFormat, const QPicture& picture, QSizeF size=QSizeF());
LIB_EXPORT bool insertQPicture(QTextCursor cursor, int SvgTextFormat, const QPicture& picture, QPointF translate, QSizeF size=QSizeF());


#endif // QMORETEXTOBJECT_H
