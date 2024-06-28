#include "async.h"

#include <sstream>

#include "command_processor.hpp"

class Context {
public:
    std::shared_ptr<CmdProc> m_proc;
private:
    std::shared_ptr<ConsoleObserverCommand> ptr_console_printer;
    std::shared_ptr<FileObserverCommand> ptr_file_printer;
public:
    Context(std::size_t num_cmd) {
        m_proc = std::make_shared<CmdProc>(num_cmd);
        ptr_console_printer = std::make_shared<ConsoleObserverCommand>();
        ptr_file_printer = std::make_shared<FileObserverCommand>();

        m_proc->Subscribe(ptr_console_printer);
        m_proc->Subscribe(ptr_file_printer);
    }

    Context(Context&&) = default;
    Context& operator= (Context&&) = default;

    Context(const Context&) = delete;
    Context& operator= (const Context&) = delete;

    ~Context() = default;
};

std::shared_ptr<Context> connect(std::size_t num_cmd) {
    return std::make_shared<Context>(num_cmd);
}

void receive(const char* str, std::size_t len, std::shared_ptr<Context>& context) {
    std::stringstream ss({ str, len });
    context->m_proc->poll(ss);
}

void disconnect(std::shared_ptr<Context>& context) {
    context.reset();
}