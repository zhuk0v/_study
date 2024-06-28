#pragma once

#include <memory>
#include <vector>

#include "command.hpp"
#include "command_obervers.hpp"

class PublisherCommands {
protected:
    std::vector<std::weak_ptr<ObserverCommand>> m_vect_cmd_observers;
public:
    void Subscribe(std::weak_ptr<ObserverCommand> ptr_cmd_observer) {
        m_vect_cmd_observers.push_back(ptr_cmd_observer);
    }

    void AppendCommands(std::vector<Command>& cmds) {
        for (auto& ptr_observer : m_vect_cmd_observers) {
            auto use_ptr = ptr_observer.lock();
            if (use_ptr) {
                use_ptr->AppendCommands(cmds);
            }
        }
        cmds.clear();
    }

    void NotifySubscribers() {
        for (auto& ptr_observer : m_vect_cmd_observers) {
            auto use_ptr = ptr_observer.lock();
            if (use_ptr) {
                use_ptr->NotifyAboutCommand();
            }
        }
    }
};

class CmdProc : public PublisherCommands {
private:
    std::size_t n_inp_cmd;
    std::vector<Command> commands;
public:
    CmdProc(std::size_t n) : n_inp_cmd(n) {}
    ~CmdProc() = default;

    void poll(std::istream& in) {
        Command cmd{};

        for (auto i = 0; ; i++) {

            if (commands.size() == n_inp_cmd) {
                AppendCommands(commands);
            }

            if (in.eof()) {
                break;
            }

            in >> cmd;

            if (cmd.IsOpenBlock()) {
                if (commands.size() != 0) {
                    AppendCommands(commands);
                }
                if (poll_dyn_block(in, cmd)) {
                    AppendCommands(commands);
                    continue;
                }
                break;
            }

            if (cmd.IsEnd()) {
                AppendCommands(commands);
                break;
            }

            commands.push_back(cmd);
        }

        NotifySubscribers();
    }

private:

    bool poll_dyn_block(std::istream& in, Command& cmd) {
        for (auto i = 0; ; i++) {
            in >> cmd;

            if (cmd.IsOpenBlock()) {
                if (poll_dyn_block(in, cmd)) {
                    continue;
                }
                return false;
            }

            if (cmd.IsEndBlock()) {
                return true;
            }

            if (cmd.IsEnd()) {
                return false;
            }

            commands.push_back(cmd);
        }
    }
};
