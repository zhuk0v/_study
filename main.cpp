#include <cassert>
#include <iostream>

#include <string>

// #include "command_processor.hpp"

#include "async.h"

#define UNUSE(x) (void)(x)

int main(int argc, char const* argv []) {
	UNUSE(argc);
	UNUSE(argv);

	// const auto num_cmd{  };
	auto context = connect(static_cast<std::size_t>(std::stoll(std::string(argv[1]))));

	std::string command_list{};
	for (int i = 0; i < 99; ++i) {
		command_list += std::to_string(i);
		command_list += " ";
	}

	receive(command_list.data(), command_list.size(), context);
	disconnect(context);

	if (context){
		std::cout << "Context is valid" << std::endl;	
	}
	else{
		std::cout << "Context is not valid" << std::endl;	
	}

	return EXIT_SUCCESS;
}
