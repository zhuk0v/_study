#include "editor.hpp"

#include "import_from_drunk_format.hpp"
#include "print_to_drunk_format.hpp"

// ----------------------------------------------------------------
void edt::Editor::ImportFile(std::string filename, std::string input_path) {
    m_files.push_back(std::make_shared<File>(filename));

    std::shared_ptr<ImportDrunk> importer = std::make_shared<ImportDrunk>(input_path);

    m_files.back()->Accept(importer);
}

// ----------------------------------------------------------------
void edt::Editor::PrintFile(std::string filename, std::string output_path) {
    auto file = GetFile(filename);

    std::shared_ptr<PrintDrunk> printer = std::make_shared<PrintDrunk>(output_path);

    file->Accept(printer);
}