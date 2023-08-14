#include "Light.h"

void Light::init() {
  // Initialize all pixels to 'off'
  for(int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);
  }
  strip.begin(); // Initialize pins for output
  strip.setBrightness(ledBrightness);
  strip.show();  // Turn all LEDs off ASAP
}

void Light::changeColor(int r, int g, int b) {
  for(int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, r, g, b);
    strip.show();
  }
}

void Light::setRed() {
  changeColor(0, 255, 0);
}

void Light::setOrange() {
  changeColor(255, 165, 0);
}

void Light::setYellow() {
  changeColor(255, 255, 0);
}

void Light::setGreen() {
  changeColor(255, 0, 0);
}

void Light::setBlue() {
  changeColor(0, 0, 255);
}