// credential.h

#ifndef CREDENTIAL_H
#define CREDENTIAL_H

// WiFi credentials
static const char* ssid     = "";
static const char* password = "";

// visualcrossing.com API key
static const char* apiKey   = "";

// Location for visualcrossing query (max 4 decimals)
static const float myLat = 00.0000; // 00.0000
static const float myLon = 00.0000; // 00.0000

// NTP timezone — set gmtOffset_sec to your UTC offset in seconds
// e.g. UTC+3:  3 * 3600     UTC-5:  -5 * 3600
static const long  gmtOffset_sec      = 3 * 3600;
static const int   daylightOffset_sec = 0; // 3600 if your locale uses DST

#endif
