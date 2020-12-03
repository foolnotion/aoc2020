#define ANKERL_NANOBENCH_IMPLEMENT
#include "advent.hpp"
#include "util.hpp"
#include <functional>

#include "nanobench.h"

int day03(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file.\n");
        return 1;
    }

    std::ifstream in(argv[1]);
    std::string line;

    int nrow = 0;
    int ncol = 0;

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
        Eigen::Map<Eigen::Array<char, 1, -1>> r(line.data(), line.size());
        a.row(row++) = r;
    }

    auto encountered_trees = [&](int right, int down) -> long {
        int x = 0, y = 0, n = 0;
        while (x < a.rows()) {
            n += a(x, y) == '#';
            x += down;
            y = (y + right) % a.cols();
        }
        return n;
    };

    std::cout << a << "\n";

    long p = encountered_trees(1, 1) * encountered_trees(3, 1) * encountered_trees(5, 1) * encountered_trees(7, 1) * encountered_trees(1, 2);

    fmt::print("part 1: {}\n", encountered_trees(3, 1));
    fmt::print("part 2: {}\n", p);
    return 0;
}
