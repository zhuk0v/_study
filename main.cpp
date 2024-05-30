#include <cassert>
#include <iostream>

#include <string>

#include "command_processor.hpp"

#define UNUSE(x) (void)(x)

int main(int argc, char const* argv []) {
	UNUSE(argc);
	UNUSE(argv);

	std::shared_ptr<ConsoleObserverCommand> ptr_console_printer = std::make_shared<ConsoleObserverCommand>();
	std::shared_ptr<FileObserverCommand> ptr_file_printer = std::make_shared<FileObserverCommand>();

	CmdProc cmd_proc{ static_cast<std::size_t>(std::stoll(std::string(argv[1]))) };

	cmd_proc.Subscribe(ptr_console_printer);
	cmd_proc.Subscribe(ptr_file_printer);

	cmd_proc.poll(std::cin);

	return EXIT_SUCCESS;
}
