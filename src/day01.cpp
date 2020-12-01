#define ANKERL_NANOBENCH_IMPLEMENT
#include "advent.hpp"
#include "util.hpp"
#include <functional>

#include "nanobench.h"

auto find_terms(std::vector<int> const& values, int n, int64_t sum, int64_t product = 1) -> std::optional<int64_t>
{
    EXPECT(n >= 2);
    EXPECT(sum > 0);

    std::vector<int> pair(n);
    std::vector<gsl::span<const int>> ranges(n);

    for (int i = 0; i < n; ++i) {
        ranges[i] = gsl::span<const int>(values.data() + i, values.size() - n + i);
    }

    auto [min_elem, max_elem] = std::minmax_element(values.begin(), values.end());

    if (n == 2) {
        for (auto x : values) {
            auto y = sum - x;

            if (std::binary_search(values.begin(), values.end(), y)) {
                return std::make_optional(x * y * product);
            }
        }
        return std::nullopt;
    } else if (n > 2) {
        for (auto v : values) {
            if (v > sum - *min_elem) continue;

            if (auto res = find_terms(values, n - 1, sum - v, v * product); res.has_value()) {
                return res;
            }
        }
    }
    return std::nullopt;
}

int day01(int argc, char** argv)
{
    if (argc < 3) {
        fmt::print("Provide an input file, a target sum and a number of terms.\n");
        return 1;
    }

    int s;
    int n;

    if (auto res = parse_number<int>(argv[2]); res.has_value()) {
        s = res.value();
    } else {
        throw std::runtime_error("Unable to parse target sum argument.");
    }

    if (auto res = parse_number<int>(argv[3]); res.has_value()) {
        n = res.value();
    } else {
        throw std::runtime_error("Unable to parse number of terms argument.");
    }

    std::vector<int> values;

    std::ifstream infile(argv[1]);
    std::string line;

    while (std::getline(infile, line)) {
        if (auto v = parse_number<int>(line); v.has_value()) {
            values.push_back(v.value());
        }
    }

    std::sort(values.begin(), values.end());

    auto res = find_terms(values, n, s);

    if (res.has_value()) {
        fmt::print("{}-term product = {}\n", n, res.value());
    } else {
        fmt::print("unable to find {}-term combination summing up to {}\n", n, s);
    }

    fmt::print("performance benchmark:\n");
    ankerl::nanobench::Bench b;
    b.run("day01", [&]() { find_terms(values, n, s); });

    return 0;
}
