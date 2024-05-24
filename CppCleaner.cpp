#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
using namespace std::filesystem;

static void RemoveTrailingBlanks(const path& pth)
{
    bool fHasTrailingBlanks = false;
    string strContent;
    string strOldLine;

    ifstream ifsOldFile(pth);
    if (!ifsOldFile.is_open()) {
        return;
    }
    while (getline(ifsOldFile, strOldLine)) {
        string_view sv(strOldLine);
        if (!sv.empty() && (sv.back() == ' ' || sv.back() == '\t')) {
            fHasTrailingBlanks = true;
            sv.remove_suffix(sv.size() - sv.find_last_not_of(" \t") - 1);
        }
        strContent += sv;
        strContent += '\n';
    }
    ifsOldFile.close();

    if (fHasTrailingBlanks) {
        path pthNew(pth.string() + ".tmp");
        ofstream ofsNewFile(pthNew);
        if (!ofsNewFile.is_open()) {
            return;
        }
        ofsNewFile << strContent;
        ofsNewFile.close();
        remove(pth);
        rename(pthNew, pth);
    }
}

static void FindRecursive(const path& pth, const vector<string>& exts)
{
    for (auto& file : directory_iterator(pth)) {
        if (is_directory(file)) {
            FindRecursive(file, exts);
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

    FindRecursive(current_path(), exts);

    return 0;
}