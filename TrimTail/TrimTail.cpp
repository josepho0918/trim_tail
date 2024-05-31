#include <execution>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include "TrimTail.h"

using namespace std;
using namespace std::filesystem;

static mutex mut;

static bool IsWhiteSpace(char ch)
{
    return isspace(static_cast<unsigned char>(ch));
}

static char ToLowerCase(char ch)
{
    return tolower(static_cast<unsigned char>(ch));
}

bool HasTrailingBlanks(const path& file_path)
{
    bool result = false;

    if (ifstream file(file_path); file.is_open()) {
        string line;
        while (getline(file, line)) {
            if (!line.empty() && IsWhiteSpace(line.back())) {
                result = true;
                break;
            }
        }
        file.close();
    }

    return result;
}

optional<string> GetCleanLine(ifstream& file)
{
    if (string line; getline(file, line)) {
        line.erase(find_if_not(line.crbegin(), line.crend(), IsWhiteSpace).base(), line.cend());
        return line;
    }

    return nullopt;
}

void RemoveTrailingBlanks(const path& file_path)
{
    if (!HasTrailingBlanks(file_path)) return;
    
    if (ifstream orig_file(file_path); orig_file.is_open()) {
        char temp_path[L_tmpnam_s];
        tmpnam_s(temp_path);
        ofstream temp_file(temp_path);

        if (!temp_file.is_open()) {
            return;
        }

        while (auto line = GetCleanLine(orig_file)) {
            temp_file << *line;
            if (!orig_file.eof()) {
                temp_file << '\n';
            }
        }

        orig_file.close();
        temp_file.close();
        rename(temp_path, file_path);
    }
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