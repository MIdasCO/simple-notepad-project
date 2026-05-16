#ifndef SPELL_CHECKER_H
#define SPELL_CHECKER_H

#include <QString>
#include <set>
#include <string>
#include <vector>

class spell_checker {
public:
    bool load_from_file(const QString& path);
    [[nodiscard]] bool is_correct_word(const QString& token) const;
    [[nodiscard]] std::vector<std::string> suggest(const QString& token, int limit = 5) const;

private:
    [[nodiscard]] static std::string normalize(const QString& token);
    [[nodiscard]] static int edit_distance(const std::string& a, const std::string& b);

    std::set<std::string> words;
};

#endif // SPELL_CHECKER_H
