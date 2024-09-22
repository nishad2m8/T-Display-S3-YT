#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include "credential.h"

/*Change to your screen resolution*/
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 170;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char *buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

// HTTPClient objects for World Time and NASA API
HTTPClient httpClient;

// Global constant for the time interval (30 minutes in milliseconds)
const unsigned long interval = 30 * 60 * 1000; // 30 minutes in milliseconds

// Define variables to track time
unsigned long lastUpdateTime = 0; // Variable to store the last time getDataFromServer() and getNasa() were called
unsigned long lastSecondsUpdate = 0;
const unsigned long secondsUpdateInterval = 1000; // Update the seconds every second

// Function prototypes
void fetchWorldTimeData();
void updateLVGLTime();

// Global variables to store time data
char timeString12hr[9];
char timeString24hr[6];
char dateString[20];
char secondsString[3];
bool isAMTime;

const char *worldTimeAbbreviation; // Global variable to store timezone abbreviation
const char *worldTimeZoneName;     // Global variable to store timezone name
const char *worldTimeCountry;      // Global variable to store country name

// URL for fetching World Time data
const char *worldTimeApiUrl = "http://worldtimeapi.org/api/ip";

String weatherDate; // Global variable to store the weather date

extern const lv_img_dsc_t *ui_imgset_moon_[30];

/*
void lv_moon_animation() // Function to animate the moon
{
    lv_obj_t *ui_Screen1_Image_Moon = lv_animimg_create(lv_scr_act());
    lv_animimg_set_src(ui_Screen1_Image_Moon, (const void **)ui_imgset_moon_, 29); // Cast to const void**
    lv_animimg_set_duration(ui_Screen1_Image_Moon, 3000);
    lv_animimg_set_repeat_count(ui_Screen1_Image_Moon, 1); // LV_ANIM_REPEAT_INFINITE
    lv_animimg_start(ui_Screen1_Image_Moon);
}
*/

void connectToWiFi()
{
    Serial.begin(115200);
    Serial.println("Connecting to WiFi");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
        lv_label_set_text(ui_Screen1_Label_bootscreen, "Connecting to WiFi");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("");
        Serial.print("WiFi connected with IP: ");
        Serial.println(WiFi.localIP());
        lv_label_set_text(ui_Screen1_Label_bootscreen, "WiFi connected");
    }
    else
    {
        Serial.println("Error connecting to WiFi");
    }

    Serial.println("\nConnected to WiFi");
    lv_obj_add_flag(ui_Screen1_Label_bootscreen, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);
}

String convertTo12HourFormat(const char *time24)
{
    // Extract hours and minutes
    int hour = atoi(time24);
    int minutes = atoi(time24 + 3);

    // Convert hours to 12-hour format
    hour = (hour % 12 == 0) ? 12 : hour % 12;

    // Format the time without seconds
    char formattedTime[6]; // Adjusted to fit the format "00:00"
    sprintf(formattedTime, "%02d:%02d", hour, minutes);

    return String(formattedTime);
}

// Function to get the day of the week
String getDayOfWeek(int day)
{
    switch (day % 7)
    {
    case 0:
        return "Sun";
    case 1:
        return "Mon";
    case 2:
        return "Tue";
    case 3:
        return "Wed";
    case 4:
        return "Thu";
    case 5:
        return "Fri";
    case 6:
        return "Sat";
    default:
        return "Unknown";
    }
}

// Function to get the month
String getMonth(int month)
{
    switch (month)
    {
    case 1:
        return "Jan";
    case 2:
        return "Feb";
    case 3:
        return "Mar";
    case 4:
        return "Apr";
    case 5:
        return "May";
    case 6:
        return "Jun";
    case 7:
        return "Jul";
    case 8:
        return "Aug";
    case 9:
        return "Sep";
    case 10:
        return "Oct";
    case 11:
        return "Nov";
    case 12:
        return "Dec";
    default:
        return "Unknown";
    }
}

void fetchWorldTimeData() // Function to fetch World Time data
{
    StaticJsonDocument<200> doc;

    Serial.println("Fetching data from World Time API...");

    httpClient.begin(worldTimeApiUrl);
    int httpCode = httpClient.GET();

    if (httpCode > 0)
    {
        Serial.println("World Time API request successful");

        String payload = httpClient.getString();
        Serial.println("Received JSON payload:");
        Serial.println(payload); // Print received JSON payload for debugging

        DeserializationError error = deserializeJson(doc, payload);

        if (!error)
        {
            // Extract timezone abbreviation and name from JSON response
            worldTimeAbbreviation = doc["abbreviation"];
            worldTimeZoneName = doc["timezone"];

            // Extract country name from timezone name
            int slashIndex = String(worldTimeZoneName).lastIndexOf('/');
            if (slashIndex != -1)
            {
                worldTimeCountry = worldTimeZoneName + slashIndex + 1;
            }
            else
            {
                worldTimeCountry = worldTimeZoneName;
            }

            const char *datetime = doc["datetime"];
            String datetimeStr = String(datetime);

            // Extract date and time components
            int tIndex = datetimeStr.indexOf('T');
            if (tIndex != -1)
            {
                // Extract date without milliseconds and timezone offset
                String dateWithoutTimezone = datetimeStr.substring(0, tIndex);
                String timeWithoutSeconds = datetimeStr.substring(tIndex + 1, tIndex + 6); // Extract hours and minutes

                // Extract day, month, and year
                int year = dateWithoutTimezone.substring(0, 4).toInt();
                int month = dateWithoutTimezone.substring(5, 7).toInt();
                int day = dateWithoutTimezone.substring(8, 10).toInt();

                // Extract the day of the week from the JSON payload
                int dayOfWeek = doc["day_of_week"].as<int>();

                // Format the date for display
                String formattedDate = String(day) + " " + getDayOfWeek(dayOfWeek) + " - " + getMonth(month) + "/" + String(year % 100);

                // Format the date for weather
                weatherDate = String(year) + "-" + (month < 10 ? "0" + String(month) : String(month)) + "-" + (day < 10 ? "0" + String(day) : String(day));

                // Convert 24-hour time to 12-hour time with AM/PM
                int hour = timeWithoutSeconds.substring(0, 2).toInt();
                int minute = timeWithoutSeconds.substring(3, 5).toInt();

                // Store the original hour value for later use
                int originalHour = hour;

                String ampm = "AM";
                if (hour == 0)
                {
                    hour = 12; // Midnight (00:00) in 12-hour format
                }
                else if (hour == 12)
                {
                    ampm = "PM";
                }
                else if (hour > 12)
                {
                    hour -= 12; // After noon, convert to 12-hour format
                    ampm = "PM";
                }

                // Store 12-hour time in timeString12hr variable
                sprintf(timeString12hr, "%02d:%02d %s", hour, minute);

                // Restore the original hour value
                hour = originalHour;

                // Store 24-hour time in timeString24hr variable
                sprintf(timeString24hr, "%02d:%02d", hour, minute);

                // Copy the formatted date string to global variable dateString
                strcpy(dateString, formattedDate.c_str());

                Serial.print("12-hour Time: ");
                Serial.println(timeString12hr);
                Serial.print("24-hour Time: ");
                Serial.println(timeString24hr);
                Serial.print("Date: ");
                Serial.println(dateString);
                Serial.print("World Time Abbreviation: ");
                Serial.println(worldTimeAbbreviation);
                Serial.print("World Time Country: ");
                Serial.println(worldTimeCountry);
            }
            else
            {
                Serial.println("Datetime format error: No 'T' character found");
            }
        }
        else
        {
            Serial.print("Failed to parse World Time API JSON: ");
            Serial.println(error.c_str());
        }
    }
    else
    {
        Serial.print("HTTP request to World Time API failed with error code: ");
        Serial.println(httpCode);
    }

    httpClient.end();
}

void updateSeconds()
{
    // Get the current time
    unsigned long currentMillis = millis();

    // Check if it's time to update the seconds
    if (currentMillis - lastSecondsUpdate >= secondsUpdateInterval)
    {
        // Update the seconds
        lastSecondsUpdate = currentMillis; // Save the last time the seconds were updated

        // Increment seconds
        int seconds = atoi(secondsString) + 1;
        if (seconds >= 60)
        {
            seconds = 0;
            // Fetch data from the World Time API to update time every minute
            fetchWorldTimeData();
            updateLVGLTime();
        }
        sprintf(secondsString, "%02d", seconds);                // Convert seconds back to string
        lv_label_set_text(ui_Screen1_Label_sec, secondsString); // Update LVGL label with seconds
    }
}

void updateLVGLTime()
{
    // Update LVGL elements with time and date data
    lv_label_set_text(ui_Screen1_Label_time, timeString12hr);

    // Convert boolean value to string representation
    const char *ampmText = isAMTime ? "AM" : "PM";
    lv_label_set_text(ui_Screen1_Label_ampm, ampmText);

    lv_label_set_text(ui_Screen1_Label_date, dateString);
}

String calculateMoonPhase(double moonphase)
{
    String phase;

    if (moonphase == 0.0 || moonphase == 1.0)
    {
        phase = "New Moon";
    }
    else if (moonphase > 0.0 && moonphase < 0.25)
    {
        phase = "Waxing Crescent";
    }
    else if (moonphase == 0.25)
    {
        phase = "First Quarter";
    }
    else if (moonphase > 0.25 && moonphase < 0.5)
    {
        phase = "Waxing Gibbous";
    }
    else if (moonphase == 0.5)
    {
        phase = "Full Moon";
    }
    else if (moonphase > 0.5 && moonphase < 0.75)
    {
        phase = "Waning Gibbous";
    }
    else if (moonphase == 0.75)
    {
        phase = "Last Quarter";
    }
    else
    {
        phase = "Waning Crescent";
    }

    return phase;
}

int calculateMoonPhaseIndex(float age)
{
    // Calculate the index of the moon phase image based on the moon age
    // Assuming you have 29 images for the moon phases
    int totalPhases = 29;
    // Normalize the age value to be within the range [0, 29)
    int index = (int)(age / 29.0 * totalPhases);
    return index;
}

String getDataFromServer()
{
    HTTPClient http;
    String payload = "";

    Serial.println("Fetching data from server...");
    String url = "https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/" + String(worldTimeCountry) + "/" + String(weatherDate) + "?unitGroup=us&key=" + String(apiKey) + "&include=days&elements=datetime,moonphase,sunrise,sunset,moonrise,moonset";
    http.begin(url);
    Serial.println(url);

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0)
    {
        payload = http.getString();
        Serial.println(payload);
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    http.end();

    return payload;
}

void parseJSON(String payload)
{
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
        Serial.println("Failed to parse JSON");
        return;
    }

    String sunrise = doc["days"][0]["sunrise"];
    String sunset = doc["days"][0]["sunset"];
    String moonrise = doc["days"][0]["moonrise"];
    String moonset = doc["days"][0]["moonset"];
    double moonphase = doc["days"][0]["moonphase"];

    // Convert times to 12-hour format without seconds
    sunrise = convertTo12HourFormat(sunrise.c_str());
    sunset = convertTo12HourFormat(sunset.c_str());
    moonrise = convertTo12HourFormat(moonrise.c_str());
    moonset = convertTo12HourFormat(moonset.c_str());

    // Update LVGL elements with time and date data
    lv_label_set_text(ui_Screen1_Label_Phase, calculateMoonPhase(moonphase).c_str());
    lv_label_set_text(ui_Screen1_Label_moonrise, moonrise.c_str());
    lv_label_set_text(ui_Screen1_Label_moonset, moonset.c_str());
    lv_label_set_text(ui_Screen1_Label_sunrise, sunrise.c_str());
    lv_label_set_text(ui_Screen1_Label_sunset, sunset.c_str());

    lv_obj_clear_flag(ui_Screen1_Image_sun_moon, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);
}

String getNasa()
{
    HTTPClient http;
    String nasaPayload = "";

    Serial.println("Fetching Nasa data from server...");

    // Construct the NASA API URL with the 24-hour format time string
    String url = "https://svs.gsfc.nasa.gov/api/dialamoon/" + String(weatherDate) + "T" + timeString24hr;
    Serial.println(url);
    http.begin(url);

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0)
    {
        nasaPayload = http.getString();
        Serial.println(nasaPayload);
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    http.end();

    return nasaPayload;
}

void parseNasa(String payload)
{
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
        Serial.println("Failed to parse JSON");
        return;
    }

    float phase = doc["phase"];
    float age = doc["age"];
    float posangle = doc["posangle"];

    Serial.print("Moon Phase: ");
    Serial.println(phase);
    Serial.print("Moon Age: ");
    Serial.println(age);
    Serial.print("Position Angle: ");
    Serial.println(posangle);

    String phaseText = calculateMoonPhase(age);

    // Set the moon phase image based on the calculated phase
    int phaseIndex = calculateMoonPhaseIndex(age);
    lv_img_set_src(ui_Screen1_Image_Moon, (const void *)ui_imgset_moon_[phaseIndex]);

    Serial.print("Phase Index : ");
    Serial.println(phaseIndex);

    lv_obj_clear_flag(ui_Screen1_Panel_border, LV_OBJ_FLAG_HIDDEN);
}

void setup()
{
    Serial.begin(115200); /* prepare for possible serial debug */

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println(LVGL_Arduino);
    Serial.println("I am LVGL_Arduino");

    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

    tft.begin();        /* TFT init */
    tft.setRotation(3); /* Landscape orientation, flipped */

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);

    ui_init();

    Serial.println("Setup done");

    // lv_moon_animation();

    // Boot screen animation
    moon_Animation(ui_Screen1_Image_bootscreeni, 1000);
    for (int i = 0; i < 600; i++)
    {
        lv_task_handler();
        delay(10);
    }
    lv_obj_add_flag(ui_Screen1_Image_bootscreeni, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);

    connectToWiFi();

    fetchWorldTimeData(); // Fetch data from World Time
    updateLVGLTime();     // Update LVGL elements with initial time data

    String weatherPayload = getDataFromServer();
    parseJSON(weatherPayload); // Pass the JSON payload obtained from getDataFromServer() to parseJSON()

    String nasaPayload = getNasa();
    parseNasa(nasaPayload);
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);

    unsigned long currentMillis = millis(); // Get the current time

    // Check if the interval has passed since the last update
    if (currentMillis - lastUpdateTime >= interval)
    {
        getDataFromServer();            // Call getDataFromServer()
        getNasa();                      // Call getNasa()
        lastUpdateTime = currentMillis; // Update the last update time
    }

    updateSeconds(); // Update the seconds
}
