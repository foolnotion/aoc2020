#include "advent.hpp"
#include "util.hpp"
#include <functional>

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
        bool found = false;
        // do a cartesian product
        auto next_pair = [&](int i, auto&& next_pair) {
            if (found) return;

            if (i == pair.size()) {
                auto s = std::reduce(pair.begin(), pair.end(), 0, std::plus {});

                if (s == sum) {
                    auto p = std::reduce(pair.begin(), pair.end(), 1ll, std::multiplies {});
                    found = true;
                    product *= p;
                    return;
                }
            };
            for (auto v : ranges[i]) {
                if (v > sum) continue;
                pair[i] = v;
                next_pair(i + 1, next_pair);
            }
        };
        next_pair(0, next_pair);
        return found ? std::make_optional(product) : std::nullopt;
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

    return 0;
}
