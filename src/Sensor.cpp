#include "Sensor.h"

float Sensor::getX() { return x; }
float Sensor::getY() { return y; }
float Sensor::getZ() { return z; }

float Sensor::getTemperature() { return temperature; }
float Sensor::getHumidity() { return humidity; }
float Sensor::getLightIntensity() { return lightIntensity; }

void Sensor::initLIS() {
  Serial.print("\nLIS3DH accelometer test: ");

  if (!lis.begin(0x18)) {
    Serial.print("Could not find LIS3DH, check wiring! ");

    light.setOrange();
    while (1) yield();

  } else {
    Serial.print("LIS3DH sensor found!");

    /* 2 << n is a bitshift operation that multiplies 2 by 2 raised to the power of n.
    The range setting is a 2-bit value, so the range setting can be 0, 1, 2, or 3.
    The G stands for gravity, and is the unit of measurement for acceleration.*/
    Serial.print("\nRange = "); 
    Serial.print(2 << lis.getRange());
    Serial.print("G");
    Serial.print("\n");
  }
}

void Sensor::initSHT() {
  Serial.print("\nSHT31 temperature-humidity sensor test: ");

  if (!sht.begin(0x44)) {
    Serial.print("Could not find SHT31, check wiring! ");

    light.setOrange();
    while (1) yield();

  } else {
    Serial.print("SHT31 sensor found!");

    Serial.print("\nHeater Enabled State: ");
    if (sht.isHeaterEnabled())
      Serial.print("ENABLED");
    else
      Serial.print("DISABLED");
      Serial.print("\n");
  }
}

void Sensor::initLTR() {
  Serial.print("\nLTR329 light sensor test: ");

  if (!ltr.begin()) { // 0x29 is already default I2C address
    Serial.print("Could not find LTR329, check wiring! ");

    light.setOrange();
    while (1) yield();

  } else {
    Serial.print("LTR329 sensor found!");

    Serial.print("\nLight gain: ");
    Serial.print(ltr.getGain());
    Serial.print("\nLight integration time: ");
    Serial.print(ltr.getIntegrationTime());
    Serial.print("\n\n");
  }
}

void Sensor::init() {
  Wire.begin(3, 4);
  initLIS();
  initSHT();
  initLTR();
  light.setGreen();
}

void Sensor::printLIS() {
  /* Display the results (acceleration is measured in m/s^2) */
    Serial.print("\nNormalized acceleration data: ");
    Serial.print("\nX: "); Serial.print(x);
    Serial.print("  \tY: "); Serial.print(y);
    Serial.print("  \tZ: "); Serial.print(z);
    Serial.print(" m/s^2 ");
}

void Sensor::printSHT() {
  Serial.print("\nTemperature: ");
  Serial.print(temperature);
  Serial.print(" deg C");

  Serial.print("\tHumidity: ");
  Serial.print(humidity);
  Serial.print(" %");
}

void Sensor::printLTR() {
  Serial.print("\nLight intensity: ");
  Serial.print(lightIntensity);
  Serial.print(" lux");
}

void Sensor::readLIS() {
  sensors_event_t event;

  if(lis.getEvent(&event)) {
    x = event.acceleration.x;
    y = event.acceleration.y;
    z = event.acceleration.z;

    printLIS();
    light.setGreen();

  } else {
    Serial.print("\nNo new data.");
    light.setOrange();
  }
}

void Sensor::readSHT() {
  temperature = sht.readTemperature();
  humidity = sht.readHumidity();

  if(isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from SHT31 sensor.");

    light.setOrange();
    return;
  }

  printSHT();
  light.setGreen();
}

void Sensor::readLTR() {
  uint16_t broadband, ir;

  if (ltr.newDataAvailable()) {
    bool valid = ltr.readBothChannels(broadband, ir);

    if (valid) {
      lightIntensity = broadband;

      printLTR();
      light.setGreen();
    
    } else {
      Serial.print("\nSomething went wrong reading light sensor channel.");
      light.setOrange();
    }
  } else {
    Serial.print("\nNo new data.");
  }
}

void Sensor::read() {
  readLIS();
  readSHT();
  readLTR();
}