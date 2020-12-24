#include <algorithm>
#include <bitset>
#include <execution>
#include <fmt/format.h>
#include <functional>
#include <initializer_list>
#include <list>
#include <robin_hood.h>
#include <stack>

#include "advent.hpp"
#include "util.hpp"

struct node {
    int64_t value;
    node* next { nullptr };
};

int day23(int argc, char** argv)
{
    //std::vector<int> cups { 3, 8, 9, 1, 2, 5, 4, 6, 7 };
    std::vector<int> cups { 6, 2, 4, 3, 9, 7, 1, 5, 8 };

    std::vector<node*> index(1'000'000 + 1);

    std::vector<node> nodes(1'000'000);

    for (int i = 0; i < cups.size(); ++i) {
        nodes[i] = { cups[i], nullptr };
        index[nodes[i].value] = &nodes[i];

        if (i > 0) {
            nodes[i - 1].next = &nodes[i];
        }
    }
    for (int i = cups.size(); i < 1'000'000; ++i) {
        nodes[i] = { i+1, nullptr };
        index[nodes[i].value] = &nodes[i];

        if (i > 0) {
            nodes[i - 1].next = &nodes[i];
        }
    }
    nodes.back().next = &nodes[0];

    int lo = 1;
    int hi = 1'000'000;

    int move = 0;
    int max_rounds = 10'000'000;

    std::array<int64_t, 3> picked;

    node *p = &nodes[0], *q = nullptr, *r = nullptr;
    for (int round = 0; round < max_rounds; ++round) {
        q = p->next;
        picked[0] = q->value; q = q->next;
        picked[1] = q->value; q = q->next;
        picked[2] = q->value;

        auto d = p->value - 1; if (d < lo) d = hi;
        while(std::find(picked.begin(), picked.end(), d) != picked.end()) {
            if (--d < lo) d = hi;
        }
        r = index[d];

        auto tmp = q->next;
        q->next = r->next;
        r->next = p->next;
        p->next = tmp;

        p = p->next;
    }

    auto x = index[1];
    fmt::print("{} {} {}\n", x->value, x->next->value, x->next->next->value);
    fmt::print("{}\n", x->next->value * x->next->next->value);
    return 0;
}
