#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
using namespace std::filesystem;

static void RemoveTrailingBlanks(const path& pth)
{
    bool has_blanks = false;
    string content;
    string line;

    ifstream orig_file(pth);
    if (!orig_file.is_open()) {
        return;
    }
    while (getline(orig_file, line)) {
        string_view sv(line);
        if (!sv.empty() && isspace(sv.back())) {
            has_blanks = true;
            auto it = find_if_not(sv.rbegin(), sv.rend(), [](char c) { return isspace(c); });
            sv.remove_suffix(it - sv.rbegin());
        }
        content += sv;
        content += '\n';
    }
    orig_file.close();

    if (has_blanks) {
        path temp_path(pth.string() + ".tmp");
        ofstream new_file(temp_path);
        if (!new_file.is_open()) {
            return;
        }
        new_file << content;
        new_file.close();
        remove(pth);
        rename(temp_path, pth);
    }
}

static void ProcessDir(const path& pth, const vector<string>& exts)
{
    for (auto& file : directory_iterator(pth)) {
        if (is_directory(file)) {
            ProcessDir(file, exts);
        }
        else {
            path filePath = file.path();
            auto fileExt = filePath.extension();
            if (auto it = find(exts.cbegin(), exts.cend(), fileExt); it != exts.cend()) {
                RemoveTrailingBlanks(filePath);
                cout << filePath.filename() << endl;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    vector<string> exts;

    if (argc > 1) {
        exts.assign(argv + 1, argv + argc);
    }
    else {
        exts = { ".h", ".c", ".hpp", ".cpp" };
    }

    ProcessDir(current_path(), exts);

    return 0;
}