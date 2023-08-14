#include <Arduino.h>
#include <WiFi.h>
#include "Network.h"
#include <Light.h>

void Network::connectToWifi() {
    Serial.println("Attempting to connect to network: " + String(ssid));

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        WiFi.begin(ssid, pass);
        delay(5000);

        // Yellow light on
        changeColor(255, 255, 0);
    }
        
    Serial.print("\nConnected to Wifi!");
    Serial.print("\nIP address: ");
    Serial.print(WiFi.localIP());
}

bool Network::syncronizeClock() {
    // Synchronize ESP32's clock with NTP server
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    // Wait for time synchronization
    Serial.print("\nWaiting for NTP time sync: ");
    time_t now = time(nullptr);

    while (now < 8 * 3600 * 2) {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }

    // @todo exception handling
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.print("\nFailed to obtain time");
        return false;
    }
    gmtime_r(&now, &timeinfo);
    Serial.printf("\nCurrent time: %s", asctime(&timeinfo));
    return true;
}

void Network::initMQTT() {
    while (!mqttClient.connected()) {
        mqttClient.setClient(secureNetworkClientHiveMQ);
        mqttClient.setServer(mqttServer, mqttPort);
        Serial.println("MQTT client buffer size: " + String(mqttClient.getBufferSize()));

        if (mqttClient.connect(sensorId, mqttUsername, mqttPassword)) {
            Serial.print("\nConnected to MQTT broker, state: ");
            Serial.print(mqttClient.state());

            mqttTopic = String(mqttUsername) + "/" + mqttTopic + String(sensorId) + "/";

            Serial.print("\nSubscribing to topic: ");
            Serial.println(mqttTopic);

            Serial.print("\nSetting up Subscription to the topic: ");
            Serial.print(mqttTopic);

            try {
                mqttClient.subscribe(mqttTopic.c_str(), 0);
                Serial.println("\nSubscription successful!");
            } catch (const std::exception& e) {
                Serial.println("\nSubscription failed!");
                Serial.println(e.what());
            }

        } else {
            Serial.print("\nFailed to connect to MQTT broker, state: ");
            Serial.print(mqttClient.state());
            Serial.println("\nTrying again in 3 seconds...");
            delay(3000);
        }
    }
}

void Network::connect() {
    Serial.println("Checking wifi...");

    if(WiFi.status() != WL_CONNECTED) {
        connectToWifi();

        while (!syncronizeClock()) {
            NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

            if (syncronizeClock()) break;
            else {
                Serial.println("\nFailed to synchronize clock, trying again in 3 seconds...");
                delay(3000);
            }
        }
        
        initMQTT();

        // Blue light on
        changeColor(0, 0, 255);
    }
}

void Network::initWifi() {
    // Connect to WiFi
    WiFi.begin(ssid, pass);
    WiFi.mode(WIFI_STA);

    //secureNetworkClient.setCACert(rootCA);
    //secureNetworkClientHiveMQ.setCACert(rootCaHiveMQ);
    connect();
}