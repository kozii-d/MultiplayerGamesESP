#include <Arduino.h>

#include "Config.h"

#include "utils/DisplayManager.h"
#include "games/Pong/Pong.h"

DisplayManager oled;
Pong pong(oled);

void setup() {
  oled.begin();
  Serial.begin(115200);
  pong.begin();

}

void loop() {
  pong.loop();
}
