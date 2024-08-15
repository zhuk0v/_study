#pragma once

#include <memory>
#include <functional>

#include "distributor.hpp"
#include "observer.hpp"

namespace pdc {

    template <typename T>
    class ConsumerBase {
    protected:
        using FuncT = std::function<void(T)>;
    public:
        virtual void Subscribe(const Dataname name, FuncT f) = 0;
    };

    template <typename T>
    class ConsumerObserver : public ConsumerBase<T>, public Observer {};

    template <typename T>
    class ConsumerImpl :
        public std::enable_shared_from_this<ConsumerImpl<T>>,
        public ConsumerObserver<T> {
    private:
        using FuncT = ConsumerBase<T>::FuncT;

        // Callback function
        FuncT m_callback;
    public:
        ConsumerImpl() : m_callback() {
            // Debug
            // std::cout << "Call ConsumerImpl constructor" << std::endl;
        }

        ~ConsumerImpl() {
            // Debug
            // std::cout << "Call ConsumerImpl destructor" << std::endl;
        }

        void Subscribe(const Dataname name, FuncT f) override {
            // Debug
            // std::cout << "Subscribe to " << name << " topic. " << "Address callback function " << &f << std::endl;
            if (!f) {
                // std::cout << "Callback function empty. " << std::endl;
            }

            // Save callback function
            m_callback = f;

            // Add observer
            Distributor::GetInstance().SetObserver(name, this->weak_from_this());
        }

        void Update(std::vector<std::uint8_t> data) override {

            // Debug
            // std::cout << "Call Update of Observer" << std::endl;

            if (m_callback) {
                m_callback(*reinterpret_cast<T*>(data.data()));
            }
        }
    };

    // Proxy
    template <typename T>
    class Consumer :
        public ConsumerBase<T> {
    private:
        using FuncT = ConsumerBase<T>::FuncT;

        std::shared_ptr<ConsumerImpl<T>> m_impl;
    public:
        Consumer() : m_impl(nullptr) {
            // Debug
            // std::cout << "Call Consumer constructor" << std::endl;

            m_impl = std::make_shared<ConsumerImpl<T>>();
        }
        ~Consumer() {
            // Debug
            // std::cout << "Call Consumer destructor" << std::endl;
        }

        void Subscribe(const Dataname name, FuncT f) {
            m_impl->Subscribe(name, f);
        }
    };


} // namespace pdc
