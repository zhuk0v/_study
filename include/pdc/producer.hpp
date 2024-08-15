#pragma once

#include <concepts>
#include <type_traits>

#include "distributor.hpp"

namespace pdc {

    template <typename T>
    concept ProducerSupportMessageType =
        std::is_copy_constructible_v<T>;

    class Producer {
    private:
        const Dataname m_data_name;

    public:
        Producer(const Dataname name, const QueueSize queue_size) : m_data_name(name) {
            // Debug
            // std::cout << "Call Producer constructor" << std::endl;

            Distributor::GetInstance().CreateQueue(name, queue_size);
        }

        ~Producer() {
            // Debug
            // std::cout << "Call Producer destructor" << std::endl;

            Distributor::GetInstance().DeleteQueue(m_data_name);
        }

        template<ProducerSupportMessageType T>
        void Publish(T& value) {
            std::uint8_t* data = reinterpret_cast<std::uint8_t*>(&value);
            std::size_t data_size_in_byte = sizeof(value);
            Distributor::GetInstance().SetMessage(m_data_name, std::vector<std::uint8_t>(data, data + data_size_in_byte));
        }
    };

} // namespace pdc
