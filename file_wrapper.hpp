#pragma once

#include <filesystem>

#include <boost/flyweight.hpp>

#include "hashes.hpp"

namespace df {

    class File {
    private:
        boost::flyweights::flyweight<std::size_t> m_block_size;

        std::filesystem::path m_path_to_file;
        std::ifstream::pos_type m_pos_in_file;

        std::shared_ptr<HashCalculator> m_ptr_hash_calculator;
        std::vector<int> m_calc_hash;
        bool m_full_calc;

        bool m_lock;
    public:
        File(std::filesystem::path path_to_file, std::size_t block_size, std::shared_ptr<HashCalculator> ptr_hash_calculator) :
            m_block_size(block_size),
            m_path_to_file(path_to_file),
            m_pos_in_file(),
            m_ptr_hash_calculator(ptr_hash_calculator),
            m_calc_hash(),
            m_full_calc(false),
            m_lock(false) {

            std::ifstream f_id(m_path_to_file, std::ios_base::binary);
            m_pos_in_file = f_id.tellg();
            f_id.close();
        }

        void DoNotCompare() {
            m_lock = true;
        }

        bool Miss() const {
            return m_lock;
        }

        auto Path() const {
            return m_path_to_file;
        }

        bool operator==(File& rhs) {
            for (std::size_t i = 0;; i++) {
                if (!AvailableHash(i) || !rhs.AvailableHash(i)) {
                    break;
                }
                if (GetHash(i) != rhs.GetHash(i)) {
                    break;
                }
                if (m_full_calc && rhs.m_full_calc) {
                    return true;
                }
            }
            return false;
        }

    private:

        auto ReadBlock() {
            std::vector<std::uint8_t> dat(m_block_size.get(), '\0');
            std::ifstream f_id(m_path_to_file, std::ios_base::binary);
            f_id.seekg(m_pos_in_file);
            f_id.read((char*)dat.data(), m_block_size.get());
            if (f_id.eof()) {
                m_full_calc = true;
            }
            m_pos_in_file = f_id.tellg();
            f_id.close();
            return dat;
        }

        int CalculateHash() {
            m_calc_hash.push_back(m_ptr_hash_calculator->Calculate(ReadBlock()));
            return m_calc_hash.back();
        }

        int GetHash(std::size_t i) {
            if (i < m_calc_hash.size()) {
                return m_calc_hash.at(i);
            }
            return CalculateHash();;
        }

        bool AvailableHash(std::size_t i) {
            return m_full_calc ? (m_calc_hash.size() > i) : true;
        }
    };

} // namespace df