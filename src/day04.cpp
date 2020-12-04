#include <cctype>
#include <fmt/format.h>
#define ANKERL_NANOBENCH_IMPLEMENT
#include "advent.hpp"
#include "util.hpp"
#include <functional>

#include "nanobench.h"

int day04(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file.\n");
        return 1;
    }

    std::string required_fields[] = { "byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid" };

    std::unordered_map<std::string_view, std::function<bool(std::string_view)>> validation_rules {
        { 
            "byr", [](auto const& s) { 
                auto res = parse_number<int>(s); 
                if (!res.has_value() || res.value() < 1920 || res.value() > 2002)
                    return false;
                return true;
            } 
        },
        {
            "iyr", [](auto const& s) {
                auto res = parse_number<int>(s); 
                if (!res.has_value() || res.value() < 2010 || res.value() > 2020)
                    return false;
                return true;
            }
        },
        {
            "eyr", [](auto const& s) {
                auto res = parse_number<int>(s); 
                if (!res.has_value() || res.value() < 2020 || res.value() > 2030)
                    return false;
                return true;
            }
        },
        {
            "hgt", [](auto const& s) {
                std::string_view unit(s.data() + s.size() - 2, 2);
                std::string_view height(s.data(), s.size() - 2);
                auto res = parse_number<int>(height);

                if (!res.has_value()) {
                    return false;
                }

                auto heightval = res.value();

                if (unit == "cm")
                    return heightval >= 150 && heightval <= 193;

                if (unit == "in")
                    return heightval >= 59 && heightval <= 76;

                return false;
            }
        },
        {
            "hcl", [](auto const& s) {
                if (s.size() != 7)
                    return false;

                if (s[0] != '#')
                    return false;

                return std::all_of(s.begin() + 1, s.end(), [](char c) {
                    return std::isdigit(c) || (std::islower(c) && std::isalpha(c));
                        });
            }
        },
        {
            "ecl", [](auto const& s) {
                return 
                    s == "amb" || s == "blu" || s == "brn" || 
                    s == "gry" || s == "grn" || s == "hzl" || s == "oth";
            }
        },
        {
            "pid", [](auto const& s) {
                return s.size() == 9
                    && std::all_of(s.begin(), s.end(), [](char c) { return std::isdigit(c); });
            }
        },
        {
            "cid", [](auto const& s) {
                return true;
            }
        }
    };

    std::ifstream in(argv[1]);
    std::string line;

    fmt::memory_buffer buf;
    bool space = false;
    std::vector<std::string_view> fields;
    int valid_entries = 0;
    while (std::getline(in, line)) {
        if (line.empty()) {
            int pos = 0;
            if (buf.size() > 0) {
                // validate passport entry
                fields.clear();
                std::string_view entry(buf.data(), buf.size()); 
                while (pos < entry.size()) {
                    if (pos = entry.find(':', pos+1); pos != -1) {
                        auto field = std::string_view(entry.data() + pos - 3, 3);

                        if (auto pos1 = entry.find(' ', pos+1)) {
                            if (pos1 == -1) pos1 = entry.size();
                            auto value = std::string_view(entry.data() + pos + 1, pos1 - pos - 1);
                            auto it = validation_rules.find(field);

                            // check if field value is valid
                            if (it != validation_rules.end()) {
                                auto v = it->second(value);
                                if (v)
                                    fields.push_back(field);
                            }
                        }
                    }
                }
                std::sort(begin(fields), end(fields));
                bool valid = std::all_of(begin(required_fields), end(required_fields), [&](auto const& f) { 
                    return std::binary_search(begin(fields), end(fields), f);
                });
                valid_entries += valid;
            }
            // encountered an empty line, reset the buffer
            buf.clear();
        } else if (buf.size() > 0) {
            fmt::format_to(buf, "{}", ' '); 
        }
        for (auto c : line) {
            if (std::isspace(c)) {
                space = true;
                continue;
            }
            if (space) {
                space = false;
                fmt::format_to(buf, "{}", ' '); 
            }
            fmt::format_to(buf, "{}", c); 
        }
    }

    fmt::print("valid: {}\n", valid_entries);

    return 0;
}
