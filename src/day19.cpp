#include <fmt/format.h>

#include <algorithm>
#include <bitset>
#include <functional>
#include <robin_hood.h>
#include <stack>

#include "advent.hpp"
#include "util.hpp"

using rule = std::vector<size_t>;

struct node {
    std::vector<rule> rules;
    std::optional<char> value;

    node()
        : rules({})
        , value(std::nullopt)
    {
    }

    bool has_val() const { return value.has_value(); }
    char val() const { return value.value(); }

    rule& operator[](size_t i) { return rules[i]; }
    rule const& operator[](size_t i) const { return rules[i]; }

    friend bool operator==(node const& n, char c) { return n.has_val() && n.val() == c; }
    friend bool operator!=(node const& n, char c) { return !(n == c); }

    friend std::ostream& operator<<(std::ostream& os, node const& n)
    {
        if (n.has_val()) {
            os << n.val();
        } else {
            for (size_t i = 0; i < n.rules.size(); ++i) {
                os << (i == 0 ? "" : " | ");
                auto& r = n.rules[i];
                for (size_t j = 0; j < r.size(); ++j) {
                    os << r[j] << (j == r.size() - 1 ? "" : " ");
                }
            }
        }
        return os;
    }
};

struct validator {
    std::vector<node> nodes_;
    fmt::memory_buffer buffer_;

    validator(std::vector<node> const& nodes)
        : nodes_(nodes)
    {
    }

    std::vector<std::string> enumerate(size_t i /* rule index */)
    {
        auto& n = nodes_[i];

        std::vector<std::string> result;
        if (n.has_val()) {
            result.emplace_back(std::string { n.val() });
            return result;
        }

        for (auto& rule : n.rules) {
            std::vector<std::vector<std::string>> ranges;
            std::transform(rule.begin(), rule.end(), std::back_inserter(ranges),
                [&](auto v) { return enumerate(v); });

            std::vector<std::string> tmp(ranges.size());
            auto product = [&](size_t i, auto&& rec) {
                if (i == ranges.size()) {
                    auto sum = std::reduce(tmp.begin(), tmp.end());
                    result.push_back(sum);
                    return;
                }
                for (auto& str : ranges[i]) {
                    tmp[i] = str;
                    rec(i + 1, rec);
                }
            };
            product(0, product);
        }
        return result;
    }

    // TODO: this method should be adjusted for part two, where
    // different subrules might match a different amount of letters
    // therefore we should not return true when the first subrule is matched,
    // but keep going and return a list of strings (or something)
    auto check(size_t i, std::string_view s) -> std::pair<size_t, bool>
    {
        char c = s.front();

        auto& n = nodes_[i];
        if (n == c)
            return { 1ul, true };

        for (auto const& rule : n.rules) {
            auto matched = 0ul;
            auto q = s;
            bool f = true;
            for (auto j : rule) {
                auto [m, ok] = check(j, q);
                if (f = ok; !ok)
                    break;
                matched += m;
                q.remove_prefix(m);
            }
            if (f) {
                return { matched, i == 0 ? q.empty() : true };
            }
        }
        return { 0, false };
    }
};

int day19(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file.\n");
        return 1;
    }

    std::string line;
    std::ifstream in(argv[1]);

    std::vector<node> nodes(200);
    size_t nodes_count = 0;

    while (std::getline(in, line)) {
        if (line.empty())
            break;
        auto tokens = split(line, ' ');

        // parse this rule's number
        auto& rule_num = tokens.front();
        auto pos = rule_num.find(':');
        std::string_view sv(rule_num.data(), pos);
        auto n = parse_number<size_t>(sv).value();
        nodes_count = std::max(nodes_count, n + 1);

        auto test = parse_number<size_t>(tokens[1]);
        if (test.has_value()) {
            rule r;
            for (size_t i = 1; i < tokens.size(); ++i) {
                if (tokens[i] == "|") {
                    nodes[n].rules.push_back(r);
                    r = rule {};
                    continue;
                }
                auto a = parse_number<size_t>(tokens[i]).value();
                r.push_back(a);
            }
            nodes[n].rules.push_back(r);
        } else {
            // terminal node
            nodes[n].value = { tokens[1][1] };
        }
    }

    std::vector<std::string> input;
    while (std::getline(in, line))
        input.push_back(line);

    nodes.resize(nodes_count);

    validator v { nodes };
    size_t i = 0;
    for (auto& n : v.nodes_) {
        std::cout << i++ << ": " << n << "\n";
    }

    if (input.empty()) {
        return 0;
    }

    auto longest = std::max_element(input.begin(), input.end(), [](auto const& a, auto const& b) { return a.size() < b.size(); });
    fmt::print("longest string: {}\n", longest->size());

    auto count = std::count_if(input.begin(), input.end(), [&](auto const& s) { auto [_, ok] = v.check(0, s); return ok; });
    fmt::print("part 1: {}\n", count);

    // part 2
    // 8: 42 | 42 8
    // 11: 42 31 | 42 11 31
    v.nodes_[8].rules = { rule { 42 }, rule { 42, 8 } };
    v.nodes_[11].rules = { rule { 42, 31 }, rule { 42, 11, 31 } };

    i = 0;
    for (auto& n : v.nodes_) {
        std::cout << i++ << ": " << n << "\n";
    }
    fmt::print("\n");

    count = std::count_if(input.begin(), input.end(), [&](auto const& s) { auto [_, ok] = v.check(0, s); return ok; });
    fmt::print("part 2: {}\n", count);

    return 0;
}
