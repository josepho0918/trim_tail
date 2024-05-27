#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>

using namespace std;
using namespace std::filesystem;

static bool IsWhiteSpace(char ch)
{
    return isspace(static_cast<unsigned char>(ch));
}

static bool HasTrailingBlanks(ifstream& file)
{
    streampos pos = file.tellg();
    bool result = false;
    string line;

    file.seekg(0);

    while (getline(file, line)) {
		if (!line.empty() && IsWhiteSpace(line.back())) {
            result = true;
            break;
		}
	}

    file.seekg(pos);

	return result;
}

static void RemoveTrailingBlanks(const path& pth)
{
    ifstream orig_file(pth);

    if (!orig_file.is_open()) {
        return;
    }

    if (HasTrailingBlanks(orig_file)) {
        const path temp_path(pth.string() + ".tmp");
        ofstream temp_file(temp_path);
        string line;

        if (!temp_file.is_open()) {
			return;
		}

		while (getline(orig_file, line)) {
			string_view sv(line);
			if (!sv.empty() && IsWhiteSpace(sv.back())) {
				auto it = find_if_not(sv.rbegin(), sv.rend(), IsWhiteSpace);
				sv.remove_suffix(distance(sv.rbegin(), it));
			}
			temp_file << sv;
			if (!orig_file.eof()) {
                temp_file << '\n';
			}
		}

		orig_file.close();
		temp_file.close();
		remove(pth);
		rename(temp_path, pth);
	}
	else {
		orig_file.close();
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