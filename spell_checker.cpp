#include "spell_checker.h"

#include <QFile>
#include <QTextStream>
#include <algorithm>
#include <cctype>
#include <limits>
#include <tuple>

bool spell_checker::load_from_file(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    words.clear();
    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            words.insert(line.toStdString());
        }
    }

    return !words.empty() && in.status() == QTextStream::Ok;
}

bool spell_checker::is_correct_word(const QString& token) const
{
    if (words.empty()) {
        return true;
    }

    const std::string normalized = normalize(token);
    if (normalized.empty()) {
        return true;
    }

    return words.contains(normalized);
}

std::vector<std::string> spell_checker::suggest(const QString& token, const int limit) const
{
    std::vector<std::string> result;
    if (limit <= 0 || words.empty()) {
        return result;
    }

    const std::string normalized = normalize(token);
    if (normalized.empty()) {
        return result;
    }

    std::vector<std::tuple<int, int, std::string>> ranked;
    ranked.reserve(2048);

    for (const auto& word : words) {
        const int len_diff = std::abs(static_cast<int>(word.size()) - static_cast<int>(normalized.size()));
        if (len_diff > 2) {
            continue;
        }

        if (!word.empty() && !normalized.empty() && word.front() != normalized.front()) {
            continue;
        }

        const int distance = edit_distance(normalized, word);
        ranked.emplace_back(distance, len_diff, word);
    }

    std::sort(ranked.begin(), ranked.end(), [](const auto& lhs, const auto& rhs) {
        if (std::get<0>(lhs) != std::get<0>(rhs)) {
            return std::get<0>(lhs) < std::get<0>(rhs);
        }
        if (std::get<1>(lhs) != std::get<1>(rhs)) {
            return std::get<1>(lhs) < std::get<1>(rhs);
        }
        return std::get<2>(lhs) < std::get<2>(rhs);
    });

    for (const auto& [distance, _, word] : ranked) {
        if (distance > 3) {
            break;
        }
        result.push_back(word);
        if (static_cast<int>(result.size()) >= limit) {
            break;
        }
    }

    return result;
}

std::string spell_checker::normalize(const QString& token)
{
    std::string normalized;
    normalized.reserve(static_cast<std::size_t>(token.size()));

    for (const QChar ch : token) {
        if (ch.isLetter()) {
            const auto lower = ch.toLower().toLatin1();
            if (lower >= 'a' && lower <= 'z') {
                normalized.push_back(lower);
            }
        }
    }

    return normalized;
}

int spell_checker::edit_distance(const std::string& a, const std::string& b)
{
    const std::size_t m = a.size();
    const std::size_t n = b.size();

    std::vector<int> prev(n + 1);
    std::vector<int> curr(n + 1);

    for (std::size_t j = 0; j <= n; ++j) {
        prev[j] = static_cast<int>(j);
    }

    for (std::size_t i = 1; i <= m; ++i) {
        curr[0] = static_cast<int>(i);
        for (std::size_t j = 1; j <= n; ++j) {
            const int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            curr[j] = std::min({
                prev[j] + 1,
                curr[j - 1] + 1,
                prev[j - 1] + cost
            });
        }
        std::swap(prev, curr);
    }

    return prev[n];
}
