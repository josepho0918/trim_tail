#include <execution>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <unordered_set>
#include "TrimTailUtils.h"

using namespace std;
using namespace std::filesystem;

static mutex mut;

static char ToLowerCase(char ch)
{
    return tolower(static_cast<unsigned char>(ch));
}

static void PrintFile(const string_view dir_path, string_view file_path)
{
    file_path.remove_prefix(dir_path.size() + 1);
    lock_guard<mutex> lock(mut);
    cout << file_path << endl;
}

static void ProcessDir(const path& dir_path, const unordered_set<string>& allowed_exts)
{
    vector<path> file_paths;

    cout << "Processing directory: " << dir_path.string() << endl;

    for (const auto& file : recursive_directory_iterator(dir_path, directory_options::skip_permission_denied)) {
        if (file.is_regular_file()) {
            string file_ext = file.path().extension().string();
            ranges::transform(file_ext, file_ext.begin(), ToLowerCase);
            if (allowed_exts.contains(file_ext)) {
                file_paths.push_back(file.path());
            }
        }
    }

    for_each(execution::par, file_paths.cbegin(), file_paths.cend(),
        [&dir_path](const auto& file_path) {
            RemoveTrailingBlanks(file_path);
            PrintFile(dir_path.string(), file_path.string());
        });
}

int main(int argc, char* argv[])
{
    auto start = chrono::high_resolution_clock::now();
    unordered_set<string> allowed_exts;

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            string str = argv[i];
            ranges::transform(str, str.begin(), ToLowerCase);
            allowed_exts.insert(str);
        }
    }
    else {
        allowed_exts = { ".h", ".c", ".hpp", ".cpp" };
    }

    ProcessDir(current_path(), allowed_exts);

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Elapsed time: " << duration.count() << " ms" << endl;

    return 0;
}