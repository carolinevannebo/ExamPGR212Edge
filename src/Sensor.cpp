#include "Sensor.h"

void Sensor::init() {
  Serial.print("\nLIS3DH accelometer test: ");

  if (!lis.begin(0x18)) {
    Serial.print("Could not start, check wiring! ");

    // Orange light on
    light.setOrange();

    while (1) yield();
  } else {
    Serial.print("LIS3DH found!");

    /* 2 << n is a bitshift operation that multiplies 2 by 2 raised to the power of n.
    The range setting is a 2-bit value, so the range setting can be 0, 1, 2, or 3.
    The G stands for gravity, and is the unit of measurement for acceleration.*/
    Serial.print("\nRange = "); 
    Serial.print(2 << lis.getRange());
    Serial.print("G");

    // Green light on
    light.setGreen();
  }
}

void Sensor::read() {
  sensors_event_t event;

  if(lis.getEvent(&event)) {
    x = event.acceleration.x;
    y = event.acceleration.y;
    z = event.acceleration.z;

    /* Display the results (acceleration is measured in m/s^2) */
    Serial.print("\nNormalized Data: ");
    Serial.print("\nX: "); Serial.print(x);
    Serial.print("  \tY: "); Serial.print(y);
    Serial.print("  \tZ: "); Serial.print(z);
    Serial.print(" m/s^2 ");

    // Green light on
    light.setGreen();
  } else {
    Serial.print("\nNo new data.");
    // Orange light on
    light.setOrange();
  }
}