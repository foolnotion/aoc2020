#include <cctype>
#include <fmt/format.h>
#define ANKERL_NANOBENCH_IMPLEMENT
#include "advent.hpp"
#include "util.hpp"
#include <functional>

#include "nanobench.h"

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
        int row[2] = { 0, 127 };
        int col[2] = { 0, 7 };

        for (auto ch : line) {
            switch(ch) {
                case 'F':
                    row[1] = row[0] + (row[1] - row[0]) / 2;
                    break;

                case 'B':
                    row[0] = row[0] + (row[1] - row[0]) / 2 + 1;
                    break;

                case 'L':
                    col[1] = col[0] + (col[1] - col[0]) / 2;
                    break;

                case 'R':
                    col[0] = col[0] + (col[1] - col[0]) / 2 + 1;
                    break;
            }
        }
        int r = std::min(row[0], row[1]);
        int c = std::min(col[0], col[1]);
        int id = r * sizeof(col) + c;

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
