#pragma once

extern bool HasTrailingBlanks(const std::filesystem::path& file_path);

extern std::optional<std::string> GetCleanLine(std::ifstream& file);

extern void RemoveTrailingBlanks(const std::filesystem::path& file_path);