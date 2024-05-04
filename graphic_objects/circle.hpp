#pragma once

#include "graphic_objects_base.hpp"

namespace edt {

    class Circle : public GraphicObjectsBase {
    public:
        /**
         * @brief Монтирование экземпляра класса
         *
         */
        Circle(std::string name) : GraphicObjectsBase(name) {}

        /**
         * @brief Размонтирование экземпляра класса
         *
         */
        ~Circle() = default;
    };
} // namespace edt