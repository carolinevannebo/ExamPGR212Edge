#ifndef LIGHT_H
#define LIGHT_H

#include <Adafruit_DotStar.h> // Used for light
#include "Credentials.h"

//Use these pin definitions for the ItsyBitsy M4
#define DATAPIN 33
#define CLOCKPIN 21
#define NUMPIXELS 1 // There is only one pixel on the board

class Light {
  Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
  
  void initializeStrip();
  void changeColor(int r, int g, int b);

  Light() {
      initializeStrip();
  };

  ~Light() {};
};
#endif