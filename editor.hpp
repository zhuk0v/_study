#pragma once

#include <memory>
#include <string>
#include <list>

#include "file.hpp"

namespace edt {

    class Editor {
    private:
        /// List of files
        std::list<std::shared_ptr<File>> m_files;

    public:
        /**
         * @brief Монтирование экземпляра класса
         *
         */
        Editor() = default;
        /**
         * @brief Размонтирование экземпляра класса
         *
         */
        ~Editor() = default;

        /**
         * @brief Create a new file with the given name
         *
         */
        void CreateFile(std::string filename) {
            // Create a new file
            m_files.push_back(std::make_shared<File>(filename));
        }

        /**
         * @brief Ищет файл в списке и возвращает его, когда находит
         *
         * @param filename
         * @return std::shared_ptr<File>
         */
        std::shared_ptr<File> GetFile(std::string filename) {
            for (auto file : m_files) {
                if (file->Name() == filename){
                    return file;
                }
            }
            return nullptr;
        }

        /**
         * @brief Импортирование данных из файла
         *
         * @param filename
         */
        void ImportFile(std::string filename, std::string input_path);

        /**
         * @brief Вывод данных в файла
         *
         * @param filename
         */
        void PrintFile(std::string filename, std::string output_path);
    };

} // namespace edt
