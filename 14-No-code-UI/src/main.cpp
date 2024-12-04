#include <Arduino.h>
#include "TFT_eSPI.h"
#include "pin_config.h"
#include "img.h"

#include "DHT.h"

// DHT22 setup
#define DHTPIN 16        // GPIO16 where the DHT22 is connected
#define DHTTYPE DHT22    // DHT22 sensor type

DHT dht(DHTPIN, DHTTYPE); // Initialize the DHT object

// Global Variables for Temperature, Humidity, and Heat Index
float temperature = 00.0;
float humidity = 00.0;
float heatIndex = 00.0;

// Initialize the TFT screen
TFT_eSPI tft = TFT_eSPI();

void readSensorData();
void updateValueUI();

void draw() {
    // Background and static UI elements
    tft.fillRect(0, 0, 320, 170, 0x40E0); // Background
    tft.drawBitmap(140, 0, image_element_temp_bits, 40, 13, 0xFFEA);
    tft.drawBitmap(0, 45, image_element_hum_bits, 18, 40, 0xFC00);
    tft.drawBitmap(302, 45, image_element_hi_bits, 18, 40, 0xFC00);
    tft.drawBitmap(109, 17, image_temp_bdr_bits, 102, 88, 0xFFEA);
    tft.drawBitmap(22, 10, image_hum_bdr_bits, 77, 89, 0xFC00);
    tft.drawBitmap(220, 10, image_hi_bdr_bits, 77, 89, 0xFC00);
    tft.drawBitmap(10, 128, image_gauge_bdr_bits, 302, 35, 0xFFEA);
    tft.drawBitmap(279, 39, image_degree_sy1_bits, 2, 2, 0xFC00);
    tft.drawBitmap(177, 24, image_degree_sy2_bits, 6, 6, 0xFFEA);

    // Static text
    tft.setTextColor(0xFFEA);
    tft.drawString("0", 8, 145);
    tft.drawString("10", 305, 145);
    tft.drawString("x10", 152, 157);
    tft.setTextSize(2);
    tft.drawString("1", 36, 138);
    tft.drawString("2", 66, 138);
    tft.drawString("3", 96, 138);
    tft.drawString("4", 125, 138);
    tft.drawString("5", 156, 138);
    tft.drawString("6", 186, 138);
    tft.drawString("7", 216, 138);
    tft.drawString("8", 246, 138);
    tft.drawString("9", 276, 138);
    
    tft.setTextColor(0xFFEA);
    tft.setTextSize(2);
    tft.drawString("Temp C", 126, 24);
    tft.setTextColor(0xFC00);
    tft.setTextSize(1);
    tft.drawString("Hum %", 43, 37);
    tft.drawString("H.Index C", 235, 39);

}

void updateDynamicUI() {
    // Clear and redraw temperature
    tft.fillRect(116, 40, 89, 59, 0x40E0); // Clear area for temperature text
    tft.setTextColor(0xFFEA);
    tft.setTextSize(7);
    tft.setFreeFont();
    tft.drawString(String((int)temperature), 122, 45);

    // Clear and redraw humidity
    tft.fillRect(27, 48, 61, 44, 0x40E0); // Clear area for humidity text
    tft.setTextColor(0xFC00);
    tft.setTextSize(5);
    tft.drawString(String((int)humidity), 29, 52);

    // Clear and redraw heat index
    tft.fillRect(230, 48, 63, 44, 0x40E0); // Clear area for heat index text
    tft.setTextColor(0xFC00);
    tft.setTextSize(5);
    tft.drawString(String((int)heatIndex), 235, 51);

    // Update progress bars
    int humidityBarWidth = map((int)humidity, 0, 100, 0, 62); // Map humidity to bar width
    int heatIndexBarWidth = map((int)heatIndex, 0, 100, 0, 62); // Map heat index to bar width

    tft.fillRect(25, 13, 62, 8, 0x40E0);   // Clear old humidity bar
    tft.fillRect(25, 13, humidityBarWidth, 8, 0xFC00);

    tft.fillRect(232, 13, 62, 8, 0x40E0);  // Clear old heat index bar
    tft.fillRect(232, 13, heatIndexBarWidth, 8, 0xFC00);

    // Update gauge position based on temperature
    int gaugeXPos = map((int)temperature, 0, 100, 5, 305);
    tft.fillRect(0, 118, 320, 9, 0x40E0); // Clear old gauge
    tft.drawBitmap(gaugeXPos, 118, image_gauge_indicator_bits, 13, 9, 0xFFEA);
}

void setup() {
    // Power on the display
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

    // Start serial communication
    Serial.begin(115200);

    dht.begin();         // Initialize the DHT sensor
    Serial.println("DHT22 initialized.");

    // Initialize the TFT screen
    tft.begin();
    tft.setRotation(3); // Set screen orientation (1/3) 

    // Clear the screen
    tft.fillScreen(0xFC00);

    // Draw static UI elements
    draw();

    // Initial update of dynamic UI
    updateDynamicUI();

}

void loop() {
    // Simulate dynamic changes for demonstration (replace with sensor readings later)
    // temperature = random(20, 40); // Random temperature (20-40 C)
    // humidity = random(30, 80);    // Random humidity (30-80%)
    // heatIndex = temperature + random(-2, 5); // Random heat index adjustment

    readSensorData();    // Read data from the DHT22 sensor

    // Update dynamic elements
    updateDynamicUI();

    delay(2000); // Update every 2 seconds
}


void readSensorData() {
    // Read temperature and humidity from DHT22
    float newTemperature = dht.readTemperature(); // Temperature in Celsius
    float newHumidity = dht.readHumidity();       // Humidity in percentage

    // Check if readings are valid
    if (isnan(newTemperature) || isnan(newHumidity)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    // Update global variables
    temperature = newTemperature;
    humidity = newHumidity;

    // Calculate heat index (uses temperature and humidity)
    heatIndex = dht.computeHeatIndex(temperature, humidity, false); // `false` indicates Celsius

    // Debug print (optional)
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.print("Heat Index: ");
    Serial.print(heatIndex);
    Serial.println(" *C");
}
