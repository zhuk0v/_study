#pragma once

#include "file.hpp"

namespace edt {

    class FileVisitor {
    public:
        /**
         * @brief Метод обработки
         * 
         * @param prt_obj 
         */
        virtual void Visit(std::shared_ptr<File> prt_obj) = 0;
    };

} // namespace edt