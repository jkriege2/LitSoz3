#ifndef QRECENTFILESMENU_H
#define QRECENTFILESMENU_H

#include <QMenu>
#include <QSettings>
#include <QList>
#include <QStringList>
#include <QMap>
#include <QFileInfo>
#include <QFileIconProvider>
#include "lib_imexport.h"

/*! \brief a special QMenu that allows to manage a list of recently loaded files
    \ingroup tools_qt
 */
class LIB_EXPORT QRecentFilesMenu : public QMenu {
        Q_OBJECT
    public:
        explicit QRecentFilesMenu(QWidget *parent = 0);

        /** \brief set the max. number of files in the list */
        void setMaxRecentFilesCount(int num);
        /** \brief returns the max. number of files in the list */
        int maxRecentFilesCount() const;
        /** \brief show icons next to the recent files */
        void setShowIcons(bool enabled);
        bool doesShowIcons() const;

        void setAlwaysEnabled(bool enabled);

        /** \brief register an icon for a given filetype this overrides the display of system icons */
        void setIconForExtension(const QString& extension, const QIcon& icon);
        /** \brief set the default icon */
        void setDefaultIcon(const QIcon& defaultIcon);
        /** \brief use file icons from system */
        void setUseSystemFileIcons(bool use);

        /*! \brief save the list of recent files into a QSettings object.

            The settings are saved under the key
              \c [prefix] + "recentfilelist"
         */
        void storeSettings(QSettings& settings, QString prefix=QString(""));
        /*! \brief read the list of recent files from a QSettings object.

            The settings are saved under the key
              \c [prefix] + "recentfilelist"
         */
        void readSettings(QSettings& settings, QString prefix=QString(""));

        /** \brief add a new file to the list of recent files */
        void addRecentFile(const QString& filename);
        /** \brief clear all recent files */
        void clearRecentFiles();
        
    signals:
        /** \brief emitted when the user clicks one of the recent files in the menu. */
        void openRecentFile(const QString& filename);
    public slots:
        void setMenuEnabled(bool enabled);
    protected:
        bool alwaysDisabled;
        QList<QAction*> m_actions;
        QStringList m_files;
        QMap<QString, QIcon> m_fileIcons;
        QIcon m_defaultIcon;
        bool m_useSystemFileIcons;
        bool m_icons;
        bool m_alwaysEnabled;
        QFileIconProvider iconProvider;
        QString strippedName(const QString &fullFileName);
        QIcon iconForFile(const QFileInfo& fileinfo);
    protected slots:
        void updateActions();
        void intOpenRecentFile();
        
};

#endif // QRECENTFILESMENU_H
