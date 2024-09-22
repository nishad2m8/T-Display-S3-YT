#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "credential.h"

#define BUTTON_PIN_1 0 // boot button
#define BUTTON_PIN_2 14

int currentAction = 0;  // 0: locationAchange, 1: locationBchange, 2: changeBG
bool bgVisible = false;  // Flag to track the visibility state of background elements

struct Location
{
    const char *zone;
    const char *locationCode;
    int locX;
    int locY;
};

Location locations[] = {
    {"-8", "SFO", 21, 28},
    {"-4", "VEN", 44, 40},
    {"-3", "ARG", 48, 58},
    {"+0", "GBR", 68, 18},
    {"+1", "DEU", 78, 19},
    {"+3", "BHR", 89, 32},
    {"+4", "UAE", 92, 34},
    {"+5:30", "IND", 101, 34},
    {"+9", "TYO", 124, 29},
    {"+11", "AUS", 127, 55}};

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 1000; // Update every 1000 milliseconds (1 second)

// Function prototypes
void updateLocalTime();
String setLocationTime(const char *timezone);
String getTimeString(const char *timezone);
String getAmPmString(const char *timezone);

int currentLocationIndexA = 0;
int currentLocationIndexB = 1;

// Global variables for location A
String locationAtime;
String locationAampm;
String locationAzone;
String locationAcode;
int locationAseconds = 0;

// Global variables for location B
String locationBtime;
String locationBampm;
String locationBzone;
String locationBcode;
int locationBseconds = 0;

/* Change to your screen resolution */
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 170;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

TFT_eSPI tft; /* TFT instance */

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

/* Read the touchpad */
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    uint16_t touchX = 0, touchY = 0;

    bool touched = false; // tft.getTouch( &touchX, &touchY, 600 );

    if (!touched)
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;

        /* Set the coordinates */
        data->point.x = touchX;
        data->point.y = touchY;

        Serial.print("Data x ");
        Serial.println(touchX);

        Serial.print("Data y ");
        Serial.println(touchY);
    }
}

lv_obj_t *ui_img_loc; // initialize lvgl image object
lv_obj_t *ui_img_map_bg;
lv_obj_t *ui_img_world_time;
lv_obj_t *ui_img_local_time;

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

    /* Initialize the display */
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /* Initialize the (dummy) input device driver */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    ui_init();

    // Setup button pin (replace BUTTON_PIN with the actual GPIO pin number)
    pinMode(BUTTON_PIN_1, INPUT_PULLUP);
    pinMode(BUTTON_PIN_2, INPUT_PULLUP);

    // Enable the animation with a delay of 2000 milliseconds (2 seconds)
    imgloc_Animation(ui_img_loc, 2000);

    Serial.println("Setup done");

    // Initialize WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    pinMode(BUTTON_PIN_1, INPUT_PULLUP);
    pinMode(BUTTON_PIN_2, INPUT_PULLUP);

    updateLocalTime();

    Serial.print("Local Time: ");
    Serial.println(timeClient.getFormattedTime());
}

void locationAchange()
{
    // Add your logic for changing the location for A here

    // Button 1 pressed, switch to the next location for A
    currentLocationIndexA = (currentLocationIndexA + 1) % (sizeof(locations) / sizeof(locations[0]));
    delay(500); // Debounce delay

    Serial.print("Button 1 Pressed. Switching location for A. New Index: ");
    Serial.println(currentLocationIndexA);

    // Update location A time
    locationAtime = getTimeString(locations[currentLocationIndexA].zone);
    locationAampm = getAmPmString(locations[currentLocationIndexA].zone);
    locationAzone = locations[currentLocationIndexA].zone;
    locationAcode = locations[currentLocationIndexA].locationCode;
    locationAseconds = timeClient.getSeconds();

    // Set the new coordinates for ui_img_loc based on the current location A
    lv_obj_set_x(ui_img_loc, locations[currentLocationIndexA].locX);
    lv_obj_set_y(ui_img_loc, locations[currentLocationIndexA].locY);

    lv_label_set_text(ui_Label_cc_a, locationAcode.c_str());

    Serial.println("Executing locationAchange");
    // Display time of the current location A
    Serial.print(locationAcode);
    Serial.print(": ");
    Serial.println(locationAtime);
}

void locationBchange()
{
    // Add your logic for changing the location for B here

    // Button 2 pressed, switch to the next location for B
    currentLocationIndexB = (currentLocationIndexB + 1) % (sizeof(locations) / sizeof(locations[0]));
    delay(500); // Debounce delay

    Serial.print("Button 2 Pressed. Switching location for B. New Index: ");
    Serial.println(currentLocationIndexB);

    // Update location B time
    locationBtime = getTimeString(locations[currentLocationIndexB].zone);
    locationBampm = getAmPmString(locations[currentLocationIndexB].zone);
    locationBzone = locations[currentLocationIndexB].zone;
    locationBcode = locations[currentLocationIndexB].locationCode;
    locationBseconds = timeClient.getSeconds();

    // Update LVGL labels with the time information
    // lv_label_set_text(ui_Label_wt_a, locationBtime.c_str());
    // lv_label_set_text(ui_Label_wt_sec_a, String(locationBseconds).c_str());
    // lv_label_set_text(ui_Label_wt_ampm, locationBampm.c_str());
    lv_label_set_text(ui_Label_cc, locationBcode.c_str());
    lv_label_set_text(ui_Label_gmt, locationBzone.c_str());


    Serial.println("Executing locationBchange");
    // Display time of the current location B
    Serial.print(locationBcode);
    Serial.print(": ");
    Serial.println(locationBtime);
}

void changeBG() {
    // Code for changeBG
    if (!bgVisible) {
        lv_obj_clear_flag(ui_img_local_time_blue, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_img_map_bg_blue, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_img_world_time_blue, LV_OBJ_FLAG_HIDDEN);

        Serial.println("Executing changeBG: Showing elements");
    } else {
        lv_obj_add_flag(ui_img_local_time_blue, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_img_map_bg_blue, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_img_world_time_blue, LV_OBJ_FLAG_HIDDEN);

        Serial.println("Executing changeBG: Hiding elements");
    }

    // Toggle the visibility state
    bgVisible = !bgVisible;
}


String getTimeString(const char *timezone)
{
    // Calculate time for the given timezone
    time_t localTime = timeClient.getEpochTime() + atoi(timezone) * 3600;

    // Adjust for +5:30 timezone
    if (strcmp(timezone, "+5:30") == 0)
    {
        localTime += 1800; // Add 30 minutes
    }

    struct tm *timeInfo = localtime(&localTime);

    // Format the time without leading zero for hour
    char formattedTime[9];                                             // HH:MM\0
    strftime(formattedTime, sizeof(formattedTime), "%l:%M", timeInfo); // 12-hour format without leading zero

    return String(formattedTime);
}

String getAmPmString(const char *timezone)
{
    time_t localTime = timeClient.getEpochTime() + atoi(timezone) * 3600;
    struct tm *timeInfo = localtime(&localTime);

    // Format AM/PM
    char ampm[3];
    strftime(ampm, sizeof(ampm), "%p", timeInfo);

    return String(ampm);
}


void updateLocalTime()
{
    // Attempt to update local time from NTP
    for (int i = 0; i < 3; i++)
    {
        if (timeClient.update())
        {
            break;
        }
        delay(1000);
    }

    // Update seconds for location A and B
    locationAseconds = timeClient.getSeconds();
    locationBseconds = timeClient.getSeconds();
}

struct tm calculateLocalTime(const char *timezone)
{
    // Calculate time for the given timezone
    time_t localTime = timeClient.getEpochTime() + atoi(timezone) * 3600;

    // Adjust for +5:30 timezone
    if (strcmp(timezone, "+5:30") == 0)
    {
        localTime += 1800; // Add 30 minutes
    }

    return *localtime(&localTime);
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);

    // Update location A time
    locationAtime = getTimeString(locations[currentLocationIndexA].zone);
    locationAampm = getAmPmString(locations[currentLocationIndexA].zone);

    // Update location B time
    locationBtime = getTimeString(locations[currentLocationIndexB].zone);
    locationBampm = getAmPmString(locations[currentLocationIndexB].zone);

    // Update LVGL labels with the time information for location A
    lv_label_set_text(ui_Label_lt_a, locationAtime.c_str());
    lv_label_set_text(ui_Label_lt_ampm, locationAampm.c_str());

    // Check if it's time to update the labels
    unsigned long currentMillis = millis();
    if (currentMillis - lastUpdateTime >= updateInterval)
    {
        lastUpdateTime = currentMillis;

        // Update labels every second
        locationBseconds = (locationBseconds + 1) % 60; // Increment seconds
    }

    // Update LVGL labels with the time information for location B
    lv_label_set_text(ui_Label_wt_a, locationBtime.c_str());
    lv_label_set_text(ui_Label_wt_ampm, locationBampm.c_str());

    // Retrieve seconds from the local time directly
    struct tm locationATimeInfo = calculateLocalTime(locations[currentLocationIndexA].zone);

    // Display seconds with two digits
    String secondsText = (locationATimeInfo.tm_sec < 10) ? "0" + String(locationATimeInfo.tm_sec) : String(locationATimeInfo.tm_sec);
    lv_label_set_text(ui_Label_wt_sec_a, secondsText.c_str());

    // Check for button press on BUTTON_PIN_1
if (digitalRead(BUTTON_PIN_1) == LOW) {
    delay(500);  // Debounce delay

    // Execute the current action based on the current state
    if (currentAction == 0) {
        // Do nothing
    } else if (currentAction == 1) {
        locationAchange();
    } else if (currentAction == 2) {
        locationBchange();
    } else if (currentAction == 3) {
        changeBG();
    }

    // Set all visibility flags to LV_OBJ_FLAG_HIDDEN
    lv_obj_add_flag(ui_action_1, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_action_2, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_action_3, LV_OBJ_FLAG_HIDDEN);

    // Set the visibility flag for the current action to 0 (not hidden)
    if (currentAction == 1) {
        lv_obj_clear_flag(ui_action_1, LV_OBJ_FLAG_HIDDEN);
    } else if (currentAction == 2) {
        lv_obj_clear_flag(ui_action_2, LV_OBJ_FLAG_HIDDEN);
    } else if (currentAction == 3) {
        lv_obj_clear_flag(ui_action_3, LV_OBJ_FLAG_HIDDEN);
    }
}

    // Check for button press on BUTTON_PIN_2
    if (digitalRead(BUTTON_PIN_2) == LOW) {
        delay(500);  // Debounce delay

        // Increment the current action state, and loop back to 0 after the last action
        currentAction++;
        if (currentAction > 3) {
            currentAction = 0;
        }

        Serial.print("Switching to action: ");
        Serial.println(currentAction);

        // Set all visibility flags to LV_OBJ_FLAG_HIDDEN
        lv_obj_add_flag(ui_action_1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_action_2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_action_3, LV_OBJ_FLAG_HIDDEN);

        // Set the visibility flag for the current action to 0 (not hidden)
        if (currentAction == 1) {
            lv_obj_clear_flag(ui_action_1, LV_OBJ_FLAG_HIDDEN);
        } else if (currentAction == 2) {
            lv_obj_clear_flag(ui_action_2, LV_OBJ_FLAG_HIDDEN);
        } else if (currentAction == 3) {
            lv_obj_clear_flag(ui_action_3, LV_OBJ_FLAG_HIDDEN);
        }
    }

}
