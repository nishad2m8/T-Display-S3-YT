#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>
#include "credential.h"

// const char *apiUrl = "https://your.mockapi.io/url";

#define BUTTON_PIN_1 0 
#define BUTTON_PIN_2 14

bool button1Pressed = false;
bool button2Pressed = false;

bool isWiFiConnected = false;

// Function declarations
String fetchJsonFromApi();
void parseJsonAndUpdateUI(const String &jsonString);

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

/*Read the touchpad*/
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

        /*Set the coordinates*/
        data->point.x = touchX;
        data->point.y = touchY;

        Serial.print("Data x ");
        Serial.println(touchX);

        Serial.print("Data y ");
        Serial.println(touchY);
    }
}

void setup()
{
    Serial.begin(115200); /* prepare for possible serial debug */

    pinMode(BUTTON_PIN_1, INPUT_PULLUP);
    pinMode(BUTTON_PIN_2, INPUT_PULLUP);

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
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    ui_init();

    Serial.println("Setup done");


    // Check WiFi status and initiate the connection if necessary
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED)
        {
            wifi_Animation(ui_Img_wifi, 0);
            delay(500); // 500ms delay
            Serial.print(".");
            lv_label_set_text(ui_Label_wifi, "Connecting...");
            lv_timer_handler(); /* let the GUI do its work */
        }    
        Serial.println("WiFi connected.");
        Serial.print("IP address: ");
        for (int i = 0; i < 5; i++)
        Serial.println(WiFi.localIP());

        lv_label_set_text(ui_Label_wifi, "Connected");
        // lv_obj_clean(lv_scr_act());
        _ui_screen_change(&ui_screen_tag, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, &ui_screen_tag_screen_init);
    }

    // Fetch JSON data from the API
    String jsonString = fetchJsonFromApi();

    // Parse JSON data and update UI
    parseJsonAndUpdateUI(jsonString);
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);

    int buttonState1 = digitalRead(BUTTON_PIN_1);
    int buttonState2 = digitalRead(BUTTON_PIN_2);

    if (buttonState1 == LOW && !button1Pressed)
    {
        Serial.println("Button 1 pressed!");
        button1Pressed = true;

        String jsonString = fetchJsonFromApi(); // Fetch JSON data from the API
        parseJsonAndUpdateUI(jsonString);       // Parse JSON data and update UI
    }
    else if (buttonState1 == HIGH && button1Pressed)
    {
        Serial.println("Button 1 released.");
        button1Pressed = false;
    }

    if (buttonState2 == LOW && !button2Pressed)
    {
        Serial.println("Button 2 pressed!");
        button2Pressed = true;
    }
    else if (buttonState2 == HIGH && button2Pressed)
    {
        Serial.println("Button 2 released.");
        button2Pressed = false;
    }
}

String fetchJsonFromApi()
{
    HTTPClient http;

    // Your API URL
    http.begin(apiUrl);

    int httpCode = http.GET();

    Serial.printf("HTTP Response Code: %d\n", httpCode);

    String jsonString = "";

    if (httpCode > 0)
    {
        if (httpCode == HTTP_CODE_OK)
        {
            jsonString = http.getString();

            // Check if JSON string is empty
            if (jsonString.length() == 0)
            {
                Serial.println("Empty JSON response");
            }
            else
            {
                Serial.print("Received JSON: ");
                Serial.println(jsonString);
            }
        }
    }
    else
    {
        Serial.printf("HTTP request failed with error code %d\n", httpCode);
    }

    http.end();

    return jsonString;
}

void parseJsonAndUpdateUI(const String &jsonString)
{
    const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(3) + 40;
    DynamicJsonDocument doc(capacity);

    // Parse JSON
    DeserializationError error = deserializeJson(doc, jsonString);

    // Check for parsing errors
    if (error)
    {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        return;
    }

    // Extract values from JSON
    const char *productName = doc[0]["proudctName"];
    float wasPrice = doc[0]["was"].as<float>();
    float nowPrice = doc[0]["now"].as<float>();

    // Calculate save percentage
    float savePercentage = ((wasPrice - nowPrice) / wasPrice) * 100;

    // Round savePercentage to two decimal places
    savePercentage = roundf(savePercentage);

    // Format strings
    char wasPriceStr[10];
    char nowPriceStr[10];
    char savePercentageStr[10];

    // Use "%.3f" to display was and now prices with 3 decimal places
    snprintf(wasPriceStr, sizeof(wasPriceStr), "%.3f", wasPrice);
    snprintf(nowPriceStr, sizeof(nowPriceStr), "%.3f", nowPrice);
    // Use "%.0f" to round savePercentage to 0 decimal places
    snprintf(savePercentageStr, sizeof(savePercentageStr), "%.0f", savePercentage);

    // Update UI components
    Serial.print("Parsed Product: ");
    Serial.println(productName);
    Serial.print("Parsed Was Price: ");
    Serial.println(wasPriceStr);
    Serial.print("Parsed Now Price: ");
    Serial.println(nowPriceStr);
    Serial.print("Parsed Save Percentage: ");
    Serial.println(savePercentageStr);

    lv_label_set_text(ui_Label_product, productName);
    lv_label_set_text(ui_label_was_price, wasPriceStr);
    lv_label_set_text(ui_label_now_price, nowPriceStr);
    lv_label_set_text(ui_label_save_value, savePercentageStr);
}
