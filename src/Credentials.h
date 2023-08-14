// This file contains the credentials for your edge service

// WiFi credentials
const char ssid[] = ""; // Your network SSID (name)
const char pass[] = ""; // Your network password

// Used for MQTT connection
const char mqttUsername[] = "";
const char mqttPassword[] = "";
const char mqttServer[]   = "";
const int mqttPort = 8883;
String mqttTopic = "elevatormonitoring/";

// NTP
const char* ntpServer = "pool.ntp.org";
const long gmtOffsetSec = 3600;
const int daylightOffsetSec = 0;
const int ntpUpdateInterval = 60000; // 1 minute in milliseconds

// Internet address to send POST to
const char * hostDomain = ""; // Render endpoint for API
const int hostPort = 443;

const int httpTimeout = 10000; // 10 seconds in milliseconds

const char * sensorId = "ESP32-KRISTIANIA"; // Change to implement additional sensors
const int ledBrightness = 60; // 0-255
unsigned long requestInterval = 300000; // 5 minutes in milliseconds