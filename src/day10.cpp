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

    size_t i = 0, j = 0;
    size_t i_prev = u.size(), j_prev = u.size();
    std::stack<std::tuple<size_t, size_t, size_t>> intervals;
    while(i < u.size() && j <= u.size()) {
        j = i+1;
        while(j < u.size() && u[j] - u[i] <= 3) {
            ++j;
        }
        if (j - i > 2) {
            auto w = j - i - 2; // interval width
            if (intervals.empty()) {
                intervals.push({ u[i], u[j-1], w });
            } else {
                auto [a, b, c] = intervals.top();
                if (u[i] < b) {
                    // we have an intersection
                    intervals.pop();
                    intervals.push({ a, u[j-1], c + w - 1 }); // -1 because we have an intersection
                } else {
                    intervals.push({ u[i], u[j-1], w });
                }
            }
        }
        ++i;
    }
    size_t p = 1;
    while(intervals.size() > 0) {
        auto [a, b, c] = intervals.top();
        intervals.pop();
        auto x = std::pow(2, c) - (b-a > 3);
        p *= x;
    }
    fmt::print("part 2: {}\n", p);

    return 0;
}


