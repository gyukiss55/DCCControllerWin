/*
* 
* RailDevice.hpp
*/
struct CrossingDevice{
	INT32 nodeID;
	INT32 deviceID;
	INT32 state;
	INT32 xBegin;
	INT32 yBegin;
	INT32 xEnd;
	INT32 yEnd;
	INT32 xSide;
	INT32 ySide;
	INT32 xText;
	INT32 yText;

	CrossingDevice();
	void Draw(HDC hDC);
	INT32 GetSizeOfDevice(RECT & rect);
};

#define MAXCROSSINGDEVICES 16
#define MINLENGTH 20

extern CrossingDevice crossingDevices[MAXCROSSINGDEVICES];

void InitCrossingDevices();
void InputCrossingDevices(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void DrawCrossingDevice(HDC hDC);


#pragma once
