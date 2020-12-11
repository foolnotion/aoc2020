#include <algorithm>
#include <bitset>
#include <functional>
#include <stack>
#include <fmt/format.h>
#include "advent.hpp"
#include "util.hpp"
#include "robin_hood.h"

#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"

int day10(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file and a value for the preamble.\n");
        return 1;
    }

    std::ifstream in(argv[1]);
    std::string line;

    std::vector<uint64_t> v;


    while(std::getline(in, line)) {
        if (line.empty()) { continue; }
        auto voltage = parse_number<uint64_t>(line).value();
        v.push_back(voltage);
    }

    std::sort(v.begin(), v.end());
    int a = 0, b = 1;
    for (size_t i = 0; i < v.size() - 1; ++i) {
        auto diff = v[i+1] - v[i];
        a += diff == 1;
        b += diff == 3;
    }
    a += v.front() == 1;
    fmt::print("part 1: {}\n", a * b);

    std::vector<int> counts;

    std::vector<int> u {0};
    std::copy(v.begin(), v.end(), std::back_inserter(u));
    u.push_back(u.back()+3);

    auto get_intervals = [](gsl::span<int> u) {
        std::vector<gsl::span<int>> intervals;
        for(size_t i = 0; i < u.size(); ++i) {
            size_t j = i+1;
            while(j < u.size() && u[j] - u[i] <= 3) {
                ++j;
            }

            if (intervals.empty()) {
                intervals.push_back({ u.data() + i, j - i });
                continue;
            }
            auto iv = intervals.back();
            auto a = iv.data() - u.data();
            auto b = a + iv.size();

            if (b > i) {
                iv = gsl::span(iv.data(), j);
                intervals.back() = iv;
            } else {
                intervals.push_back({ u.data() + i, j - i });
            }
        }
        return intervals;
    };

    auto part2 = [](gsl::span<int> u) -> uint64_t {
        std::vector<uint64_t> counts(u.size(), 0);
        counts.back() = 1;

        for(int i = u.size() - 2; i >= 0; --i) {
            for (size_t j = i + 1; j < u.size(); ++j) {
                if (u[j] - u[i] > 3)
                    break;
                counts[i] += counts[j];
            }
        }
        return counts.front(); 
    };

    auto part2_hyb = [&](gsl::span<int> u) -> uint64_t {
        auto intervals = get_intervals(u);
        auto p = std::transform_reduce(intervals.begin(), intervals.end(), uint64_t{1}, std::multiplies{}, [&](auto iv) { return part2(iv); });
        return p;
    };
    auto p2_dyn = part2(u);
    auto p2_hyb = part2_hyb(u);

    fmt::print("part 2 (dyn): {}\n", p2_dyn);
    fmt::print("part 2 (hyb): {}\n", p2_hyb);

    ankerl::nanobench::Bench bench;
    bench.performanceCounters(true).minEpochIterations(10000);
    bench.run("part 2 dyn", [&]() { part2(u); });
    bench.run("part 2 dyn+hyb", [&]() { part2_hyb(u); });

    return 0;
}


