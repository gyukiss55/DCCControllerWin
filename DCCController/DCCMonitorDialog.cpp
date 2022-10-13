/*
* DCCMonitorDialog.cpp
*/

#include <string>
#include "framework.h"
#include "DCCController.h"
#include "DCCMonitorDialog.h"
#include "SendURL.h"

// Message handler for about box.
INT_PTR CALLBACK DCCMonitorDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
