
#include "resource.h"
#include "framework.h"

#include <format>
#include <iostream>
#include <string>
#include <string_view>

#include "MapCommand.h"

struct MapID2Command {
    uint8_t idFrom;
    uint8_t idTo;
    uint8_t command1;
    uint8_t commandLength;
    uint8_t* mask;
};

uint8_t mask_0_4[] = { 0x10, 1, 2, 4, 8 };
uint8_t mask_5_12[] = { 1, 2, 4, 8 };
uint8_t mask_13_28[] = { 1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80 };

MapID2Command commandMaps[] = {
    {0, 4, 0x80, 1, mask_0_4 },
    {5, 8, 0xB0, 1, mask_5_12 },
    {9, 12, 0xA0, 1, mask_5_12 },
    {13, 20, 0xDE, 2, mask_13_28 },
    {21, 28, 0xDF, 2, mask_13_28 },

    {0, 0, 0, 0, nullptr }
};

int actualFunction[4 * 6] = {
        0, 0, 0, 0, // F0- F4
        0, 0, 0, 0, // F5- F8
        0, 0, 0, 0, // F9- F12
        0, 0, 0, 0, // F13- F20
        0, 0, 0, 0, // F21- F28
        0, 0, 0, 0  // F29- F35
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
            value = actualFunction[channel * 4 + i];
            if (state)
                value |= commandMaps[i].mask[functionID - commandMaps[i].idFrom];
            else
                value &= (commandMaps[i].mask[functionID - commandMaps[i].idFrom]) ^ 0xff;
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
