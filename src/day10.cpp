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

    std::cout << as_map(u).transpose() << "\n";

    auto get_intervals = [](gsl::span<int> u) {
        size_t i = 0, j = 0;
        std::stack<gsl::span<int>> intervals;
        while(i < u.size() && j <= u.size()) {
            j = i+1;
            while(j < u.size() && u[j] - u[i] <= 3) {
                ++j;
            }

            if (intervals.empty()) {
                intervals.push({ u.data() + i, j - i });
            } else {
                auto iv = intervals.top();
                intervals.pop();

                auto a = iv.data() - u.data();
                auto b = a + iv.size();


                if (b > i) {
                    iv = gsl::span(iv.data(), iv.data() + j);
                }

                intervals.push(iv);
            }


            ++i;
        }
        return intervals;
    };

    auto part2 = [](gsl::span<int> u) -> uint64_t {
        std::vector<uint64_t> counts(u.size(), 0);
        counts.back() = 1;

        for(int i = u.size() - 2; i >= 0; --i) {
            for (size_t j = i + 1; j < u.size(); ++j) {
                if (u[j] - u[i] <= 3) { counts[i] += counts[j]; }
            }
        }
        return counts.front(); 
    };

    auto part2_hyb = [&](gsl::span<int> u) -> uint64_t {
        auto intervals = get_intervals(u);
        uint64_t p = 1;
        while (!intervals.empty()) {
            auto iv = intervals.top();
            intervals.pop();
            p *= part2(iv);
        }
        return p;
    };
    auto p2_dyn = part2(u);
    auto p2_hyb = part2_hyb(u);

    fmt::print("part 2 (dyn): {}\n", p2_dyn);
    fmt::print("part 2 (hyb): {}\n", p2_hyb);

    ankerl::nanobench::Bench bench;
    bench.performanceCounters(true).run("part 2 dyn", [&]() { part2(u); });
    bench.performanceCounters(true).run("part 2 dyn+hyb", [&]() { part2_hyb(u); });

    return 0;
}


