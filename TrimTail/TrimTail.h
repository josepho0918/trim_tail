#pragma once

extern bool HasTrailingBlanks(std::ifstream& file);

extern std::optional<std::string> GetCleanLine(std::ifstream& file);

extern void RemoveTrailingBlanks(const std::filesystem::path& file_path);