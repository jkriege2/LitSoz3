#include "filetools.h"
#include <QtCore>


bool touchFile(const QString& filename) {
    if (QFile::exists(filename)) return true;
    QFile f(filename);
    if (f.open(QIODevice::WriteOnly)) {
        f.close();
        return true;
    }
    return false;
}
