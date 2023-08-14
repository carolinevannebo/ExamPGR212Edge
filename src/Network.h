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

class Network {
        WiFiUDP ntpUDP;
        HTTPClient http;
        PubSubClient mqttClient;
        WiFiClient networkClient;
        WiFiClientSecure secureNetworkClient, secureNetworkClientHiveMQ;

        NTPClient timeClient(WiFiUDP ntpUDP, const char* ntpServer, long gmtOffset_sec, int daylightOffset_sec);
        //NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

        void initWifi();
        void connect();
        void connectToWifi();
        bool syncronizeClock(); // Hvis denne returnerer false, må vi kanskje prøve å koble til wifi på nytt
        void initMQTT();

        Network() {
            initWifi();
        };

        ~Network() {};
};
#endif