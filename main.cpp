#include <cstdlib>

#include "editor.hpp"

#define UNUSE(x) (void)(x)



int main(int argc, char const* argv []) {
	UNUSE(argc);
	UNUSE(argv);

	// Create Editor
	edt::Editor editor{};

	// Create file
	editor.CreateFile("file_1");

	// Import from file
	editor.ImportFile("file_2", "/tmp/file.drunk");
	
	// Import to file
	editor.PrintFile("file_1", "/tmp/file.drunk");

	// Create circle on the file_1
	edt::file_cmd::CreateGraphicsObjectCommand create_cmd(editor.GetFile("file_1"), "circle", "sun");
	create_cmd.execute();

	// Remove circle on the file_1
	edt::file_cmd::RemoveGraphicsObjectCommand remove_cmd(editor.GetFile("file_1"), "circle", "sun");
	remove_cmd.execute();

	return EXIT_SUCCESS;
}
