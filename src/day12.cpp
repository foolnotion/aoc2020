#include <fmt/format.h>

#include <algorithm>
#include <bitset>
#include <functional>
#include <stack>

#include "advent.hpp"
#include "util.hpp"

int day12(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file and a value for the preamble.\n");
        return 1;
    }

    std::ifstream in(argv[1]);
    std::string line;

    std::vector<std::pair<char, int>> input;

    while (std::getline(in, line)) {
        char dir = line[0];
        int val;
        std::string_view sv(line.data() + 1, line.size() - 1);
        if (auto res = parse_number<int>(sv); res.has_value()) {
            val = res.value();
        } else {
            throw std::runtime_error(fmt::format("cannot parse value from {}\n", sv));
        }
        input.emplace_back(dir, val);
    }

    // part 1
    std::array<int, 4> ship { 0, 0, 0, 0 };
    char labels[] { 'E', 'S', 'W', 'N' };
    uint8_t i = 0; // 0 - east, 1 - south, 2 - west, 3 - north

    for (auto [dir, val] : input) {
        switch (dir) {
        case 'R': {
            i = (i + val / 90) % 4;
            break;
        }
        case 'L': {
            i = (i + (360 - val) / 90) % 4;
            break;
        }
        case 'F': {
            ship[i] += val;
            break;
        }
        case 'N': {
            ship[3] += val;
            break;
        }
        case 'S': {
            ship[1] += val;
            break;
        }
        case 'E': {
            ship[0] += val;
            break;
        }
        case 'W': {
            ship[2] += val;
            break;
        }
        default: {
            break;
        }
        }
    }

    std::cout << "ship: " << as_map(ship).transpose() << "\n";
    fmt::print("part 1: {}\n", std::abs(ship[1] - ship[3]) + std::abs(ship[0] - ship[2]));

    // part 2
    std::array<int, 4> waypoint { 10, 0, 0, 1 };
    ship = { 0, 0, 0, 0 }; // reset ship position
    i = 0; // reset orientation to facing east
    
    for (auto [dir, val] : input) {
        switch (dir) {
        case 'R': {
            // rotate the waypoint around the ship clockwise
            std::rotate(waypoint.rbegin(), waypoint.rbegin() + val / 90, waypoint.rend());
            break;
        }
        case 'L': {
            // rotate the waypoint around the ship counter-clockwise
            std::rotate(waypoint.begin(), waypoint.begin() + val / 90, waypoint.end());
            break;
        }
        case 'F': {
            for (int j = 0; j < 4; ++j)
                ship[j] += val * waypoint[j];
            break;
        }
        case 'N': {
            waypoint[3] += val;
            break;
        }
        case 'S': {
            waypoint[1] += val;
            break;
        }
        case 'E': {
            waypoint[0] += val;
            break;
        }
        case 'W': {
            waypoint[2] += val;
            break;
        }
        default: {
            break;
        }
        }
    }

    std::cout << "ship: " << as_map(ship).transpose() << "\n";
    fmt::print("part 2: {}\n", std::abs(ship[1] - ship[3]) + std::abs(ship[0] - ship[2]));

    return 0;
}
