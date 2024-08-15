#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <algorithm>

#include "observer.hpp"



#include <iostream>



class Subject {
protected:
    std::mutex m_mtx_change_observers_vector;
    std::vector<std::weak_ptr<Observer>> m_list_observers;

public:
    ~Subject() {
        // Debug
        // std::cout << "Call Subject destructor" << std::endl;

        std::lock_guard<std::mutex> lock(m_mtx_change_observers_vector);
    };

    void Attach(std::weak_ptr<Observer> ptr_observer) {
        std::lock_guard<std::mutex> lock(m_mtx_change_observers_vector);

        // Debug
        // std::cout << "Attach observer to queue" << (ptr_observer.lock() == nullptr) << std::endl;

        m_list_observers.push_back(ptr_observer);
    }

    std::vector<std::weak_ptr<Observer>> GetObservables() {
        std::lock_guard<std::mutex> lock(m_mtx_change_observers_vector);
        UpdateObservables();
        return m_list_observers;
    }

private:

    void UpdateObservables() {
        m_list_observers.erase(
            std::remove_if(m_list_observers.begin(), m_list_observers.end(), [](auto& o) { return o.lock() == nullptr; }),
            m_list_observers.end()
        );
    }
};