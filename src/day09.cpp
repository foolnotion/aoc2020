#include <bitset>
#include <functional>
#include <fmt/format.h>
#include "advent.hpp"
#include "util.hpp"
#include "robin_hood.h"

#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"

int day09(int argc, char** argv)
{
    if (argc < 3) {
        fmt::print("Provide an input file and a value for the preamble.\n");
        return 1;
    }

    std::ifstream in(argv[1]);
    std::string line;

    std::vector<uint64_t> vec;

    while(std::getline(in, line)) {
        if (line.empty()) { continue; }
        vec.push_back(parse_number<uint64_t>(line).value());
    }

    int preamble = parse_number<int>(argv[2]).value();

    auto part1 = [&]() -> std::optional<uint64_t> {
        robin_hood::unordered_set<uint64_t> values(vec.begin(), vec.begin() + preamble);
        for (auto it = vec.begin() + preamble; it != vec.end(); ++it) {
            bool has_sum = false;
            for (auto it2 = it - preamble; it2 < it; ++it2) {
                if (values.find(*it - *it2) != values.end()) {
                    has_sum = true;
                    break;
                }
            }
            if (has_sum) {
                values.erase(*(it - preamble));
                values.insert(*it);
                continue;
            }
            return { *it };
        }
        return std::nullopt;
    };

    auto part2 = [&](uint64_t target) -> std::optional<uint64_t> {
        std::vector<uint64_t> sum(vec.size());
        std::inclusive_scan(vec.begin(), vec.end(), sum.begin());

        // we need to find two terms a, b in the cummulative sum such that a < b and b - a = target
        for (size_t i = 0; i < sum.size(); ++i) {
            auto a = sum[i];
            auto b = target + a;

            if (auto it = std::lower_bound(sum.begin(), sum.end(), b); it != sum.end() && *it == b) {
                auto j = std::distance(sum.begin(), it);
                std::vector<uint64_t> range(vec.begin() + i + 1, vec.begin() + j + 1);
                auto [min, max] = std::minmax_element(range.begin(), range.end());
                return { *min + *max };
            }
        }
        return std::nullopt;
    };

    auto p1 = part1();

    auto target = p1.value();
    auto p2 = part2(target);

    fmt::print("part 1: {}\n", target);
    fmt::print("part 2: {}\n", p2.value());

    ankerl::nanobench::Bench b;
    b.performanceCounters(true).minEpochIterations(100);
    b.run("part 1", [&]() { part1(); });
    b.run("part 2", [&]() { part2(target); });

    return 0;
}
