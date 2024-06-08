// MeasureTime.h

#pragma once

#include <windows.h>

#include <string>


class MeasureTime {
	LARGE_INTEGER frequency;
	LARGE_INTEGER start;
	LARGE_INTEGER end;
	std::string name;

	void Init();
public:
	MeasureTime();
	MeasureTime(const char * name);
	~MeasureTime();
	void GetElapsedTime(std::string& dump);
};
