#include <fmt/format.h>

#include <algorithm>
#include <bitset>
#include <functional>
#include <robin_hood.h>
#include <stack>

#include "advent.hpp"
#include "util.hpp"

#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"

// use Eigen Tensors
#include <unsupported/Eigen/CXX11/Tensor>

template<typename T>
long count_active(T const& m) {
    Eigen::Map<const Eigen::Array<char, -1, 1>> map(m.data(), m.size());
    return (map == '#').count();
}

template<typename T>
T get_cube(T const& m, std::array<int, T::NumDimensions> coord) {
    constexpr int N = T::NumDimensions;
    for (int i = 0; i < N; ++i) coord[i]--;
    std::array<int, N> offsets = coord;
    std::array<int, N> extents;
    std::fill(extents.begin(), extents.end(), 3);
    T s = m.slice(offsets, extents);
    return s;
}

template<typename T, typename... Coords>
char read_value(T const& m, Coords... coords) {
    return m(coords...);
}

template<typename T, typename... Coords>
void write_value(T& m, char v, Coords... coords) {
    m(coords...) = v;
}

int day17(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file.\n");
        return 1;
    }

    constexpr int N = 4;
    using Tensor = Eigen::Tensor<char, N>;

    constexpr int STEPS = 6;
    constexpr int R = 100;

    fmt::print("N: {}, R: {}, M: {}\n", N, R, R/2);

    Tensor m(R, R, R, R);
    m.setConstant('.');

    std::ifstream in(argv[1]);
    std::string line;
    int x = -1, y = -1;
    auto mid = R / 2;
    int r = 0;
    while(std::getline(in, line)) {
        r = line.size();
        std::cout << line << "\n";
        for (auto c : line) {
            m(mid + x, mid + y++, mid, mid) = c;
        }
        x = x + 1;
        y = -1;
    }
    Eigen::array<int, Tensor::NumDimensions> center = { mid, mid, mid, mid };

    Tensor s = get_cube(m, center);
    Tensor m_new = m;

    std::array<int, N> indices; std::fill(indices.begin(), indices.end(), 0);

    for (int step = 1; step <= STEPS; ++step) {
        r += 2;
        int o = mid - step - 1;
        std::array<int, N> offsets; std::fill(offsets.begin(), offsets.end(), o);
        std::array<int, N> extents; std::fill(extents.begin(), extents.end(), r);
        Tensor current_region = m.slice(offsets, extents);
        //std::cout << "current region:\n" << current_region << "\n\n";

        for (int i = o; i < o + r; ++i) {
            for (int j = o; j < o + r; ++j) {
                for (int k = o; k < o + r; ++k) {
                    for (int l = o; l < o + r; ++l) {
                        auto s = get_cube(m, { i, j, k, l });
                        auto c = count_active(s) - (read_value(m, i, j, k, l) == '#');

                        char v;
                        if (m(i, j, k, l) == '#') {
                            v = (c == 2 || c == 3) ? '#' : '.';
                        } else if (m(i, j, k, l) == '.') {
                            v = (c == 3) ? '#' : '.';
                        }
                        write_value(m_new, v, i, j, k, l);
                    }
                }
            }
        }
        Tensor new_region = m_new.slice(offsets, extents);
        std::cout << "after " << step << " cycle(s):\n" << new_region << "\n\n";
        std::swap(m, m_new);
    }
    auto p1 = count_active(m);
    fmt::print("part 1: {}\n", p1);
    
    return 0;
}
