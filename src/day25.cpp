#include <algorithm>
#include <bitset>
#include <execution>
#include <fmt/format.h>
#include <functional>
#include <initializer_list>
#include <list>
#include <map>
#include <robin_hood.h>
#include <stack>

#include "advent.hpp"
#include "util.hpp"

template <class T>
T mul_mod(T a, T b, T m)
{
    if (m == 0)
        return a * b;

    T r = T();

    while (a > 0) {
        if (a & 1)
            if ((r += b) > m)
                r %= m;
        a >>= 1;
        if ((b <<= 1) > m)
            b %= m;
    }
    return r;
}

template <class T>
T pow_mod(T a, T n, T m)
{
    T r = 1;

    while (n > 0) {
        if (n & 1)
            r = mul_mod(r, a, m);
        a = mul_mod(a, a, m);
        n >>= 1;
    }
    return r;
}

int day25(int argc, char** argv)
{
    uint64_t a { 1526110 };
    uint64_t b { 20175123 };
    uint64_t m { 20201227 };

    uint64_t x{1}, y{1};

    uint64_t p{0};

    while (x != a && ++p) {
        x = mul_mod(uint64_t{7}, x, m);
    }

    fmt::print("{}\n", pow_mod(b, p, m));

    return 0;
}
