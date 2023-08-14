#ifndef SENSOR_H
#define SENSOR_H

// Used for sensors @todo: magnet to sensor to detect door open/close
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

#include <Light.h>

class Sensor {
    private:
        // Sensor instances
        Adafruit_LIS3DH lis = Adafruit_LIS3DH();
        float x, y, z;

        // x: left/right
        // z: up/down
        // y: forward/backward

        Light& light = Light::getInstance();

    public:
        void init();
        void read();
        void print();
        float getX();
        float getY();
        float getZ();

        Sensor(){
            init();
        };
};
#endif