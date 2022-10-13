/*
* RegistryIO.h
*/

#pragma once

#include <string>

bool ReadRegistry(const char* key, std::string& str_data);
bool WriteRegistry(const char* key, const char* str_data);