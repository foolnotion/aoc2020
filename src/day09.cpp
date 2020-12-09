#include <fmt/format.h>
#include "advent.hpp"
#include "util.hpp"
#include <functional>
#include <bitset>

#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"

// taken from day 1
auto find_terms(gsl::span<uint64_t> values, int n, uint64_t sum, uint64_t product = 1) -> std::optional<uint64_t>
{
    if (sum <= 0) {
        return std::nullopt;
    }

    if (n == 2) {
        for (auto x : values) {
            auto y = sum - x;

            if (std::binary_search(values.begin(), values.end(), y)) {
                return std::make_optional(x * y * product);
            }
        }
    } else if (n > 2) {
        for (auto x : values) {
            if (x > sum) continue;

            if (auto res = find_terms(values, n - 1, sum - x, x * product); res.has_value()) {
                return res;
            }
        }
    }
    return std::nullopt;
}

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

    std::optional<uint64_t> res;
    uint64_t target;
    for (auto it = vec.begin() + preamble; it != vec.end(); ++it) {
        std::vector<uint64_t> range(it - preamble, it);
        std::sort(range.begin(), range.end());
        res = find_terms(range, 2, *it, 1);
        if (!res.has_value()) {
            target = *it;
            fmt::print("part 1: {}\n", *it);
            break;
        }
    }

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
            fmt::print("part 2: {}\n", *min + *max);
            break;
        }
    }


    return 0;
}
