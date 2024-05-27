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

    file.clear();
    file.seekg(pos);

	return result;
}

static void RemoveTrailingBlanks(const path& file_path)
{
    ifstream orig_file(file_path);

    if (!orig_file.is_open()) {
        return;
    }

    if (HasTrailingBlanks(orig_file)) {
        const path temp_path(file_path.string() + ".tmp");
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
		remove(file_path);
		rename(temp_path, file_path);
	}
	else {
		orig_file.close();
    }
}

static void ProcessDir(const path& dir_path, const unordered_set<string>& allowed_exts)
{
    for (const auto& file : recursive_directory_iterator(dir_path, directory_options::skip_permission_denied)) {
        if (file.is_regular_file()) {
            const path& file_path = file.path();
            const string file_ext = file_path.extension().string();
            if (find_if(allowed_exts.cbegin(), allowed_exts.cend(),
                [&file_ext](const string& ext) {
                    return _stricmp(ext.c_str(), file_ext.c_str()) == 0;
                }) != allowed_exts.cend())
            {
                RemoveTrailingBlanks(file_path);
                cout << file_path.filename() << endl;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    unordered_set<string> allowed_exts;

    if (argc > 1) {
        allowed_exts.insert(argv + 1, argv + argc);
    }
    else {
        allowed_exts = { ".h", ".c", ".hpp", ".cpp" };
    }

    ProcessDir(current_path(), allowed_exts);

    return 0;
}