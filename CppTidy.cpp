#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>

using namespace std;
using namespace std::filesystem;

static bool IsWhiteSpace(char ch) {
    return (ch >= 0 && ch <= 127) && isspace(ch);
}

static void RemoveTrailingBlanks(const path& pth)
{
    bool has_blanks = false;
    ostringstream content;
    string line;
    fstream file;

    file.open(pth, ios::in);
    if (!file.is_open()) {
        return;
    }
    while (getline(file, line)) {
        string_view sv(line);
        if (!sv.empty() && (unsigned char)sv.back() <= 127 && IsWhiteSpace(sv.back())) {
            has_blanks = true;
            auto it = find_if_not(sv.rbegin(), sv.rend(), IsWhiteSpace);
            sv.remove_suffix(it - sv.rbegin());
        }
        content << sv;
        if (!file.eof()) {
            content << '\n';
        }
    }
    file.close();

    if (has_blanks) {
        const path temp_path(pth.string() + ".tmp");
        file.open(temp_path, ios::out | ios::trunc);
        if (!file.is_open()) {
            return;
        }
        file << content.str();
        file.close();
        remove(pth);
        rename(temp_path, pth);
    }
}

static void ProcessDir(const path& pth, const unordered_set<string>& exts)
{
    for (auto& file : recursive_directory_iterator(pth, directory_options::skip_permission_denied)) {
        if (file.is_regular_file()) {
            const path& file_path = file.path();
            const path& file_ext = file_path.extension();
            if (exts.find(file_ext.string()) != exts.cend()) {
                RemoveTrailingBlanks(file_path);
                cout << file_path.filename() << endl;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    unordered_set<string> exts;

    if (argc > 1) {
        exts.insert(argv + 1, argv + argc);
    }
    else {
        exts = { ".h", ".c", ".hpp", ".cpp" };
    }

    ProcessDir(current_path(), exts);

    return 0;
}