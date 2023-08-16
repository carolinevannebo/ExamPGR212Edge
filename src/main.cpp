#include <Arduino.h>

#include "Light.h" // Used for light
#include "Sensor.h" // Used for sensors @todo: magnet to sensor to detect door open/close
#include "Network.h" // Used for sending data to server
// @todo - test OTA library
//#include "OTA.h"

void setup() {
  //Serial.end();
  Serial.begin(115200);
  delay(5000);
  //while (!Serial) delay(10);
  Serial.println("Starting");

  Serial.println("Starting LED");
  Light& light = Light::getInstance();

  Serial.println("Starting Sensor");
  Sensor& sensor = Sensor::getInstance();

  Serial.println("Starting Network");
  Network& network = Network::getInstance();

  // Initialize cookies
  // Initialize OTA
  /*
  // initialize Serial1 for communication with the WiFi module
  Serial1.begin(115200);
  WiFi.init(&Serial1);

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println();
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  // waiting for connection to Wifi network with settings in the WiFi module
  Serial.println("Waiting for connection to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print('.');
  }
  Serial.println();
  Serial.println("Connected to WiFi network.");

  // start the WiFi OTA library with internal (flash) based storage
  ArduinoOTA.begin(WiFi.localIP(), "Arduino", "password", InternalStorage);

  // you're connected now, so print out the status:
  printWifiStatus();
  */
}

void loop() {
  // check for WiFi OTA updates
  //ArduinoOTA.poll();

  Serial.println("\n\nLooping");
  // add your normal loop code below ...
  Network& network = Network::getInstance();
  network.updateTimeClient();
  network.loopMQTT();

  Sensor& sensor = Sensor::getInstance();
  sensor.read();
  
  network.sendToBroker(
    sensor.getTemperature(), 
    sensor.getHumidity(), 
    sensor.getLightIntensity(), 
    sensor.getX(), 
    sensor.getY(), 
    sensor.getZ()
  );
  
  // @todo determine danger, if true:
  //network.sendToServer(sensor.getX(), sensor.getY(), sensor.getZ());
  delay(1000);
}

/*void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}*/