#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <map>
#include <atomic>
#include <condition_variable>
#include <thread>

#include "CTPL/ctpl.h"

#include "data_queue.hpp"
#include "command.hpp"





#include <iostream>




namespace pdc {

    class ThreadPool {
    private:
        /* data */
    public:
        ThreadPool(/* args */) {}
        ~ThreadPool() {}
    };

    // Aliases
    using Dataname = std::string;

    class DistributorImpl {
    private:
        struct TreadParameters {
            std::thread m_thread;
            std::atomic<bool> m_exit_thread_flag = false;

            std::mutex m_condition_mutex;
            std::condition_variable m_condition;
            std::atomic<bool> m_stopped_flag = false;

            void WaitStop() {
                // Debug
                // std::cout << "Start waiting stop" << std::endl;

                while (!m_stopped_flag.load()) {}

                // Debug
                // std::cout << "End waiting stop" << std::endl;
            }

            void Stop() {
                if (m_stopped_flag.load()) {
                    return;
                }

                // Debug
                // std::cout << "Stopping thread" << std::endl;

                m_stopped_flag.store(true);
                std::unique_lock<std::mutex> lock{ m_condition_mutex };
                m_condition.wait(lock);
                lock.unlock();
            }

            void Resume() {
                if (!m_stopped_flag.load()) {
                    return;
                }

                // Debug
                // std::cout << "Resume thread" << std::endl;

                m_stopped_flag.store(false);
                std::lock_guard<std::mutex> lock{ m_condition_mutex };
                m_condition.notify_all();
            }

            inline auto& IsExit() {
                return m_exit_thread_flag;
            }

            void End() {
                m_exit_thread_flag.store(true);
                if (m_thread.joinable()) {
                    m_thread.join();
                }
            }
        };

    private:
        // Aliases
        using PtrQueueOfData = std::shared_ptr<QueueOfData>;
        using MapOfQueue = std::map<Dataname, PtrQueueOfData>;

        // Data
        MapOfQueue m_data;
        // Mutex of data
        std::mutex m_mtx_change_map;

        // General thread
        TreadParameters m_gen_thread_param;
        // Thread pool
        ctpl::thread_pool m_exc_thread_pool;

    public:
        DistributorImpl() : m_data(), m_mtx_change_map(), m_gen_thread_param(), m_exc_thread_pool(5) {
            std::lock_guard<std::mutex> lock(m_mtx_change_map);

            // Debug
            // std::cout << "Call DistributorImpl constructor" << std::endl;

            // Start thread
            m_gen_thread_param.m_thread = std::thread(&DistributorImpl::DataProcessing, this);
            // Wait
            m_gen_thread_param.WaitStop();
        }

        ~DistributorImpl() {
            // Debug
            // std::cout << "Call DistributorImpl destructor" << std::endl;

            // Stop thread pool
            m_exc_thread_pool.stop();

            // Stop thread
            m_gen_thread_param.End();

            std::lock_guard<std::mutex> lock(m_mtx_change_map);
        }

        void CreateQueue(const Dataname name, const QueueSize queue_size) {
            if (AvailableQueueOfData(name)) {
                // throw std::logic_error("A producer with that name already exists");
                return;
            }
            std::lock_guard<std::mutex> lock(m_mtx_change_map);
            m_data[name] = std::make_shared<QueueOfData>(queue_size);
        }

        void DeleteQueue(const Dataname name) {
            std::lock_guard<std::mutex> lock(m_mtx_change_map);
            m_data[name]->Delete();
        }

        void SetMessage(const Dataname name, const Data data) {
            // Check
            if (!AvailableQueueOfData(name)) {
                // throw std::logic_error("A producer with that name not exists");
                return;
            }

            // Debug
            // std::cout << "Call SetMessage" << std::endl;

            // Append data to queue
            m_data[name]->Put(data);
            // Resume thread
            m_gen_thread_param.Resume();
        }

        void SetObserver(const Dataname name, std::weak_ptr<Observer> ptr_observer) {
            // Wait
            while (true) {
                std::lock_guard<std::mutex> lock(m_mtx_change_map);

                if (AvailableQueueOfData(name)) {
                    break;
                }
                // Debug
                // std::cout << "A producer with that name not exists" << std::endl;
            }

            // Append observer to queue
            m_data[name]->Attach(ptr_observer);
        }

    private:
        bool AvailableQueueOfData(const Dataname name) {
            return m_data.contains(name);
        }

        void DataProcessing() {
            // Debug
            // std::cout << "General thread start" << std::endl;

            while (!m_gen_thread_param.IsExit()) {
                // Stop if there are no commands
                m_gen_thread_param.Stop();
                // Process the command
                while (true) {
                    // Update
                    UpdateMap();

                    // Process
                    std::size_t num_cmd = PollQueues();

                    // Debug
                    // std::cout << "Number generate commands " << num_cmd << std::endl;

                    // Check
                    if (!num_cmd) {
                        break;
                    }
                }
            }
        }

        void UpdateMap() {
            std::lock_guard<std::mutex> lock(m_mtx_change_map);
            for (auto it = m_data.begin(); it != m_data.end();) {
                if (it->second->IsDelete()) {

                    // Debug
                    // std::cout << "Remove topic queue" << std::endl;

                    it = m_data.erase(it);
                }
                else {
                    ++it;
                }
            }
        };

        std::size_t PollQueues() {
            std::lock_guard<std::mutex> lock(m_mtx_change_map);

            std::size_t num_cmd{ 0 };
            for (auto& [queue_name, ptr_queue] : m_data) {
                num_cmd += PollQueue(ptr_queue);

                // Debug
                // std::cout << "Processed " << queue_name << " queue" << std::endl;
            }
            return num_cmd;
        }

        std::size_t PollQueue(PtrQueueOfData& ptr_queue) {
            std::size_t num_cmd{ 0 };
            // Get the list of observers
            auto observers = ptr_queue->GetObservables();

            // Debug
            // std::cout << "Num. obs. " << observers.size() << std::endl;


            for (;;) {
                // Get data
                auto data = ptr_queue->TakeAndPop();

                // Check
                if (!data) {

                    // Debug
                    // std::cout << "Empty data" << std::endl;

                    break;
                }

                // Generate and send commands
                for (auto& observer : observers) {

                    // Debug
                    // std::cout << "Send data to observer " << &observer << std::endl;

                    if (!observer.lock()) {

                        // Debug
                        // std::cout << "Observer is deleted" << std::endl;

                        continue;
                    }

                    // Debug
                    // std::cout << "Send data from queue, to observer: len - " << data.value().size() << " byte, address - " << observer.lock() << std::endl;

                    // Send command to thread pool for execution
                    m_exc_thread_pool.push([cmd = Command(data.value(), observer)](int) mutable {

                        // Debug
                        // std::cout << "Call lambda from thread pool. Thread " << id << std::endl;
                        // if (cmd.Empty()) {
                        //    std::cout << "Command in lambda from thread pool is empty" << std::endl;
                        // }

                        // Execute command
                        cmd.execute();
                    }
                    );

                    num_cmd += 1;
                }

                // Debug
                // std::cout << "All observer notify using thread pool. " << num_cmd << std::endl;

            }
            return num_cmd;
        }
    };

    class Distributor : public DistributorImpl {
    protected:
        Distributor() {
            // Debug
            // std::cout << "Call Distributor constructor" << std::endl;
        }
        ~Distributor() {
            // Debug
            // std::cout << "Call Distributor destructor" << std::endl;
        }

    public:
        Distributor(const Distributor& other) = delete;
        Distributor(Distributor&& other) = delete;
        void operator=(const Distributor&) = delete;
        void operator=(Distributor&&) = delete;

        static Distributor& GetInstance() {
            static Distributor instance;
            return instance;
        }
    };

} // namespace pdc
