/*
* DCCMonitorDialog.cpp
*/

#include "framework.h"
#include <string>
#include <CommCtrl.h>
#include "DCCController.h"
#include "DCCMonitorDialog.h"
#include "SendURL.h"
#include "RegistryIO.h"

#define SLIDER_MIN -15
#define SLIDER_MAX 15

bool SaveDCCDlgContent(HWND hDlg, std::string& feedback);
bool ReadDCCDlgContent(HWND hDlg, std::string& feedback);
bool ExecuteCommand(HWND hDlg, WPARAM wParam, const char* nodeStr, const uint32_t* rids);

const char* dccCommands[] = {
    "6F",   // forward full speed
    "4F",   // backward full speed
    "60",   // stop

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


const uint32_t rids3[] = {
    IDC_BUTTON_SF3,
    IDC_BUTTON_SB3,
    IDC_BUTTON_SS3,

    IDC_BUTTON_MP31,
    IDC_BUTTON_MP32,
    IDC_BUTTON_MP33,
    IDC_BUTTON_MP34,
    IDC_BUTTON_MP35,

    0
};
const uint32_t rids4[] = {
    IDC_BUTTON_SF4,
    IDC_BUTTON_SB4,
    IDC_BUTTON_SS4,

    IDC_BUTTON_MP41,
    IDC_BUTTON_MP42,
    IDC_BUTTON_MP43,
    IDC_BUTTON_MP44,
    IDC_BUTTON_MP45,

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

int g_scrollY = 0;

HWND hScrb0 = 0;
HWND hScrb1 = 0;
HWND hScrb2 = 0;
HWND hScrb3 = 0;

bool SendDCCSpeedCommand(HWND hDlg, int node, int pos);
bool InitSliders(HWND hDlg);

// Message handler for about box.
INT_PTR CALLBACK DCCMonitorDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    auto initSliders([](HWND hDlg)
    {
        int pos1 = 0;
        int pos2 = 0;
        int actpos = 0;

        hScrb0 = GetDlgItem(hDlg, idcScrollbars[0]);
        hScrb1 = GetDlgItem(hDlg, idcScrollbars[1]);
        hScrb2 = GetDlgItem(hDlg, idcScrollbars[2]);
        hScrb3 = GetDlgItem(hDlg, idcScrollbars[3]);
        SendMessage(hScrb0, TBM_SETRANGEMIN, 1, SLIDER_MIN);
        SendMessage(hScrb0, TBM_SETRANGEMAX, 1, SLIDER_MAX);
        SendMessage(hScrb0, TBM_SETPOS, 1, 0);
        SendMessage(hScrb1, TBM_SETRANGEMIN, 1, SLIDER_MIN);
        SendMessage(hScrb1, TBM_SETRANGEMAX, 1, SLIDER_MAX);
        SendMessage(hScrb1, TBM_SETPOS, 1, 0);
        SendMessage(hScrb2, TBM_SETRANGEMIN, 1, SLIDER_MIN);
        SendMessage(hScrb2, TBM_SETRANGEMAX, 1, SLIDER_MAX);
        SendMessage(hScrb2, TBM_SETPOS, 1, 0);
        SendMessage(hScrb3, TBM_SETRANGEMIN, 1, SLIDER_MIN);
        SendMessage(hScrb3, TBM_SETRANGEMAX, 1, SLIDER_MAX);
        SendMessage(hScrb3, TBM_SETPOS, 1, 0);

        ReadDCCDlgContent(hDlg, strDCCFeedback);

    });


    auto executeButtonCommand([](HWND hDlg, WPARAM wParam, const uint32_t* rids, HWND hScrb, const char* nodeStr)
    {
        if (wParam == rids[0])
            SendMessage(hScrb, TBM_SETPOS, 1, SLIDER_MAX);
        if (wParam == rids[1])
            SendMessage(hScrb, TBM_SETPOS, 1, SLIDER_MIN);
        if (wParam == rids[2])
            SendMessage(hScrb, TBM_SETPOS, 1, 0);

        ExecuteCommand(hDlg, wParam, nodeStr, rids);

    });

    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        {
            initSliders(hDlg);
        }
        return (INT_PTR)TRUE;
    case WM_HSCROLL:
    case WM_VSCROLL:
        {
            auto action = LOWORD(wParam);
            HWND hScroll = (HWND)lParam;
            int pos = 0;
            if (action == SB_PAGERIGHT ||
                action == SB_PAGELEFT ||
                action == SB_THUMBPOSITION ||
                action == SB_THUMBTRACK) {
                pos = (int)SendMessage(hScroll, TBM_GETPOS, 0, 0);
            } else
                break;

            int node = -1;
            if (hScroll == hScrb0) {
                node = 0;
            } else if (hScroll == hScrb1) {
                node = 1;
            } else if (hScroll == hScrb2) {
                node = 2;
            } else if (hScroll == hScrb3) {
                node = 3;
            }
            else
                break;

            SendDCCSpeedCommand(hDlg, node, pos);

        }
        break;
        
    case WM_COMMAND:
        {
            switch (wParam) {
            case IDC_BUTTON_SB1:
            case IDC_BUTTON_SF1:
            case IDC_BUTTON_SS1:
            case IDC_BUTTON_MP11:
            case IDC_BUTTON_MP12:
            case IDC_BUTTON_MP13:
            case IDC_BUTTON_MP14:
            case IDC_BUTTON_MP15:
                executeButtonCommand(hDlg, wParam, rids1, hScrb0, "0");
                break;
            case IDC_BUTTON_SB2:
            case IDC_BUTTON_SF2:
            case IDC_BUTTON_SS2:
            case IDC_BUTTON_MP21:
            case IDC_BUTTON_MP22:
            case IDC_BUTTON_MP23:
            case IDC_BUTTON_MP24:
            case IDC_BUTTON_MP25:
                executeButtonCommand(hDlg, wParam, rids2, hScrb1, "1");
                break;
            case IDC_BUTTON_SB3:
            case IDC_BUTTON_SF3:
            case IDC_BUTTON_SS3:
            case IDC_BUTTON_MP31:
            case IDC_BUTTON_MP32:
            case IDC_BUTTON_MP33:
            case IDC_BUTTON_MP34:
            case IDC_BUTTON_MP35:
                executeButtonCommand(hDlg, wParam, rids3, hScrb2, "2");
                break;
            case IDC_BUTTON_SB4:
            case IDC_BUTTON_SF4:
            case IDC_BUTTON_SS4:
            case IDC_BUTTON_MP41:
            case IDC_BUTTON_MP42:
            case IDC_BUTTON_MP43:
            case IDC_BUTTON_MP44:
            case IDC_BUTTON_MP45:
                executeButtonCommand(hDlg, wParam, rids4, hScrb3, "3");
                break;
            case IDC_BUTTON_SAVE:
                {
                    SaveDCCDlgContent(hDlg, strDCCFeedback);
                    InvalidateRect(hWndMain, NULL, TRUE);
                }
                break;
            case IDC_BUTTON_READ:
                {
                    ReadDCCDlgContent(hDlg, strDCCFeedback);
                    InvalidateRect(hWndMain, NULL, TRUE);
                }
                break;

            }
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
        }
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

bool SendDCCSpeedCommand(HWND hDlg, int node, int pos)
{
    char ipAddr[512];
    char dccCommand[3] = { '4', '0', 0 };
    char nodeStr[2] = { '0', 0 };

    nodeStr[0] = '0' + (uint8_t)node;
    if (pos > 0 && pos <= SLIDER_MAX) {
        dccCommand[0] = '6';
        if (pos < 10)
            dccCommand[1] = '0' + pos;
        else
            dccCommand[1] = 'A' + pos - 10;
    }
    else if (pos < 0 && pos >= SLIDER_MIN) {
        dccCommand[0] = '4';
        if (pos > -10)
            dccCommand[1] = '0' +(- pos);
        else
            dccCommand[1] = 'A' +(- pos - 10);
    }
    else if (pos == 0) {
        dccCommand[0] = '4';
        dccCommand[1] = '0';
    }
    else if (pos != 0)
        return false;
    SendDlgItemMessageA(hDlg,
        IDC_EDIT_IPADDRESS,
        WM_GETTEXT,
        (WPARAM)sizeof(ipAddr),
        (LPARAM)ipAddr);
    SendURL(ipAddr, nodeStr, dccCommand, strDCCFeedback);
    InvalidateRect(hWndMain, NULL, TRUE);
    return true;
}

