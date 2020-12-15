#include <fmt/format.h>

#include <algorithm>
#include <bitset>
#include <functional>
#include <robin_hood.h>
#include <stack>

#include "advent.hpp"
#include "util.hpp"

int day15(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file.\n");
        return 1;
    }

    std::ifstream in(argv[1]);
    std::string line;
    std::getline(in, line);
    auto tokens = split(line, ',');

    std::vector<uint64_t> nums;
    std::transform(tokens.begin(), tokens.end(), std::back_inserter(nums), [](auto const& v) { return parse_number<uint64_t>(v).value(); });

    robin_hood::unordered_map<uint64_t, uint64_t> mem;
    for (size_t i = 0; i < nums.size(); ++i) {
        mem.insert({ nums[i], i+1 });
    }

    uint64_t n = 0;
    uint64_t limit = 30'000'000;
    for (size_t turn = nums.size()+1; turn < limit; ++turn) {
        if (auto [it, ok] = mem.insert({ n, turn }); !ok) {
            n = turn - it->second;
            it->second = turn;
        } else {
            n = 0;
        }
    }
    fmt::print("n = {}\n", n);

    return 0;
}

