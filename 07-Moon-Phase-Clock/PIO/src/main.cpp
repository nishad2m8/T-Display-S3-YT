#include <Arduino.h>
#include <lvgl.h>
#include <ui.h>

#include "display.h"
#include "wifi_manager.h"
#include "time_sync.h"
#include "weather_client.h"

extern const lv_img_dsc_t *ui_imgset_moon_[30];

namespace
{
    enum class BootPhase { WaitWifi, WaitNtp, Running };
    BootPhase     bootPhase     = BootPhase::WaitWifi;

    unsigned long lastRefreshMs      = 0;
    unsigned long lastFetchAttemptMs = 0;
    constexpr unsigned long refreshIntervalMs = 30UL * 60UL * 1000UL;
    constexpr unsigned long fetchRetryMs      = 5000;

    // Phase-label dot animation
    bool          weatherArrived = false;
    unsigned long lastDotMs      = 0;
    int           dotIndex       = 0;
    constexpr unsigned long dotIntervalMs = 500;

    // Moon image loading animation
    bool          moonLoadingActive = false;
    unsigned long lastMoonFrameMs   = 0;
    int           moonFrame         = 0;
    constexpr unsigned long moonFrameMs = 100;

    void hideBootScreen()
    {
        lv_anim_del(ui_Screen1_Image_bootscreeni, NULL);
        lv_obj_add_flag(ui_Screen1_Image_bootscreeni, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_add_flag(ui_Screen1_Label_bootscreen, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);
    }

    void showPlaceholders()
    {
        lv_label_set_text(ui_Screen1_Label_sunrise,  "-");
        lv_label_set_text(ui_Screen1_Label_sunset,   "-");
        lv_label_set_text(ui_Screen1_Label_moonrise, "-");
        lv_label_set_text(ui_Screen1_Label_moonset,  "-");
        lv_label_set_text(ui_Screen1_Label_Phase,    ".");
        lv_obj_clear_flag(ui_Screen1_Image_sun_moon, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(ui_Screen1_Panel_border,   LV_OBJ_FLAG_HIDDEN);

        moonLoadingActive = true;
        moonFrame         = 0;
        lastMoonFrameMs   = millis();
    }

    void tickPhaseLoadingDots()
    {
        if (millis() - lastDotMs < dotIntervalMs) return;
        lastDotMs = millis();
        dotIndex = (dotIndex + 1) % 3;
        static const char *frames[] = {".", "..", "..."};
        lv_label_set_text(ui_Screen1_Label_Phase, frames[dotIndex]);
    }

    void tickMoonLoadingAnimation()
    {
        if (!moonLoadingActive) return;
        if (millis() - lastMoonFrameMs < moonFrameMs) return;
        lastMoonFrameMs = millis();
        moonFrame = (moonFrame + 1) % 29;
        lv_img_set_src(ui_Screen1_Image_Moon, (const void *)ui_imgset_moon_[moonFrame]);
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Boot");

    Display::begin();
    ui_init();

    // Boot screen
    moon_Animation(ui_Screen1_Image_bootscreeni, 1000);
    lv_label_set_text(ui_Screen1_Label_bootscreen, "Connecting to WiFi");

    WeatherClient::begin();
    WifiManager::begin();
}

void loop()
{
    lv_timer_handler();
    delay(5);

    WifiManager::update();

    switch (bootPhase)
    {
        case BootPhase::WaitWifi:
            if (WifiManager::isConnected())
            {
                lv_label_set_text(ui_Screen1_Label_bootscreen, "Syncing time");
                TimeSync::beginNtp();
                bootPhase = BootPhase::WaitNtp;
            }
            break;

        case BootPhase::WaitNtp:
            if (TimeSync::update())
            {
                hideBootScreen();
                showPlaceholders();
                WeatherClient::requestFetch();
                lastFetchAttemptMs = millis();
                bootPhase = BootPhase::Running;
            }
            break;

        case BootPhase::Running:
            TimeSync::tickSeconds();

            if (!weatherArrived)
            {
                if (WeatherClient::hasNewResult())
                {
                    moonLoadingActive = false;
                    WeatherClient::applyResultToUI();
                    weatherArrived = true;
                    lastRefreshMs  = millis();
                }
                else
                {
                    tickPhaseLoadingDots();
                    tickMoonLoadingAnimation();
                    if (!WeatherClient::isFetching() && WifiManager::isConnected() &&
                        (millis() - lastFetchAttemptMs >= fetchRetryMs))
                    {
                        WeatherClient::requestFetch();
                        lastFetchAttemptMs = millis();
                    }
                }
            }
            else
            {
                if (WeatherClient::hasNewResult())
                {
                    WeatherClient::applyResultToUI();
                }
                if (WifiManager::isConnected() && (millis() - lastRefreshMs >= refreshIntervalMs))
                {
                    WeatherClient::requestFetch();
                    lastRefreshMs = millis();
                }
            }
            break;
    }
}
