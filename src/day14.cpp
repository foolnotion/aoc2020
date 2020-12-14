#include <fmt/format.h>

#include <algorithm>
#include <bitset>
#include <functional>
#include <robin_hood.h>
#include <stack>

#include "advent.hpp"
#include "util.hpp"

int day14(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file.\n");
        return 1;
    }

    std::ifstream in(argv[1]);
    std::string line;

    robin_hood::unordered_map<uint64_t, uint64_t> mem;
    std::string mask;

    std::vector<std::string> input;

    while(std::getline(in, line)) {
        if(line.empty()) continue;
        input.push_back(line);
    }

    // part 1
    for (auto const& line : input) {
        auto tokens = split(line, ' ');

        if (tokens[0] == "mask") {
            mask = tokens[2];
        } else {
            // parse memory address
            auto addr = parse_number<uint64_t>(std::string_view(tokens[0].data() + 4, tokens[0].size() - 5)).value();

            // parse value and apply mask
            auto val = parse_number<uint64_t>(tokens[2]).value();
            auto bits = std::bitset<36>(val);
            for(size_t i = 0, j = 35; i < bits.size(); ++i, --j) {
                if (mask[j] == '0') bits[i] = false;
                else if (mask[j] == '1') bits[i] = true;
            }
            mem[addr] = bits.to_ulong();
        }
    }

    auto sum = std::transform_reduce(mem.begin(), mem.end(), 0ul, std::plus{}, [](auto p) { return p.second; }); 
    fmt::print("part 1: {}\n", sum);


    // part 2
    std::bitset<36> bits;
    std::vector<int> floating;
    mem.clear();

    // helper function to handle the floating bits
    auto helper = [&](size_t v, size_t i, auto &&rec) {
        if (i == floating.size()) {
            mem[bits.to_ulong()] = v;
            return;
        }
        auto j = floating[i];
        for (auto b : { true, false }) {
            auto x = bits[j];
            bits[j] = b;
            rec(v, i+1, rec);
            bits[j] = x;
        }
    };

    for (auto const& line : input) {
        auto tokens = split(line, ' ');

        if (tokens[0] == "mask") {
            mask = tokens[2];
        } else {
            // parse memory address
            auto addr = parse_number<uint64_t>(std::string_view(tokens[0].data() + 4, tokens[0].size() - 5)).value();

            // parse value and apply mask
            auto val = parse_number<uint64_t>(tokens[2]).value();
            bits = std::bitset<36>(addr);
            floating.clear();
            for(size_t i = 0, j = 35; i < bits.size(); ++i, --j) {
                if (mask[j] == '0') { 
                    // unchanged
                } else if (mask[j] == '1') {
                    bits[i] = true;
                } else if (mask[j] == 'X') {
                    floating.push_back(i);
                }
            }
            helper(val, 0, helper);
        }
    }

    sum = std::transform_reduce(mem.begin(), mem.end(), 0ul, std::plus{}, [](auto p) { return p.second; }); 
    fmt::print("part 2: {}\n", sum);

    return 0;
}
