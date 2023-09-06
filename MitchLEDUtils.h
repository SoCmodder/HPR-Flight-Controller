#ifndef MITCH_LED_UTILS_H
#define MITCH_LED_UTILS_H

#define BLINK_RATE_FAST 100
#define BLINK_RATE_SLOW 600
#define LED_PIN 16

enum MitchColor { RED, GREEN, BLUE, CYAN, MAGENTA };

namespace LED {
  class MitchLED {
    public:
      void initLED();
      uint32_t getPixelColor(MitchColor color);
      void lightOn(MitchColor color);
      void lightOff();
      void blinkFast(int times, MitchColor color);
      void blinkSlow(int times, MitchColor color);
  };
}

#endif
