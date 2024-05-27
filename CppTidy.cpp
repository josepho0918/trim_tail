#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>

using namespace std;
using namespace std::filesystem;

static bool IsWhiteSpace(char ch) {
    return isspace(static_cast<unsigned char>(ch));
}

static void RemoveTrailingBlanks(const path& pth)
{
    bool has_blanks = false;
    ostringstream content;
    string line;

    ifstream orig_file(pth);
    if (!orig_file.is_open()) {
        return;
    }
    while (getline(orig_file, line)) {
        string_view sv(line);
        if (!sv.empty() && IsWhiteSpace(sv.back())) {
            has_blanks = true;
            auto it = find_if_not(sv.rbegin(), sv.rend(), IsWhiteSpace);
            sv.remove_suffix(distance(sv.rbegin(), it));
        }
        content << sv;
        if (!orig_file.eof()) {
            content << '\n';
        }
    }
    orig_file.close();

    if (has_blanks) {
        const path temp_path(pth.string() + ".tmp");
        ofstream temp_file(temp_path);
        if (!temp_file.is_open()) {
            return;
        }
        temp_file << content.str();
        temp_file.close();
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