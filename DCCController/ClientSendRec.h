/*
 * ClientSendRec.h
*/
#pragma once

#include <string>

int32_t __cdecl ClientSendRec(const char* serverName, const char* sendText, std::string& receiveText);

