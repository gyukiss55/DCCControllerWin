// SemaphoreHandler.cpp


#include <windows.h>
#include <stdio.h>
#include <vector>
#include <string>

#include "SemaphoreHandler.h"


SemaphoreHandler* SemaphoreHandler::instance = nullptr;


/* MutexItem begin */
MutexItem::MutexItem() : ghMutex (0)
{
    ghMutex = CreateMutex(NULL, FALSE, NULL);
}

MutexItem::MutexItem(const wchar_t* nameIn) : ghMutex(0)
{
    if (nameIn != nullptr) {
        name = nameIn;
        ghMutex = CreateMutex(NULL, FALSE, name.c_str());

    } else
        ghMutex = CreateMutex(NULL, FALSE, NULL);

}

MutexItem::~MutexItem()
{
    CloseHandle(ghMutex);
}

const std::wstring& MutexItem::Name()const
{
    return name;
}

DWORD MutexItem::Wait(DWORD millisec) // WAIT_OBJECT_0 =0, WAIT_TIMEOUT, WAIT_ABANDONED, WAIT_FAILED
{
    return WaitForSingleObject(ghMutex, millisec);
}

BOOL MutexItem::Release()
{
    return ReleaseMutex(ghMutex);
}


/* MutexItem end */

/* SemaphoreItem begin */

SemaphoreItem::SemaphoreItem() : hSemaphore (0)
{
    CreateSemaphoreItem();
}

SemaphoreItem::SemaphoreItem(const wchar_t* nameIn) : hSemaphore (0)
{
    if (nameIn != nullptr)
        name = nameIn;
    CreateSemaphoreItem();
}

SemaphoreItem::~SemaphoreItem()
{
    CloseHandle(hSemaphore);
}

const std::wstring& SemaphoreItem::Name()const
{
    return name;
}

DWORD SemaphoreItem::Wait(DWORD millisec) // WAIT_OBJECT_0 =0, WAIT_TIMEOUT, WAIT_ABANDONED, WAIT_FAILED
{
    return WaitForSingleObject(hSemaphore, millisec);
}

BOOL SemaphoreItem::Release()
{
    return ReleaseSemaphore(hSemaphore, 1, NULL);
}

int SemaphoreItem::CreateSemaphoreItem()
{
    // Initial count of the semaphore
    LONG initialCount = 1;

    // Maximum count of the semaphore
    LONG maxCount = 1;

    // Creating a semaphore
    const wchar_t* namePtr = nullptr;
    if (name.length() > 0)
        namePtr = name.c_str();

    hSemaphore = CreateSemaphore(
        NULL,           // Default security attributes
        initialCount,   // Initial count
        maxCount,       // Maximum count
        namePtr          // Semaphore name
    );

    if (hSemaphore == NULL) {
        return 1;
    }

    return 0;
}


/* SemaphoreItem end */

SemaphoreHandler::SemaphoreHandler()
{
}

DWORD SemaphoreHandler::WaitForMutex(const wchar_t* name, DWORD millisec)
{
    MutexItem* m = nullptr;
    for (auto mi : mutexItems) {
        if (name == nullptr) {
            if (mi.Name().size() == 0) {
                m = &mi;
                break;
            }
        }
        else {
            if (mi.Name() == name) {
                m = &mi;
                break;
            }

        }
    }
    if (m == nullptr) {
        m = new MutexItem(name);
        if (m != nullptr)
            mutexItems.push_back(*m);
    }
    return m->Wait(millisec);
}

DWORD SemaphoreHandler::ReleaseMutex(const wchar_t* name)
{
    MutexItem* m = nullptr;
    for (auto mi : mutexItems) {
        if (name == nullptr) {
            if (mi.Name().size() == 0) {
                m = &mi;
                break;
            }
        }
        else {
            if (mi.Name() == name) {
                m = &mi;
                break;
            }

        }
    }
    if (m == nullptr) {
        return FALSE;
    }
    return m->Release();

}

DWORD SemaphoreHandler::WaitForSemaphore(const wchar_t* name, DWORD millisec)
{
    SemaphoreItem* s = nullptr;
    for (auto si : semaphoreItems) {
        if (name == nullptr) {
            if (si.Name().size() == 0) {
                s = &si;
                break;
            }
        }
        else {
            if (si.Name() == name) {
                s = &si;
                break;
            }

        }
    }
    if (s == nullptr) {
        s = new SemaphoreItem(name);
        if (s != nullptr)
            semaphoreItems.push_back(*s);
    }
    return s->Wait(millisec);
}

DWORD SemaphoreHandler::ReleaseSemaphore(const wchar_t* name)
{
    SemaphoreItem* s = nullptr;
    for (auto si : semaphoreItems) {
        if (name == nullptr) {
            if (si.Name().size() == 0) {
                s = &si;
                break;
            }
        }
        else {
            if (si.Name() == name) {
                s = &si;
                break;
            }

        }
    }
    if (s == nullptr) {
        return FALSE;
    }
    return s->Release();
}

SemaphoreHandler* SemaphoreHandler::GetInstance()
{
    if (instance == nullptr) {
        instance = new SemaphoreHandler;
    }
    return instance;
}

/* SemaphoreHandler end */

/* AutoMutex begin */

AutoMutex::AutoMutex(const wchar_t* nameIn)
{
    SemaphoreHandler* semaphoreHandler = SemaphoreHandler::GetInstance();
    if (nameIn != nullptr)
        name = nameIn;
    semaphoreHandler->WaitForMutex(nameIn);
}

AutoMutex::~AutoMutex()
{
    SemaphoreHandler* semaphoreHandler = SemaphoreHandler::GetInstance();
    if (name.length() > 0)
        semaphoreHandler->ReleaseMutex(name.c_str());
    else
        semaphoreHandler->ReleaseMutex();
}

/* AutoMutex end */

int CreateSemaphore() {
    // Initial count of the semaphore
    LONG initialCount = 1;

    // Maximum count of the semaphore
    LONG maxCount = 1;

    // Creating a semaphore
    HANDLE hSemaphore = CreateSemaphore(
        NULL,           // Default security attributes
        initialCount,   // Initial count
        maxCount,       // Maximum count
        NULL            // Semaphore name
    );

    if (hSemaphore == NULL) {
        printf("CreateSemaphore error: %d\n", GetLastError());
        return 1;
    }

    // Use the semaphore here

    // Close the semaphore handle when done
    CloseHandle(hSemaphore);

    return 0;
}



int OpenSemaphore() {
    // Open an existing semaphore
    HANDLE hSemaphore = OpenSemaphore(
        SEMAPHORE_ALL_ACCESS, // Desired access
        FALSE,                // Handle inheritance option
        L"MySemaphore"        // Semaphore name
    );

    if (hSemaphore == NULL) {
        printf("OpenSemaphore error: %d\n", GetLastError());
        return 1;
    }

    // Use the semaphore here

    // Close the semaphore handle when done
    CloseHandle(hSemaphore);

    return 0;
}


int WaitForSemapfore() {
    HANDLE hSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
    if (hSemaphore == NULL) {
        printf("CreateSemaphore error: %d\n", GetLastError());
        return 1;
    }

    // Wait for the semaphore
    DWORD dwWaitResult = WaitForSingleObject(
        hSemaphore, // Handle to the semaphore
        INFINITE    // Wait indefinitely
    );

    switch (dwWaitResult) {
        // The semaphore was signaled
    case WAIT_OBJECT_0:
        printf("Semaphore was signaled\n");
        // Perform the necessary work
        break;

        // An error occurred
    default:
        printf("Wait error: %d\n", GetLastError());
        return 1;
    }

    // Use the semaphore here

    // Release the semaphore
    ReleaseSemaphore(hSemaphore, 1, NULL);

    // Close the semaphore handle when done
    CloseHandle(hSemaphore);

    return 0;
}


int SignalingSemaphore() {
    HANDLE hSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
    if (hSemaphore == NULL) {
        printf("CreateSemaphore error: %d\n", GetLastError());
        return 1;
    }

    // Signal the semaphore
    if (!ReleaseSemaphore(
        hSemaphore, // Handle to the semaphore
        1,          // Increment the count by 1
        NULL        // Not interested in the previous count
    )) {
        printf("ReleaseSemaphore error: %d\n", GetLastError());
        return 1;
    }

    // Use the semaphore here

    // Close the semaphore handle when done
    CloseHandle(hSemaphore);

    return 0;
}



#define THREAD_COUNT 3

HANDLE ghSemaphore;

DWORD WINAPI ThreadProc(LPVOID lpParam) {
    UNREFERENCED_PARAMETER(lpParam);

    // Wait for the semaphore
    DWORD dwWaitResult = WaitForSingleObject(ghSemaphore, INFINITE);
    switch (dwWaitResult) {
        // The semaphore was signaled
    case WAIT_OBJECT_0:
        printf("Thread %d: Inside the semaphore\n", GetCurrentThreadId());
        // Simulate some work
        Sleep(2000);
        printf("Thread %d: Leaving the semaphore\n", GetCurrentThreadId());
        // Release the semaphore
        if (!ReleaseSemaphore(ghSemaphore, 1, NULL)) {
            printf("ReleaseSemaphore error: %d\n", GetLastError());
        }
        break;

        // An error occurred
    default:
        printf("Wait error: %d\n", GetLastError());
        return 1;
    }

    return 0;
}

int TestSemaphoreMultiThread ()
{
    HANDLE aThread[THREAD_COUNT];
    DWORD ThreadID;

    // Create a semaphore with initial and maximum counts of 1
    ghSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
    if (ghSemaphore == NULL) {
        printf("CreateSemaphore error: %d\n", GetLastError());
        return 1;
    }

    // Create worker threads
    for (int i = 0; i < THREAD_COUNT; i++) {
        aThread[i] = CreateThread(
            NULL,            // Default security attributes
            0,               // Default stack size
            ThreadProc,      // Thread function
            NULL,            // Parameter to thread function
            0,               // Default creation flags
            &ThreadID);      // Receive thread identifier

        if (aThread[i] == NULL) {
            printf("CreateThread error: %d\n", GetLastError());
            return 1;
        }
    }

    // Wait for all threads to terminate
    WaitForMultipleObjects(THREAD_COUNT, aThread, TRUE, INFINITE);

    // Close thread and semaphore handles
    for (int i = 0; i < THREAD_COUNT; i++) {
        CloseHandle(aThread[i]);
    }
    CloseHandle(ghSemaphore);

    return 0;
}


#define THREADCOUNT 2

HANDLE ghMutex;

DWORD WINAPI WriteToDatabase(LPVOID);

int TestMutexHandling(void)
{
    HANDLE aThread[THREADCOUNT];
    DWORD ThreadID;
    int i;

    // Create a mutex with no initial owner

    ghMutex = CreateMutex(
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex

    if (ghMutex == NULL)
    {
        printf("CreateMutex error: %d\n", GetLastError());
        return 1;
    }

    // Create worker threads

    for (i = 0; i < THREADCOUNT; i++)
    {
        aThread[i] = CreateThread(
            NULL,       // default security attributes
            0,          // default stack size
            (LPTHREAD_START_ROUTINE)WriteToDatabase,
            NULL,       // no thread function arguments
            0,          // default creation flags
            &ThreadID); // receive thread identifier

        if (aThread[i] == NULL)
        {
            printf("CreateThread error: %d\n", GetLastError());
            return 1;
        }
    }

    // Wait for all threads to terminate

    WaitForMultipleObjects(THREADCOUNT, aThread, TRUE, INFINITE);

    // Close thread and mutex handles

    for (i = 0; i < THREADCOUNT; i++)
        CloseHandle(aThread[i]);

    CloseHandle(ghMutex);

    return 0;
}

DWORD WINAPI WriteToDatabase(LPVOID lpParam)
{
    // lpParam not used in this example
    UNREFERENCED_PARAMETER(lpParam);

    DWORD dwCount = 0, dwWaitResult;

    // Request ownership of mutex.

    while (dwCount < 20)
    {
        dwWaitResult = WaitForSingleObject(
            ghMutex,    // handle to mutex
            INFINITE);  // no time-out interval

        switch (dwWaitResult)
        {
            // The thread got ownership of the mutex
        case WAIT_OBJECT_0:
            __try {
                // TODO: Write to the database
                printf("Thread %d writing to database...\n",
                    GetCurrentThreadId());
                dwCount++;
            }

            __finally {
                // Release ownership of the mutex object
                if (!ReleaseMutex(ghMutex))
                {
                    // Handle error.
                }
            }
            break;

            // The thread got ownership of an abandoned mutex
            // The database is in an indeterminate state
        case WAIT_ABANDONED:
            return FALSE;
        }
    }
    return TRUE;
}
