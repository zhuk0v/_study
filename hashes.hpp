#pragma once

#include <vector>

#include <boost/crc.hpp>

namespace df {

    class HashCalculator {
    public:
        virtual int Calculate(std::vector<std::uint8_t> dat) = 0;
    };

    class Crc32HashCalculator : public HashCalculator {
    public:
        int Calculate(std::vector<std::uint8_t> dat) {
            boost::crc_32_type result{};
            result.process_bytes(dat.data(), dat.size());
            return result.checksum();
        }
    };

} // namespace df



