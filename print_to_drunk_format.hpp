#pragma once

#include <string>

#include "file_visitor.hpp"

namespace edt {

    /**
     * @brief Класс для записи данных в файл в выдуманном формате Drunk
     * 
     */
    class PrintDrunk : public FileVisitor {
    public:
        /**
         * @brief Монтирование экземпляра класса
         *
         */
        PrintDrunk(std::string filename){
            // Инициализация
        }

        void Visit(std::shared_ptr<File> prt_obj) override {
            // Реализация записи в файл
        }
    };

} // namespace edt