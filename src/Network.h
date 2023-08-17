#ifndef NETWORK_H
#define NETWORK_H

// Used for sending data to server
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "Credentials.h"

// Used for validating certificates
#include <NTPClient.h>

#include <ArduinoJson.h> // Used for parsing JSON
#include <PubSubClient.h> // Used for real-time communication MQTT
#include "Light.h"
#include "Sensor.h"

class Network {
    private:
        WiFiUDP ntpUDP;
        HTTPClient httpClient;
        PubSubClient mqttClient;
        WiFiClient networkClient;
        WiFiClientSecure secureNetworkClient, secureNetworkClientHiveMQ;

        String payload;
        String clientId = "ESP32Client-";
        DynamicJsonDocument& doc = *(new DynamicJsonDocument(1024));

        Credentials& credentials = Credentials::getInstance();
        Sensor& sensor = Sensor::getInstance();
        Light& light = Light::getInstance();

        void init();
        void initWifi();
        void initMQTT();
        void requestURL();
        void requestURLBackup();
        bool syncronizeClock(); // Hvis denne returnerer false, må vi kanskje prøve å koble til wifi på nytt pga sertifikat
        void initJson(
            String id, 
            float temperature, 
            float humidity, 
            float lightIntensity, 
            float x, 
            float y, 
            float z,
            bool isDoorOpen
        );

        Network() {
            init();
        };

    public:
        static Network& getInstance() {
            static Network instance;
            return instance;
        }

        void connect();
        void updateTimeClient();
        void serverInterval();
        void loopMQTT();
        void sendToBroker(
            float temperature, 
            float humidity, 
            float lightIntensity, 
            float x, 
            float y, 
            float z,
            bool isDoorOpen
        );
        void sendToServer(
            float temperature, 
            float humidity, 
            float lightIntensity, 
            float x, 
            float y, 
            float z,
            bool isDoorOpen
        );
        
        NTPClient& timeClient = *(new NTPClient(ntpUDP, credentials.ntpServer, credentials.gmtOffsetSec, credentials.daylightOffsetSec));
};
#endif