#include <boost/filesystem.hpp>
#include <execution>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <unordered_set>
#include "TrimTail.h"

using namespace std;
namespace fs = std::filesystem;

struct StringHash {
    using is_transparent = void;
    size_t operator()(string_view sv) const {
        hash<string_view> hasher;
        return hasher(sv);
    }
};

using StringSet = unordered_set<string, StringHash, equal_to<>>;

static mutex mut;

static bool IsWhiteSpace(char ch)
{
    return isspace(static_cast<unsigned char>(ch));
}

static char ToLowerCase(char ch)
{
    return static_cast<char>(tolower(static_cast<unsigned char>(ch)));
}

bool HasTrailingBlanks(const fs::path& file_path)
{
    if (ifstream file(file_path); file.is_open()) {
        for (string line; getline(file, line);) {
            if (!line.empty() && IsWhiteSpace(line.back())) {
                return true;
            }
        }
    }

    return false;
}

static optional<string> GetCleanLine(fstream& file)
{
    if (string line; getline(file, line)) {
        line.erase(ranges::find_if_not(line | views::reverse, IsWhiteSpace).base(), line.cend());
        return line;
    }

    return nullopt;
}

void RemoveTrailingBlanks(const fs::path& file_path)
{
    if (!HasTrailingBlanks(file_path)) return;

    auto temp_path = fs::temp_directory_path() / boost::filesystem::unique_path().string();

    if (auto [orig_file, temp_file] = make_pair<fstream, fstream>(
            fstream(file_path, ios::in),
            fstream(temp_path, ios::out | ios::trunc)
        );
        orig_file.is_open() && temp_file.is_open())
    {
        while (auto line = GetCleanLine(orig_file)) {
            temp_file << *line;
            if (!orig_file.eof()) {
                temp_file << '\n';
            }
        }
    }
    else return;

    rename(temp_path, file_path);
}

static void PrintFile(const fs::path& dir_path, const fs::path& file_path)
{
    scoped_lock lock(mut);
    cout << fs::relative(file_path, dir_path).string() << endl;
}

static void ProcessDir(const fs::path& dir_path, const StringSet& allowed_exts)
{
    vector<fs::path> file_paths;

    cout << "Processing directory: " << dir_path.string() << endl;

    for (const auto& file : fs::recursive_directory_iterator(dir_path, fs::directory_options::skip_permission_denied)) {
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
            PrintFile(dir_path, file_path);
        });
}

int main(int argc, char* argv[])
{
    auto start = chrono::high_resolution_clock::now();
    StringSet allowed_exts;

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

    ProcessDir(fs::current_path(), allowed_exts);

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Elapsed time: " << duration.count() << " ms" << endl;

    return 0;
}