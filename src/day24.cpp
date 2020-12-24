#include <algorithm>
#include <bitset>
#include <execution>
#include <fmt/format.h>
#include <functional>
#include <initializer_list>
#include <list>
#include <robin_hood.h>
#include <stack>
#include <map>

#include "advent.hpp"
#include "util.hpp"

enum DIR { E, SE, SW, W, NW, NE };

using point_t = point<int64_t>;

std::array<point_t, 6> moves {
    point_t{ +1,  0, -1 }, // E
    point_t{  0, +1, -1 }, // SE
    point_t{ -1, +1,  0 }, // SW
    point_t{ -1,  0, +1 }, // W
    point_t{  0, -1, +1 }, // NW
    point_t{ +1, -1,  0 }  // NE
};

std::array<std::string, 6> names { "E", "SE", "SW", "W", "NW", "NE" };

int day24(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("provide an input file.\n");
        exit(1);
    }

    std::ifstream in(argv[1]);
    std::string line;

    robin_hood::unordered_set<std::string_view> valid_directions {
        "e", "se", "sw", "w", "nw", "ne"
    };

    auto str2dir = [](std::string_view sv) {
        if (sv == "e")  return DIR::E;
        if (sv == "se") return DIR::SE;
        if (sv == "sw") return DIR::SW;
        if (sv == "w")  return DIR::W;
        if (sv == "nw") return DIR::NW;
        if (sv == "ne") return DIR::NE;
        throw std::runtime_error(fmt::format("unknown direction: {}\n", sv));
    };

    robin_hood::unordered_flat_map<point_t, int64_t> flipped;

    while (std::getline(in, line)) {
        size_t i = 0, j = 0;

        std::vector<DIR> directions;

        while (j < line.size()) {
            if (j < line.size() - 1)
                ++j;
            std::string_view sv(line.data() + i, j - i + 1);

            if (!valid_directions.contains(sv)) {
                --j;
                sv.remove_suffix(1);
            }
            i = j + 1;
            j = i;

            directions.push_back(str2dir(sv));
        }

        point_t p{0,0,0};
        for (auto dir : directions) {
            auto [x, y, z] = p;
            auto [dx, dy, dz] = moves[dir];
            p = point_t{ x+dx, y+dy, z+dz };
        }
        ++flipped[p];
    }

    int black = std::count_if(flipped.begin(), flipped.end(), [](auto const& it) { return it.second % 2; });
    fmt::print("part 1: {}\n", black);

    std::vector<point_t> tiles;
    for (int i = 0; i < 100; ++i) {
        tiles.clear();

        // gather all neighbours
        for (auto const& [p, f] : flipped) {
            for (auto const& [dx, dy, dz] : moves) {
                point_t q{ p.x+dx, p.y+dy, p.z+dz };
                tiles.push_back(q);
            }
        }

        for (auto const& n : tiles) {
            flipped.try_emplace(n, 0);
        }

        tiles.clear();
        for (auto const& [p, f] : flipped) {
            bool b = f % 2; // true-black, false-white
            auto [x, y, z] = p;

            int count = 0;

            for (auto const& [dx, dy, dz] : moves) {
                point_t q{ x+dx, y+dy, z+dz };
                if (auto it = flipped.find(q); it != flipped.end()) {
                    count += it->second % 2;
                }
            }

            if ((b && (count == 0 || count > 2)) || (!b && count == 2) ) {
                tiles.push_back(p);
            }
        }

        for (auto const& t : tiles) {
            auto it = flipped.find(t);
            ++it->second;
        }

        black = std::count_if(flipped.begin(), flipped.end(), [](auto const& it) { return it.second % 2; });
        fmt::print("day {}: {}\n", i+1, black);
    }

    return 0;
}
