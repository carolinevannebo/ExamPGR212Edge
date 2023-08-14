#include "Light.h"

void Light::initializeStrip() {
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