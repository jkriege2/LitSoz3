#include "qkeysequenceedit.h"
#include <QDebug>

#include <QShortcut>
QKeySequenceEdit::QKeySequenceEdit(QWidget *parent) :
    QEnhancedLineEdit(parent)
{
    currentKS=QKeySequence();

    btnClear=new JKStyledButton(QIcon(":/QKeySequenceEdit/clear.png"), this);
    btnClear->setFocusPolicy(Qt::NoFocus);
    addButton(btnClear);
    connect(btnClear, SIGNAL(clicked()), this, SLOT(clearThis()));
}

QKeySequence QKeySequenceEdit::getKeySequence() const {
    return currentKS;
}

void QKeySequenceEdit::setKeySequence(QKeySequence sequence) {
    currentKS=sequence;
    setText(sequence.toString(QKeySequence::PortableText));
}

void QKeySequenceEdit::clearThis() {
    currentKS=QKeySequence();
    clear();
}

void QKeySequenceEdit::keyPressEvent(QKeyEvent *event) {
    QKeySequence sequence(event->key() | event->modifiers());
    event->accept();

    Qt::Key key = static_cast<Qt::Key>(event->key());

    // the user have clicked just and only the special keys Ctrl, Shift, Alt, Meta.
    if(key == Qt::Key_Control ||
        key == Qt::Key_Shift ||
        key == Qt::Key_Alt ||
        key == Qt::Key_Meta)
    {
        return;
    }

    if ((event->modifiers() & Qt::KeypadModifier) != 0)
        sequence=QKeySequence(event->key() | (event->modifiers() & (~Qt::KeypadModifier)));
    setText(sequence.toString(QKeySequence::PortableText));
    currentKS=sequence;
}

void QKeySequenceEdit::contextMenuEvent(QContextMenuEvent *event) {
    event->accept();
}

