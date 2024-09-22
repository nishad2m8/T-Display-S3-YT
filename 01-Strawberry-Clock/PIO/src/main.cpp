#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <DHT.h>
#include "ui.h"

#include "credential.h"

// Function declarations
void connectToWiFi();
void setupTime();
void readWiFiInfo();
void readDHTSensor();

// DHT sensor configuration
#define DHTPIN 13         // Pin where the DHT22 is connected
#define DHTTYPE DHT22     // DHT 22 (AM2302) sensor type
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor

// NTP Client configuration
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 3600 * 3); // Set GMT+03:00 initially

// Screen dimensions
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 170;

// LVGL display buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

// TFT display
TFT_eSPI tft = TFT_eSPI();

// LVGL log function
#if LV_USE_LOG != 0
void my_print(const char *buf)
{
  Serial.printf(buf);
  Serial.flush();
}
#endif

// LVGL display flush function
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  // Start writing to the TFT display
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  // Notify LVGL that flush is complete
  lv_disp_flush_ready(disp);
}

// LVGL touchpad read function
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  uint16_t touchX = 0, touchY = 0;
  bool touched = false; // tft.getTouch( &touchX, &touchY, 600 );

  if (!touched)
  {
    // No touch detected
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    // Touch detected
    data->state = LV_INDEV_STATE_PR;

    // Set the coordinates
    data->point.x = touchX;
    data->point.y = touchY;

    Serial.print("Data x ");
    Serial.println(touchX);

    Serial.print("Data y ");
    Serial.println(touchY);
  }
}

// Setup function
void setup()
{
  Serial.begin(115200);
  lv_init();

  // LVGL log registration
#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print);
#endif

  // TFT display initialization
  tft.begin();
  tft.setRotation(3);

  // LVGL display buffer initialization
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

  // LVGL display driver initialization
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // LVGL touchpad driver initialization
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  // LVGL UI initialization
  ui_init();
  Serial.println("UI Setup done");

  connectToWiFi();
  setupTime();
  readWiFiInfo();

  dht.begin();
}

// Function to connect to WiFi
void connectToWiFi()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    lv_label_set_text(ui_Labelssid, "Connecting to WiFi");
  }

  Serial.println("Connected to WiFi");
  lv_label_set_text(ui_Labelssid, "Connected");
}

// Function to setup NTP time client
void setupTime()
{
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    delay(500);
    Serial.println("Waiting for time to be set...");
    now = time(nullptr);
  }
  Serial.println("Time is set!");
}

// Function to read WiFi information
void readWiFiInfo()
{
  // Get SSID (WiFi network name)
  String ssid = WiFi.SSID();

  // Get RSSI (Received Signal Strength Indication)
  int rssi = WiFi.RSSI();

  // Convert RSSI to a percentage (0 to 100)
  int maxRSSI = -20;  // Assuming the maximum RSSI is -20 (adjust if needed)
  int minRSSI = -100; // Assuming the minimum RSSI is -100 (adjust if needed)

  // Map RSSI to the range 0 to 100
  int percentageRSSI = map(rssi, minRSSI, maxRSSI, 0, 100);

  // Ensure the percentage is within the valid range
  percentageRSSI = constrain(percentageRSSI, 0, 100);

  Serial.print("RSSI %: ");
  Serial.println(percentageRSSI);
  Serial.print("Original RSSI: ");
  Serial.println(rssi);

  // Convert RSSI to a string
  char rssiStr[4];
  sprintf(rssiStr, "%d", percentageRSSI);

  // Passing data to LVGL UI
  lv_label_set_text(ui_Labelssid, ssid.c_str());
  lv_label_set_text(ui_Labelrssi, rssiStr);
}

// Function to read DHT sensor
void readDHTSensor()
{
  // Read temperature and humidity from DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  // Calculate feels like using computeHeatIndex
  float heatIndex = dht.computeHeatIndex(temperature, humidity, false);

  // Convert float values to strings without decimal places
  char temperatureStr[10];
  char humidityStr[10];
  char heatIndexStr[10];

  sprintf(temperatureStr, "%d", static_cast<int>(temperature));
  sprintf(humidityStr, "%d", static_cast<int>(humidity));
  sprintf(heatIndexStr, "%d", static_cast<int>(heatIndex));

  // Print temperature, humidity, and heat index values
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.print("%, Heat Index: ");
  Serial.println(heatIndex);

  // Passing data to LVGL UI
  lv_label_set_text(ui_Labeltemp, strcat(temperatureStr, "°C"));
  strcat(humidityStr, "%");
  lv_label_set_text(ui_Labelhum, humidityStr);
  lv_label_set_text(ui_Labelfeelslike, heatIndexStr);
  lv_label_set_text(ui_Labelc, "°C");
}

// Loop function
void loop()
{
  lv_timer_handler();
  delay(5);

  // Read WiFi and DHT information
  readWiFiInfo();
  readDHTSensor();

  // Update the NTP client
  timeClient.update();
  Serial.println("Updated NTP client");

  struct tm timeinfo;
  if (getLocalTime(&timeinfo))
  {
    Serial.println("Successfully obtained local time");

    // Adjust for GMT+03:00
    int hour = (timeinfo.tm_hour + 3) % 24;
    int hour12 = (hour % 12 == 0) ? 12 : hour % 12;
    int minute = timeinfo.tm_min;
    int second = timeinfo.tm_sec;

    // AM or PM
    String ampm = (hour < 12) ? "AM" : "PM";
    hour = (hour == 0) ? 12 : hour;

    // Format hour, minute, and second
    char formattedHour[3];
    char formattedMinute[3];
    char formattedSecond[3];
    sprintf(formattedHour, "%02d", hour12);
    sprintf(formattedMinute, "%02d", minute);
    sprintf(formattedSecond, "%02d", second);

    // Format date and day
    char dateBuffer[30];
    strftime(dateBuffer, sizeof(dateBuffer), "%d", &timeinfo);
    String date = dateBuffer;

    char dayBuffer[5];
    strftime(dayBuffer, sizeof(dayBuffer), "%a", &timeinfo);
    String day = dayBuffer;

    // Print formatted time
    Serial.printf("%s:%s:%s %s, %s\n", formattedHour, formattedMinute, formattedSecond, ampm.c_str(), date.c_str());

    // Toggle colon visibility every second
    static bool colonVisible = true;
    colonVisible = !colonVisible;
    lv_label_set_text(ui_Labelcolon, colonVisible ? ":" : " ");

    // Passing data to LVGL UI
    lv_label_set_text(ui_Labelhour, formattedHour);
    lv_label_set_text(ui_Labelmin, formattedMinute);
    lv_label_set_text(ui_Labelabbr, ampm.c_str());
    lv_label_set_text(ui_Labeldate, date.c_str());
    lv_label_set_text(ui_Labelday, day.c_str());
  }
  else
  {
    Serial.println("Failed to obtain local time");
  }

  delay(1000);
}
