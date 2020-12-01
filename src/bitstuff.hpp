#ifndef ADVENT_BITSTUFF_CPP
#define ADVENT_BITSTUFF_CPP

#include <cstdint>
#include <tuple>
#include <array>

namespace bitstuff {
    namespace quibble {
        // the for quibbles in a 64-bit unsigned int
        static constexpr uint64_t q3 = 0xffff000000000000;
        static constexpr uint64_t q2 = 0x0000ffff00000000;
        static constexpr uint64_t q1 = 0x00000000ffff0000;
        static constexpr uint64_t q0 = 0x000000000000ffff;

        template<int I = 0>
        static uint16_t unpack(uint64_t p)
        {
            return static_cast<uint16_t>(p & quibble::q0);
        }

        template<>
        uint16_t unpack<1>(uint64_t p)
        {
            return static_cast<uint16_t>((p & quibble::q1) >> 16);
        }

        template<>
        uint16_t unpack<2>(uint64_t p)
        {
            return static_cast<uint16_t>((p & quibble::q2) >> 32);
        }

        template<>
        uint16_t unpack<3>(uint64_t p)
        {
            return static_cast<uint16_t>((p & quibble::q3) >> 48);
        }

        static uint64_t pack(uint16_t x3, uint16_t x2, uint16_t x1, uint16_t x0)
        {
            return (static_cast<uint64_t>(x3) << 48) |
                   (static_cast<uint64_t>(x2) << 32) |
                   (static_cast<uint64_t>(x1) << 16) | x0;
        }

        std::tuple<uint16_t, uint16_t, uint16_t, uint16_t>
        static unpack(uint64_t p)
        {
            auto x0 = unpack<0>(p);
            auto x1 = unpack<1>(p);
            auto x2 = unpack<2>(p);
            auto x3 = unpack<3>(p);
            return { x3, x2, x1, x0 };
        }

        static uint64_t upper_half(uint64_t p)
        {
            auto mask = quibble::q3 | quibble::q2;
            return p & mask; 
        }

        static uint64_t lower_half(uint64_t p)
        {
            auto mask = quibble::q1 | quibble::q0;
            return p & mask;
        }
    }
}
#endif
