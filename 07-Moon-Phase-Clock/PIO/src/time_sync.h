#pragma once

#include <Arduino.h>

namespace TimeSync
{
    void beginNtp();
    bool update();
    void tickSeconds();

    extern char   timeString12hr[9];
    extern char   timeString24hr[6];
    extern char   dateString[20];
    extern char   secondsString[3];
    extern bool   isAMTime;
    extern String weatherDate;
}
