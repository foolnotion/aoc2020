#include <fmt/format.h>
#include "advent.hpp"
#include "util.hpp"
#include <functional>
#include <bitset>

#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"

enum class opcode : int { ACC, JMP, NOP };
std::string names[] { "acc", "jmp", "nop" };

struct instruction {
    opcode op;
    int val;

    friend std::ostream& operator<<(std::ostream& os, instruction in) {
        os << names[static_cast<int>(in.op)] << "\t" << in.val;
        return os;
    }
};

opcode str_to_op(std::string const& s) {
    if (s == "acc") return opcode::ACC;
    if (s == "jmp") return opcode::JMP;
    if (s == "nop") return opcode::NOP;
    throw std::runtime_error(fmt::format("unknown op {}\n", s));
}

struct vm {
    vm(gsl::span<instruction> _code) : A{0}, P{0}, code(_code) {}
    vm(vm const& other) : A(other.A), P(other.P), code(other.code) {}

    bool step() {
        if (P >= code.size()) {
            return false;
        }

        auto in = code[P++];

        switch(in.op) {
            case opcode::ACC:
                A += in.val;
                break;
            case opcode::JMP:
                P += in.val - 1; // cancel the above increment
                break;
            default:
                // do nothing
                break;
        };

        return true;
    }

    bool run(gsl::span<int> indices) {
        while (true) {
            if(!indices[P]) {
                indices[P] = true;
                step();
                if (terminated_normally()) {
                    return true;
                }
                continue;
            }
            return false;
        }
    }

    bool terminated_normally() const { return P >= code.size(); }

    void reset() { A = 0; P = 0; }

    // registers
    int A;

    int P; // instruction pointer
    gsl::span<instruction> code;
};

int day08(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file.\n");
        return 1;
    }

    std::ifstream in(argv[1]);
    std::string line;

    std::vector<instruction> code;

    while(std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }
        auto tokens = split(line, ' ');
        std::transform(begin(tokens[0]), end(tokens[0]), begin(tokens[0]), [](char c) { return std::tolower(c); });
        std::string_view sv(tokens[1].data() + (tokens[1][0] == '+'), tokens[1].size() - (tokens[1][0] == '+'));
        auto res = parse_number<int>(sv); 
        if (!res.has_value()) {
            throw std::runtime_error(fmt::format("could not parse value {} into an int\n", tokens[1]));
        }
        code.push_back(instruction { str_to_op(tokens[0]), res.value() });
    }

    vm vm(code);

    std::vector<int> idx(code.size(), false);
    vm.run(idx); 
    fmt::print("part 1: {}\n", vm.A);

    int ip = 0;

    auto part2 = [&]() -> std::optional<int> {
        vm.reset();
        std::fill(idx.begin(), idx.end(), false);

        while (true) {
            auto &in = vm.code[vm.P];
            if (in.op == opcode::JMP || in.op == opcode::NOP) {
                auto op_old = in.op;
                auto op_new = in.op == opcode::JMP ? opcode::NOP : opcode::JMP;

                in.op = op_new;
                auto idx1 = idx;
                struct vm vm1(vm);
                if (vm1.run(idx1)) {
                    return { vm1.A };
                }
                in.op = op_old;
            }
            idx[vm.P] = true;
            vm.step();
            if (vm.terminated_normally()) {
                return { vm.A };
            }
        }
        return std::nullopt;
    };
    auto res = part2();
    if (res.has_value()) { 
        fmt::print("part 2: {}\n", res.value());
    } else {
        fmt::print("part 2 no solution\n");
    }

    // performance benchmark
    ankerl::nanobench::Bench b;
    b.performanceCounters(true).minEpochIterations(10);
    b.run("day8 perf", part2);

    return 0;
}
