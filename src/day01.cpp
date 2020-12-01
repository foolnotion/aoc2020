#include "advent.hpp"
#include "util.hpp"

int day01(int argc, char** argv) {
    if (argc < 2) {
        fmt::print("Provide an input file\n");
        return 1;
    }

    std::vector<int> values;

    std::ifstream infile(argv[1]);
    std::string line;

    int rows = 0, cols = 0; // width, height
    while (std::getline(infile, line)) {
        if (auto v = parse_number<int>(line); v.has_value()) {
            values.push_back(v.value());
        }
    }

    std::sort(values.begin(), values.end());

    auto s = 2020; // target sum

    for (auto x : values) {
        auto y = s - x;

        if (std::binary_search(values.begin(), values.end(), y)) {
            fmt::print("x = {}, y = {}, x * y = {}\n", x, y, x * y);
        }
    }

    gsl::span<int> ranges[] = { values, values };
    int pair[] = { 0, 0 };

    // of course this could be more simple but this is more "generic"
    std::vector<int> twosums;
    std::vector<int> twoproducts;
    auto next_pair = [&](auto i, auto&& next_pair) {
        if (i == std::size(pair)) {
            if (!std::is_sorted(std::begin(pair), std::end(pair))) {
                return;
            }
            auto sum = std::reduce(std::begin(pair), std::end(pair), 0, std::plus{});
            if (sum < 2020) {
                twosums.push_back(sum);
                twoproducts.push_back(std::reduce(std::begin(pair), std::end(pair), 1, std::multiplies{}));
            }
            return;
        }

        for (auto v : ranges[i]) {
            pair[i] = v;
            next_pair(i + 1, next_pair);
        }
    };
    next_pair(0, next_pair);

    for(size_t i = 0; i < twosums.size(); ++i) {
        auto x = twosums[i];
        auto y = s - x;

        if (std::binary_search(values.begin(), values.end(), y)) {
            fmt::print("triple: x = {}, y = {}, x * y = {}\n", x, y, twoproducts[i] * y);
        }
    }

    return 0;
}

