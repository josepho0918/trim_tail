// TrimTailUtils.cpp : 定義靜態程式庫的函式。
//

#include "pch.h"
#include "framework.h"
#include <filesystem>
#include <fstream>
#include <ranges>
#include "TrimTailUtils.h"

using namespace std;
using namespace std::filesystem;

static bool IsWhiteSpace(char ch)
{
    return isspace(static_cast<unsigned char>(ch));
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
        line.erase(ranges::find_if_not(line | views::reverse, IsWhiteSpace).base(), line.cend());
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