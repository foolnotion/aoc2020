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

int day11(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file and a value for the preamble.\n");
        return 1;
    }

    std::ifstream in(argv[1]);
    std::string line;

    int nrow = 0, ncol = 0;

    while (std::getline(in, line)) {
        ++nrow;
        ncol = line.size();
    }

    using A = Eigen::Array<char, -1, -1>;

    A a(nrow, ncol);

    in.clear();
    in.seekg(0);

    int row = 0;
    while (std::getline(in, line)) {
        a.row(row++) = as_map(line);
    }

    //std::cout << a << "\n";

    auto count_occupied_p1 = [](Eigen::Ref<A> a, int row, int col) {
        int occupied = 0;
        for (int i = row-1; i <= row+1; ++i) {
            for (int j = col-1; j <= col+1; ++j) {
                if (i == row && j == col) continue;
                occupied += (i >= 0 && j >= 0 && i < a.rows() && j < a.cols()) && a(i, j) == '#';
            }
        }
        return occupied;
    };

    auto count_occupied_p2 = [](Eigen::Ref<A> a, int row, int col) {
        int occupied = 0;
        // up-left
        int i = row, j = col;
        while(--i >= 0 && --j >= 0) {
            if(a(i, j) == '.') {
                continue;
            }
            occupied += a(i, j) == '#';
            break;
        }
        // left
        i = row; j = col;
        while(--j >= 0) {
            if(a(i, j) == '.') {
                continue;
            }
            occupied += a(i, j) == '#';
            break;
        }
        // down-left
        i = row; j = col;
        while(++i < a.rows() && --j >= 0) {
            if(a(i, j) == '.') {
                continue;
            }
            occupied += a(i, j) == '#';
            break;
        }
        // down
        i = row; j = col;
        while(++i < a.rows()) {
            if(a(i, j) == '.') {
                continue;
            }
            occupied += a(i, j) == '#';
            break;
        }
        // down-right
        i = row; j = col;
        while(++i < a.rows()  && ++j < a.cols()) {
            if(a(i, j) == '.') {
                continue;
            }
            occupied += a(i, j) == '#';
            break;
        }
        // right
        i = row; j = col;
        while(++j < a.cols()) {
            if(a(i, j) == '.') {
                continue;
            }
            occupied += a(i, j) == '#';
            break;
        }
        // up-right 
        i = row; j = col;
        while(--i >= 0 && ++j < a.cols()) {
            if(a(i, j) == '.') {
                continue;
            }
            occupied += a(i, j) == '#';
            break;
        }
        // up 
        i = row; j = col;
        while(--i >= 0) {
            if(a(i, j) == '.') {
                continue;
            }
            occupied += a(i, j) == '#';
            break;
        }
        return occupied;
    };

    int count = -1;
    int iter = 0;

    A b;
    while (count != 0) {
        count = 0;
        b = a;
        for(int i = 0; i < a.rows(); ++i) {
            for (int j = 0; j < a.cols(); ++j) {
                if (a(i, j) == '.') continue;
                auto oc = count_occupied_p2(a, i, j);
                if (a(i, j) == 'L' && oc == 0) {
                    b(i, j) = '#';
                    ++count;
                } 
                if (a(i, j) == '#' && oc >= 5) {
                    b(i, j) = 'L';
                    ++count;
                }
            }
        }
        a = b;
    }

    fmt::print("part 1: {}\n", (a == '#').count());

    return 0;
}
