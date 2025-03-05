#include "regextools.h"
#include<QRegularExpression>
#include<QRegularExpressionMatch>
#include "cachingtools.h"


namespace LS3Tools_private {
    struct LS3RXKey {
        inline LS3RXKey(const QString& regex_, QRegularExpression::PatternOptions options_=QRegularExpression::NoPatternOption, bool wildcard_=false):
            regex(regex_), options(options_), wildcard(wildcard_)
        {}

        inline std::shared_ptr<QRegularExpression> make() const {
            if (wildcard) {
                return std::make_shared<QRegularExpression>(QRegularExpression::fromWildcard(regex, options.testAnyFlag(QRegularExpression::CaseInsensitiveOption)?Qt::CaseInsensitive:Qt::CaseSensitive));
            } else {
                return std::make_shared<QRegularExpression>(regex,options);
            }
        }

        bool operator==(const LS3RXKey& other) const {
            return (wildcard==other.wildcard) && (options==other.options) && (regex==other.regex);
        }
        bool operator!=(const LS3RXKey& other) const {
            return !operator==(other);
        }
        QString regex;
        QRegularExpression::PatternOptions options;
        bool wildcard;
    };
}

template<>
struct std::hash<LS3Tools_private::LS3RXKey>
{
    std::size_t operator()(const LS3Tools_private::LS3RXKey& s) const noexcept
    {
        const std::size_t h1 = std::hash<QString>{}(s.regex);
        const std::size_t h2 = std::hash<bool>{}(s.wildcard);
        const std::size_t h3 = std::hash<int>{}(s.options);
        return (h1 ^ (h2 << 1))^ (h3 << 1); // or use boost::hash_combine
    }
};

std::shared_ptr<QRegularExpression> ls3_getRegExFromCache(const QString& pattern, QRegularExpression::PatternOptions options, bool wildcard) {
    static LS3DataCache<std::shared_ptr<QRegularExpression>, LS3Tools_private::LS3RXKey, LS3DataCacheThreadSafe> rxCache([](const LS3Tools_private::LS3RXKey& key) { return key.make(); }, 500);
    return rxCache.get_inline(pattern, options, wildcard);
}


bool ls3_rxContains(const QString& text, const QString &regex, size_t offset, QStringList* caps)
{
    const auto rx=ls3_getRegExFromCache(regex);
    const auto m=rx->match(text, offset);
    if (caps) *caps=m.capturedTexts();
    return m.hasMatch();
}


qsizetype ls3_rxIndexIn(const QString& text, const QString &regex, size_t offset, QStringList* caps)
{
    const auto rx=ls3_getRegExFromCache(regex);
    QRegularExpressionMatch rmatch;
    const qsizetype idx=text.indexOf(*rx, offset, &rmatch);
    if (caps) *caps=rmatch.capturedTexts();
    return idx;
}


bool ls3_rxPartiallyMatchesAt(const QString& text, const QString &regex, size_t offset, QStringList* caps)
{
    const auto rx=ls3_getRegExFromCache(regex);
    const auto m=rx->match(text, offset, QRegularExpression::NormalMatch, QRegularExpression::AnchorAtOffsetMatchOption);
    if (caps) *caps=m.capturedTexts();
    return m.hasMatch();
}



bool ls3_rxExactlyMatches(const QString& text, const QString &regex, QStringList* caps)
{
    const auto rx=ls3_getRegExFromCache(regex);
    const auto m=rx->match(text);
    if (caps) *caps=m.capturedTexts();
    return m.hasMatch() && m.captured(0).size()==text.size();
}
