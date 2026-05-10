#include "wifi_manager.h"

#include <Arduino.h>
#include <WiFi.h>

#include "credential.h"

namespace WifiManager
{
    static volatile State currentState  = State::Idle;
    static unsigned long  lastAttemptMs = 0;
    static unsigned long  lastTickMs    = 0;
    static constexpr unsigned long retryIntervalMs = 15000;
    static constexpr unsigned long tickIntervalMs  = 500;

    static void onEvent(WiFiEvent_t event)
    {
        switch (event)
        {
            case ARDUINO_EVENT_WIFI_STA_GOT_IP:
                currentState = State::Connected;
                Serial.print("WiFi got IP: ");
                Serial.println(WiFi.localIP());
                break;

            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
                if (currentState == State::Connected)
                {
                    Serial.println("WiFi: lost connection");
                    currentState  = State::Connecting;
                    lastAttemptMs = millis();
                }
                break;

            default:
                break;
        }
    }

    void begin()
    {
        WiFi.mode(WIFI_STA);
        WiFi.setSleep(false);
        WiFi.onEvent(onEvent);
        WiFi.begin(ssid, password);
        currentState  = State::Connecting;
        lastAttemptMs = millis();
        Serial.println("WiFi: connecting...");
    }

    void update()
    {
        if (millis() - lastTickMs < tickIntervalMs) return;
        lastTickMs = millis();

        if (currentState != State::Connecting) return;

        if (millis() - lastAttemptMs > retryIntervalMs)
        {
            Serial.println("WiFi: retry...");
            WiFi.disconnect();
            WiFi.begin(ssid, password);
            lastAttemptMs = millis();
        }
    }

    State state()       { return currentState; }
    bool  isConnected() { return currentState == State::Connected; }
}
