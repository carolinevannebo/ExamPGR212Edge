// Used for sensors @todo: magnet to sensor to detect door open/close
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// Used for software SPI
#define LIS3DH_CLK 13
#define LIS3DH_MISO 12
#define LIS3DH_MOSI 11
#define LIS3DH_CS 10 // Also used for hardware SPI

// Sensor instances
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

float x, y, z;