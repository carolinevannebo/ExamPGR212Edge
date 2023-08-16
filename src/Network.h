#ifndef NETWORK_H
#define NETWORK_H

// Used for sending data to server
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "Credentials.h"

// Used for validating certificates
#include <FS.h>
#include <NTPClient.h>
// ... iclude certificate files

//#include <MQTT.h> // Used for MQTT -- tror denne kan fjernes pga linja under
#include <ArduinoJson.h> // Used for parsing JSON
#include <PubSubClient.h> // Used for real-time communication MQTT
#include "Light.h"

class Network {
    private:
        WiFiUDP ntpUDP;
        HTTPClient httpClient;
        PubSubClient mqttClient;
        WiFiClient networkClient;
        WiFiClientSecure secureNetworkClient, secureNetworkClientHiveMQ;

        String payload;
        DynamicJsonDocument& doc = *(new DynamicJsonDocument(1024));

        Credentials credentials;
        Light& light = Light::getInstance();

        //const char* cert = rootCert;
        //const char* certMQ = rootCertHiveMQ;

        void init();
        void initWifi();
        void initMQTT();
        void requestURL();
        bool syncronizeClock(); // Hvis denne returnerer false, må vi kanskje prøve å koble til wifi på nytt pga sertifikat
        void initJson(
            String id, 
            float temperature, 
            float humidity, 
            float lightIntensity, 
            float x, 
            float y, 
            float z
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
        void loopMQTT();
        void sendToBroker(
            float temperature, 
            float humidity, 
            float lightIntensity, 
            float x, 
            float y, 
            float z
        );
        void sendToServer(
            float temperature, 
            float humidity, 
            float lightIntensity, 
            float x, 
            float y, 
            float z
        );


        NTPClient& timeClient = *(new NTPClient(ntpUDP, credentials.ntpServer, credentials.gmtOffsetSec, credentials.daylightOffsetSec));
};
#endif