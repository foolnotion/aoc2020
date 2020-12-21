#include <fmt/format.h>

#include <algorithm>
#include <bitset>
#include <functional>
#include <robin_hood.h>
#include <stack>

#include "advent.hpp"
#include "util.hpp"

template <typename T, int S = 10>
struct tile {
    enum { DIMENSION = S };
    using M = Eigen::Matrix<T, S, S>;
    size_t id;
    M m;

    friend bool operator==(tile const& a, tile const& b) { return a.id == b.id; }
    friend bool operator!=(tile const& a, tile const& b) { return a.id != b.id; }

    friend std::ostream& operator<<(std::ostream& os, tile const& t)
    {
        os << "id: " << t.id << "\n";
        os << t.m << "\n";
        return os;
    }

    tile() = delete;

    tile(size_t id_, M const& m_)
        : id(id_)
        , m(m_)
    {
    }
    tile(size_t id_, M&& m_)
        : id(id_)
        , m(std::move(m_))
    {
    }

    tile(tile const& other)
        : id { other.id }
        , m(other.m)
    {
    }
    tile(tile&& other)
        : id { other.id }
        , m(std::move(other.m))
    {
    }

    tile& operator=(tile const& other)
    {
        tile tmp(other);
        std::swap(this->m, tmp.m);
        this->id = tmp.id;
        return *this;
    }

    std::vector<tile> dihedral_group() const
    {
        auto tmp = *this;
        return {
            tmp,
            tmp.transpose().flip_horizontal(),
            tmp.flip_horizontal().flip_vertical(),
            tmp.transpose().flip_vertical(),
            tmp.flip_vertical(),
            tmp.transpose(),
            tmp.flip_horizontal(),
            tmp.flip_horizontal().flip_vertical().transpose()
        };
    }

    auto left() const -> typename M::ConstColXpr { return m.col(0); }
    auto right() const -> typename M::ConstColXpr { return m.col(m.cols() - 1); }
    auto top() const -> typename M::ConstRowXpr { return m.row(0); }
    auto bottom() const -> typename M::ConstRowXpr { return m.row(m.rows() - 1); }

    auto match_right(tile const& other) const -> bool
    {
        return right() == other.left();
    }

    auto match_left(tile const& other) const -> bool
    {
        return left() == other.right();
    }

    auto match_top(tile const& other) const -> bool
    {
        return top() == other.bottom();
    }

    auto match_bottom(tile const& other) const -> bool
    {
        return bottom() == other.top();
    }

    auto match_horizontal(tile const& other) const -> bool
    {
        return match_left(other) || match_right(other);
    }

    auto match_vertical(tile const& other) const -> bool
    {
        return match_top(other) || match_bottom(other);
    }

    auto match_any_side(tile const& other) const -> bool
    {
        return match_horizontal(other) || match_vertical(other);
    }

    template <typename U, typename V>
    bool are_equal(U u, V v) const
    {
        // can't use Eigen because of different dimensions
        gsl::span<typename U::Scalar> uu(u.data(), u.size());
        gsl::span<typename V::Scalar> vv(v.data(), v.size());
        return std::equal(uu.begin(), uu.end(), vv.begin());
    }

    template <typename U, typename V>
    bool are_even(U v, V u) const
    {
        return (v.array() == '#').count() == (u.array() == '#').count();
    }

    void flip_horizontal_in_place()
    {
        Eigen::PermutationMatrix<-1, -1> p(m.cols());
        auto ptr = p.indices().data();
        auto len = p.indices().size();
        std::iota(ptr, ptr + len, 0);
        std::reverse(ptr, ptr + len);
        m = m * p;
    }

    void flip_vertical_in_place()
    {
        Eigen::PermutationMatrix<-1, -1> p(m.cols());
        auto ptr = p.indices().data();
        auto len = p.indices().size();
        std::iota(ptr, ptr + len, 0);
        std::reverse(ptr, ptr + len);
        m = p * m;
    }

    void transpose_in_place()
    {
        m.transposeInPlace();
    }

    void reverse_in_place()
    {
        m.reverseInPlace();
    }

    tile flip_horizontal() const
    {
        auto tmp = *this;
        tmp.flip_horizontal_in_place();
        return tmp;
    }

    tile flip_vertical() const
    {
        auto tmp = *this;
        tmp.flip_vertical_in_place();
        return tmp;
    }

    tile transpose() const
    {
        auto tmp = *this;
        tmp.transpose_in_place();
        return tmp;
    }

    tile reverse() const
    {
        auto tmp = *this;
        tmp.reverse_in_place();
        return tmp;
    }
};

int day20(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file.\n");
        return 1;
    }

    std::string line;
    std::ifstream in(argv[1]);

    using Tile = tile<char, 10>;

    std::vector<Tile> tiles;

    while (std::getline(in, line)) {
        size_t id;
        Tile::M m;
        if (auto pos = line.find(':', 0); pos != std::string::npos) {
            id = parse_number<size_t>(std::string_view(line.data() + 5, pos - 5)).value();
        }
        long row = 0;
        while (std::getline(in, line) && !line.empty()) {
            Eigen::Map<Eigen::Array<char, -1, 1>> map(line.data(), line.size());
            m.row(row++) = map;
        }
        tiles.push_back({ id, m });
    }

    fmt::print("tiles: {}\n", tiles.size());

    // part 1
    auto dim = static_cast<size_t>(std::sqrt(tiles.size()));

    std::vector<Tile> all;
    for (auto const& t : tiles) {
        for (auto&& u : t.dihedral_group()) {
            all.push_back(u);
        }
    }
    fmt::print("all tiles size: {}\n", all.size());

    robin_hood::unordered_set<size_t> visited;
    std::stack<size_t> ordered;

    Eigen::Matrix<size_t, -1, -1> image(dim, dim);
    gsl::span<size_t> array(image.data(), dim * dim);

    size_t steps = 0;
    auto check = [&](size_t count, auto&& rec) -> bool {
        ++steps;
        if (count == tiles.size()) {
            return true;
        }

        auto row = count % dim;
        auto col = count / dim;

        for (size_t i = 0; i < all.size(); ++i) {
            auto const& u = all[i];

            if (visited.contains(u.id)) {
                continue;
            }

            if (row > 0 && !u.match_left(all[image(row - 1, col)])) {
                continue;
            }

            if (col > 0 && !u.match_top(all[image(row, col - 1)])) {
                continue;
            }

            visited.insert(u.id);
            image(row,col) = i;
            auto res = rec(count + 1, rec);
            visited.erase(u.id);
            if (res) {
                return true;
            }
        }
        return false;
    };
    bool found = false;
    for (size_t i = 0; i < all.size(); ++i) {
        image(0,0) = i;
        auto const& u = all[i];
        visited.insert(u.id);
        auto res = check(1, check);
        visited.erase(u.id);
        if (res) {
            found = true;
            break;
        }
    }

    if (!found) {
        fmt::print("unable to find arrangement.");
        exit(1);
    }
    fmt::print("steps: {}\n", steps);
    std::vector<size_t> ids;
    for (auto i : array) {
        ids.push_back(all[i].id);
    }

    auto const D = Tile::DIMENSION - 2;
    Eigen::Map<Eigen::Matrix<size_t, -1, -1>> map(ids.data(), dim, dim);
    std::cout << map << "\n\n";
    Eigen::Array<char, -1, -1> stitched(dim * D, dim * D);

    // assemble the tiles into the final image
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            auto const& m = all[image(i, j)].m;
            stitched.block(i * D, j * D, D, D) = m.block(1, 1, m.rows()-1, m.cols()-1).transpose();
        }
    }
    stitched = (stitched == '#').select(stitched, ' ');
    tile<char, -1> img(0, stitched);

    std::string s0 = "                  # "; 
    std::string s1 = "#    ##    ##    ###";
    std::string s2 = " #  #  #  #  #  #   ";
    Eigen::Matrix<char, 3, 20> mon;
    mon.row(0) = as_map(s0);
    mon.row(1) = as_map(s1);
    mon.row(2) = as_map(s2);

    auto mon_scales = (mon.array() == '#').count();
    auto x = (img.m.array() == '#').count();

    auto match_monster = [&](auto const& mat) {
        for (int i = 0; i < mon.rows(); ++i) {
            for (int j = 0; j < mon.cols(); ++j) {
                if (mon(i, j) == '#' && mat(i, j) != mon(i, j)) {
                    return false;
                }
            }
        }
        return true;
    };

    for (auto&& g : img.dihedral_group()) {
        auto n = x;

        for (int i = 0; i <= g.m.rows() - mon.rows(); ++i) {
            for (int j = 0; j <= g.m.cols() - mon.cols(); ++j) {
                if (match_monster(g.m.block(i, j, mon.rows(), mon.cols()))) {
                    n -= mon_scales;
                    j += mon.cols();
                }
            }
        }
        if (n < x) {
            fmt::print("n = {}\n", n);
            break;
        }
    }

    return 0;
}
