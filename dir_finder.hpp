#pragma once

#include <memory>
#include <string>
#include <map>


#include <boost/range.hpp>



#include <fstream>

#include <iostream>

#include "file_wrapper.hpp"

namespace df {

    class DirFinderDuplicate {
    private:
        std::vector<File> m_files;
        std::vector<std::vector<std::filesystem::path>> m_duplicates;

    public:
        DirFinderDuplicate(
            std::vector<std::string> dir_for_scan,
            std::vector<std::string> dir_for_excl,
            int scan_level,
            int min_file_size,
            std::vector<std::string>,
            int block_size,
            std::string hash_func) :

            m_files() {
            // Check
            for (auto& dir_name : dir_for_scan) {
                if (!std::filesystem::is_directory(dir_name)) {
                    throw std::logic_error("The input path is not a directory");
                }
            }

            // Check
            for (auto& dir_name : dir_for_excl) {
                if (!std::filesystem::is_directory(dir_name)) {
                    throw std::logic_error("The exclude path is not a directory");
                }
            }

            // Create hash calculator
            std::shared_ptr<HashCalculator> ptr_hash_calculator;
            if (hash_func == "crc32") {
                ptr_hash_calculator = std::make_shared<Crc32HashCalculator>();
            }

            // Collect path
            {
                auto fits_the_size = [](const std::size_t& min_file_size, const std::string path_to_file) -> bool {
                    return std::filesystem::file_size(path_to_file) >= min_file_size;
                };

                auto is_excluded = [](const std::vector<std::string>& dir_for_excl, const std::string path_to_dir) -> bool {
                    if (dir_for_excl.size() == 0) {
                        return false;
                    }
                    return std::find(dir_for_excl.begin(), dir_for_excl.end(), path_to_dir) != dir_for_excl.end();
                };

                auto check_file = [&](auto& entry) -> bool {
                    if (!fits_the_size(min_file_size, entry.string())) {
                        return false;
                    }
                    return true;
                };

                std::function<void(std::filesystem::path, std::vector<File>&)> read_dir;
                read_dir = [&, this](std::filesystem::path path_to_dir, std::vector<File>& files) -> void {
                    for (auto& entry : boost::make_iterator_range(std::filesystem::directory_iterator(path_to_dir), {})) {
                        std::filesystem::file_status fs = std::filesystem::status(entry);
                        switch (fs.type()) {
                        case std::filesystem::file_type::regular:
                            if (check_file(entry.path())) {
                                files.emplace_back(entry.path(), block_size, ptr_hash_calculator);
                            }
                            break;
                        case std::filesystem::file_type::directory:
                            if (scan_level == 1 && !is_excluded(dir_for_excl, entry.path().string())) {
                                read_dir(entry.path(), files);
                            }
                            break;
                        default:
                            break;
                        }
                    }
                };

                // Loop
                for (auto& path_to_dir : dir_for_scan) {
                    read_dir(path_to_dir, m_files);
                }
            }
        }

        ~DirFinderDuplicate() = default;

        void Run() {

            std::vector<std::filesystem::path> duplicates{};
            for (std::size_t i = 0; i < m_files.size(); i++) if (!m_files[i].Miss()) {
                for (std::size_t j = i + 1; j < m_files.size(); j++) if (!m_files[j].Miss()) {
                    if (m_files[i] == m_files[j]) {
                        duplicates.emplace_back(m_files[j].Path());
                        m_files[j].DoNotCompare();
                    }
                }
                if (duplicates.size() > 0) {
                    duplicates.emplace_back(m_files[i].Path());
                    m_files[i].DoNotCompare();

                    Output(std::move(duplicates));
                }
            }
        }

        void Output(std::vector<std::filesystem::path> duplicates_path) {
            for (auto& path : duplicates_path) {
                std::cout << path << std::endl;
            }
            std::cout << std::endl;
        }
    };

} // namespace df