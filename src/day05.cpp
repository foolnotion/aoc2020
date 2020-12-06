#include <fmt/format.h>
#include "advent.hpp"
#include "util.hpp"
#include <functional>
#include <bitset>

int day05(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file.\n");
        return 1;
    }

    std::ifstream in(argv[1]);
    std::string line;

    int maxID = 0;

    std::vector<int> ids;

    while(std::getline(in, line)) {
        std::bitset<10> bits;
        size_t i = 10;
        for (auto c : line) {
            bits[--i] = (c == 'R' || c == 'B');
        }
        int id = bits.to_ulong();
        maxID = std::max(id, maxID);
        ids.push_back(id);
    }

    fmt::print("part 1: {}\n", maxID);

    std::sort(ids.begin(), ids.end());

    int seatID;
    for (size_t i = 1; i < ids.size(); ++i) {
        if (ids[i] - ids[i-1] == 2) {
            seatID = ids[i] - 1;
            break;
        }
    }

    fmt::print("part 2: {}\n", seatID);

    return 0;
}
