// DCCController.cpp : Defines the entry point for the application.
//

#include <string>
#include "framework.h"
#include "DCCController.h"
#include "SendURL.h"


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HWND hwndDCCControl = 0;
HWND hWndMain = 0;

std::string strLocalTime;
std::string strDCCFeedback;


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DCCControlDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

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

   initURL();
   DialogBox(hInstance, MAKEINTRESOURCE(IDD_DCCCONTROLLER_DIALOG), hWnd, DCCControlDlg);
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
            case IDM_CONTROL_DCCCONTROL:
                if (!IsWindow(hwndDCCControl)) {
                    hwndDCCControl = CreateDialog(hInst,
                        MAKEINTRESOURCE(IDD_DIALOG_DCCControl),
                        hWnd,
                        (DLGPROC)DCCControlDlg);
                    ShowWindow(hwndDCCControl, SW_SHOW);

                }
//                DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_TEST), hWnd, DCCControlDlg);
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                closeURL();
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_TIMER:
        {
            switch (wParam)
            {
            case IDT_TIMER1:
                // process the 10-milli second timer 
                {
                    SYSTEMTIME lt;
                    GetLocalTime(&lt);
                    char buffer[128];
                    snprintf(buffer, sizeof (buffer), " The local time is: %02d:%02d:%02d", lt.wHour, lt.wMinute, lt.wSecond);
                    strLocalTime = buffer;
                    InvalidateRect(hWnd,NULL, TRUE);
                }
                return 0;

            } 
            break;
        }
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hDC = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            RECT rect;
            GetClientRect(hWnd, &rect);

            TextOutA(hDC, 10, rect.bottom - 30, strLocalTime.c_str(), (int)strLocalTime.size());
            TextOutA(hDC, 10, rect.bottom - 60, strDCCFeedback.c_str(), (int)strDCCFeedback.size());
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

// Message handler for about box.
INT_PTR CALLBACK DCCControlDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    char buffer[128];
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (wParam) {
        case IDC_BUTTON_BS0:
        case IDC_BUTTON_BS1F:
        case IDC_BUTTON_BS1B:
        case IDC_BUTTON_BLF:
        case IDC_BUTTON_BLB:
        case IDC_BUTTON_A3S1F:
        case IDC_BUTTON_A3S1B:
            sendURL((int)wParam, nullptr, strDCCFeedback);
            InvalidateRect(hWndMain, NULL, TRUE);
            break;
        case IDC_BUTTON_SENDEXP1:
            {
                GetDlgItemTextA(hDlg, IDC_EDIT_EXPLICITE1, buffer, sizeof(buffer));
                sendURL((int)wParam, buffer, strDCCFeedback);
                InvalidateRect(hWndMain, NULL, TRUE);
                break;
            }
        case IDC_BUTTON_SENDEXP2:
            {
                GetDlgItemTextA(hDlg, IDC_EDIT_EXPLICITE2, buffer, sizeof(buffer));
                sendURL((int)wParam, buffer, strDCCFeedback);
                InvalidateRect(hWndMain, NULL, TRUE);
                break;
            }
        case IDC_BUTTON_SENDEXP3:
            {
                GetDlgItemTextA(hDlg, IDC_EDIT_EXPLICITE3, buffer, sizeof(buffer));
                sendURL((int)wParam, buffer, strDCCFeedback);
                InvalidateRect(hWndMain, NULL, TRUE);
                break;
            }
        }
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
