#include "time_sync.h"

#include <time.h>
#include <ui.h>

#include "credential.h"

namespace TimeSync
{
    char   timeString12hr[9] = {0};
    char   timeString24hr[6] = {0};
    char   dateString[20]    = {0};
    char   secondsString[3]  = "00";
    bool   isAMTime          = true;
    String weatherDate;

    static bool          ntpStarted    = false;
    static bool          firstSyncDone = false;
    static unsigned long lastSecMs     = 0;
    static constexpr unsigned long secondsTickMs = 1000;

    static const char *dayShort(int wday0Sun)
    {
        static const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
        if (wday0Sun < 0 || wday0Sun > 6) return "Unk";
        return days[wday0Sun];
    }

    static const char *monthShort(int month1)
    {
        static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                       "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        if (month1 < 1 || month1 > 12) return "Unk";
        return months[month1 - 1];
    }

    static void formatAndDisplay(const struct tm &t)
    {
        int year   = t.tm_year + 1900;
        int month  = t.tm_mon + 1;
        int day    = t.tm_mday;
        int wday   = t.tm_wday;
        int hour24 = t.tm_hour;
        int minute = t.tm_min;

        // Display strings
        String formattedDate = String(day) + " " + dayShort(wday) + " - " + monthShort(month) + "/" + String(year % 100);
        weatherDate = String(year) + "-" + (month < 10 ? "0" + String(month) : String(month)) + "-" + (day < 10 ? "0" + String(day) : String(day));

        isAMTime    = (hour24 < 12);
        int hour12  = hour24 % 12;
        if (hour12 == 0) hour12 = 12;
        const char *ampm = isAMTime ? "AM" : "PM";

        snprintf(timeString12hr, sizeof(timeString12hr), "%02d:%02d %s", hour12, minute, ampm);
        snprintf(timeString24hr, sizeof(timeString24hr), "%02d:%02d", hour24, minute);
        strncpy(dateString, formattedDate.c_str(), sizeof(dateString) - 1);
        dateString[sizeof(dateString) - 1] = '\0';
        snprintf(secondsString, sizeof(secondsString), "%02d", t.tm_sec);

        // Push to LVGL
        lv_label_set_text(ui_Screen1_Label_time, timeString12hr);
        lv_label_set_text(ui_Screen1_Label_ampm, ampm);
        lv_label_set_text(ui_Screen1_Label_date, dateString);
        lv_label_set_text(ui_Screen1_Label_sec, secondsString);
    }

    void beginNtp()
    {
        if (ntpStarted) return;
        configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");
        ntpStarted = true;
        Serial.println("NTP: started");
    }

    bool update()
    {
        if (!ntpStarted) return false;

        time_t now = time(nullptr);
        if (now < 100000) return false;

        struct tm t;
        localtime_r(&now, &t);

        formatAndDisplay(t);
        if (!firstSyncDone)
        {
            firstSyncDone = true;
            Serial.print("NTP synced: ");
            Serial.println(dateString);
        }
        lastSecMs = millis();
        return true;
    }

    void tickSeconds()
    {
        if (!firstSyncDone) return;
        if (millis() - lastSecMs < secondsTickMs) return;

        lastSecMs += secondsTickMs;

        int sec = atoi(secondsString) + 1;
        if (sec >= 60)
        {
            // Resync from OS clock on rollover to avoid drift
            time_t now = time(nullptr);
            struct tm t;
            localtime_r(&now, &t);
            formatAndDisplay(t);
            return;
        }
        snprintf(secondsString, sizeof(secondsString), "%02d", sec);
        lv_label_set_text(ui_Screen1_Label_sec, secondsString);
    }
}
