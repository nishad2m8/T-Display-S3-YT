# PV Energy Monitor

- 📁 PIO - PlatformIO
- 📁 SLS - SquareLine Studio 

## credential.h
```c++
// credential.h
#ifndef CREDENTIAL_H
#define CREDENTIAL_H

// Define your Wi-Fi credentials
const char *ssid = "";
const char *password = "";

// Define your MQTT broker information
const char *mqtt_server = "192.168.0.0";
const int mqtt_port = 1883;
const char *mqtt_user = "";         
const char *mqtt_password = ""; 

// MQTT topic definitions
const char *mqtt_topic = "lilygo_s3";

#endif
```


## Node-RED fuction node script
```java
// Function to generate random values within a specific range
function getRandom(min, max) {
    return Math.random() * (max - min) + min;
}

// Define the random ranges for the fields
const randomPayload = {
    soc: getRandom(0, 100), // State of Charge, 50% to 100%
    bms_difference: getRandom(0, 100), // BMS difference, 0 to 10 units
    pv_power: getRandom(0, 10), // Solar power, 0 to 5000 watts
    home_power: getRandom(0, 10), // Home power, 0 to 500 watts
    daily_pv_energy: getRandom(0, 20), // Daily solar energy, 0 to 20 kWh
    daily_home_energy: getRandom(0, 50), // Daily home energy, 0 to 15 kWh
    daily_appl_1_energy: getRandom(0, 10) // Daily appliance energy, 0 to 10 kWh
};

// Assign the random payload to the message payload
msg.payload = randomPayload;

// Return the message with the random payload
return msg;
```