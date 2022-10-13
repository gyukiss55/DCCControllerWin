/*
* RegistryIO.cpp
*/
#include <string>
#include "framework.h"

#include "RegistryIO.h"

HKEY registry_key;
const char* sk = "SOFTWARE\\KGySoft\\DCCController";

bool ReadRegistry(const char* key, std::string& str_data)
{
    DWORD size = 0;
    const DWORD dwFlags = RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND;
    auto status = RegGetValueA(HKEY_CURRENT_USER, sk, key, dwFlags, NULL, NULL, &size);
    if ((status == ERROR_SUCCESS) && (size > 1))
    {
        str_data.resize(size - 1);
        status = RegGetValueA(HKEY_CURRENT_USER, sk, key, dwFlags, NULL, &str_data[0], &size);
        return true;
    }
    return false;
}

bool WriteRegistry(const char* key, const char* str_data)
{
    auto openRes = RegOpenKeyA(HKEY_CURRENT_USER, sk, &registry_key);
    LONG setRes = RegSetValueExA(registry_key, key, 0, REG_EXPAND_SZ, (const BYTE *)str_data, strlen (str_data));
    return setRes == 0;
}