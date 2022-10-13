/*
* DCCMonitorDialog.cpp
*/

#include <string>
#include "framework.h"
#include "DCCController.h"
#include "DCCMonitorDialog.h"
#include "SendURL.h"
#include "RegistryIO.h"

bool SaveDCCDlgContent(HWND hDlg, std::string& feedback);
bool ReadDCCDlgContent(HWND hDlg, std::string& feedback);
bool ExecuteCommand(HWND hDlg, WPARAM wParam, const char* nodeStr, const uint32_t* rids);

const char* dccCommands[] = {
    "7F",   // forward full speed
    "60",   // stop
    "5F",   // backward full speed

    "81",   // mp3 1. rec
    "82",   // mp3 2. rec
    "83",   // mp3 3. rec
    "84",   // mp3 4. rec
    "85",   // mp3 5. rec

    nullptr
};

const uint32_t rids1[] = {
    IDC_BUTTON_SF1,
    IDC_BUTTON_SB1,
    IDC_BUTTON_SS1,

    IDC_BUTTON_MP11,
    IDC_BUTTON_MP12,
    IDC_BUTTON_MP13,
    IDC_BUTTON_MP14,
    IDC_BUTTON_MP15,

    0
};

const uint32_t rids2[] = {
    IDC_BUTTON_SF2,
    IDC_BUTTON_SB2,
    IDC_BUTTON_SS2,

    IDC_BUTTON_MP21,
    IDC_BUTTON_MP22,
    IDC_BUTTON_MP23,
    IDC_BUTTON_MP24,
    IDC_BUTTON_MP25,

    0
};

static uint32_t idcEditNodes[] = { 
    IDC_EDIT_NODEADDRESS1,
    IDC_EDIT_NODEADDRESS2,
    IDC_EDIT_NODEADDRESS3,
    IDC_EDIT_NODEADDRESS4,
    IDC_EDIT_NODEADDRESS5,
    IDC_EDIT_NODEADDRESS6 };

static uint32_t idcScrollbars[] = { 
    IDC_SLIDER_SPEED1,
    IDC_SLIDER_SPEED2,
    IDC_SLIDER_SPEED3,
    IDC_SLIDER_SPEED4 };



// Message handler for about box.
INT_PTR CALLBACK DCCMonitorDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    char buffer[128];
    switch (message)
    {
    case WM_INITDIALOG:
        {
            int pos1 = 0;
            int pos2 = 0;
            int actpos = 0;

            SCROLLBARINFO gsbi;
            HWND hScrb0 = GetDlgItem(hDlg, idcScrollbars[0]);
            HWND hScrb1 = GetDlgItem(hDlg, idcScrollbars[1]);
            HWND hScrb2 = GetDlgItem(hDlg, idcScrollbars[2]);
            HWND hScrb3 = GetDlgItem(hDlg, idcScrollbars[3]);
            int ret = GetScrollBarInfo(hScrb0, OBJID_CLIENT, &gsbi);
            ret = GetScrollRange(hScrb0, SB_CTL, &pos1, &pos2);
            actpos = GetScrollPos(hScrb0, SB_CTL);
            //SetScrollPos(hScrb0, SB_CTL, actpos + (gsbi.xyThumbTop - gsbi.xyThumbBottom) / 2, TRUE);
            ret = SetScrollRange(hScrb0, OBJID_CLIENT, 0, 100, TRUE);
            SetScrollPos(hScrb0, SB_CTL, 50, TRUE);
            ret = SetScrollRange(hScrb1, OBJID_CLIENT, -100, 100, TRUE);
            SetScrollPos(hScrb1, SB_CTL, 0, TRUE);
            ret = SetScrollRange(hScrb2, OBJID_CLIENT, -15, 15, TRUE);
            ret = SetScrollRange(hScrb3, OBJID_CLIENT, -1000, 0, TRUE);

        }
        return (INT_PTR)TRUE;
    case WM_HSCROLL:
    case WM_VSCROLL:
        switch (LOWORD(wParam)) {
            case SB_TOP:
            case SB_BOTTOM:
            case SB_THUMBPOSITION:
            case SB_THUMBTRACK:
            {
                SCROLLINFO si;

                ZeroMemory(&si, sizeof(si));
                si.cbSize = sizeof(si);
                si.fMask = SIF_TRACKPOS;

                int pos1 = 0;
                int pos2 = 0;
                int actpos = 0;
                HWND hScrb0 = GetDlgItem(hDlg, idcScrollbars[0]);
                int ret = SetScrollRange(hScrb0, OBJID_CLIENT, 0, 100, TRUE);
                ret = GetScrollInfo(hScrb0, OBJID_CLIENT, &si);
                ret = GetScrollRange(hScrb0, SB_CTL, &pos1, &pos2);
                actpos = GetScrollPos(hScrb0, SB_CTL);

                strDCCFeedback = "Actpos:";


             }
                // Initialize SCROLLINFO structure
            break;
        }
        
    case WM_COMMAND:
        switch (wParam) {
            case IDC_BUTTON_SB1:
            case IDC_BUTTON_SF1:
            case IDC_BUTTON_SS1:
            case IDC_BUTTON_MP11:
            case IDC_BUTTON_MP12:
            case IDC_BUTTON_MP13:
            case IDC_BUTTON_MP14:
            case IDC_BUTTON_MP15:
                {
                    const char* nodeStr = "0";
                    const uint32_t* rids = rids1;

                    ExecuteCommand(hDlg, wParam, nodeStr, rids);
               }
                break;
            case IDC_BUTTON_SB2:
            case IDC_BUTTON_SF2:
            case IDC_BUTTON_SS2:
            case IDC_BUTTON_MP21:
            case IDC_BUTTON_MP22:
            case IDC_BUTTON_MP23:
            case IDC_BUTTON_MP24:
            case IDC_BUTTON_MP25:
                {
                    const char* nodeStr = "1";
                    const uint32_t* rids = rids2;

                    ExecuteCommand(hDlg, wParam, nodeStr, rids);
               }
                break;
           case IDC_BUTTON_SAVE:
            {
                SaveDCCDlgContent(hDlg, strDCCFeedback);
                InvalidateRect(hWndMain, NULL, TRUE);
                break;
            }
            case IDC_BUTTON_READ:
            {
                ReadDCCDlgContent(hDlg, strDCCFeedback);
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

bool SaveDCCDlgContent(HWND hDlg, std::string& feedback)
{
    char buffer[512];

    SendDlgItemMessageA(hDlg,
        IDC_EDIT_IPADDRESS,
        WM_GETTEXT,
        (WPARAM)sizeof(buffer),
        (LPARAM)buffer);

    feedback = "ip address:";
    feedback += buffer;

    bool ret = WriteRegistry("IPAddress", buffer);
    for (int i = 0; i < MAX_NODE_NUMBER; ++i) {
        SendDlgItemMessageA(hDlg,
            idcEditNodes[i],
            WM_GETTEXT,
            (WPARAM)sizeof(buffer),
            (LPARAM)buffer);
        std::string nodeNr("Node");
        nodeNr += i + '0';
        ret &= WriteRegistry(nodeNr.c_str(), buffer);
    }
    return ret;
}

bool ReadDCCDlgContent(HWND hDlg, std::string& feedback)
{
    std::string str_data;

    if (ReadRegistry ("IPAddress", str_data)) {

        SendDlgItemMessageA(hDlg,
            IDC_EDIT_IPADDRESS,
            WM_SETTEXT,
            (WPARAM)0,       // line 0 
            (LPARAM)str_data.c_str ());

        feedback = "ip address:";
        feedback += str_data;

        for (int i = 0; i < MAX_NODE_NUMBER; ++i) {
            std::string nodeNr("Node");
            nodeNr += i + '0';
            if (ReadRegistry(nodeNr.c_str(), str_data)) {
                SendDlgItemMessageA(hDlg,
                    idcEditNodes[i],
                    WM_SETTEXT,
                    (WPARAM)0,       // line 0 
                    (LPARAM)str_data.c_str());

                feedback = " node:";
                feedback += str_data;
            }

        }
        return true;
    }
    return false;
}

bool ExecuteCommand(HWND hDlg, WPARAM wParam, const char* nodeStr, const uint32_t * rids)
{

    char ipAddr[512];
    const char* dccCommand = nullptr;
    for (int i = 0; rids[i] != 0; ++i) {
        if (rids[i] == wParam)
            dccCommand = dccCommands[i];
    }

    SendDlgItemMessageA(hDlg,
        IDC_EDIT_IPADDRESS,
        WM_GETTEXT,
        (WPARAM)sizeof(ipAddr),
        (LPARAM)ipAddr);
    SendURL(ipAddr, nodeStr, dccCommand, strDCCFeedback);
    InvalidateRect(hWndMain, NULL, TRUE);
    return true;
}

