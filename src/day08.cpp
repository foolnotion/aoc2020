#include <fmt/format.h>
#include "advent.hpp"
#include "util.hpp"
#include <functional>
#include <bitset>

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

    auto execute = [](struct vm& vm) {
        vm.reset();
        std::unordered_set<int> indices;
        while (true) {
            if(auto [it, ok] = indices.insert(vm.P); ok) {
                vm.step();
                if (vm.terminated_normally()) {
                    return true;
                }
                continue;
            }
            return false;
        }
    };

    execute(vm);
    fmt::print("part 1: {}\n", vm.A);

    for (auto& in : code) {
        if (!(in.op == opcode::JMP || in.op == opcode::NOP)) {
            continue;
        }

        auto op_old = in.op;
        auto op_new = in.op == opcode::JMP ? opcode::NOP : opcode::JMP;

        in.op = op_new;
        if (execute(vm)) {
            fmt::print("part 2: {}\n", vm.A);
            break;
        }
        in.op = op_old;
    }
    return 0;
}
