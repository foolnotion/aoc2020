#define ANKERL_NANOBENCH_IMPLEMENT
#include "advent.hpp"
#include "util.hpp"
#include <functional>

#include "nanobench.h"

int day02(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file.\n");
        return 1;
    }

    std::ifstream infile(argv[1]);
    std::string line;

    int n1 = 0;
    int n2 = 0;

    while (std::getline(infile, line)) {
        auto parts = split(line, ':');
        auto rule  = split(parts[0], ' ');
        auto lim   = split(rule[0], '-');
        auto lo    = parse_number<int>(lim[0]).value();
        auto hi    = parse_number<int>(lim[1]).value();
        auto l     = rule[1][0];
        auto pass  = parts[1].substr(1, parts[1].size()-1);

        std::array<int, 26> counts;
        counts.fill(0);
        for (auto c : pass) {
            auto i = c - 'a';
            counts[i]++;
        }
        auto i = l - 'a';
        n1 += (counts[i] >= lo && counts[i] <= hi);
        char a = pass[lo-1];
        char b = pass[hi-1];
        n2 += (a == l) != (b == l);
    }
    fmt::print("valid part 1: {}\n", n1);
    fmt::print("valid part 2: {}\n", n2);

    return 0;
}
