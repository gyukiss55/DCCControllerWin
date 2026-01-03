/*
* DCCMonitorDialog.cpp
*/

#include "resource.h"
#include "framework.h"

#include <format>
#include <iostream>
#include <string>
#include <string_view>

#include <CommCtrl.h>
#include "DCCController.h"
#include "DCCMonitorDialog.h"
#include "SendURL.h"
#include "RegistryIO.h"
#include "MacroImporter.h"

#define SLIDER_MIN -15
#define SLIDER_MAX 15

int32_t timeStampDCCCommands = 0;

bool SaveDCCDlgContent(HWND hDlg, std::string& feedback);
bool ReadDCCDlgContent(HWND hDlg, std::string& feedback);
bool ExecuteCommand(HWND hDlg, WPARAM wParam, uint32_t nodeCntrID, const char* nodeStr, const uint32_t* rids);
bool AppendTimeStamp(std::string& dccCommand);
bool ChangeStatus(HWND hDlg, std::string& strReceive);

const char* dccCommandsPushButton[] = {
    nullptr,
    "6F",   // forward full speed
    "4F",   // backward full speed
    "60",   // stop

    nullptr
};

const char* dccCommandsRadioButton[] = {
    "90",   // cross 0 left
    "A0",   // cross 0 right
    "91",   // cross 1 left
    "A1",   // cross 1 right
    "92",   // cross 2 left
    "A2",   // cross 2 right
    "93",   // cross 3 left
    "A3",   // cross 3 right
    "94",   // cross 4 left
    "A4",   // cross 4 right

    nullptr
};

#define CHANNEL_INDEX               0
#define SPEED_DIRECT_FIRST_INDEX    1
#define SPEED_DIRECT_LAST_INDEX     3
#define SPEED_DEC_INDEX             4
#define SPEED_INC_INDEX             5
#define EXPL_COMM_INDEX             6
#define EXPL_EDIT_INDEX             7
#define FUNC_FIRST_INDEX            8
#define FUNC_LAST_INDEX             33


const uint32_t rids1[] = {
    0,

    IDC_BUTTON_SF1,
    IDC_BUTTON_SB1,
    IDC_BUTTON_SS1,

    IDC_BUTTON_MINUS1,
    IDC_BUTTON_PLUS1,

    IDC_BUTTON_COMEXEC1,

    IDC_EDIT_COMMAND1,

    IDC_CHECK1,
    IDC_CHECK2,
    IDC_CHECK3,
    IDC_CHECK4,
    IDC_CHECK5,
    IDC_CHECK6,
    IDC_CHECK7,
    IDC_CHECK8,
    IDC_CHECK9,
    IDC_CHECK10,
    IDC_CHECK11,
    IDC_CHECK12,
    IDC_CHECK13,
    IDC_CHECK14,
    IDC_CHECK15,
    IDC_CHECK16,
    IDC_CHECK17,
    IDC_CHECK18,
    IDC_CHECK19,
    IDC_CHECK20,
    IDC_CHECK21,
    IDC_CHECK22,
    IDC_CHECK23,
    IDC_CHECK24,
    IDC_CHECK25,
    IDC_CHECK101,
    IDC_CHECK102,
    IDC_CHECK103,
    IDC_CHECK104,
    IDC_CHECK105,

    0
};

const uint32_t rids2[] = {
    1, 

    IDC_BUTTON_SF2,
    IDC_BUTTON_SB2,
    IDC_BUTTON_SS2,

    IDC_BUTTON_MINUS2,
    IDC_BUTTON_PLUS2,

    IDC_BUTTON_COMEXEC2,

    IDC_EDIT_COMMAND2,

    IDC_CHECK26,
    IDC_CHECK27,
    IDC_CHECK28,
    IDC_CHECK29,
    IDC_CHECK30,
    IDC_CHECK31,
    IDC_CHECK32,
    IDC_CHECK33,
    IDC_CHECK34,
    IDC_CHECK35,
    IDC_CHECK36,
    IDC_CHECK37,
    IDC_CHECK38,
    IDC_CHECK39,
    IDC_CHECK40,
    IDC_CHECK41,
    IDC_CHECK42,
    IDC_CHECK43,
    IDC_CHECK44,
    IDC_CHECK45,
    IDC_CHECK46,
    IDC_CHECK47,
    IDC_CHECK48,
    IDC_CHECK49,
    IDC_CHECK50,
    IDC_CHECK106,
    IDC_CHECK107,
    IDC_CHECK108,
    IDC_CHECK109,
    IDC_CHECK110,

    0
};


const uint32_t rids3[] = {
    2,

    IDC_BUTTON_SF3,
    IDC_BUTTON_SB3,
    IDC_BUTTON_SS3,

    IDC_BUTTON_MINUS3,
    IDC_BUTTON_PLUS3,

    IDC_BUTTON_COMEXEC3,

    IDC_EDIT_COMMAND3,

    IDC_CHECK51,
    IDC_CHECK52,
    IDC_CHECK53,
    IDC_CHECK54,
    IDC_CHECK55,
    IDC_CHECK56,
    IDC_CHECK57,
    IDC_CHECK58,
    IDC_CHECK59,
    IDC_CHECK60,
    IDC_CHECK61,
    IDC_CHECK62,
    IDC_CHECK63,
    IDC_CHECK64,
    IDC_CHECK65,
    IDC_CHECK66,
    IDC_CHECK67,
    IDC_CHECK68,
    IDC_CHECK69,
    IDC_CHECK70,
    IDC_CHECK71,
    IDC_CHECK72,
    IDC_CHECK73,
    IDC_CHECK74,
    IDC_CHECK75,
    IDC_CHECK111,
    IDC_CHECK112,
    IDC_CHECK113,
    IDC_CHECK114,
    IDC_CHECK115,
    IDC_CHECK116,
    IDC_CHECK117,
    IDC_CHECK118,
    IDC_CHECK119,
    IDC_CHECK120,

    0
};
const uint32_t rids4[] = {
    3,

    IDC_BUTTON_SF4,
    IDC_BUTTON_SB4,
    IDC_BUTTON_SS4,

    IDC_BUTTON_MINUS4,
    IDC_BUTTON_PLUS4,

    IDC_BUTTON_COMEXEC4,

    IDC_EDIT_COMMAND4,

    IDC_CHECK76,
    IDC_CHECK77,
    IDC_CHECK78,
    IDC_CHECK79,
    IDC_CHECK80,
    IDC_CHECK81,
    IDC_CHECK82,
    IDC_CHECK83,
    IDC_CHECK84,
    IDC_CHECK85,
    IDC_CHECK86,
    IDC_CHECK87,
    IDC_CHECK88,
    IDC_CHECK89,
    IDC_CHECK90,
    IDC_CHECK91,
    IDC_CHECK92,
    IDC_CHECK93,
    IDC_CHECK94,
    IDC_CHECK95,
    IDC_CHECK96,
    IDC_CHECK97,
    IDC_CHECK98,
    IDC_CHECK99,
    IDC_CHECK100,
    IDC_CHECK116,
    IDC_CHECK117,
    IDC_CHECK118,
    IDC_CHECK119,
    IDC_CHECK120,

    0
};

const uint32_t rids5[] = {
    IDC_RADIO_SWITCHLEFT_1_1,
    IDC_RADIO_SWITCHRIGHT_1_1,
    IDC_RADIO_SWITCHLEFT_1_2,
    IDC_RADIO_SWITCHRIGHT_1_2,
    IDC_RADIO_SWITCHLEFT_1_3,
    IDC_RADIO_SWITCHRIGHT_1_3,
    IDC_RADIO_SWITCHLEFT_1_4,
    IDC_RADIO_SWITCHRIGHT_1_4,
    IDC_RADIO_SWITCHLEFT_1_5,
    IDC_RADIO_SWITCHRIGHT_1_5
};

const uint32_t rids6[] = {
    IDC_RADIO_SWITCHLEFT_2_1,
    IDC_RADIO_SWITCHRIGHT_2_1,
    IDC_RADIO_SWITCHLEFT_2_2,
    IDC_RADIO_SWITCHRIGHT_2_2,
    IDC_RADIO_SWITCHLEFT_2_3,
    IDC_RADIO_SWITCHRIGHT_2_3,
    IDC_RADIO_SWITCHLEFT_2_4,
    IDC_RADIO_SWITCHRIGHT_2_4,
    IDC_RADIO_SWITCHLEFT_2_5,
    IDC_RADIO_SWITCHRIGHT_2_5
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

HWND hComboBox = 0;

int actualSpeed[4] = { 0, 0, 0, 0 };
int actualFunction[4*6] = {
        0, 0, 0, 0, // F0- F4
        0, 0, 0, 0, // F5- F8
        0, 0, 0, 0, // F9- F12
        0, 0, 0, 0, // F13- F20
        0, 0, 0, 0, // F21- F28
        0, 0, 0, 0  // F29- F35
    };

bool SendDCCSpeedCommand(HWND hDlg, uint32_t nodeCntrId, int node, int pos);
bool InitSliders(HWND hDlg);

std::string ReadIPAddress(HWND hDlg)
{
    char ipAddr[20];
    SendDlgItemMessageA(hDlg,
        IDC_EDIT_IPADDRESS,
        WM_GETTEXT,
        (WPARAM)sizeof(ipAddr),
        (LPARAM)ipAddr);
    return std::string(ipAddr);
}


bool ExecuteEmergencyStopCommand(HWND hDlg, WPARAM wParam)
{
    std::string ipAddress = ReadIPAddress(hDlg);

    std::string nodeStr = "0";
    std::string dccCommand = "0061";
    AppendTimeStamp(dccCommand);

    std::string strReceive;

    SendURL(ipAddress.c_str(), nodeStr.c_str(), dccCommand.c_str(), strReceive, strDCCFeedback);
    ChangeStatus(hDlg, strReceive);

    for (int ch = 0; ch < 4; ch++) {
        SendMessage(GetDlgItem(hDlg, idcScrollbars[ch]), TBM_SETPOS, 1, 0);
        actualSpeed[ch] = 0;
    }
    
    InvalidateRect(hWndMain, NULL, TRUE);
    return true;
}


void InitCheckBoxes(HWND hDlg)
{
    std::wstring str;
    HWND hWndControl = 0;
    uint8_t j = 1;

    const uint32_t* cbidPtrArray[] = { &rids1[0] , &rids2[0], &rids3[0], &rids4[0], nullptr};

    for (uint32_t j = 0; j < 4; j++) {
        const uint32_t* cbidPtr = cbidPtrArray[j] + 8;
        for (uint32_t i = 0; i < 30; i++)
        {
            str = std::wstring(L"F");
            str += std::to_wstring(i);
            str += std::wstring(L"/");
            str += std::to_wstring(j + 1);
            hWndControl = GetDlgItem(hDlg, cbidPtr[i]);
            SetWindowText(hWndControl, str.c_str());
        }
    }

}

void InitComboBox(HWND hDlg)
{
    MacroImporter* imp = MacroImporter::GetInsance();
    if (imp) {
        imp->Import();
        HWND hWndCombo = GetDlgItem(hDlg, IDC_COMBO_MACRO);
        for (int i = 0; i < imp->GetContainer().Get().size(); ++i) {
            std::wstring name = imp->GetContainer().Get()[i].GetName();
            SendMessage(hWndCombo, CB_ADDSTRING, 0, (LPARAM)name.c_str ());
        }
    }
}


void ExecuteMacro(HWND hDlg)
{
    MacroImporter* imp = MacroImporter::GetInsance();
    if (imp) {
        HWND hWndCombo = GetDlgItem(hDlg, IDC_COMBO_MACRO);
        WPARAM selIndex = (WPARAM)SendMessage(hWndCombo, CB_GETCURSEL, 0, 0);

        if (selIndex != CB_ERR) {
            wchar_t buffer[255];
            memset(buffer, 0, sizeof(buffer));

            SendMessageW(hWndCombo, CB_GETLBTEXT, selIndex, (LPARAM)buffer);
            imp->ExecuteMacro(buffer);
            
        }

    }
}


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



    auto executePushButtonCommand([](HWND hDlg, WPARAM wParam, uint32_t nodeCntrId, const uint32_t* rids, HWND hScrb, const char* nodeStr)
    {
        if (wParam == rids[SPEED_DIRECT_FIRST_INDEX]) {
            SendMessage(hScrb, TBM_SETPOS, 1, SLIDER_MAX);
            actualSpeed[rids[CHANNEL_INDEX]] = 15;
        }
        if (wParam == rids[SPEED_DIRECT_FIRST_INDEX + 1]) {
            SendMessage(hScrb, TBM_SETPOS, 1, SLIDER_MIN);
            actualSpeed[rids[CHANNEL_INDEX]] = -15;
        }
        if (wParam == rids[SPEED_DIRECT_FIRST_INDEX + 2]) {
            SendMessage(hScrb, TBM_SETPOS, 1, 0);
            actualSpeed[rids[CHANNEL_INDEX]] = 0;
        }

        ExecuteCommand(hDlg, wParam, nodeCntrId, nodeStr, rids);

        if (wParam == rids[SPEED_DEC_INDEX] || rids[SPEED_INC_INDEX])
            SendMessage(hScrb, TBM_SETPOS, 1, actualSpeed[rids[CHANNEL_INDEX]]);

    });

    auto executeRadioButtonCommand([](HWND hDlg, WPARAM wParam, uint32_t nodeCntrId, const uint32_t* rids, const char* nodeStr)
        {
             ExecuteCommand(hDlg, wParam, nodeCntrId, nodeStr, rids);

        });

    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        {
            initSliders(hDlg);
            InitCheckBoxes(hDlg);
            InitComboBox(hDlg);


            SendDlgItemMessageA(hDlg,
                IDC_CHECK_SENSOR_1_1,
                BM_SETCHECK,
                FALSE,
                0);

            SendDlgItemMessageA(hDlg,
                IDC_CHECK_SENSOR_1_2,
                BM_SETCHECK,
                FALSE,
                0);
        }
        return (INT_PTR)TRUE;
    case WM_HSCROLL:
    case WM_VSCROLL:
        {
            auto action = LOWORD(wParam);
            HWND hScroll = (HWND)lParam;
            int pos = 0;
            if (action == SB_PAGELEFT ||
                action == SB_PAGERIGHT ||
                action == SB_LINELEFT ||
                action == SB_LINERIGHT ||
                action == SB_THUMBPOSITION ||
                action == SB_THUMBTRACK) {
                pos = (int)SendMessage(hScroll, TBM_GETPOS, 0, 0);
            } else
                break;

            int node = -1;
            uint32_t nodeCtrlID = 0;
            if (hScroll == hScrb0) {
                node = 0;
                nodeCtrlID = IDC_EDIT_NODEADDRESS1;
            } else if (hScroll == hScrb1) {
                node = 1;
                nodeCtrlID = IDC_EDIT_NODEADDRESS2;
            } else if (hScroll == hScrb2) {
                node = 2;
                nodeCtrlID = IDC_EDIT_NODEADDRESS3;
            } else if (hScroll == hScrb3) {
                node = 3;
                nodeCtrlID = IDC_EDIT_NODEADDRESS4;
            }
            else
                break;

            SendDCCSpeedCommand(hDlg, nodeCtrlID, node, pos);

        }
        break;
        
    case WM_COMMAND:
        {
            switch (wParam) {
            case IDC_EMERGENCYSTOP:
                ExecuteEmergencyStopCommand(hDlg, wParam);
            case IDC_BUTTON_SB1:
            case IDC_BUTTON_SF1:
            case IDC_BUTTON_SS1:
            case IDC_BUTTON_COMEXEC1:
            case IDC_BUTTON_MINUS1:
            case IDC_BUTTON_PLUS1:
            case IDC_CHECK1:
            case IDC_CHECK2:
            case IDC_CHECK3:
            case IDC_CHECK4:
            case IDC_CHECK5:
            case IDC_CHECK6:
            case IDC_CHECK7:
            case IDC_CHECK8:
            case IDC_CHECK9:
            case IDC_CHECK10:
            case IDC_CHECK11:
            case IDC_CHECK12:
            case IDC_CHECK13:
            case IDC_CHECK14:
            case IDC_CHECK15:
            case IDC_CHECK16:
            case IDC_CHECK17:
            case IDC_CHECK18:
            case IDC_CHECK19:
            case IDC_CHECK20:
            case IDC_CHECK21:
            case IDC_CHECK22:
            case IDC_CHECK23:
            case IDC_CHECK24:
            case IDC_CHECK25:
            case IDC_CHECK101:
            case IDC_CHECK102:
            case IDC_CHECK103:
            case IDC_CHECK104:
            case IDC_CHECK105:
                executePushButtonCommand(hDlg, wParam, IDC_EDIT_NODEADDRESS1, rids1, hScrb0, "0");
                break;
            case IDC_BUTTON_SB2:
            case IDC_BUTTON_SF2:
            case IDC_BUTTON_SS2:
            case IDC_BUTTON_COMEXEC2:
            case IDC_BUTTON_MINUS2:
            case IDC_BUTTON_PLUS2:
            case IDC_CHECK26:
            case IDC_CHECK27:
            case IDC_CHECK28:
            case IDC_CHECK29:
            case IDC_CHECK30:
            case IDC_CHECK31:
            case IDC_CHECK32:
            case IDC_CHECK33:
            case IDC_CHECK34:
            case IDC_CHECK35:
            case IDC_CHECK36:
            case IDC_CHECK37:
            case IDC_CHECK38:
            case IDC_CHECK39:
            case IDC_CHECK40:
            case IDC_CHECK41:
            case IDC_CHECK42:
            case IDC_CHECK43:
            case IDC_CHECK44:
            case IDC_CHECK45:
            case IDC_CHECK46:
            case IDC_CHECK47:
            case IDC_CHECK48:
            case IDC_CHECK49:
            case IDC_CHECK50:
            case IDC_CHECK106:
            case IDC_CHECK107:
            case IDC_CHECK108:
            case IDC_CHECK109:
            case IDC_CHECK110:
                executePushButtonCommand(hDlg, wParam, IDC_EDIT_NODEADDRESS2, rids2, hScrb1, "1");
                break;
            case IDC_BUTTON_SB3:
            case IDC_BUTTON_SF3:
            case IDC_BUTTON_SS3:
            case IDC_BUTTON_COMEXEC3:
            case IDC_BUTTON_MINUS3:
            case IDC_BUTTON_PLUS3:
            case IDC_CHECK51:
            case IDC_CHECK52:
            case IDC_CHECK53:
            case IDC_CHECK54:
            case IDC_CHECK55:
            case IDC_CHECK56:
            case IDC_CHECK57:
            case IDC_CHECK58:
            case IDC_CHECK59:
            case IDC_CHECK60:
            case IDC_CHECK61:
            case IDC_CHECK62:
            case IDC_CHECK63:
            case IDC_CHECK64:
            case IDC_CHECK65:
            case IDC_CHECK66:
            case IDC_CHECK67:
            case IDC_CHECK68:
            case IDC_CHECK69:
            case IDC_CHECK70:
            case IDC_CHECK71:
            case IDC_CHECK72:
            case IDC_CHECK73:
            case IDC_CHECK74:
            case IDC_CHECK75:
            case IDC_CHECK111:
            case IDC_CHECK112:
            case IDC_CHECK113:
            case IDC_CHECK114:
            case IDC_CHECK115:
                executePushButtonCommand(hDlg, wParam, IDC_EDIT_NODEADDRESS3, rids3, hScrb2, "2");
                break;
            case IDC_BUTTON_SB4:
            case IDC_BUTTON_SF4:
            case IDC_BUTTON_SS4:
            case IDC_BUTTON_COMEXEC4:
            case IDC_BUTTON_MINUS4:
            case IDC_BUTTON_PLUS4:
            case IDC_CHECK76:
            case IDC_CHECK77:
            case IDC_CHECK78:
            case IDC_CHECK79:
            case IDC_CHECK80:
            case IDC_CHECK81:
            case IDC_CHECK82:
            case IDC_CHECK83:
            case IDC_CHECK84:
            case IDC_CHECK85:
            case IDC_CHECK86:
            case IDC_CHECK87:
            case IDC_CHECK88:
            case IDC_CHECK89:
            case IDC_CHECK90:
            case IDC_CHECK91:
            case IDC_CHECK92:
            case IDC_CHECK93:
            case IDC_CHECK94:
            case IDC_CHECK95:
            case IDC_CHECK96:
            case IDC_CHECK97:
            case IDC_CHECK98:
            case IDC_CHECK99:
            case IDC_CHECK100:
            case IDC_CHECK116:
            case IDC_CHECK117:
            case IDC_CHECK118:
            case IDC_CHECK119:
            case IDC_CHECK120:
                executePushButtonCommand(hDlg, wParam, IDC_EDIT_NODEADDRESS4, rids4, hScrb3, "3");
                break;

            case IDC_RADIO_SWITCHLEFT_1_1:
            case IDC_RADIO_SWITCHRIGHT_1_1:
            case IDC_RADIO_SWITCHLEFT_1_2:
            case IDC_RADIO_SWITCHRIGHT_1_2:
            case IDC_RADIO_SWITCHLEFT_1_3:
            case IDC_RADIO_SWITCHRIGHT_1_3:
            case IDC_RADIO_SWITCHLEFT_1_4:
            case IDC_RADIO_SWITCHRIGHT_1_4:
            case IDC_RADIO_SWITCHLEFT_1_5:
            case IDC_RADIO_SWITCHRIGHT_1_5:
                executeRadioButtonCommand(hDlg, wParam, IDC_EDIT_NODEADDRESS5, rids5, "4");
                break;

            case IDC_RADIO_SWITCHLEFT_2_1:
            case IDC_RADIO_SWITCHRIGHT_2_1:
            case IDC_RADIO_SWITCHLEFT_2_2:
            case IDC_RADIO_SWITCHRIGHT_2_2:
            case IDC_RADIO_SWITCHLEFT_2_3:
            case IDC_RADIO_SWITCHRIGHT_2_3:
            case IDC_RADIO_SWITCHLEFT_2_4:
            case IDC_RADIO_SWITCHRIGHT_2_4:
            case IDC_RADIO_SWITCHLEFT_2_5:
            case IDC_RADIO_SWITCHRIGHT_2_5:
                executeRadioButtonCommand(hDlg, wParam, IDC_EDIT_NODEADDRESS6, rids6, "5");
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
            case IDC_BUTTON_EXECMAC:
                {
                    ExecuteMacro(hDlg);
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

uint32_t HexaStringConvert(const char* str)
{
    uint32_t ret = 0;
    for (int i = 0; i < strlen(str); ++i) {
        ret <<=  4;
        if (str[i] >= '0' && str[i] <= '9')
            ret += str[i] - '0';
        else if (str[i] >= 'A' && str[i] <= 'F')
            ret += str[i] + 10 - 'A';
        else if (str[i] >= 'a' && str[i] <= 'f')
            ret += str[i] + 10 - 'a';
        else
            break;
    }
    return ret;
}

bool ChangeStatus(HWND hDlg, std::string& strReceive)
{
    std::string::size_type pos = 0;
    for (; pos < strReceive.size(); ) {
        pos = strReceive.find('>', pos + 1);
        if (pos < strReceive.size() && (strReceive.substr (pos + 1, 5) == std::string("Node:"))) {
            std::string node = strReceive.substr(pos + 6, 2);
            std::string type = strReceive.substr(pos + 14, 1);
            std::string cross = strReceive.substr(pos + 22, 4);
            std::string sensor = strReceive.substr(pos + 34, 4);
            OutputDebugStringA("\nChangeStatus - Node:");
            OutputDebugStringA(node.c_str ());
            OutputDebugStringA(" type:");
            OutputDebugStringA(type.c_str ());
            OutputDebugStringA(" cross:");
            OutputDebugStringA(cross.c_str ());
            OutputDebugStringA(" sensor:");
            OutputDebugStringA(sensor.c_str ());

            char nodeID[8];

            SendDlgItemMessageA(hDlg,
                IDC_EDIT_NODEADDRESS5,
                WM_GETTEXT,
                (WPARAM)sizeof(nodeID),
                (LPARAM)nodeID);

 
            int nodeFound = -1;
            if (node == std::string(nodeID)) {
                nodeFound = 0;
            }
            else {
                SendDlgItemMessageA(hDlg,
                    IDC_EDIT_NODEADDRESS6,
                    WM_GETTEXT,
                    (WPARAM)sizeof(nodeID),
                    (LPARAM)nodeID);
                if (node == std::string(nodeID))
                    nodeFound = 1;
            }

            if (nodeFound < 0)
                return false;

            int sensorValue = HexaStringConvert(sensor.c_str());

            if (nodeFound == 0) {

                if (sensorValue & 0x01)
                    SendDlgItemMessageA(hDlg,
                        IDC_CHECK_SENSOR_1_1,
                        BM_SETCHECK,
                        FALSE,
                        0);

                else SendDlgItemMessageA(hDlg,
                        IDC_CHECK_SENSOR_1_1,
                        BM_SETCHECK,
                        TRUE,
                        0);

                if (sensorValue & 0x02)
                    SendDlgItemMessageA(hDlg,
                        IDC_CHECK_SENSOR_1_2,
                        BM_SETCHECK,
                        FALSE,
                        0);

                else SendDlgItemMessageA(hDlg,
                    IDC_CHECK_SENSOR_1_2,
                    BM_SETCHECK,
                    TRUE,
                    0);

                if (sensorValue & 0x04)
                    SendDlgItemMessageA(hDlg,
                        IDC_CHECK_SENSOR_1_3,
                        BM_SETCHECK,
                        FALSE,
                        0);

                else SendDlgItemMessageA(hDlg,
                    IDC_CHECK_SENSOR_1_3,
                    BM_SETCHECK,
                    TRUE,
                    0);

                if (sensorValue & 0x08)
                    SendDlgItemMessageA(hDlg,
                        IDC_CHECK_SENSOR_1_4,
                        BM_SETCHECK,
                        FALSE,
                        0);

                else SendDlgItemMessageA(hDlg,
                    IDC_CHECK_SENSOR_1_4,
                    BM_SETCHECK,
                    TRUE,
                    0);

                if (sensorValue & 0x10)
                    SendDlgItemMessageA(hDlg,
                        IDC_CHECK_SENSOR_1_5,
                        BM_SETCHECK,
                        FALSE,
                        0);

                else SendDlgItemMessageA(hDlg,
                    IDC_CHECK_SENSOR_1_5,
                    BM_SETCHECK,
                    TRUE,
                    0);

            }
            else {

                if (sensorValue & 0x01)
                    SendDlgItemMessageA(hDlg,
                        IDC_CHECK_SENSOR_2_1,
                        BM_SETCHECK,
                        FALSE,
                        0);

                else SendDlgItemMessageA(hDlg,
                    IDC_CHECK_SENSOR_2_1,
                    BM_SETCHECK,
                    TRUE,
                    0);

                if (sensorValue & 0x02)
                    SendDlgItemMessageA(hDlg,
                        IDC_CHECK_SENSOR_2_2,
                        BM_SETCHECK,
                        FALSE,
                        0);

                else SendDlgItemMessageA(hDlg,
                    IDC_CHECK_SENSOR_2_2,
                    BM_SETCHECK,
                    TRUE,
                    0);

                if (sensorValue & 0x04)
                    SendDlgItemMessageA(hDlg,
                        IDC_CHECK_SENSOR_2_3,
                        BM_SETCHECK,
                        FALSE,
                        0);

                else SendDlgItemMessageA(hDlg,
                    IDC_CHECK_SENSOR_2_3,
                    BM_SETCHECK,
                    TRUE,
                    0);

                if (sensorValue & 0x08)
                    SendDlgItemMessageA(hDlg,
                        IDC_CHECK_SENSOR_2_4,
                        BM_SETCHECK,
                        FALSE,
                        0);

                else SendDlgItemMessageA(hDlg,
                    IDC_CHECK_SENSOR_2_4,
                    BM_SETCHECK,
                    TRUE,
                    0);

                if (sensorValue & 0x10)
                    SendDlgItemMessageA(hDlg,
                        IDC_CHECK_SENSOR_2_5,
                        BM_SETCHECK,
                        FALSE,
                        0);

                else SendDlgItemMessageA(hDlg,
                    IDC_CHECK_SENSOR_2_5,
                    BM_SETCHECK,
                    TRUE,
                    0);

            }

        }
    }
    return true;
}

struct MapID2Command {
    uint8_t idFrom;
    uint8_t idTo;
    uint8_t command1;
    uint8_t commandLength;
    uint8_t* mask;
};

uint8_t mask_0_4[] = { 0x10, 1, 2, 4, 8 };
uint8_t mask_5_12[] = { 1, 2, 4, 8 };
uint8_t mask_13_28[] = {1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80};

MapID2Command commandMaps[] = {
    {0, 4, 0x80, 1, mask_0_4 },
    {5, 8, 0xB0, 1, mask_5_12 },
    {9, 12, 0xA0, 1, mask_5_12 },
    {13, 20, 0xDE, 2, mask_13_28 },
    {21, 28, 0xDF, 2, mask_13_28 },

    {0, 0, 0, 0, nullptr }
};

std::string ToggleFunction(int channel, int functionID, int state)
{
    // f0 - f4 - 0x80-0x9F      1 0 0 F0 F4 F3 F2 F1
    // f5 - f12 - 0xA0-0xBF     1 0 1 1 F8 F7 F6 F5
    //                          1 0 1 0 F12 F11 F10 F9
    // f13 - f20 - 0xDE ,  F20 F19 F18 F17 F16 F15 F14 F13 (2 byte command !)
    // f21 - f28 - 0xDF ,  F28 F27 F26 F25 F24 F23 F22 F21 (2 byte command !)
    // f29 - f36 - 0xD8 ,  F28 F27 F26 F25 F24 F23 F22 F21 (2 byte command !)
    // f37 - f44 - 0xD9 ,  F28 F27 F26 F25 F24 F23 F22 F21 (2 byte command !)
    // f45 - f52 - 0xDA ,  F28 F27 F26 F25 F24 F23 F22 F21 (2 byte command !)
    // f53 - f60 - 0xDB ,  F28 F27 F26 F25 F24 F23 F22 F21 (2 byte command !)
    // f61 - f68 - 0xDC ,  F28 F27 F26 F25 F24 F23 F22 F21 (2 byte command !)

    std::string ret;
    int value = 0;
    for (int i = 0; commandMaps[i].commandLength > 0; i++) {
        if (commandMaps[i].idFrom <= functionID && commandMaps[i].idTo >= functionID) {
            char postCommand[10];
            value = actualFunction[channel*4+i];
            if (state)
                value |= commandMaps[i].mask [functionID - commandMaps[i].idFrom];
            else
                value &= (commandMaps[i].mask [functionID - commandMaps[i].idFrom]) ^ 0xff;
            actualFunction[channel * 4 + i] = value;
            if (commandMaps[i].commandLength == 2) {
                sprintf_s(postCommand, "%02X%02X", commandMaps[i].command1, value);
            }
            else {
                sprintf_s(postCommand, "%02X", value + commandMaps[i].command1);
            }
            ret = postCommand;
            break;
        }
    }

    return ret;
}


bool ExecuteCommand(HWND hDlg, WPARAM wParam, uint32_t nodeCntrId, const char* nodeStr, const uint32_t * rids)
{
//    HWND hCtrl = GetDlgItem(hDlg, rids[SPEED_DIRECT_FIRST_INDEX]);
//    LONG style = GetWindowLongA(hCtrl, GWL_STYLE);
//    LONG extStyle = GetWindowLongA(hCtrl, GWL_EXSTYLE);

    std::string dccCommandPart2;
    for (int i = SPEED_DIRECT_FIRST_INDEX; i <= SPEED_DIRECT_LAST_INDEX; ++i) {
        if (rids[i] == wParam)
            dccCommandPart2 = dccCommandsPushButton[i];
    }
    for (int i = FUNC_FIRST_INDEX; i <= FUNC_LAST_INDEX; ++i) {
        if (rids[i] == wParam) {
            LRESULT state = SendDlgItemMessageA(hDlg, wParam, BM_GETSTATE, 0, 0);
            dccCommandPart2 = ToggleFunction(rids[CHANNEL_INDEX], i - FUNC_FIRST_INDEX, (state & BST_CHECKED));
        }
    }


    std::string ipAddress = ReadIPAddress(hDlg);

    char nodeID[10];
    SendDlgItemMessageA(hDlg,
        nodeCntrId,
        WM_GETTEXT,
        (WPARAM)sizeof(nodeID),
        (LPARAM)nodeID);
    std::string dccCommand(nodeID);
    if (dccCommand.length() == 0)
        return false;
    if (dccCommand.length() == 1)
        dccCommand.insert(dccCommand.begin(), '0');

    if (dccCommandPart2.length() == 0) {
        if (wParam == rids[SPEED_DEC_INDEX]) {
            if (actualSpeed[rids[CHANNEL_INDEX]] > -14)
                actualSpeed[rids[CHANNEL_INDEX]]--;
        }
        else if (wParam == rids[SPEED_INC_INDEX]) {
            if (actualSpeed[rids[CHANNEL_INDEX]] < 14)
                actualSpeed[rids[CHANNEL_INDEX]]++;

        }
        if (wParam == rids[SPEED_DEC_INDEX] || wParam == rids[SPEED_INC_INDEX]) {
            if (actualSpeed[rids[CHANNEL_INDEX]] == 0) {
                dccCommand += "60";
            }
            else if (actualSpeed[rids[CHANNEL_INDEX]] > 0) {
                char expCommand[10];
                sprintf_s(expCommand, "%02X", (actualSpeed[rids[CHANNEL_INDEX]] + 1 + 0x60));
                dccCommand += expCommand;
            }
            else {
                char expCommand[10];
                sprintf_s(expCommand, "%02X", (-actualSpeed[rids[CHANNEL_INDEX]] + 1 + 0x40));
                dccCommand += expCommand;
            }
        }
        else if (wParam == rids[EXPL_COMM_INDEX]) {
            char expCommand[100];
            memset(expCommand, 0, sizeof(expCommand));
            SendDlgItemMessageA(hDlg,
                rids[EXPL_EDIT_INDEX],
                WM_GETTEXT,
                (WPARAM)sizeof(expCommand - 1),
                (LPARAM)expCommand);
            dccCommand += expCommand;
        }
        else return false;
    } else 
        dccCommand += dccCommandPart2;

    AppendTimeStamp(dccCommand);

    std::string strReceive;

    SendURL(ipAddress.c_str(), nodeStr, dccCommand.c_str(), strReceive, strDCCFeedback);
    ChangeStatus(hDlg, strReceive);

    InvalidateRect(hWndMain, NULL, TRUE);
    return true;
}

bool SendDCCSpeedCommand(HWND hDlg, uint32_t nodeCntrId, int node, int pos)
{
    char dccCommandPart2[3] = { '4', '0', 0 };
    char nodeStr[2] = { '0', 0 };

    nodeStr[0] = '0' + (uint8_t)node;
    if (pos > 0 && pos <= SLIDER_MAX) {
        dccCommandPart2[0] = '6';
        if (pos < 10)
            dccCommandPart2[1] = '0' + pos;
        else
            dccCommandPart2[1] = 'A' + pos - 10;
    }
    else if (pos < 0 && pos >= SLIDER_MIN) {
        dccCommandPart2[0] = '4';
        if (pos > -10)
            dccCommandPart2[1] = '0' +(- pos);
        else
            dccCommandPart2[1] = 'A' +(- pos - 10);
    }
    else if (pos == 0) {
        dccCommandPart2[0] = '6';
        dccCommandPart2[1] = '0';
    }
    else if (pos != 0)
        return false;
  
    std::string ipAddress = ReadIPAddress(hDlg);

    char nodeID[10];
    SendDlgItemMessageA(hDlg,
        nodeCntrId,
        WM_GETTEXT,
        (WPARAM)sizeof(nodeID),
        (LPARAM)nodeID);

    std::string dccCommand(nodeID);
    dccCommand += dccCommandPart2;

    AppendTimeStamp(dccCommand);

    std::string strReceive;

    SendURL(ipAddress.c_str(), nodeStr, dccCommand.c_str(), strReceive, strDCCFeedback);
    InvalidateRect(hWndMain, NULL, TRUE);
    return true;
}

bool AppendTimeStamp(std::string& dccCommand)
{
    char buffer[32];
    ++timeStampDCCCommands;
    sprintf_s(buffer, "%04X", timeStampDCCCommands);
    dccCommand += buffer;
    return true;
}

