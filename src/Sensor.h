#ifndef SENSOR_H
#define SENSOR_H

// Used for sensors @todo: magnet to sensor to detect door open/close
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

#include <Adafruit_SHT31.h>
#include <Adafruit_LTR329_LTR303.h>

#include <Light.h>

class Sensor {
    private:
        // Sensor instances
        Adafruit_LIS3DH lis = Adafruit_LIS3DH();
        float x, y, z;

        // x: left/right
        // z: up/down
        // y: forward/backward

        Adafruit_SHT31 sht = Adafruit_SHT31();
        Adafruit_LTR329 ltr = Adafruit_LTR329();
        float temperature, humidity, lightIntensity;

        Light& light = Light::getInstance();

        void initLIS();
        void initSHT();
        void initLTR();

        void readLIS();
        void readSHT();
        void readLTR();

        void printLIS();
        void printSHT();
        void printLTR();

    public:
        void init();
        void read();

        float getX();
        float getY();
        float getZ();

        float getTemperature();
        float getHumidity();
        float getLightIntensity();

        Sensor(){
            init();
        };
};
#endif