#include "Network.h"

void Network::initWifi() {
    Serial.println("Attempting to connect to network: " + String(ssid));

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        WiFi.begin(ssid, pass);
        delay(5000);

        // Yellow light on
        light.setYellow();
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
        initWifi();

        while (!syncronizeClock()) {
            Serial.print(".");
            delay(500);
            if (syncronizeClock()) break;
            else {
                Serial.println("\nFailed to synchronize clock, trying again in 3 seconds...");
                delay(3000);
            }
        }
        
        initMQTT();

        timeClient.begin();
        timeClient.setTimeOffset(gmtOffsetSec);
        timeClient.setUpdateInterval(ntpUpdateInterval);

        // Blue light on
        light.setBlue();

        delay(1000);
    }
}

void Network::init() {
    // Connect to WiFi
    WiFi.begin(ssid, pass);
    WiFi.mode(WIFI_STA);

    // @todo
    //secureNetworkClient.setCACert(rootCA);
    //secureNetworkClientHiveMQ.setCACert(rootCaHiveMQ);
    connect();
}

void Network::initJson(String id, float x, float y, float z) {
    payload = "";
    doc.clear();

    doc["sensorId"] = id;
    doc["sensorX"] = x;
    doc["sensorY"] = y;
    doc["sensorZ"] = z;

    serializeJson(doc, payload);

    Serial.println("\nJSON payload: " + payload);
}

void Network::requestURL() {
    Serial.println("\nSending request to: " + String(hostDomain));

    httpClient.setTimeout(httpTimeout);

    if (!httpClient.begin(secureNetworkClient, String(hostDomain))) {
        Serial.println("\nFailed to connect to: " + String(hostDomain));
        return;
    }

    httpClient.addHeader("Content-Type", "application/json");
    int httpResponseCode = httpClient.POST(payload);

    if (httpResponseCode > 0) {
        Serial.println("\nPOST request sent successfully");
        Serial.print("HTTP response code: ");
        Serial.println(httpResponseCode);

        // Get the response from the server
        String response = httpClient.getString();
        Serial.print("Response from server: ");
        Serial.println(response);

        light.setGreen();
    } else {
        Serial.println("\nError on sending POST request, error code: ");
        Serial.println(httpResponseCode);
    }

    Serial.println("Closing connection");
    httpClient.end();
}

void Network::loopMQTT() {
    // Handle reconnection to MQTT broker
    if (!mqttClient.connected()) {
        Serial.println("Disconnected from MQTT broker");
        initMQTT();
    }

    mqttClient.loop();
    delay(10);
}

void Network::sendToBroker(float x, float y, float z) {
    String topicX = mqttTopic + "X";
    String topicY = mqttTopic + "Y";
    String topicZ = mqttTopic + "Z";

    if (WiFi.status() == WL_CONNECTED) {
        if (mqttClient.connected()) {
            Serial.println("\nSending data to broker...");
            
            try {
                mqttClient.publish(topicX.c_str(), String(x).c_str());
                mqttClient.publish(topicY.c_str(), String(y).c_str());
                mqttClient.publish(topicZ.c_str(), String(z).c_str());

                Serial.println("\nPublishing successful!");
                light.setGreen();

            } catch (const std::exception& e) {
                Serial.println("\nPublishing failed!");
                Serial.println(e.what());
            }
        } else {
            Serial.println("\nMQTT client not connected, trying to reconnect...");
            initMQTT();
        }
    } else {
        Serial.println("\nWiFi not connected, trying to reconnect...");
        connect();
    }
}

void Network::sendToServer(float x, float y, float z) {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nSending data to server...");
        initJson(sensorId, x, y, z);
        requestURL();
        
    } else {
        Serial.println("\nWiFi not connected, trying to reconnect...");
        connect();
    }
}