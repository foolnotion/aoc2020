#include <algorithm>
#include <bitset>
#include <fmt/format.h>
#include <functional>
#include <robin_hood.h>
#include <stack>
#include <execution>

#include "advent.hpp"
#include "util.hpp"

template<typename T>
struct hasher {
    uint64_t hash(const uint8_t* key, size_t len) noexcept
    {
        return XXH3_64bits(key, len);
    }

    uint64_t operator()(const T* key, size_t len) noexcept
    {
        return hash(reinterpret_cast<uint8_t const*>(key),
                    len * sizeof(T) / sizeof(uint8_t));
    }
};

template<typename T>
struct deck {
private:
    std::vector<T> v;
    size_t i;

    T* ptr() { return v.data() + i; }
    T const* ptr() const { return v.data() + i; }
public:
    deck(std::vector<size_t> const& values) : v(values), i(0) {}

    T front() const { return v[i]; }
    T pop_front() { return v[i++]; }

    void push_back(T t) { v.push_back(t); }

    gsl::span<T> as_span() { return gsl::span<T>(ptr(), size()); }
    gsl::span<const T> as_span() const { return gsl::span<const T>(ptr(), size()); }

    bool empty() const { return i == v.size(); }
    size_t size() const { return v.size() - i; }

    uint64_t hash() const { return hasher<T>{}(ptr(), size()); }

    friend std::ostream& operator<<(std::ostream& os, deck<T> const& d) {
        auto s = d.as_span();
        for (size_t i = 0; i < s.size(); ++i) fmt::print("{}{}", s[i], i == s.size()-1 ? "" : " ");
        return os;
    }

    deck<T> slice(size_t offset, size_t len) {
        auto s = as_span();
        decltype(v) v1(s.begin() + offset, s.begin() + offset + len);
        return deck<T>(v1);
    }
};

int day22(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file.\n");
        return 1;
    }

    std::string line;
    std::ifstream in(argv[1]);

    std::vector<size_t> players[2];

    int idx = -1;
    while(std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }
        if (line.find("Player") != std::string::npos) {
            idx++;
            continue;
        }
        auto v = parse_number<size_t>(line).value();
        players[idx].push_back(v);
    }

    deck<size_t> p1(players[0]);
    deck<size_t> p2(players[1]);

    using Deck = deck<size_t>;
    auto game = [](Deck& p1, Deck& p2, auto &&rec) -> size_t {
        robin_hood::unordered_set<uint64_t> set;

        while(!(p1.empty() || p2.empty())) {
            std::array<uint64_t, 2> hh { p1.hash(), p2.hash() };
            if (auto [it, ok] = set.insert(hasher<uint64_t>{}(hh.data(), hh.size())); !ok) {
                return 0;
            }

            auto c1 = p1.pop_front();
            auto c2 = p2.pop_front();

            size_t winner;
            // If both players have at least as many cards remaining
            // in their deck as the value of the card they just drew,
            // the winner of the round is determined by playing a new game
            if (p1.size() >= c1 && p2.size() >= c2) {
                auto q1 = p1.slice(0, c1);
                auto q2 = p2.slice(0, c2);
                winner = rec(q1, q2, rec);
            } else {
                winner = c2 > c1;
            }
            // Otherwise, at least one player must not have enough cards
            // left in their deck to recurse; the winner of the round is
            // the player with the higher-value card
            if (winner) {
                p2.push_back(c2);
                p2.push_back(c1);
            } else {
                p1.push_back(c1);
                p1.push_back(c2);
            }
        }

        return p1.empty();
    };

    auto winner = game(p1, p2, game);
    fmt::print("\n== Post-game results ==\n");
    fmt::print("Player 1's deck: ");
    std::cout << p1 << "\n";
    fmt::print("Player 2's deck: ");
    std::cout << p2 << "\n";
    fmt::print("\n");

    auto p = (winner ? p2 : p1).as_span();

    int multiplier = 0;
    auto score = std::transform_reduce(std::execution::seq,
            p.rbegin(),
            p.rend(),
            0ul, std::plus{},
            [&](auto v) { return ++multiplier * v; });
    fmt::print("score: {}\n", score);

    return 0;
}
