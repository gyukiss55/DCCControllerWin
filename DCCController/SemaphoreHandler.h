//SemaphoreHander.h
#pragma once

#include <windows.h>
#include <stdio.h>
#include <vector>
#include <string>

class MutexItem {
    HANDLE ghMutex;
    std::wstring name;
public:
    MutexItem();
    MutexItem(const wchar_t* name);
    ~MutexItem();
    const std::wstring& Name()const;
    DWORD Wait(DWORD millisec = INFINITE); // WAIT_OBJECT_0 =0, WAIT_TIMEOUT, WAIT_ABANDONED, WAIT_FAILED
    BOOL Release();
};

class SemaphoreItem {
    HANDLE hSemaphore;
    std::wstring name;
    int CreateSemaphoreItem();
public:
    SemaphoreItem();
    SemaphoreItem(const wchar_t* name);
    ~SemaphoreItem();
    const std::wstring& Name()const;
    DWORD Wait(DWORD millisec = INFINITE); // WAIT_OBJECT_0 =0, WAIT_TIMEOUT, WAIT_ABANDONED, WAIT_FAILED
    BOOL Release();
};

class SemaphoreHandler {
    std::vector<MutexItem> mutexItems;
    std::vector<SemaphoreItem> semaphoreItems;
    static SemaphoreHandler* instance;
    SemaphoreHandler();
public:
    DWORD WaitForMutex(const wchar_t* name = nullptr, DWORD millisec = INFINITE);
    DWORD ReleaseMutex(const wchar_t* name = nullptr);
    DWORD WaitForSemaphore(const wchar_t* name = nullptr, DWORD millisec = INFINITE);
    DWORD ReleaseSemaphore(const wchar_t* name = nullptr);
    static SemaphoreHandler* GetInstance();
};

class AutoMutex{
    std::wstring name;
public: 
    AutoMutex(const wchar_t* name = nullptr);
    ~AutoMutex();
};

