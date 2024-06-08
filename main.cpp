#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>

#include <boost/program_options.hpp>

#include "dir_finder.hpp"

#define UNUSE(x) (void)(x)

auto command_line_description() {
	boost::program_options::options_description description{ "Options" };
	description.add_options()
		("scan-dir,D", boost::program_options::value<std::vector<std::string>>(), "Directories to scan ()")
		("excl-dir,E", boost::program_options::value<std::vector<std::string>>(), "Directories to exclude")
		("level,L", boost::program_options::value<int>(), "Scan level(1-All, 0-Only the specified)")
		("min-file-size,F", boost::program_options::value<int>()->default_value(1), "Minimum file size in bytes")
		("mask,M", boost::program_options::value<std::vector<std::string>>(), "Masks of file names allowed for comparison")
		("size-block,S", boost::program_options::value<int>(), "The size of the block used to read files in bytes")
		("hash-func,H", boost::program_options::value<std::string>(), "Hashing algorithms");

	return description;
}

void parse_command_line(boost::program_options::variables_map& vm, int argc, char const* argv []) {
	try {
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, command_line_description()), vm);
		boost::program_options::notify(vm);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}

int main(int argc, char const* argv []) {

	boost::program_options::variables_map vm{};
	parse_command_line(vm, argc, argv);

	df::DirFinderDuplicate finder_duplicates{
		vm["scan-dir"].as<std::vector<std::string>>(),
		vm["excl-dir"].as<std::vector<std::string>>(),
		vm["level"].as<int>(),
		vm["min-file-size"].as<int>(),
		vm["mask"].as<std::vector<std::string>>(),
		vm["size-block"].as<int>(),
		vm["hash-func"].as<std::string>()
	};

	finder_duplicates.Run();

	return EXIT_SUCCESS;
}
