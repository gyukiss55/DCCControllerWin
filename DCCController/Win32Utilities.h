// Win32Utilities.h

#pragma once

#include <windows.h>
#include <tchar.h>
#include <vector>
#include <string>

void EnumerateFilesInDirectory(const TCHAR* directoryPath, std::vector<std::wstring>& result);

std::wstring GetCurrentPath();

void ReadTextFile(const TCHAR* filePath, std::string& result);

std::vector<std::string> SplitString(const std::string& str, char delimiter);

std::wstring ConvertUTF8ToUnicode(const std::string& utf8Str);

std::string WStringToString(const std::wstring& w);

std::string FormatString(const char* format, ...);
