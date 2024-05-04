#pragma once

#include <memory>
#include <string>

namespace edt {

    /**
     * @brief Абстрактный класс для всех графических объектов
     *
     */
    class GraphicObjectsBase {
    private:
        std::string m_name;
    public:
        /**
         * @brief Монтирование экземпляра класса
         *
         */
        GraphicObjectsBase(std::string name) : m_name(name) {}

        /**
         * @brief Размонтирование экземпляра класса
         *
         */
        virtual ~GraphicObjectsBase() = default;
    
        /**
         * @brief Имя объекта
         * 
         * @return auto 
         */
        auto Name() {
            return m_name;
        }
    };

} // namespace edt
