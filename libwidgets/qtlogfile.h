/*
  Name: qtlogfile.h
  Copyright: (c) 2008-2010
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/


/** \file qtlogfile.h
  * \ingroup tools_qt
  *
  * A QTextBrowser descendent with completer capabilities
  */

#ifndef QTLOGFILE_H
#define QTLOGFILE_H

#include <QTextStream>
#include <QTextBrowser>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QSettings>
#include <QString>
#include <QList>
#include <QFile>
#include <QDateTime>
#include <QAction>
#include <QMenu>
#include <QFileInfo>
#include <QDesktopServices>
#include <QContextMenuEvent>
#include "../tools.h"
#include "../lib_imexport.h"
#include <QMutex>
#include <QMutexLocker>
#include <QVBoxLayout>
#include "qenhancedlineedit.h"
#include <QToolButton>
#include <QCheckBox>
#include "jkstyledbutton.h"


/** \brief this is the indention increment when calling LogFile.inc_indent()
  * \ingroup logging
 */
#define LOGFILE_INDENT_INC 2

/*! \brief A logging class for Qt which outputs to a QTextBrowser (from which it is derived) and a file
    \ingroup logging


  This class also introduces a store for the logging messages, i.e. all messages that were written out
  to a file will be stored internally (and cleared when the log widget is cleared). So when opening
  a file all messages logged so far will be written out to the file. This allows to define the log file
  AFTER creating the widget and to change the log file, while the logged messages are preserved.

  \note the log functions are thread safe!!!
  */

class LIB_EXPORT QtLogFile : public QWidget {
        Q_OBJECT
    protected:

        QTextBrowser* browser;
        QVBoxLayout* lay;
        QWidget* widFind;
        QEnhancedLineEdit* edtFind;
        QToolButton* btnFindNext;
        QToolButton* btnFindPrev;
        QCheckBox* chkCaseSensitive;
        QCheckBox* chkFindWholeWord;

        mutable QMutex* mutex;

        /** \brief filename of the log file, standard is \c logfile.log */
        QString log_filename;

        /** \brief file handle for the log file */
        QFile* log_file;
        /** \brief used for text I/O to file */
        QTextStream log_file_stream;

        /** \brief determines whether message will be put onto the console log_console */
        bool log_to_console;

        /** \brief determines whether message will be put into the file log_filename */
        bool log_to_file;

        /** \brief determines whether to append to a log file (true, default) or to generate a new one on each run (false) */
        bool log_file_append;

        /** \brief internal: this is set true if the last written character was a linebreak \c 0x0A */
        bool last_was_linebreak;

        /** \brief log date and time (formatted according to log_date_time_template) at the start of each line */
        bool log_date_time;

        /** \brief format string for date and time, if log_date_time is \c true */
        QString log_date_time_template;

        /** \brief internal: the current depth of indention */
        int current_indent;

        /** \brief internal: add indention spaces if necessary */
        virtual void do_indent();

        /** \brief internal: add indention spaces after each linebreak in the string */
        virtual QString indentify(QString text);

		/** \brief internal: output some text message without indetify step  */
		virtual void raw_log_text(QString message);
    public:
        /** \brief class constructor */
        QtLogFile(QWidget *parent = 0);
        /** \brief class destructor */
        virtual ~QtLogFile();
        /** \brief clear the text in the log widget */
        virtual void clearLog();
        /** \brief clear the text in the log store, but not in the widget */
        virtual void clearLogStore();
		/** \brief open a new log file and activate loggin to file
		 *
		 * This method also writes any information in the internal store to the file immediately,
		 * if the variable writeSTore is \c true.
		 */
		virtual void open_logfile(QString filename, bool writeStore);

        /** \brief save the widget settings to a QSettings object */
        void saveSettings(QSettings& s, QString namePrefix=QString(""));
        /** \brief read the widget settings from a QSettings object */
        void readSettings(QSettings& s, QString namePrefix=QString(""));


		/** \brief get the current log file name */
		GetMacro(QString, log_filename);
		/** \brief set whether to log to the console */
        SetMacroIBefore(bool, log_to_console, QMutexLocker locker(mutex));
		/** \brief get whether we are logging to the console */
		GetMacro(bool, log_to_console);
		/** \brief get whether we are logging to a file */
		GetMacro(bool, log_to_file);
		/** \brief set whether to append to a log_file (true) or to generate a new one (false) */
        SetMacroIBefore(bool, log_file_append, QMutexLocker locker(mutex));
		/** \brief get whether to append to a log file (true) or to generate a new one (false). */
		GetMacro(bool, log_file_append);

        GetMacro(bool, log_date_time);
        SetMacroIBefore(bool, log_date_time, QMutexLocker locker(mutex));
        GetMacro(QString, log_date_time_template);
        SetMacroIBefore(QString, log_date_time_template, QMutexLocker locker(mutex));

		/** \brief close the current log file (if one is opened) and deactivate loggin to file */
		virtual void close_logfile();

		/** \brief output a linebreak */
		virtual void log_linebreak();

		/** \brief output a horicontal line + linebreak */
		virtual void log_line();

		/** \brief output a double horicontal line + linebreak */
		virtual void log_dline();

		/** \brief output a horicontal line + linebreak which incorporates the text \c caption */
        virtual void log_header(QString caption);

		/** \brief output a double horicontal line + linebreak which incorporates the text \c caption */
        virtual void log_dheader(QString caption);

		/** \brief output a horicontal line + linebreak, as an error message */
		virtual void log_error_line();

		/** \brief output a double horicontal line + linebreak, as an error message */
		virtual void log_error_dline();

		/** \brief output a horicontal line + linebreak which incorporates the text \c caption, as an error message */
        virtual void log_error_header(QString caption);

		/** \brief output a double horicontal line + linebreak which incorporates the text \c caption, as an error message */
        virtual void log_error_dheader(QString caption);

		/** \brief output a horicontal line + linebreak, as an warning message */
		virtual void log_warning_line();

		/** \brief output a double horicontal line + linebreak, as an warning message */
		virtual void log_warning_dline();

		/** \brief output a horicontal line + linebreak which incorporates the text \c caption, as an warning message */
        virtual void log_warning_header(QString caption);

		/** \brief output a double horicontal line + linebreak which incorporates the text \c caption, as an warning message */
        virtual void log_warning_dheader(QString caption);

		/** \brief increase the indention depth */
        virtual void inc_indent();

		/** \brief decrease the indention depth */
        virtual void dec_indent();

    public slots:
		/** \brief output some text message */
		virtual void log_text(QString header, QString message);
		/** \brief output some warning message */
		virtual void log_warning(QString header, QString message);
		/** \brief output some error message */
		virtual void log_error(QString header, QString message);


		/** \brief output some text message */
		virtual void log_text(QString message);
		/** \brief output some warning message */
		virtual void log_warning(QString message);
		/** \brief output some error message */
		virtual void log_error(QString message);
        /** \brief output some error message  and adds a linebreak*/
        virtual void log_error_linebreak(QString message);
        /** \brief output some warning message  and adds a linebreak*/
        virtual void log_warning_linebreak(QString message);
        /** \brief output some text message  and adds a linebreak*/
        virtual void log_text_linebreak(QString message);


        /** \brief output some error message  and adds a linebreak*/
        virtual void log_error_linebreak_queue(QString message);
        /** \brief output some warning message  and adds a linebreak*/
        virtual void log_warning_linebreak_queue(QString message);
        /** \brief output some text message  and adds a linebreak*/
        virtual void log_text_linebreak_queue(QString message);

        void showLogFolder();
    signals:
        void emit_log_error_linebreak(QString message);
        void emit_log_warning_linebreak(QString message);
        void emit_log_text_linebreak(QString message);
    protected slots:
        void showContextMenu(const QPoint& p );
        void searchLog(bool checked);
        void findNext();
        void findPrev();
    protected:

        QAction* actSelectAll;
        QAction* actClear;
        QAction* actCopy;
        QAction* actShowLogFolder;
        QAction* actFind;
        QAction* actFindNext;
        QAction* actFindPrev;

        QString htmlify(const QString& message) const;
        QString dehtmlify(const QString& message) const;


    private:

        /** \brief color of a standard text message */
        QColor textColor;
        /** \brief color of an error message */
        QColor errorColor;
        /** \brief color of a warning message */
        QColor warningColor;
        /** \brief color of an information message */
        QColor infoColor;
        /** \brief the current output color (initially: textColor) */
        QColor currentColor;
        /** \brief \c true, when the current message should be typeset in bold letters */
        bool currentBold;
        /** \brief font size to use for messages */
        int fontSize;
        /** \brief font family to use for messages */
        QString fontName;
        /** \brief indicates whether to use fixed pitch */
        bool fixedPitch;
        /** \brief store for the loged messages text */
        QList<QString> logStore;
};

#endif // QTLOGFILE_H
