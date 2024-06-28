#pragma once

#include <memory>
#include <chrono>
#include <queue>
#include <condition_variable>
#include <iostream>
#include <fstream>
#include <thread>
#include <functional>

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/wait_traits.hpp>

class ObserverCommand {
protected:
    std::queue<std::vector<Command>> m_cmd_queue;
    std::mutex m_mutex_for_queue_read;

    std::mutex m_mutex;
    std::condition_variable m_condition;
public:
    void AppendCommands(const std::vector<Command>& cmds) {
        std::lock_guard<std::mutex> l(m_mutex_for_queue_read);
        m_cmd_queue.push(cmds);
    }

    void NotifyAboutCommand() {
        std::lock_guard<std::mutex> l(m_mutex);
        m_condition.notify_all();
    }

protected:
    virtual void OutputCommandsImpl(const std::vector<Command> cmds) = 0;

    void GetNextCommands(std::vector<Command>& cmds) {
        std::lock_guard<std::mutex> lq(m_mutex_for_queue_read);

        if (!m_cmd_queue.empty()) {
            cmds = m_cmd_queue.front();
            m_cmd_queue.pop();
        }
        else {
            cmds.resize(0);
        }
    }
};

struct TreadParameters {
    std::thread m_thread;
    std::atomic<bool> m_stopped = false;
};



class ConsoleObserverCommand : public ObserverCommand {
private:
    TreadParameters m_asynchronous_prms;
public:
    ConsoleObserverCommand() {
        m_asynchronous_prms.m_thread = std::thread(
            &ConsoleObserverCommand::OutputQueue,
            this
        );
    }

    ~ConsoleObserverCommand() {
        m_asynchronous_prms.m_stopped = true;
        m_condition.notify_all();
        if (m_asynchronous_prms.m_thread.joinable()) {
            m_asynchronous_prms.m_thread.join();
        }
    }

private:

    void OutputQueue() {
        while (!m_asynchronous_prms.m_stopped) {
            std::unique_lock<std::mutex> l{ m_mutex };
            m_condition.wait(l);

            {
                std::lock_guard<std::mutex> lq(m_mutex_for_queue_read);
                if (m_asynchronous_prms.m_stopped && m_cmd_queue.empty()) {
                    break;
                }
            }

            // while (!m_cmd_queue.empty()) {
            //     OutputCommandsImpl(m_cmd_queue.front());
            //     m_cmd_queue.pop();
            // }

            std::vector<Command> cmds{};
            while (true) {
                GetNextCommands(cmds);

                if (cmds.size() == 0) {
                    break;
                }

                OutputCommandsImpl(cmds);
            }

            l.unlock();
        }
    }

    void OutputCommandsImpl(const std::vector<Command> cmds) override {
        std::cout << "bulk: ";
        for (std::size_t i = 0; i < cmds.size() - 1; i++) {
            std::cout << cmds[i] << ',';
        }
        std::cout << cmds.back() << std::endl;
    }
};



class FileObserverCommand : public ObserverCommand {
private:
    static inline std::atomic<std::size_t> append_unique_id = 0;

    std::array<TreadParameters, 2> m_tread_prms_pool;
public:
    FileObserverCommand() {
        for (auto& thp : m_tread_prms_pool) {
            thp.m_thread = std::thread(
                [&, this]() {
                FileObserverCommand::OutputQueue(std::ref(thp.m_stopped));
            }
            );
        }
    }
    ~FileObserverCommand() {
        for (auto& thp : m_tread_prms_pool) {
            thp.m_stopped = true;
        }
        m_condition.notify_all();
        for (auto& thp : m_tread_prms_pool) {
            if (thp.m_thread.joinable()) {
                thp.m_thread.join();
            }
        }

    }

private:

    void OutputQueue(std::atomic<bool>& stop_flag) {
        while (!stop_flag) {
            std::unique_lock<std::mutex> l{ m_mutex };

            m_condition.wait(l);
            l.unlock();

            {
                std::lock_guard<std::mutex> lq(m_mutex_for_queue_read);
                if (stop_flag && m_cmd_queue.empty()) {
                    break;
                }
            }

            std::vector<Command> cmds{};
            while (true) {
                GetNextCommands(cmds);

                if (cmds.size() == 0) {
                    break;
                }

                OutputCommandsImpl(cmds);
            }
        }

    }

    void OutputCommandsImpl(const std::vector<Command> cmds) {
        std::string filename{ "bulk" };

        auto current_time = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
        filename += std::to_string(current_time);
        filename += "_";

        auto tread_id = std::this_thread::get_id();
        filename += std::to_string(std::hash<std::thread::id>{}(tread_id));
        filename += "_";

        filename += std::to_string(append_unique_id++);

        filename += ".log";
        std::ofstream fid{ filename };

        fid << "bulk: ";
        for (std::size_t i = 0; i < cmds.size() - 1; i++) {
            fid << cmds[i] << ',';
        }
        fid << cmds.back() << std::endl;

        fid.close();
    }
};


