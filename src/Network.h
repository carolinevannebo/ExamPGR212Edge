#ifndef NETWORK_H
#define NETWORK_H

#include "Credentials.h"

// Used for sending data to server
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "Credentials.h"

// Used for validating certificates
#include <FS.h>
#include <NTPClient.h>
// ... iclude certificate files

#include <MQTT.h> // Used for MQTT -- tror denne kan fjernes pga linja under
#include <ArduinoJson.h> // Used for parsing JSON
#include <PubSubClient.h> // Used for real-time communication MQTT
#include <Light.h>

class Network {
    private:
        WiFiUDP ntpUDP;
        HTTPClient httpClient;
        PubSubClient mqttClient;
        WiFiClient networkClient;
        WiFiClientSecure secureNetworkClient, secureNetworkClientHiveMQ;
        NTPClient& timeClient = *(new NTPClient(ntpUDP, ntpServer, gmtOffsetSec, daylightOffsetSec));

        String payload;
        DynamicJsonDocument& doc = *(new DynamicJsonDocument(1024));

        Light& light = Light::getInstance();

        void initWifi();
        bool syncronizeClock(); // Hvis denne returnerer false, må vi kanskje prøve å koble til wifi på nytt
        void initMQTT();
        void initJson(String id, float x, float y, float z);
        void requestURL();

    public:
        void init();
        void connect();
        void sendToBroker(float x, float y, float z);
        void sendToServer(float x, float y, float z);
        NTPClient getTimeClient() { return timeClient; };

        Network() {
            init();
        };

        static Network& getInstance() { // In case I want to use singleton, but I think not for now
            static Network instance;
            return instance;
        };
};
#endif