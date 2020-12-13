#include <fmt/format.h>

#include <algorithm>
#include <bitset>
#include <functional>
#include <stack>

#include "advent.hpp"
#include "util.hpp"

int day13(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file.\n");
        return 1;
    }

    std::ifstream in(argv[1]);
    std::string line;

    std::getline(in, line);
    auto s = parse_number<uint64_t>(line).value();

    std::getline(in, line);
    auto tokens = split(line, ',');
    std::vector<std::pair<uint64_t, uint64_t>> buses;

    size_t i = 0;
    for (auto& tok : tokens) {
        if (auto res = parse_number<uint64_t>(tok); res.has_value()) {
            buses.push_back({ res.value(), i });
        }
        ++i;
    }

    uint64_t ans = std::numeric_limits<uint64_t>::max();
    uint64_t id;
    for (auto [time, period] : buses) {
        auto wait = time - s % time;
        if (ans > wait) {
            ans = wait;
            id = time;
        }
    }
    fmt::print("part 1: {}\n", ans * id);

    // part 2
    uint64_t a{0}, b{1};
    for (auto [interval, offset] : buses) {
        for (size_t k = 0; k < interval; ++k) {
            auto c = a + k * b;
            if ((c + offset) % interval == 0) {
                a = c;
                b = std::lcm(b, interval);
                break;
            }
        }
    }
    fmt::print("part 2: {}\n", a);
    return 0;
}
