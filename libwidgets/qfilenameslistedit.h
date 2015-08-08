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

#ifndef QFILENAMESLISTEDIT_H
#define QFILENAMESLISTEDIT_H

#include <QTableView>
#include <QListView>
#include <QWidget>
#include <QStringListModel>
#include <QFileIconProvider>
#include <QItemDelegate>
#include <QPixmap>
#include <QAbstractButton>
#include "lib_imexport.h"
#include <QVBoxLayout>
#include <QMimeData>
#include <QUrl>

class QFilenamesListEdit;

/*! \brief QStrignListModel descendent to manage lists of files

    entries in this model are treated as files, relative to a given baseDir!
    The items are decorated with a file icon, extracted from a QFileIconProvider
*/
class LS3LIBWIDGETS_EXPORT QFilenamesListEditModel: public QStringListModel {
        Q_OBJECT
    public:
        QFilenamesListEditModel( QObject *parent = 0 );
        QFilenamesListEditModel( const QStringList & strings, QObject * parent = 0 );
        virtual QVariant data(const QModelIndex &index, int role) const;
        void setBaseDir(const QString& baseDir);
        void addNewFile(const QString& filename);
        virtual Qt::DropActions supportedDropActions() const;
        virtual Qt::DropActions supportedDragActions() const;
        Qt::ItemFlags flags(const QModelIndex &index) const;

        QMimeData * mimeData(const QModelIndexList & indexes) const;
        QStringList mimeTypes() const;
protected:
        QFileIconProvider* iconProvider;
        QString m_baseDir;
        QFilenamesListEdit* fnedt;

};



/*! \brief QItemDelegate that is used together with QFilenamesListEditModel

    This delegate displays additional symbols at the rhs of the list, that allow to remove files,
    add new files, select a new file and open the files. URLs (starting with http:// ...) are also
    recognized and treated separately.
*/
class QFilenamesListEditDelegate : public QItemDelegate {
        Q_OBJECT
    public:
        /** Default constructor */
        QFilenamesListEditDelegate(QObject *parent=NULL, QWidget* focusWidget=NULL);
        /** Default destructor */
        virtual ~QFilenamesListEditDelegate();
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setBaseDir(const QString& baseDir);
protected:
        QIcon pixOpen;
        QIcon pixExecute;
        QIcon pixDelete;
        QIcon pixAdd;
        QIcon pixWWW;
        QIcon pixUp;
        QIcon pixDown;
        int pixWidth;
        int pixDistance;
        QString m_baseDir;
        QWidget* focusWidget;

        enum Buttons {
            OpenButton,
            DeleteButton,
            ExecuteButton,
            AddButton,
            RightOfButtons
        };

        int buttonXPos(Buttons i, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        bool isUrl(const QString &fn) const;

        bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
};



/*! \brief manages a list of files

    This widget dispalys a listview with additional controls that allow to manage a list of files.

    \note an event filter on the QListView is used to catch focusOut events. For each such event the
    widget itself gets and looses the focus, so data is stored by QDataMapper.
*/
class LS3LIBWIDGETS_EXPORT QFilenamesListEdit : public QWidget {
        Q_OBJECT
        Q_PROPERTY(QStringList data READ getFiles WRITE setFiles USER true)
        Q_PROPERTY(QString filesAsString READ getFilesAsString WRITE setFilesFromString)

    public:
        explicit QFilenamesListEdit(QWidget *parent = 0);
        QStringList getFiles() const;
        QString getFilesAsString() const;
        void setBaseDir(const QString& baseDir);
        void addButton(QAbstractButton* button);
        void focusOut();
    signals:
        
    public slots:
        void setFilesFromString(const QString& files);
        void setFiles(const  QStringList& list);
        void addNewFile(const QString& filename);

    protected:
        QFilenamesListEditModel* m_files;
        //QTableView* list;
        QListView* list;
        QFilenamesListEditDelegate* delegate;
        QVBoxLayout* layButton;

        virtual void dropEvent(QDropEvent *event);
        virtual void dragEnterEvent(QDragEnterEvent* event);

};

class QFilenamesListEditEventFilter : public QObject {
        Q_OBJECT
    public:
        QFilenamesListEditEventFilter(QFilenamesListEdit* parent=NULL);
    protected:
        bool eventFilter(QObject *obj, QEvent *event);
        QFilenamesListEdit* fnlist;
};


#endif // QFILENAMESLISTEDIT_H
