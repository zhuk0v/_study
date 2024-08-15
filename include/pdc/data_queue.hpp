#pragma once

#include <optional>

#include <boost/circular_buffer.hpp>

#include "subject.hpp"




#include <iostream>





namespace pdc {
    using QueueSize = std::size_t;
    using Data = std::vector<std::uint8_t>;

    class QueueOfData : public Subject {
    private:
        std::mutex m_mtx_data;
        boost::circular_buffer<Data> m_storage;

        std::atomic<bool> m_del_flag;

    public:
        QueueOfData(const QueueSize size) : m_mtx_data(), m_storage(size), m_del_flag(false) {
            // Debug
            // std::cout << "Call QueueOfData constructor" << std::endl;
        }

        ~QueueOfData() {
            // Debug
            // std::cout << "Call QueueOfData destructor" << std::endl;

            std::lock_guard<std::mutex> lock(m_mtx_data);
        }

        void Put(const Data data) {
            std::lock_guard<std::mutex> lock(m_mtx_data);

            m_storage.push_back(data);

            // Debug
            // std::cout << "Put data to queue. New size " << m_storage.size() << std::endl;
        }

        std::optional<Data> TakeAndPop() {
            std::lock_guard<std::mutex> lock(m_mtx_data);
            if (Contains()) {
                auto element = m_storage.front();

                m_storage.pop_front();

                // Debug
                // std::cout << "Take data from queue. New size " << m_storage.size() << std::endl;

                return element;
            }
            return std::nullopt;
        }

        void Delete() {
            m_del_flag.store(true);
        }

        inline auto& IsDelete() {
            return m_del_flag;
        }

    private:

        bool Contains() {
            return m_storage.size() > 0;
        }
    };


} // namespace pdc
