#include <iostream>
#include <string>

#include "resource.h"
#include "SendURL.h"
#include "ClientSendRec.h"

std::string strFeedback;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}


void SendURL(
    const char* ipAddress,
    const char* channel,
    const char* dccCommand,
    std::string& feedback)
{
    strFeedback = "";
        std::string str;
    str += "/ec?ch=";
    str += channel;
    str += "&dcc=";
    if (dccCommand != nullptr)
        str += dccCommand;

    std::string recString;
    int32_t err = ClientSendRec(ipAddress, str.c_str(), recString);
    if (err != 0) {
        strFeedback += "ClientSendRec ERROR!!";

    }
    else {
        strFeedback += "Send URL:";
        strFeedback += str;

    }

    feedback = strFeedback;
}
