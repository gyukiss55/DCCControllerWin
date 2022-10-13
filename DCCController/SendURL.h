/*
* SendURL.h
*/

#pragma once

#include <string>


void SendURL(
    const char* ipAddress,
    const char* channel,
    const char* dccCommand,
    std::string& feedback);
