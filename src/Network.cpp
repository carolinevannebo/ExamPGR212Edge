#include "Network.h"

void Network::initWifi() {
    Serial.println("Attempting to connect to network: " + String(credentials.ssid));

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        WiFi.begin(credentials.ssid, credentials.pass);
        delay(5000);

        light.setYellow();
    }
        
    Serial.print("\nConnected to Wifi!");
    Serial.print("\nIP address: ");
    Serial.print(WiFi.localIP());
    Serial.print("\n");
}

bool Network::syncronizeClock() {
    // Synchronize ESP32's clock with NTP server
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    //configTime(TZ_Europe_Berlin, "pool.ntp.org", "time.nist.gov");

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
        Serial.print("\nFailed to obtain time\n");
        return false;
    }
    gmtime_r(&now, &timeinfo);
    Serial.printf("\nCurrent time: %s", asctime(&timeinfo)); // riktig dato men ikke klokkeslett, tidssone?
    Serial.print("\n");
    return true;
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
}

void Network::initMQTT() {
    Serial.print("\nStarting MQTT");
    clientId += String(random(0xffff), HEX);

    while (!mqttClient.connected()) {
        mqttClient.setClient(secureNetworkClientHiveMQ);
        mqttClient.setServer(credentials.mqttServer, credentials.mqttPort);
        mqttClient.setCallback(callback);
        Serial.println("\nMQTT client buffer size: " + String(mqttClient.getBufferSize()));

        if (mqttClient.connect(clientId.c_str(), credentials.mqttUsername, credentials.mqttPassword)) {
            Serial.print("\nConnected to MQTT broker, state: ");
            Serial.print(mqttClient.state());

            credentials.mqttTopic = String(credentials.mqttUsername) + "/" + credentials.mqttTopic + String(credentials.sensorId) + "/";

            Serial.print("\nSubscribing to topic: ");
            Serial.println(credentials.mqttTopic);

            try {
                mqttClient.subscribe(credentials.mqttTopic.c_str(), 0);
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
        timeClient.setTimeOffset(credentials.gmtOffsetSec);
        timeClient.setUpdateInterval(credentials.ntpUpdateInterval);

        light.setBlue();

        delay(1000);
    }
}

void Network::init() {
    WiFi.begin(credentials.ssid, credentials.pass);
    Serial.println("\nWifi has been initialized");

    WiFi.mode(WIFI_STA);
    Serial.println("Wifi mode has been set to WIFI_STA ");

    secureNetworkClient.setCACert(credentials.rootCert);
    secureNetworkClientHiveMQ.setCACert(credentials.rootCertHiveMQ);

    connect();
}

void Network::initJson(
        String id, 
        float temperature, float humidity, float lightIntensity, 
        float x, float y, float z,
        bool isDoorOpen) {

    payload = "";
    doc.clear();

    doc["sensorId"] = id;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["light"] = lightIntensity;
    doc["x"] = x;
    doc["y"] = y;
    doc["z"] = z;
    doc["door"] = isDoorOpen ? "Open" : "Closed";

    serializeJson(doc, payload);

    Serial.println("\nJSON payload: " + payload);
}

void Network::requestURLBackup() { // TCP
    Serial.println("\nRequesting URL with TCP...");
    Serial.println("\nSending request to: " + String(credentials.hostDomainLocal));

    if (!networkClient.connect(credentials.hostDomainLocal, credentials.hostPortLocal)) {
        Serial.println("\nFailed to connect to: " + String(credentials.hostDomainLocal));
        return;
    }

    String postRequest = String("POST ") + "/" + " HTTP/1.1\r\n" +
                "Host: " + String(credentials.hostDomainLocal) + "\r\n" +
                "Content-Type: application/json\r\n" +
                "Content-Length: " + String(payload.length()) + "\r\n" +
                "Connection: close\r\n\r\n" +
                payload;

    networkClient.print(postRequest);

    // Wait for the server to respond
    while (!networkClient.available()) {
        delay(10);
    }

    unsigned long timeout = millis();
    while (networkClient.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            networkClient.stop();
            return;
        }
    }

    String response = "";
    while (networkClient.available()) {
        response += networkClient.readString();
        Serial.print("\n Request sent: ");
        Serial.print(postRequest);
        Serial.print("\n\n Response from server: ");
        Serial.print(response);
    }

    networkClient.stop();
}

void Network::requestURL() { // SSL
    Serial.println("\nRequesting URL with SSL...");
    Serial.println("\nSending request to: " + String(credentials.hostDomain));

    httpClient.setTimeout(credentials.httpTimeout);

    if (!httpClient.begin(secureNetworkClient, String(credentials.hostDomain))) {
        Serial.println("\nFailed to connect to: " + String(credentials.hostDomain));
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
        httpClient.end();

    } else {
        Serial.println("\nError on sending POST request, error code: ");
        Serial.print(httpResponseCode);
        httpClient.end();

        Serial.println("\nHandling error...");
        requestURLBackup();
    }
}

void Network::updateTimeClient() {
    timeClient.update();

    if (timeClient.getMinutes() == 0) {
    // Set the ESP32's internal clock every hour
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }
  }
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

void Network::sendToBroker(float temperature, float humidity, float lightIntensity, float x, float y, float z, bool isDoorOpen) {
    String topicTemp = credentials.mqttTopic + "Temperature";
    String topicHum = credentials.mqttTopic + "Humidity";
    String topicLight = credentials.mqttTopic + "LightIntensity";

    String topicX = credentials.mqttTopic + "X";
    String topicY = credentials.mqttTopic + "Y";
    String topicZ = credentials.mqttTopic + "Z";

    String topicDoor = credentials.mqttTopic + "Door";

    if (WiFi.status() == WL_CONNECTED) {
        if (mqttClient.connected()) {
            char tempPayload[20];
            char humPayload[20];
            char lightPayload[20];

            snprintf(tempPayload, sizeof(tempPayload), "%.2f *C", temperature);
            snprintf(humPayload, sizeof(humPayload), "%.2f %%", humidity);
            snprintf(lightPayload, sizeof(lightPayload), "%.2f lux", lightIntensity);
    
            unsigned int tempPayloadLength = strlen(tempPayload);
            unsigned int humPayloadLength = strlen(humPayload);
            unsigned int lightPayloadLength = strlen(lightPayload);

            Serial.println("\nSending data to broker...");
            
            try {
                Serial.print("\nPublishing temperature: ");
                bool publishTemp = mqttClient.publish(topicTemp.c_str(), tempPayload, tempPayloadLength);
                Serial.print(publishTemp ? "successful!" : "failed!");

                Serial.print("\nPublishing humidity: ");
                bool publishHum = mqttClient.publish(topicHum.c_str(), humPayload, humPayloadLength);
                Serial.print(publishHum ? "successful!" : "failed!");

                Serial.print("\nPublishing light intensity: ");
                bool publishLight = mqttClient.publish(topicLight.c_str(), lightPayload, lightPayloadLength);
                Serial.print(publishLight ? "successful!" : "failed!");

                Serial.print("\nPublishing X: ");
                bool publishX = mqttClient.publish(topicX.c_str(), String(x).c_str());
                Serial.print(publishX ? "successful!" : "failed!");

                Serial.print("\nPublishing Y: ");
                bool publishY = mqttClient.publish(topicY.c_str(), String(y).c_str());
                Serial.print(publishY ? "successful!" : "failed!");

                Serial.print("\nPublishing Z: ");
                bool publishZ = mqttClient.publish(topicZ.c_str(), String(z).c_str());
                Serial.print(publishZ ? "successful!" : "failed!");

                Serial.print("\nPublishing door: ");
                bool publishDoor = isDoorOpen ? mqttClient.publish(topicDoor.c_str(), "Open") : mqttClient.publish(topicDoor.c_str(), "Closed");
                Serial.print(publishDoor ? "successful!" : "failed!");

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

void Network::sendToServer(float temperature, float humidity, float lightIntensity, float x, float y, float z, bool isDoorOpen) {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nSending data to server...");
        initJson(credentials.sensorId, temperature, humidity, lightIntensity, x, y, z, isDoorOpen);
        requestURL();
        
    } else {
        Serial.println("\nWiFi not connected, trying to reconnect...");
        connect();
    }
}

void Network::serverInterval() {
    unsigned long currentTime = millis();
    if (currentTime - credentials.lastRequestSent > credentials.requestInterval) {
        Serial.println("Sending request to server");
        sendToServer(
            sensor.getTemperature(), 
            sensor.getHumidity(), 
            sensor.getLightIntensity(), 
            sensor.getX(), 
            sensor.getY(), 
            sensor.getZ(),
            sensor.getIsDoorOpen()
        );
        credentials.lastRequestSent = currentTime;
    }
}