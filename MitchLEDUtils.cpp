#include <Adafruit_NeoPixel.h>
#include "MitchLEDUtils.h"

Adafruit_NeoPixel pixel(1, LED_PIN, NEO_RGB + NEO_KHZ800);

using namespace HPR;

void MitchLED::initLED() {
  pixel.begin();
  Serial.println("LED OK!");
  delay(500);
}

/**
  ********************
    Color = [R, G, B]
  ********************
  Yellow = 150, 150, 0
  Cyan = 0, 150, 150
  Purple = 150, 0, 150
  White = 150, 150, 150  
  Magenta = 153, 0, 153
  Orange = 255, 128, 0
*/
uint32_t MitchLED::getPixelColor(MitchColor color) {
  uint32_t c = pixel.Color(0, 0, 0);
  switch(color) {
    case RED: 
      c = pixel.Color(150, 0, 0);
      break;
    case GREEN:
      c = pixel.Color(0, 150, 0);
      break;
    case BLUE:
      c = pixel.Color(0, 0, 150);
      break;
    case CYAN:
      c = pixel.Color(0, 150, 150);
      break;
    case MAGENTA:
      c = pixel.Color(153, 0, 153);
      break; 
  }
  return c;
}

void MitchLED::lightOn(MitchColor color) {
  uint32_t pColor = getPixelColor(color);
  pixel.setPixelColor(0, pColor);
  pixel.show();
}

void MitchLED::lightOff() {
  pixel.clear();
  pixel.show();
}

void MitchLED::blinkFast(int times, MitchColor color) {
  uint32_t pixelColor = getPixelColor(color);
  for(int i=0; i<times; i++) {
    pixel.setPixelColor(0, pixelColor);
    pixel.show();
    delay(BLINK_RATE_FAST);
    pixel.clear();
    pixel.show();
    delay(BLINK_RATE_FAST);
  }
}

void MitchLED::blinkSlow(int times, MitchColor color) {
  uint32_t pixelColor = getPixelColor(color);
  for(int i=0; i<times; i++) {
    pixel.setPixelColor(0, pixelColor);
    pixel.show();
    delay(BLINK_RATE_SLOW);
    pixel.clear();
    pixel.show();
    delay(BLINK_RATE_SLOW);
  }
}
