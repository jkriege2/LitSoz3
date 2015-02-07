#ifndef LS3EXTRACTMETADATAPLUGIN_H
#define LS3EXTRACTMETADATAPLUGIN_H

#include <QString>
#include <QMap>
#include <QVariant>
#include "lib_imexport.h"

class LS3LIB_EXPORT LS3ExtractMetadataPlugin {
    public:
        /** \brief retrieve metadata from a given DOI (waits until requests are complete */
        virtual QMap<QString, QVariant> getMetadataFromDoi(QString DOI)=0;
};



Q_DECLARE_INTERFACE(LS3ExtractMetadataPlugin,
                     "jkrieger.de.litsoz3.plugins.LS3ExtractMetadataPlugin/1.0")

#endif // LS3EXTRACTMETADATAPLUGIN_H
