#pragma once

#include <memory>
#include <vector>

#include "observer.hpp"


#include <iostream>



namespace pdc {

    class Command {
    private:

        std::vector<std::uint8_t> m_data;
        std::weak_ptr<Observer> m_observer;

    public:
        Command(std::vector<std::uint8_t>& data, std::weak_ptr<Observer> observer) :
            m_data(data), m_observer(observer) {
            // Debug
            // std::cout << "Call Command constructor" << std::endl;
        }
        ~Command() {
            // Debug
            // std::cout << "Call Command destructor (" << m_data.size() << ", " << m_observer.lock() << ")" << std::endl;
        }

        bool Empty(){
            return (m_data.empty()) || (m_observer.lock() == nullptr);
        }

        void execute() {

            // Debug
            // std::cout << "Execute of command " << std::endl;

            if (auto prt_observer = m_observer.lock()) {
                prt_observer->Update(m_data);
            }

        }
    };

} // namespace pdc
