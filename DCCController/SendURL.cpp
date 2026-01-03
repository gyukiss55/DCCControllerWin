/*
* SendURL.cpp
*/

#include "framework.h"
#include <iostream>
#include <string>
#include <debugapi.h>

#include "resource.h"
#include "SendURL.h"
#include "ClientSendRec.h"
#include "WEBGetCommand.h"
#include "MeasureTime.h"
#include "SemaphoreHandler.h"

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
    std::string& receiveString,
    std::string& feedback)
{
    MeasureTime mt ("SendURL");

    AutoMutex mutex(L"SendURL");

    strFeedback = "";
    std::string sendStr;
    sendStr += "/ec?ch=";
    sendStr += channel;
    sendStr += "&dcc=";
    
    sendStr += dccCommand;

    OutputDebugStringA("\nBefore Fill:\n");
    OutputDebugStringA(sendStr.c_str ());

    std::string sendString;
    FillinSendString(sendString, sendStr.c_str (), ipAddress);
    std::string recString;
    OutputDebugStringA("\nAfter Fill:\n");
    OutputDebugStringA(sendString.c_str());
    int err = ClientSendRec(ipAddress, sendString.c_str(), recString);;
    printf("ClientSendRec err:%d rec len:%zd rec str:#%s#\n", err, recString.length(), recString.c_str());
    if (err != 0) {
        strFeedback += "ClientSendRec ERROR!!";

    }
    else {
        strFeedback += "Send URL:";
        strFeedback += sendString;

        strFeedback += " Response:";
        strFeedback += recString;

        OutputDebugStringA("\nRecString:\n");
        OutputDebugStringA(recString.c_str ());

        receiveString = recString;
    }

    feedback = strFeedback;
}
