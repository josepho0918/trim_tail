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

static char ToLowerCase(char ch)
{
	return tolower(static_cast<unsigned char>(ch));
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
            auto it = find_if_not(line.crbegin(), line.crend(), IsWhiteSpace);
            line.erase(it.base(), line.cend());
			temp_file << line;
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
    cout << "Processing directory: " << dir_path.string() << endl;

    for (const auto& file : recursive_directory_iterator(dir_path, directory_options::skip_permission_denied)) {
        if (file.is_regular_file()) {
            const path& file_path = file.path();
            string file_ext = file_path.extension().string();
            transform(file_ext.cbegin(), file_ext.cend(), file_ext.begin(), ToLowerCase);
            if (allowed_exts.contains(file_ext)) {
                RemoveTrailingBlanks(file_path);
                cout << file_path.string().substr(dir_path.string().size()) << endl;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    unordered_set<string> allowed_exts;

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            string str = argv[i];
            transform(str.cbegin(), str.cend(), str.begin(), ToLowerCase);
            allowed_exts.insert(str);
        }
    }
    else {
        allowed_exts = { ".h", ".c", ".hpp", ".cpp" };
    }

    ProcessDir(current_path(), allowed_exts);

    return 0;
}