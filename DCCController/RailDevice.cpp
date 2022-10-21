/*
*RailDevice.cpp
*/



#include "framework.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;

#include "RailDevice.h"

CrossingDevice crossingDevices[MAXCROSSINGDEVICES];

INT32 generalInputState = 0;
INT32 crossingInputState = 0;

CrossingDevice inputCrossingDevice;

void InitCrossingDevices()
{
	for (int i = 0; i < MAXCROSSINGDEVICES; ++i) {
		memset(crossingDevices, 0, sizeof(crossingDevices));
	}
}

void InputCrossingDevices(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_KEYDOWN && wParam == VK_ESCAPE) {
		generalInputState = 0;
		crossingInputState = 0;
		return;
	} else if (crossingInputState == 0 && message == WM_CHAR && (wParam == 'C' || wParam == 'c')) {
		generalInputState = 1;
		++crossingInputState;
		memset(&inputCrossingDevice, 0, sizeof(inputCrossingDevice));
	} else if (crossingInputState == 1 && message == WM_CHAR) {
		if (wParam >= '0' && wParam <= '9') {
			++crossingInputState;
			inputCrossingDevice.deviceID = wParam - '0';
		}
		else if (wParam >= 'A' && wParam <= 'F') {
			++crossingInputState;
			inputCrossingDevice.deviceID = wParam - 'A' + 10;
		}
		else if (wParam >= 'a' && wParam <= 'f') {
			++crossingInputState;
			inputCrossingDevice.deviceID = wParam - 'a' + 10;
		}
	} else if (crossingInputState == 2 && message == WM_LBUTTONDOWN) {
		++crossingInputState;
		WPARAM xPos = LOWORD(lParam);
		WPARAM yPos = HIWORD(lParam);
		inputCrossingDevice.xBegin = xPos;
		inputCrossingDevice.yBegin = yPos;
	} else if (crossingInputState == 3 && message == WM_LBUTTONUP) {
		++crossingInputState;
		WPARAM xPos = LOWORD(lParam);
		WPARAM yPos = HIWORD(lParam);
		inputCrossingDevice.xEnd = xPos;
		inputCrossingDevice.yEnd = yPos;
	} else if (crossingInputState == 4 && message == WM_LBUTTONDOWN) {
		++crossingInputState;
		WPARAM xPos = LOWORD(lParam);
		WPARAM yPos = HIWORD(lParam);
		inputCrossingDevice.xSide = xPos;
		inputCrossingDevice.ySide = yPos;
		
	} else if (crossingInputState == 5 && message == WM_LBUTTONDOWN) {
		crossingInputState = 0;
		generalInputState = 0;
		WPARAM xPos = LOWORD(lParam);
		WPARAM yPos = HIWORD(lParam);
		inputCrossingDevice.xText = xPos;
		inputCrossingDevice.yText = yPos;
		
		if (inputCrossingDevice.deviceID < MAXCROSSINGDEVICES) {
			RECT rect;
			inputCrossingDevice.GetSizeOfDevice(rect);
			crossingDevices[inputCrossingDevice.deviceID] = inputCrossingDevice;
			InvalidateRect(hWnd, &rect, TRUE);
		}

	}


}

CrossingDevice::CrossingDevice() :
			nodeID (0),
			deviceID(0),
			state(0),
			xBegin(0),
			yBegin(0),
			xEnd(0),
			yEnd(0),
			xSide(0),
			ySide(0),
			xText(0),
			yText(0)
{

}

INT32 CrossingDevice::GetSizeOfDevice(RECT& rect)
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


void CrossingDevice::Draw(HDC hDC)
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
//	PointF      pointF((xBegin + xEnd)/2, (yBegin + yEnd) / 2 + 20.0f);
	PointF      pointF(xText, yText);
	const wchar_t * leftOrRight = L"Left";
	if (this->state == 1)
		leftOrRight = L"Right";
	wchar_t strCross[30];
	wsprintf(strCross, L"Cross %d %s", this->deviceID, leftOrRight);
	graphics.DrawString(strCross, -1, &font, pointF, &brush);

}

void DrawCrossingDevice(HDC hDC)
{
	for (int i = 0; i < MAXCROSSINGDEVICES; ++i) {
		crossingDevices[i].Draw(hDC);
	}

}

