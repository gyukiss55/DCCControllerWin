/*
* 
* RailDevice.hpp
*/

#include <string>
#include "FileIO.h"

struct BasicDevice {
	INT32 nodeID;
	INT32 deviceID;
	INT32 state;
	INT32 xBegin;
	INT32 yBegin;
	INT32 xEnd;
	INT32 yEnd;
	INT32 xText;
	INT32 yText;

	BasicDevice();
	virtual void Draw(HDC hDC) = 0;
	virtual INT32 GetSizeOfDevice(RECT& rect);
	virtual bool Save(FileOutput& file);
	virtual bool Read(FileInput& file);
};



struct SwitchDevice : public BasicDevice {
	INT32 xSide;
	INT32 ySide;

	SwitchDevice();
	void Draw(HDC hDC);
//	INT32 GetSizeOfDevice(RECT & rect);
	bool Save(FileOutput& file);
	bool Read(FileInput& file);
};

struct SensorDevice : public BasicDevice {
	SensorDevice();
	void Draw(HDC hDC);
//	INT32 GetSizeOfDevice(RECT& rect);
};

#define MAXSWITCHDEVICES 16
#define MAXSENSORDEVICES 16
#define MINLENGTH 20

#define INIFILEVERSION L"TR0100"
#define INIFILENAME0 L"Track.ini"
#define JPGFILENAME0 L"Track.jpg"

extern SwitchDevice switchDevices[MAXSWITCHDEVICES];
extern SensorDevice sensorDevices[MAXSENSORDEVICES];
extern std::wstring iniFileName;
extern std::wstring jpgFileName;

void InitDevices();
void InputDevices(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void DrawDevices(HDC hDC);
bool SaveDevices();

#pragma once
