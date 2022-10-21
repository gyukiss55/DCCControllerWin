// DCCController.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#include <string>
#include "DCCController.h"
#include "SendURL.h"
#include "DCCMonitorDialog.h"
#include "RailDevice.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HWND hwndDCCControl = 0;
HWND hwndDCCMonitor = 0;
HWND hWndMain = 0;

std::string strLocalTime;
std::string strDCCFeedback;


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DCCControlDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void ShowStatus(HWND hWnd, HDC hDC);
void RefreshStatus(HWND hWnd);
void OnPaintDrawLine(HDC hdc);
void OnKeyPress(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void OnKeyUpDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void OnMouseMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int mainCreateSliderControl();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

    //mainCreateSliderControl();
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    InitCrossingDevices();

    // Initialize GDI+.


    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DCCCONTROLLER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DCCCONTROLLER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    GdiplusShutdown(gdiplusToken);
    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DCCCONTROLLER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DCCCONTROLLER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   SetTimer(hWnd,             // handle to main window 
       IDT_TIMER1,            // timer identifier 
       1000,                   // 10 - milli second interval 
       (TIMERPROC)NULL);

   //DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_DCCControl), hWnd, DCCControlDlg);
   //DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_DCCMONITOR), hWnd, DCCMonitorDlg);
   hWndMain = hWnd;
   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_CONTROL_DCCMONITOR:
                if (!IsWindow(hwndDCCMonitor)) {
                    hwndDCCMonitor = CreateDialog(hInst,
                    MAKEINTRESOURCE(IDD_DIALOG_DCCMONITOR),
                    hWnd,
                    (DLGPROC)DCCMonitorDlg);
                    ShowWindow(hwndDCCMonitor, SW_SHOW);
                }
                //                DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_TEST), hWnd, DCCControlDlg);
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_CHAR:
        OnKeyPress(hWnd, message, wParam, lParam);
        break;
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
        OnKeyUpDown(hWnd, message, wParam, lParam);
        break;

    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:

    case WM_MOUSEMOVE:

    case WM_MOUSEWHEEL:

        OnMouseMsg(hWnd, message, wParam, lParam);
        break;
    case WM_TIMER:
    {
        switch (wParam)
        {
        case IDT_TIMER1:
            RefreshStatus(hWnd);
            return 0;
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        ShowStatus(hWnd, hDC);

        OnPaintDrawLine(hDC);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            hwndDCCControl = 0;
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


void ShowStatus(HWND hWnd, HDC hDC)
{
    RECT rect;
    GetClientRect(hWnd, &rect);

    TextOutA(hDC, 10, rect.bottom - 30, strLocalTime.c_str(), (int)strLocalTime.size());
    TextOutA(hDC, 10, rect.bottom - 60, strDCCFeedback.c_str(), (int)strDCCFeedback.size());
}


void RefreshStatus(HWND hWnd)
{
    SYSTEMTIME lt;
    GetLocalTime(&lt);
    char buffer[128];
    snprintf(buffer, sizeof(buffer), " The local time is: %02d:%02d:%02d", lt.wHour, lt.wMinute, lt.wSecond);
    strLocalTime = buffer;

    RECT rect;
    GetClientRect(hWnd, &rect);
    rect.top = rect.bottom - 80;
    InvalidateRect(hWnd, &rect, TRUE);
}


void OnKeyPress(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    InputCrossingDevices(hWnd, message, wParam, lParam);
}


void OnMouseMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE:
        {
            InputCrossingDevices(hWnd, message, wParam, lParam);
            WPARAM xPos = LOWORD(lParam);
            WPARAM yPos = HIWORD(lParam);
        }
        break;
    case WM_MOUSEHWHEEL:
        {
            WPARAM zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

        }
        break;

    }

}

void OnKeyUpDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    InputCrossingDevices(hWnd, message, wParam, lParam);

    WORD vkCode = LOWORD(wParam);                                 // virtual-key code

    WORD keyFlags = HIWORD(lParam);

    WORD scanCode = LOBYTE(keyFlags);                             // scan code
    BOOL isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED; // extended-key flag, 1 if scancode has 0xE0 prefix

    if (isExtendedKey)
        scanCode = MAKEWORD(scanCode, 0xE0);

    BOOL wasKeyDown = (keyFlags & KF_REPEAT) == KF_REPEAT;        // previous key-state flag, 1 on autorepeat
    WORD repeatCount = LOWORD(lParam);                            // repeat count, > 0 if several keydown messages was combined into one message

    BOOL isKeyReleased = (keyFlags & KF_UP) == KF_UP;             // transition-state flag, 1 on keyup

    // if we want to distinguish these keys:
    switch (vkCode)
    {
    case VK_SHIFT:   // converts to VK_LSHIFT or VK_RSHIFT
    case VK_CONTROL: // converts to VK_LCONTROL or VK_RCONTROL
    case VK_MENU:    // converts to VK_LMENU or VK_RMENU
        vkCode = LOWORD(MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX));
        break;
    }

}

void OnPaintDrawLine(HDC hDC)
{
    Graphics graphics(hDC);
    Pen      pen(Color(255, 0, 0, 255));
//   graphics.DrawLine(&pen, 0, 0, 200, 100);

    SolidBrush  brush(Color(255, 0, 0, 255));
    FontFamily  fontFamily(L"Times New Roman");
    Font        font(&fontFamily, 24, FontStyleRegular, UnitPixel);
    PointF      pointF(10.0f, 20.0f);
//    graphics.DrawString(L"DCC Controller!", -1, &font, pointF, &brush);

    Image image(L"Track4.jpg");
    graphics.DrawImage(&image, 10, 10);

    DrawCrossingDevice(hDC);

}
