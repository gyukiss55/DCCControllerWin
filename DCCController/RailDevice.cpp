/*
*RailDevice.cpp
*/



#include "framework.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;

#include "RailDevice.h"
#include "FileIO.h"

SwitchDevice switchDevices[MAXSWITCHDEVICES];
SensorDevice sensorDevices[MAXSENSORDEVICES];

std::wstring iniFileName;
std::wstring jpgFileName;


INT32 generalInputState = 0;
INT32 inputState = 0;

SwitchDevice inputSwitchDevice;
SensorDevice inputSensorDevice;

void InitDevices()
{
	for (int i = 0; i < MAXSWITCHDEVICES; ++i) {
		switchDevices[i] = SwitchDevice();
	}
	for (int i = 0; i < MAXSENSORDEVICES; ++i) {
		sensorDevices[i] = SensorDevice();
	}
}

void InputDevices(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_KEYDOWN && wParam == VK_ESCAPE) {
		generalInputState = 0;
		inputState = 0;
		return;
	} else if (inputState == 0 && message == WM_CHAR && (wParam == 'C' || wParam == 'c')) {
		generalInputState = 'C';
		++inputState;
		memset(&inputSwitchDevice, 0, sizeof(inputSwitchDevice));
	} else if (inputState == 0 && message == WM_CHAR && (wParam == 'S' || wParam == 's')) {
		generalInputState = 'S';
		++inputState;
		memset(&inputSwitchDevice, 0, sizeof(inputSwitchDevice));
	} else if (inputState == 'C') {
		if (message == WM_CHAR) {
			if (wParam >= '0' && wParam <= '9') {
				++inputState;
				inputSwitchDevice.deviceID = (INT32)wParam - '0';
			}
			else if (wParam >= 'A' && wParam <= 'F') {
				++inputState;
				inputSwitchDevice.deviceID = (INT32)wParam - 'A' + 10;
			}
			else if (wParam >= 'a' && wParam <= 'f') {
				++inputState;
				inputSwitchDevice.deviceID = (INT32)wParam - 'a' + 10;
			}
		} else if (inputState == 2 && message == WM_LBUTTONDOWN) {
			++inputState;
			inputSwitchDevice.xBegin = (INT32)LOWORD(lParam);
			inputSwitchDevice.yBegin = (INT32)HIWORD(lParam);
		} else if (inputState == 3 && message == WM_LBUTTONUP) {
			++inputState;
			inputSwitchDevice.xEnd = (INT32)LOWORD(lParam);
			inputSwitchDevice.yEnd = (INT32)HIWORD(lParam);
		} else if (inputState == 4 && message == WM_LBUTTONDOWN) {
			++inputState;
			inputSwitchDevice.xSide = (INT32)LOWORD(lParam);
			inputSwitchDevice.ySide = (INT32)HIWORD(lParam);
		
		}
		else if (inputState == 5 && message == WM_LBUTTONDOWN) {
			inputState = 0;
			generalInputState = 0;
			inputSwitchDevice.xText = (INT32)LOWORD(lParam);
			inputSwitchDevice.yText = (INT32)HIWORD(lParam);

			if (inputSwitchDevice.deviceID < MAXSWITCHDEVICES) {
				RECT rect;
				inputSwitchDevice.GetSizeOfDevice(rect);
				switchDevices[inputSwitchDevice.deviceID] = inputSwitchDevice;
				InvalidateRect(hWnd, &rect, TRUE);
			}
		}
	} else if (inputState == 'S') {
		 if (message == WM_CHAR) {
			 if (wParam >= '0' && wParam <= '9') {
				 ++inputState;
				 inputSensorDevice.deviceID = (INT32)wParam - '0';
			 }
			 else if (wParam >= 'A' && wParam <= 'F') {
				 ++inputState;
				 inputSensorDevice.deviceID = (INT32)wParam - 'A' + 10;
			 }
			 else if (wParam >= 'a' && wParam <= 'f') {
				 ++inputState;
				 inputSensorDevice.deviceID = (INT32)wParam - 'a' + 10;
			 }
		 }
		 else if (inputState == 2 && message == WM_LBUTTONDOWN) {
			 ++inputState;
			 inputSensorDevice.xBegin = (INT32)LOWORD(lParam);
			 inputSensorDevice.yBegin = (INT32)HIWORD(lParam);
		 }
		 else if (inputState == 3 && message == WM_LBUTTONUP) {
			 ++inputState;
			 inputSensorDevice.xEnd = (INT32)LOWORD(lParam);
			 inputSensorDevice.yEnd = (INT32)HIWORD(lParam);
		 }
		 else if (inputState == 4 && message == WM_LBUTTONDOWN) {
			 inputState = 0;
			 generalInputState = 0;
			 inputSensorDevice.xText = (INT32)LOWORD(lParam);
			 inputSensorDevice.yText = (INT32)HIWORD(lParam);

			 if (inputSensorDevice.deviceID < MAXSENSORDEVICES) {
				 RECT rect;
				 inputSensorDevice.GetSizeOfDevice(rect);
				 sensorDevices[inputSensorDevice.deviceID] = inputSensorDevice;
				 InvalidateRect(hWnd, &rect, TRUE);
			 }
		 }
	}
}

BasicDevice::BasicDevice() :
			nodeID (0),
			deviceID(0),
			state(0),
			xBegin(0),
			yBegin(0),
			xEnd(0),
			yEnd(0),
			xText(0),
			yText(0)
{

}

SwitchDevice::SwitchDevice() :
			xSide(0),
			ySide(0)
{

}


SensorDevice::SensorDevice()
{

}

INT32 BasicDevice::GetSizeOfDevice(RECT& rect)
{
	INT32 s1 = (xBegin > xEnd) ? (xBegin - xEnd) : (xEnd - xBegin);
	INT32 s2 = (yBegin > yEnd) ? (yBegin - yEnd) : (yEnd - yBegin);
	s1 = (s1 > s2) ? s1 : s2;

	if (s1 > MINLENGTH) {
		rect.left = (xBegin + xEnd) / 2 - s1;
		rect.right = (xBegin + xEnd) / 2 + s1;
		rect.top = (yBegin + yEnd) / 2 - s1;
		rect.bottom = (yBegin + yEnd) / 2 + s1;
		return 2 * s1;

	}
	return 0;
}


void SwitchDevice::Draw(HDC hDC)
{
	if ((xBegin == xEnd) || (yBegin == yEnd))
		return;
	Graphics graphics(hDC);
	Pen      pen1(Color(255, 0, 255, 0));
	pen1.SetWidth(10.);
	graphics.DrawLine(&pen1, xBegin, yBegin, xEnd, yEnd);
	Pen      pen2(Color(255, 255, 0, 0));
	pen2.SetWidth(6.);
	graphics.DrawLine(&pen2, xBegin, yBegin, xSide, ySide);


	SolidBrush  brush(Color(255, 0, 0, 255));
	FontFamily  fontFamily(L"Times New Roman");
	Font        font(&fontFamily, 24, FontStyleRegular, UnitPixel);
	PointF      pointF((Gdiplus::REAL)xText, (Gdiplus::REAL)yText);
	const wchar_t * leftOrRight = L"Left";
	if (this->state == 1)
		leftOrRight = L"Right";
	wchar_t strSwitch[30];
	wsprintf(strSwitch, L"Switch %d %s", this->deviceID, leftOrRight);
	graphics.DrawString(strSwitch, -1, &font, pointF, &brush);

}

void SensorDevice::Draw(HDC hDC)
{
	if ((xBegin == xEnd) || (yBegin == yEnd))
		return;
	Graphics graphics(hDC);
	Pen      pen1(Color(255, 255, 0, 255));
	pen1.SetWidth(10.);
	graphics.DrawLine(&pen1, xBegin, yBegin, xEnd, yEnd);


	SolidBrush  brush(Color(255, 0, 0, 255));
	FontFamily  fontFamily(L"Times New Roman");
	Font        font(&fontFamily, 24, FontStyleRegular, UnitPixel);
	PointF      pointF((Gdiplus::REAL)xText, (Gdiplus::REAL)yText);
	const wchar_t* onOrOff = L"On";
	if (this->state == 1)
		onOrOff = L"Off";
	wchar_t strSensor[30];
	wsprintf(strSensor, L"Sensor %d %s", this->deviceID, onOrOff);
	graphics.DrawString(strSensor, -1, &font, pointF, &brush);

}

bool BasicDevice::Save(FileOutput& file)
{
	file.Write(TEXT("DE"), 4);
	file.Write(&nodeID, sizeof(nodeID));
	file.Write(&deviceID, sizeof(deviceID));
	file.Write(&state, sizeof(state));
	file.Write(&xBegin, sizeof(xBegin));
	file.Write(&yBegin, sizeof(yBegin));
	file.Write(&xEnd, sizeof(xEnd));
	file.Write(&yEnd, sizeof(yEnd));
	file.Write(&xText, sizeof(xText));
	file.Write(&yText, sizeof(yText));
	return true;
}

bool BasicDevice::Read(FileInput& file)
{
	return true;
}

bool SwitchDevice::Save(FileOutput& file)
{
	BasicDevice::Save(file);
	file.Write(TEXT("SW"), 4);
	file.Write(&xSide, sizeof(xSide));
	file.Write(&ySide, sizeof(ySide));
	return true;
}

bool SwitchDevice::Read(FileInput& file)
{
	return true;
}

void DrawDevices(HDC hDC)
{
	for (int i = 0; i < MAXSWITCHDEVICES; ++i) {
		switchDevices[i].Draw(hDC);
	}
	for (int i = 0; i < MAXSENSORDEVICES; ++i) {
		sensorDevices[i].Draw(hDC);
	}

}

bool SaveDevices()
{
	if (iniFileName.length() == 0)
		iniFileName = INIFILENAME0;
	if (jpgFileName.length() == 0)
		jpgFileName = JPGFILENAME0;

	FileOutput file;
	if (file.Open(jpgFileName.data ())) {
		file.Write(INIFILEVERSION, 6);
		DWORD len = jpgFileName.size() + 1;
		file.Write(&len, sizeof (len));
		file.Write(jpgFileName.c_str (), len);
		file.Write(TEXT("SW"), 4);
		len = MAXSWITCHDEVICES;
		file.Write(&len, sizeof(len));
		for (int i = 0; i < MAXSWITCHDEVICES; ++i) {
			switchDevices[i].Save(file);
		}
		file.Write(TEXT("SE"), 4);
		len = MAXSENSORDEVICES;
		file.Write(&len, sizeof(len));
		for (int i = 0; i < MAXSENSORDEVICES; ++i) {
			sensorDevices[i].Save(file);
		}
		file.Close();
		return true;

	}
	return false;

}

bool ReadDevices()
{
	return true;

}


