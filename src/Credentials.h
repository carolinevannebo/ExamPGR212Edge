// This file contains the credentials for your edge service
#ifndef CREDENTIALS_H
#define CREDENTIALS_H
#include <Arduino.h>

class Credentials {
    public:
        Credentials() {};

        // WiFi credentials
        const char* ssid = "Freeloadere"; // Your network SSID (name)
        const char* pass = "ZFQS7YPDYQQYV7"; // Your network password

        // Used for MQTT connection
        String mqttTopic= "pig-binge-monitoring/";
        const char* mqttUsername = "EdgeService";
        const char* mqttPassword= "Kristiania1914";
        const char* mqttServer = "099bf917add24ebfae77efd1236af9e5.s1.eu.hivemq.cloud";
        const int mqttPort= 8883;

        // Internet address to send POST to
        const char* hostDomain = ""; // Render endpoint for API
        const int hostPort = 443;

        // NTP
        const char* ntpServer= "pool.ntp.org";
        const long gmtOffsetSec = 3600;
        const int daylightOffsetSec = 0;
        const int ntpUpdateInterval = 60000; // 1 minute in milliseconds

        const char * sensorId = "ESP32-NORTH"; // Change to implement additional sensors
        const int httpTimeout = 10000; // 10 seconds in milliseconds
        const int ledBrightness = 60; // 0-255

        unsigned long requestInterval = 300000; // 5 minutes in milliseconds
        unsigned long lastRequestSent = 0;
};
#endif