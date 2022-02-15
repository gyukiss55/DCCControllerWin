#include <iostream>
#include <string>
#include <curl/curl.h>

#include "resource.h"
#include "SendURL.h"

#define _URL_COMMAND_BCSTOP_                    "192.168.4.1/bs0"
#define _URL_COMMAND_BCSPEED1FORWARD_           "192.168.4.1/bs1f"
#define _URL_COMMAND_BCSPEED1BACKWARD_          "192.168.4.1/bs1b"
#define _URL_COMMAND_BCLIGHTFORWARD_            "192.168.4.1/blf"
#define _URL_COMMAND_BCLIGHTBACKWARD_           "192.168.4.1/blb"
#define _URL_COMMAND_ADDRESS3SPEED1FORWARD_     "192.168.4.1/a3s1f"
#define _URL_COMMAND_ADDRESS3SPEED1BACKWARD_    "192.168.4.1/a3s1b"
#define _URL_COMMAND_EXPLICITECOMMAND_          "192.168.4.1/ec"


CURL* curl;
std::string strFeedback;


void initURL()
{
    curl = curl_easy_init();
}

void closeURL()
{
    curl_easy_cleanup(curl);

}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void send_URL(const char* url)
{
    CURLcode res;

    if (curl != nullptr) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strFeedback);
        res = curl_easy_perform(curl);
        if (res == CURLE_OPERATION_TIMEDOUT)
            strFeedback = "CURLE_OPERATION_TIMEDOUT";
    }
}

void sendURL(int command, const char* strAppend, std::string& feedback)
{
    strFeedback = "";
    switch (command) {
    case IDC_BUTTON_BS0:
        send_URL(_URL_COMMAND_BCSTOP_);
        break;
    case IDC_BUTTON_BS1F:
        send_URL(_URL_COMMAND_BCSPEED1FORWARD_);
        break;
    case IDC_BUTTON_BS1B:
        send_URL(_URL_COMMAND_BCSPEED1BACKWARD_);
        break;
    case IDC_BUTTON_BLF:
        send_URL(_URL_COMMAND_BCLIGHTFORWARD_);
        break;
    case IDC_BUTTON_BLB:
        send_URL(_URL_COMMAND_BCLIGHTBACKWARD_);
        break;
    case IDC_BUTTON_A3S1F:
        send_URL(_URL_COMMAND_ADDRESS3SPEED1FORWARD_);
        break;
    case IDC_BUTTON_A3S1B:
        send_URL(_URL_COMMAND_ADDRESS3SPEED1BACKWARD_);
        break;
    case IDC_BUTTON_SENDEXP1:
    case IDC_BUTTON_SENDEXP2:
    case IDC_BUTTON_SENDEXP3:
        {
            std::string str(_URL_COMMAND_EXPLICITECOMMAND_);
            str += "?ch=0&dcc=";
            if (strAppend != nullptr)
                str += strAppend;
            send_URL(str.c_str ());
        }
        break;
    }
    feedback = strFeedback;
}
