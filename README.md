# CppTidy

CppTidy is a utility written in C++ that helps maintain clean and consistent code formatting. It recursively scans a directory for files with specific extensions (.h, .c, .hpp, .cpp by default) and removes trailing white spaces from each line in these files.

## How to Use

1. Build the project.
2. Run the built executable in your terminal. By default, it will scan the current directory and target .h, .c, .hpp, .cpp files.
3. If you want to specify different file extensions, pass them as arguments when running the program. For example, if you want to target .txt and .md files, you would run: `./CppTidy .txt .md`

## Note

Please be aware that this program modifies files in place. It's recommended to back up your files or use a version control system to track changes.
