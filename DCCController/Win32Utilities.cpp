//Win32Utilities.cpp

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string>
#include <string_view>
#include <vector>
#include <iostream>


#include "Win32Utilities.h"

void TestCPP17()
{
    std::string_view sv = "C++17 OK";
    std::cout << sv << '\n';
}

void EnumerateFilesInDirectory(const TCHAR* directoryPath, std::vector<std::wstring>& result) {
    result.clear();
    
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    // Create a search pattern
    TCHAR searchPattern[MAX_PATH];
    _stprintf_s(searchPattern, MAX_PATH, _T("%s\\*"), directoryPath);

    // Find the first file in the directory
    hFind = FindFirstFile(searchPattern, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        _tprintf(_T("FindFirstFile failed (%d)\n"), GetLastError());
        return;
    }

    do {
        // Skip "." and ".." directories
        if (_tcscmp(findFileData.cFileName, _T(".")) != 0 && _tcscmp(findFileData.cFileName, _T("..")) != 0) {
            // Check if it's a directory or a file
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                _tprintf(_T("Directory: %s\n"), findFileData.cFileName);
            }
            else {
                _tprintf(_T("File: %s\n"), findFileData.cFileName);
                result.push_back(std::wstring (findFileData.cFileName));
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    DWORD dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) {
        _tprintf(_T("FindNextFile failed (%d)\n"), dwError);
    }

    FindClose(hFind);
}

std::wstring GetCurrentPath()
{
    std::wstring ret = L"";
    TCHAR buffer[MAX_PATH];

    DWORD dwRet = GetCurrentDirectory(MAX_PATH, buffer);

    if (dwRet == 0) {
        _tprintf(_T("GetCurrentDirectory failed (%d)\n"), GetLastError());
    }
    else if (dwRet > MAX_PATH) {
        _tprintf(_T("Buffer too small; need %d characters\n"), dwRet);
    }
    else {
        _tprintf(_T("Current working directory: %s\n"), buffer);
        ret = buffer;
    }
    return ret;
}

void ReadTextFile(const TCHAR* filePath, std::string& result)
{
    result = "";

    HANDLE hFile = CreateFile(
        filePath,               // File name
        GENERIC_READ,           // Desired access
        0,                      // Share mode
        NULL,                   // Security attributes
        OPEN_EXISTING,          // Creation disposition
        FILE_ATTRIBUTE_NORMAL,  // Flags and attributes
        NULL                    // Template file
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        _tprintf(_T("Could not open file (error %d)\n"), GetLastError());
        return;
    }

    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) {
        _tprintf(_T("Could not get file size (error %d)\n"), GetLastError());
        CloseHandle(hFile);
        return;
    }

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        _tprintf(_T("Memory allocation failed\n"));
        CloseHandle(hFile);
        return;
    }

    DWORD bytesRead;
    BOOL readResult = ReadFile(
        hFile,      // File handle
        buffer,     // Buffer
        fileSize,   // Number of bytes to read
        &bytesRead, // Number of bytes read
        NULL        // Overlapped
    );

    if (!readResult || bytesRead != fileSize) {
        _tprintf(_T("Could not read file (error %d)\n"), GetLastError());
        free(buffer);
        CloseHandle(hFile);
        return;
    }

    // Null-terminate the buffer and print it
    buffer[fileSize] = '\0';
    printf("%s\n", buffer);
    result = buffer;

    // Clean up
    free(buffer);
    CloseHandle(hFile);
}


// Function to split a string by a given delimiter
std::vector<std::string> SplitString(const std::string& str, char delimiter)
{
    std::vector<std::string> result;
    std::string token;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos) {
        token = str.substr(start, end - start);
        result.push_back(token);
        start = end + 1;
        end = str.find(delimiter, start);
    }

    token = str.substr(start, end - start);
    result.push_back(token);

    return result;
}

std::wstring ConvertUTF8ToUnicode(const std::string& utf8Str) {
    // Get the required size for the wide string
    int wideCharSize = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
    if (wideCharSize == 0) {
        std::cerr << "MultiByteToWideChar failed with error " << GetLastError() << std::endl;
        return std::wstring();
    }

    // Allocate memory for the wide string
    std::wstring wideStr(wideCharSize, 0);

    // Perform the conversion
    int result = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wideStr[0], wideCharSize);
    if (result == 0) {
        std::cerr << "MultiByteToWideChar failed with error " << GetLastError() << std::endl;
        return std::wstring();
    }

    return wideStr;
}


std::string WStringToString(const std::wstring& w)
{
    if (w.empty()) return {};

    int size = WideCharToMultiByte(
        CP_ACP, 0,
        w.c_str(), -1,
        nullptr, 0,
        nullptr, nullptr
    );

    std::string result(size, '\0');

    WideCharToMultiByte(
        CP_UTF8, 0,
        w.c_str(), -1,
        result.data(), size,
        nullptr, nullptr
    );

    return result;
}

std::string FormatString(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    // Get the size needed
    int size = vsnprintf(nullptr, 0, format, args);
    va_end(args);
    if (size < 0) {
        return std::string();
    }
    std::string result(size, '\0');
    va_start(args, format);
    vsnprintf(&result[0], size + 1, format, args);
    va_end(args);
    return result;
}

