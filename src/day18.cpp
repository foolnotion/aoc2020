#include <fmt/format.h>

#include <algorithm>
#include <bitset>
#include <functional>
#include <robin_hood.h>
#include <stack>

#include "advent.hpp"
#include "util.hpp"

constexpr char lparen = '(';
constexpr char rparen = ')';
constexpr char add = '+';
constexpr char mul = '*';
constexpr char term = 'T';
constexpr char space = ' ';

template <char... args>
inline bool is(char c) { return ((c == args) || ...); }

struct token {
    char symbol;
    int64_t value;
    size_t precedence;
};

class parser {
    std::vector<token> tokens_;
    std::string expr_;

    size_t i;

    token next_term()
    {
        while (i < expr_.size() && std::isspace(expr_[i]))
            ++i;

        size_t j = i;

        while (j < expr_.size() && !std::isspace(expr_[j]) && !is<lparen, rparen, add, mul>(expr_[j])) {
            ++j;
        }

        auto tok = std::string_view(expr_.data() + i, j - i);
        auto res = parse_number<int64_t>(tok);
        // check if we could parse the number
        if (!res.has_value()) {
            fmt::print("ERROR: could not parse token {} as a number\n", tok);
            fmt::print("Full expression: {}\n", expr_);
            exit(1);
        }
        i = j;
        return { term, res.value(), 0 };
    }

    token next_op()
    {
        while (i < expr_.size() && std::isspace(expr_[i]))
            ++i;
        char c = expr_[i++];
        EXPECT(!(is<lparen, rparen>(c)));
        EXPECT((is<add, mul>(c)));
        return { c, 0, 0 };
    }

    void parse()
    {
        if (i >= expr_.size())
            return;

        while (std::isspace(expr_[i]))
            ++i;

        char c = expr_[i];
        if (is<lparen>(c)) {
            tokens_.push_back({ lparen, 0 });
            ++i;
            return parse();
        }

        // get the term
        auto t = next_term();
        tokens_.push_back(t);

        // after a term we may have rparen
        while (i < expr_.size() && is<rparen>(expr_[i])) {
            tokens_.push_back({ rparen, 0 });
            ++i;
        }

        if (i < expr_.size()) {
            // after the term we should have an operator or expression end
            auto op = next_op();
            tokens_.push_back(op);
        }

        // now we should have a subexpression left of the same form
        return parse();
    }

public:
    std::vector<token> parse(std::string const& expr)
    {
        i = 0;
        expr_ = expr;
        parse();
        return tokens_;
    }
};

template <char OP = add>
struct func {
    template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T>> = true>
    int64_t operator()(T t) { return t; }

    template <typename... Terms>
    int64_t operator()(Terms... terms) { return (terms + ...); }
};

template <>
struct func<mul> {
    template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T>> = true>
    int64_t operator()(T t) { return t; }

    template <typename... Terms>
    int64_t operator()(Terms... terms) { return (terms * ...); }
};

template <typename... T>
int64_t eval_terms(char op, T... terms)
{
    if (is<add>(op)) {
        return func<add> {}(terms...);
    }
    if (is<mul>(op)) {
        return func<mul> {}(terms...);
    }
    throw std::runtime_error("unknown symbol");
}

class interpreter {

private:
    robin_hood::unordered_map<char, size_t> precedence {
        { add, 2 },
        { mul, 1 }
    };

public:
    int64_t eval(gsl::span<token> tokens)
    {
        std::vector<std::tuple<size_t, size_t, std::optional<int64_t>>> paren_pairs(tokens.size());

        // identify all paren pairs
        int count = 0;
        std::stack<size_t> stk;
        for (size_t i = 0; i < tokens.size(); ++i) {
            auto s = tokens[i].symbol;

            if (is<lparen>(s)) {
                stk.push(i);
            }
            if (is<rparen>(s)) {
                auto last_l = stk.top();
                stk.pop();
                paren_pairs[last_l] = { last_l, i, std::nullopt };
            }
        }

        // evaluate
        for (auto it = paren_pairs.rbegin(); it != paren_pairs.rend(); ++it) {
            if (!is<lparen>(tokens[std::distance(it, paren_pairs.rend()) - 1].symbol)) {
                continue;
            }
            auto [i, j, e] = *it;

            // gather terms and operators
            std::stack<int64_t> terms;
            std::stack<char> operators;

            for (size_t k = i + 1; k < j; ++k) {
                auto s = tokens[k].symbol;

                if (is<lparen>(s)) {

                    terms.push(std::get<2>(paren_pairs[k]).value());
                    auto [start_idx, end_idx, _] = paren_pairs[k];
                    k += end_idx - start_idx - 1;

                } else if (is<term>(s)) {

                    terms.push(tokens[k].value);

                } else if (is<add, mul>(s)) {

                    operators.push(s);
                }
            }

            while (!operators.empty()) {
                auto op1 = operators.top();
                operators.pop();

                int64_t result;
                if (operators.empty()) {
                    auto term = terms.top();
                    terms.pop();
                    result = eval_terms(op1, term);

                    while (!terms.empty()) {

                        term = terms.top();
                        terms.pop();
                        result = eval_terms(op1, result, term);
                    }
                    std::get<2>(*it) = { result };
                    break;
                }

                auto op2 = operators.top();
                operators.pop();

                // if I have two operators it means there are at least three terms
                auto a = terms.top();
                terms.pop();
                auto b = terms.top();
                terms.pop();
                auto c = terms.top();
                terms.pop();

                if (precedence[op1] < precedence[op2]) {
                    auto v = eval_terms(op2, c, b);
                    terms.push(v);
                    terms.push(a);
                    operators.push(op1);
                } else {
                    auto v = eval_terms(op1, a, b);
                    terms.push(c);
                    terms.push(v);
                    operators.push(op2);
                }
            }
        }

        return std::get<2>(paren_pairs[0]).value();
    }
};

int day18(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file.\n");
        return 1;
    }

    std::vector<std::string> examples {
        "1 + 2 * 3 + 4 * 5 + 6",
        "1 + (2 * 3) + (4 * (5 + 6))",
        "2 * 3 + (4 * 5)",
        "5 + (8 * 3 + 9 + 3 * 4 * 3)",
        "5 * 9 * (7 * 3 * 3 + 9 * 3 + (8 + 6 * 4))",
        "((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2"
    };

    fmt::print("examples test\n");
    for (auto& ex : examples) {
        ex = '(' + ex + ')';
        fmt::print("{} = {}\n", ex, '?');
        parser p;
        auto tokens = p.parse(ex);

        interpreter interp;
        auto result = interp.eval(tokens);
        fmt::print("{} = {}\n\n", ex, result);
    }

    std::ifstream in(argv[1]);
    std::string line;

    int64_t sum{0};

    while(std::getline(in, line)) {
        parser p;
        auto tokens = p.parse("(" + line + ")");
        interpreter interp;
        sum += interp.eval(tokens);
    }

    fmt::print("sum = {}\n", sum);

    return 0;
}
