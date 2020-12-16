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

struct range {
    uint64_t a;
    uint64_t b;

    bool contains(uint64_t v) const { return v >= a && v <= b; }
    bool operator<(range const r) const { return std::tie(a, b) < std::tie(r.a, r.b); }

    friend std::ostream& operator<<(std::ostream& os, range const& r){
        os << r.a << "-" << r.b;
        return os;
    }
};

int day16(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file.\n");
        return 1;
    }

    std::ifstream in(argv[1]);
    std::string line;

    std::vector<std::array<range, 2>> ranges;
    std::vector<std::string> range_names;

    // parse valid ranges
    while (std::getline(in, line) && !line.empty()) {
        auto p = line.find(':');
        std::string_view sv(line.data(), p);
        range_names.push_back(std::string(sv));

        int i = p+2;
        int j = 0;
        while(line[i+j] != 'o') ++j;
        sv = std::string_view(line.data() + i, j-1);

        auto tok = split(std::string(sv), '-');
        auto a = parse_number<uint64_t>(tok[0]).value();
        auto b = parse_number<uint64_t>(tok[1]).value();
        range r1 { a, b };

        i += j + 3;
        j = line.size() - i;
        sv = std::string_view(line.data() + i, j);
        tok = split(std::string(sv), '-');
        a = parse_number<uint64_t>(tok[0]).value();
        b = parse_number<uint64_t>(tok[1]).value();
        range r2 { a, b };

        ranges.push_back({ r1, r2 });
    };

    std::vector<uint64_t> myticket;

    std::getline(in, line); // skip empty line
    std::getline(in, line); // skip the line that says your ticket:
    // my ticket
    auto tok = split(line, ',');
    std::transform(tok.begin(), tok.end(), std::back_inserter(myticket), [](auto const& s) { return parse_number<uint64_t>(s).value(); });

    // other tickets
    std::getline(in, line); // skip empty line
    std::getline(in, line); // skip the line that says nearby:
    std::vector<std::vector<uint64_t>> nearby;
    while (std::getline(in, line) && !line.empty()) {
        auto tok = split(line, ',');
        std::vector<uint64_t> other;
        std::transform(tok.begin(), tok.end(), std::back_inserter(other), [](auto const& s) { return parse_number<uint64_t>(s).value(); });
        nearby.push_back(other);
    }

    auto is_valid = [&](auto const& ticket) {
        auto error_rate = 0ul;
        for (auto v : ticket) {
            if (std::none_of(ranges.begin(), ranges.end(), [&](auto const& r) { return r[0].contains(v) || r[1].contains(v); })) {
                error_rate += v;
            }
        }
        return std::pair{ error_rate, error_rate == 0 };
    };

    auto p1 = 0ul;
    for (auto const& t : nearby) {
        auto [err, valid] = is_valid(t);
        p1 += err;
    }
    fmt::print("part 1: {}\n", p1);

    std::vector<std::vector<uint64_t>> valid_tickets;
    std::copy_if(nearby.begin(), nearby.end(), std::back_inserter(valid_tickets), [&](auto const& t) { return is_valid(t).second; });

    Eigen::Matrix<uint64_t, -1, -1, Eigen::ColMajor> m(valid_tickets.size(), ranges.size());
    int row = 0;
    for (auto const& t : valid_tickets) {
        m.row(row++) = as_map(t);
    }

    auto part2 = [&]() {
        Eigen::Matrix<int64_t, -1, -1, Eigen::ColMajor> cache(m.cols(), m.cols());
        cache.setZero();

        uint64_t p{1};
        std::vector<std::pair<int, int>> counts(ranges.size());
        for (size_t i = 0; i < cache.rows(); ++i) {
            auto range = ranges[i];
            for (size_t j = 0; j < cache.cols(); ++j) {
                auto values = gsl::span<uint64_t>(m.col(j).data(), m.col(j).size());
                cache(i, j) = std::all_of(values.begin(), values.end(), [&](auto v) { return range[0].contains(v) || range[1].contains(v); });
            }
            counts[i] = { i, (cache.row(i).array() == 1).count() };
        }
        std::sort(counts.begin(), counts.end(), [](auto a, auto b) { return a.second < b.second; });

        for (auto [index, count] : counts) {
            auto row = cache.row(index);
            int idx;
            row.maxCoeff(&idx);
            cache.col(idx).setZero();

            if (range_names[index].find("departure") != std::string::npos) {
                p *= myticket[idx];
            }
        }
        return p;
    };
    auto p2 = part2();
    fmt::print("part 2: {}\n", p2);

    ankerl::nanobench::Bench b;
    b.performanceCounters(true).minEpochIterations(100);

    b.run("part 2", [&]() { part2(); });

    return 0;
}
