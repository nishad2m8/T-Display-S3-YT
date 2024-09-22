#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>
#include "credential.h"

/* Change to your screen resolution */
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 170;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char *buf)
{
    Serial.printf("%s", buf);
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


// Global variables for MQTT data
String soc, bms_difference, pv_power, home_power, daily_pv_energy, daily_home_energy, daily_appl_1_energy;
static int previous_soc_val = 0;

WiFiClient espClient;
PubSubClient client(espClient);

/* Function to connect to Wi-Fi */
void setupWiFi()
{
    Serial.print("Connecting to Wi-Fi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi connected.");
}

/* Function to update the knob position */
void setMvBarKnobXPos(int bms_difference)
{
    int x_pos = (bms_difference <= 40) ? map(bms_difference, 0, 40, 0, 140) : map(bms_difference, 41, 100, 140, 170);
    x_pos = constrain(x_pos, 0, 180);

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, ui_img_mv_bar_knob);
    lv_anim_set_values(&anim, lv_obj_get_x(ui_img_mv_bar_knob), x_pos);
    lv_anim_set_time(&anim, 500);
    lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_start(&anim);
}

/* Function to update gauge needle rotation */
void update_gauge_needle(lv_obj_t *img_needle, float power, float max_power)
{
    float angle_degrees = constrain((power / max_power) * 180, 0, 180) * 10.0;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, img_needle);
    lv_anim_set_values(&a, lv_img_get_angle(img_needle), static_cast<uint16_t>(angle_degrees + 0.5));
    lv_anim_set_time(&a, 500);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_angle);
    lv_anim_start(&a);
}

/* Function to set the battery fill color based on SOC with animation */
void setBatteryFillColor(int soc)
{
    lv_color_t color;
    if (soc <= 20)
        color = lv_color_hex(0xff0a00); // Red
    else if (soc <= 30)
        color = lv_color_hex(0xffc700); // Orange
    else if (soc <= 40)
        color = lv_color_hex(0xf7ff00); // Yellow
    else if (soc <= 70)
        color = lv_color_hex(0x91ff00); // Light green
    else if (soc <= 100)
        color = lv_color_hex(0x08ff00); // Green
    else
        color = lv_color_hex(0x000000); // Default to black if out of range

    lv_obj_set_style_img_recolor(ui_img_battery_fill, color, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void update_battery_knob(int soc_val)
{
    int new_y_pos = static_cast<int>((-65 * soc_val) / 100);
    int previous_y_pos = static_cast<int>((-65 * previous_soc_val) / 100);

    lv_anim_t anim_knob;
    lv_anim_init(&anim_knob);
    lv_anim_set_var(&anim_knob, ui_img_battery_knob);
    lv_anim_set_values(&anim_knob, previous_y_pos, new_y_pos);
    lv_anim_set_time(&anim_knob, 500);
    lv_anim_set_exec_cb(&anim_knob, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_start(&anim_knob);

    previous_soc_val = soc_val;
}

/* Function to initialize the chart values to 0 */
void initializeChartToZero()
{
    static lv_chart_series_t *ser = lv_chart_get_series_next(ui_Chart_TP, NULL); // Assuming a single series
    for (int i = 0; i < lv_chart_get_point_count(ui_Chart_TP); i++)
    {
        lv_chart_set_value_by_id(ui_Chart_TP, ser, i, 0);
    }
}

/* Callback function to handle incoming MQTT messages */
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    char jsonData[256];
    strncpy(jsonData, (char *)payload, length);
    jsonData[length] = '\0';

    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, jsonData);

    if (error)
    {
        Serial.printf("Failed to parse JSON: %s\n", error.c_str());
        return;
    }

    float soc_val = doc["soc"].as<float>();                                 // Flot value of SOC
    int bms_difference_val = doc["bms_difference"].as<int>();               // Integer value of BMS difference
    float pv_power_val = doc["pv_power"].as<float>();                       // Float value of PV power
    float home_power_val = doc["home_power"].as<float>();                   // Float value of home power
    float daily_pv_energy_val = doc["daily_pv_energy"].as<float>();         // Float value of daily PV energy
    float daily_home_energy_val = doc["daily_home_energy"].as<float>();     // Float value of daily home energy
    float daily_appl_1_energy_val = doc["daily_appl_1_energy"].as<float>(); // Float value of daily appliance 1 energy

    auto formatValue = [](float value, int decimals) {
        String str = String(value, decimals);
        str.replace('.', ',');
        return str;
    };

    soc = formatValue(soc_val, 1);
    bms_difference = String(bms_difference_val);
    pv_power = formatValue(pv_power_val, 2);
    home_power = formatValue(home_power_val, 2);
    daily_pv_energy = formatValue(daily_pv_energy_val, 2);
    daily_home_energy = formatValue(daily_home_energy_val, 2);
    daily_appl_1_energy = formatValue(daily_appl_1_energy_val, 2);

    update_gauge_needle(ui_img_needle_pv, pv_power_val, 9.99);
    update_gauge_needle(ui_img_needle_home, home_power_val, 9.99);

    lv_label_set_text(ui_Label_pv_kV, pv_power.c_str());            // Update PV power label
    lv_label_set_text(ui_Label_home_kV, home_power.c_str());        // Update home power label
    lv_label_set_text(ui_Label_TP, daily_pv_energy.c_str());        // Update total power label
    lv_label_set_text(ui_Label_CH, daily_home_energy.c_str());      // Update charge/discharge label
    lv_label_set_text(ui_Label_CA, daily_appl_1_energy.c_str());    // Update appliance 1 energy label
    lv_label_set_text(ui_Label_bat, soc.c_str());                   // Update battery SOC label
    lv_label_set_text(ui_Label_mV, bms_difference.c_str());         //Update BMS difference label

    static lv_chart_series_t *ser = lv_chart_get_series_next(ui_Chart_TP, NULL);
    lv_chart_set_next_value(ui_Chart_TP, ser, daily_pv_energy_val);

    update_battery_knob(soc_val);
    setBatteryFillColor(static_cast<int>(soc_val));
    setMvBarKnobXPos(bms_difference_val);
}

/* Function to print current state of global variables */
void printGlobalVariables()
{
    Serial.println("Current MQTT Data:");
    Serial.printf("SOC: %s\nBMS Difference: %s\nPV Power: %s\nHome Power: %s\nDaily PV Energy: %s\nDaily Home Energy: %s\nDaily Appliance 1 Energy: %s\n",
                  soc.c_str(), bms_difference.c_str(), pv_power.c_str(), home_power.c_str(),
                  daily_pv_energy.c_str(), daily_home_energy.c_str(), daily_appl_1_energy.c_str());
}

/* Setup MQTT with the appropriate topic subscription */
void setupMQTT()
{
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(mqttCallback);

    while (!client.connected())
    {
        Serial.print("Connecting to MQTT...");
        String clientId = "ESP32Client-" + String(random(0xffff), HEX);

        if (client.connect(clientId.c_str(), mqtt_user, mqtt_password))
        {
            Serial.println("MQTT connected.");
            client.subscribe(mqtt_topic);
        }
        else
        {
            Serial.printf("Failed to connect, rc=%d. Trying again in 5 seconds.\n", client.state());
            delay(5000);
        }
    }
}

/* Function to reconnect to MQTT broker if disconnected */
void reconnectMQTT()
{
    if (!client.connected())
    {
        setupMQTT();
    }
}

/* Function to maintain MQTT connection and process messages */
void handleMQTT()
{
    if (client.connected())
    {
        client.loop();
    }
    else
    {
        reconnectMQTT();
    }
}

/* Arduino setup function */
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
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /* Initialize the (dummy) input device driver */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);

    ui_init(); // Initialize the UI
    Serial.println("Setup done");

    // Allow the display to show the loading screen
    lv_timer_handler();
    delay(100); // Short delay to ensure the loading screen is displayed

    setupWiFi(); // Connect to Wi-Fi
    setupMQTT(); // Connect to MQTT broker

    initializeChartToZero(); // Initialize chart values to 0
}

/* Arduino loop function */
void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);

    handleMQTT(); // Keep MQTT connection alive and process messages

    // Periodically print global variables for debugging
    static unsigned long lastPrintTime = 0;
    if (millis() - lastPrintTime > 10000)
    {
        printGlobalVariables(); // Print current state of MQTT data
        lastPrintTime = millis();
    }
}
