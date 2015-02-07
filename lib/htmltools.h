#ifndef HTMLTOOLS_H
#define HTMLTOOLS_H

#include<QMap>
#include<QString>
#include<QVariant>
#include "htmltokenizer.h"
#include "lib_imexport.h"

/*! \brief harvest as many metadata from HTML string as possible (using regular expressions internally)

    In addition to Dublin Core metadata, this tries to identify as much metadata as possible.

    \see <a href="http://dublincore.org/documents/dces/">http://dublincore.org/documents/dces/</a>
    \see <a href="http://www.crossref.org/CrossTech/2008/05/natures_metadata_for_web_pages_1.html">http://www.crossref.org/CrossTech/2008/05/natures_metadata_for_web_pages_1.html</a>
    \see <a href="http://www.monperrus.net/martin/accurate+bibliographic+metadata+and+google+scholar">http://www.monperrus.net/martin/accurate+bibliographic+metadata+and+google+scholar</a>
 */
LS3LIB_EXPORT QMap<QString, QVariant> extractMetadataFromHTML(const QString& data, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions, const QList<QString>& ands);
/** \brief same as extractMetadataFromHTML(), but does not extract COinS metadata */
LS3LIB_EXPORT QMap<QString, QVariant> extractHTMLMetadata(const QString& data, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions, const QList<QString>& ands);

/*! \brief extract metadata encoded in OpenURL COinS, as a list of records

     This function searches the first <span> element which contains COinS data and extracts this COinS data.

     \see <a href="http://ocoins.info/">OpenURL COinS: A Convention to Embed Bibliographic Metadata in HTML</a>
     \see <a href="http://ocoins.info/cobg.html">OpenURL COinS: Brief guide to Implementing OpenURL 1.0 Context Object for Journal Articles</a>
     \see <a href="http://ocoins.info/cobgbook.html">OpenURL COinS: Brief guide to Implementing OpenURL 1.0 ContextObject for Books</a>
     \see <a href="http://www.niso.org/kst/reports/standards/kfile_download?id%3Austring%3Aiso-8859-1=Z39-88-2004.pdf&pt=RkGKiXzW643YeUaYUqZ1BFwDhIG4-24RJbcZBWg8uE4vWdpZsJDs4RjLz0t90_d5_ymGsj_IKVaGZww13HuDlZQ8NBt1sTxP_v4iiGqH7rSaAeVDnMfeKJrrJ_JSEGPB">OpenURL</a>
  */
LS3LIB_EXPORT QList<QMap<QString, QVariant> > extractCoins(const QString& data, const QSet<QString>& name_prefixes, const QSet<QString>& name_additions, const QList<QString>& ands);


/*! \brief remove HTML comments from text */
LS3LIB_EXPORT QString removeHTMLComments(const QString& data);
/*! \brief remove HTML markup from text */
LS3LIB_EXPORT QString removeHTML(const QString& data);

LS3LIB_EXPORT QByteArray guessEncoding(const QByteArray& encodingFromReply, const QByteArray& data);

#endif // HTMLTOOLS_H
