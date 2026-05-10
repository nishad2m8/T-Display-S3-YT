#include "weather_client.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <ui.h>

#include "credential.h"

extern const lv_img_dsc_t *ui_imgset_moon_[30];

namespace WeatherClient
{
    // Result populated by background task, consumed by main thread
    struct Result
    {
        char        sunrise[12];
        char        sunset[12];
        char        moonrise[12];
        char        moonset[12];
        const char *phaseName;
        int         frameIndex;
        bool        valid;
    };

    static Result            latestResult     = {};
    static volatile bool     fetchInProgress  = false;
    static volatile bool     resultPending    = false;
    static SemaphoreHandle_t requestSemaphore = nullptr;
    static TaskHandle_t      taskHandle       = nullptr;

    // Helpers
    static void to12HourFormat(const char *time24, char *out, size_t outSize)
    {
        if (!time24 || time24[0] == '\0')
        {
            out[0] = '\0';
            return;
        }
        int hour   = atoi(time24);
        int minute = atoi(time24 + 3);
        const char *ampm = (hour < 12) ? "AM" : "PM";
        int h12 = hour % 12;
        if (h12 == 0) h12 = 12;
        snprintf(out, outSize, "%02d:%02d %s", h12, minute, ampm);
    }

    static const char *phaseName(double m)
    {
        if (m == 0.0 || m == 1.0)              return "New Moon";
        if (m >  0.0  && m <  0.25)            return "Waxing Crescent";
        if (m == 0.25)                         return "First Quarter";
        if (m >  0.25 && m <  0.5)             return "Waxing Gibbous";
        if (m == 0.5)                          return "Full Moon";
        if (m >  0.5  && m <  0.75)            return "Waning Gibbous";
        if (m == 0.75)                         return "Last Quarter";
        return "Waning Crescent";
    }

    static int frameIndex(double m)
    {
        int idx = ((int)(m * 29)) % 29;
        if (idx < 0) idx += 29;
        return idx;
    }

    static bool doFetch(Result &out)
    {
        Serial.println("Weather: fetching...");

        String location = String(myLat, 4) + "," + String(myLon, 4);
        String url = "https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/" +
                     location +
                     "/today?unitGroup=us&key=" + String(apiKey) +
                     "&include=days&elements=datetime,moonphase,sunrise,sunset,moonrise,moonset";

        HTTPClient http;
        http.begin(url);
        http.setTimeout(10000);

        int code = http.GET();
        if (code <= 0)
        {
            Serial.print("Weather: HTTP error ");
            Serial.println(code);
            http.end();
            return false;
        }

        String payload = http.getString();
        http.end();

        StaticJsonDocument<512> doc;
        DeserializationError err = deserializeJson(doc, payload);
        if (err)
        {
            Serial.print("Weather: JSON parse error: ");
            Serial.println(err.c_str());
            return false;
        }

        const char *sunrise   = doc["days"][0]["sunrise"]  | "";
        const char *sunset    = doc["days"][0]["sunset"]   | "";
        const char *moonrise  = doc["days"][0]["moonrise"] | "";
        const char *moonset   = doc["days"][0]["moonset"]  | "";
        double      moonphase = doc["days"][0]["moonphase"] | 0.0;

        to12HourFormat(sunrise,  out.sunrise,  sizeof(out.sunrise));
        to12HourFormat(sunset,   out.sunset,   sizeof(out.sunset));
        to12HourFormat(moonrise, out.moonrise, sizeof(out.moonrise));
        to12HourFormat(moonset,  out.moonset,  sizeof(out.moonset));
        out.phaseName  = phaseName(moonphase);
        out.frameIndex = frameIndex(moonphase);
        out.valid      = true;

        Serial.print("Weather: phase=");
        Serial.print(moonphase);
        Serial.print(" frame=");
        Serial.println(out.frameIndex);
        return true;
    }

    static void taskLoop(void *)
    {
        for (;;)
        {
            xSemaphoreTake(requestSemaphore, portMAX_DELAY);

            fetchInProgress = true;
            Result tmp = {};
            if (doFetch(tmp))
            {
                latestResult  = tmp;
                resultPending = true;
            }
            fetchInProgress = false;
        }
    }

    // Public API
    void begin()
    {
        if (taskHandle) return;
        requestSemaphore = xSemaphoreCreateBinary();
        xTaskCreatePinnedToCore(taskLoop, "weather", 8192, nullptr, 1, &taskHandle, 0);
    }

    void requestFetch()
    {
        if (fetchInProgress) return;
        xSemaphoreGive(requestSemaphore);
    }

    bool isFetching()    { return fetchInProgress; }
    bool hasNewResult()  { return resultPending; }

    void applyResultToUI()
    {
        if (!resultPending || !latestResult.valid) return;

        lv_label_set_text(ui_Screen1_Label_Phase,    latestResult.phaseName);
        lv_label_set_text(ui_Screen1_Label_sunrise,  latestResult.sunrise);
        lv_label_set_text(ui_Screen1_Label_sunset,   latestResult.sunset);
        lv_label_set_text(ui_Screen1_Label_moonrise, latestResult.moonrise);
        lv_label_set_text(ui_Screen1_Label_moonset,  latestResult.moonset);

        lv_img_set_src(ui_Screen1_Image_Moon,
                       (const void *)ui_imgset_moon_[latestResult.frameIndex]);

        lv_obj_clear_flag(ui_Screen1_Image_sun_moon, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(ui_Screen1_Panel_border,   LV_OBJ_FLAG_HIDDEN);

        resultPending = false;
    }
}
