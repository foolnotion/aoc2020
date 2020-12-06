#include <fmt/format.h>
#include "advent.hpp"
#include "util.hpp"
#include <functional>
#include <bitset>

int day06(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file.\n");
        return 1;
    }

    std::ifstream in(argv[1]);
    std::string line;

    std::bitset<26> bits; bits.reset();
    std::bitset<26> all; all.set();

    int count1 = 0, count2 = 0;

    while(std::getline(in, line)) {
        if (line.empty()) {
            count1 += bits.count();
            bits.reset();

            count2 += all.count();
            all.set();

            continue;
        }
        std::bitset<26> tmp;
        for (char c : line) {
            bits.set(c - 'a', true);
            tmp.set(c - 'a', true);
        }
        all &= tmp;
    }
    fmt::print("part 1: {}\n", count1);
    fmt::print("part 2: {}\n", count2);

    return 0;
}

