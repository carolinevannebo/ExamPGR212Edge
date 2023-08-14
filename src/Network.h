#ifndef NETWORK_H
#define NETWORK_H

#include "Credentials.h"

// Used for sending data to server
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "Credentials.h"

// Used for validating certificates
#include <FS.h>
#include <NTPClient.h>
// ... iclude certificate files

#include <MQTT.h> // Used for MQTT -- tror denne kan fjernes pga linja under
#include <PubSubClient.h> // Used for real-time communication
#include "Light.h"

class Network {
    private:
        WiFiUDP ntpUDP;
        HTTPClient http;
        PubSubClient mqttClient;
        WiFiClient networkClient;
        WiFiClientSecure secureNetworkClient, secureNetworkClientHiveMQ;

        NTPClient timeClient(WiFiUDP ntpUDP, const char* ntpServer, long gmtOffset_sec, int daylightOffset_sec);
        //NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

        Light& light = Light::getInstance();

        void initWifi();
        bool syncronizeClock(); // Hvis denne returnerer false, må vi kanskje prøve å koble til wifi på nytt
        void initMQTT();

    public:
        void init();
        void connect();

        Network() {
            init();
        };

        static Network& getInstance() { // In case I want to use singleton, but I think not for now
            static Network instance;
            return instance;
        };
};
#endif