#include "Arduboy2Ex.h"

Arduboy2Ext arduboy;

#include "game.h"

void setup() {
  arduboy.boot();
  arduboy.flashlight();
  arduboy.setFrameRate(60);
}

void loop() {
  tick();
}

