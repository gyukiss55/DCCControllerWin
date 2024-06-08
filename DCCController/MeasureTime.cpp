// MeasureTime.cpp
#include <windows.h>
#include <stdio.h>
#include <string>

#include "MeasureTime.h"


MeasureTime::MeasureTime()
{
    Init();
}

MeasureTime::MeasureTime(const char * nameIn)
{
    name = nameIn;
    Init();
}

MeasureTime::~MeasureTime()
{
    std::string str;
    GetElapsedTime(str);
    OutputDebugStringA(str.c_str());
}

void MeasureTime::Init()
{
    // Get the frequency of the high-resolution performance counter
    QueryPerformanceFrequency(&frequency);

    // Get the starting count
    QueryPerformanceCounter(&start);

}

void MeasureTime::GetElapsedTime(std::string& dump)
{
    QueryPerformanceCounter(&end);

    // Calculate the elapsed time in milliseconds
    double elapsedTime = (double)(end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
    char buffer[512];
    if (name.length() > 0) {
        sprintf_s(buffer, sizeof (buffer), "%s - Elapsed time: %.2f milliseconds\n", name.c_str (), elapsedTime);
    }
    else {
        sprintf_s(buffer, sizeof (buffer), "Elapsed time: %.2f milliseconds\n", elapsedTime);
    }
    dump = buffer;
}


// Function to measure elapsed time in milliseconds
void MeasureElapsedTime()
{
    LARGE_INTEGER frequency, start, end;
    double elapsedTime;

    // Get the frequency of the high-resolution performance counter
    QueryPerformanceFrequency(&frequency);

    // Get the starting count
    QueryPerformanceCounter(&start);

    // Code block for which the time is measured
    Sleep(1000); // Sleep for 1 second to simulate some work

    // Get the ending count
    QueryPerformanceCounter(&end);

    // Calculate the elapsed time in milliseconds
    elapsedTime = (double)(end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;

    printf("Elapsed time: %.2f milliseconds\n", elapsedTime);
}

// Entry point for the application
int TestMeasureTime()
{
    MeasureElapsedTime();
    return 0;
}
