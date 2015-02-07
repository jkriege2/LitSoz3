#ifndef PREVIEWSTYLEMANAGER_H
#define PREVIEWSTYLEMANAGER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QPair>
#include <QtAlgorithms>
#include <QMap>
#include <QVariant>

/*! \brief manages and executes preview styles

*/
class PreviewStyleManager : public QObject {
        Q_OBJECT
    public:
        PreviewStyleManager(QObject* parent);
        virtual ~PreviewStyleManager();


        /** \brief return a list of the names of the preview styles */
        QStringList styles() const;

        /** \brief return the evaluated HTML for the i-th style */
        QString createPreview(int i, const QMap<QString, QVariant>& data) const;

        /** \brief return the name for the i-th style */
        QString getName(int i) const;
    protected:
};

#endif // PREVIEWSTYLEMANAGER_H
