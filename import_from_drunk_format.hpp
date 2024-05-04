#pragma once

#include <string>

#include "file_visitor.hpp"

namespace edt {

    /**
     * @brief Класс для вычитывания данных из файла, записанный в выдуманном формате Drunk
     *
     */
    class ImportDrunk : public FileVisitor {
    public:
        /**
         * @brief Монтирование экземпляра класса
         *
         */
        ImportDrunk(std::string filename) {
            // Инициализация
        }

        void Visit(std::shared_ptr<File> prt_obj) override {
            // Реализация вычитывания из файла
        }
    };

} // namespace edt