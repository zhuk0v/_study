#pragma once

#include <memory>
#include <chrono>

#include <iostream>
#include <fstream>

class ObserverCommand {
public:
    virtual void NotifyAboutCommand(const std::vector<Command>& cmds) = 0;
};

class ConsoleObserverCommand : public ObserverCommand {
public:
    void NotifyAboutCommand(const std::vector<Command>& cmds) override {
        std::cout << "bulk: ";
        for (std::size_t i = 0; i < cmds.size() - 1; i++) {
            std::cout << cmds[i] << ',';
        }
        std::cout << cmds.back() << std::endl;
    }
};

class FileObserverCommand : public ObserverCommand {
public:
    void NotifyAboutCommand(const std::vector<Command>& cmds) override {
        std::string filename{ "bulk" };
        auto current_time = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
        filename += std::to_string(current_time);
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


