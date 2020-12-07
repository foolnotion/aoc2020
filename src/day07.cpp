#include <fmt/format.h>
#include "advent.hpp"
#include "util.hpp"
#include <functional>
#include <bitset>

int day07(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file.\n");
        return 1;
    }

    std::ifstream in(argv[1]);
    std::string line;

    using item = std::tuple<int, std::string>;
    std::unordered_map<std::string, std::vector<item>> contains;
    std::unordered_map<std::string, std::vector<std::string>> contained;

    while(std::getline(in, line)) {
        if (line.empty()) continue;
        auto tokens = split(line, ' ');

        auto key = tokens[0] + tokens[1];

        if (tokens[4] == "no") {
            continue;
        }

        for (size_t i = 4; i < tokens.size(); i += 4) {
            auto qty = parse_number<int>(tokens[i]);
            if (!qty.has_value()) {
                fmt::print("unable to parse qty from string {}\n", tokens[i]);
                std::abort();
            }

            auto s = tokens[i+1] + tokens[i+2]; 
            contains[key].emplace_back(qty.value(), s);
            contained[s].push_back(key);
        }
    }

    std::unordered_set<std::string> counted;
    auto get_count = [&](std::string const& color, auto&& rec) {
        if (auto [it, ok] = counted.insert(color); ! ok) {
            return;
        }
        if (auto it = contained.find(color); it != contained.end()) {
            for (auto& s : it->second) {
                rec(s, rec);
            }
        }
    };
    get_count("shinygold", get_count);
    fmt::print("part 1: {}\n", counted.size()-1);

    auto get_content_count = [&](std::string const& color, auto&& rec) {
        int c = 1;
        if (auto it = contains.find(color); it != contains.end()) {
            c += std::transform_reduce(begin(it->second), end(it->second), 0, std::plus{}, [&](auto const& a) { 
                auto [q1, s1] = a;
                return q1 * rec(s1, rec);
            });
        } 
        return c;
    };
    fmt::print("part 2: {}\n", get_content_count("shinygold", get_content_count) - 1);

    return 0;
}
