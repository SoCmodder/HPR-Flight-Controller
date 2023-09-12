#ifndef HPR_SENSOR_HELPER_H
#define HPR_SENSOR_HELPER_H

namespace HPR {
  enum MitchColor { RED, GREEN, BLUE, CYAN, MAGENTA };
  #define SCREEN_WIDTH 128 // OLED display width, in pixels
  #define SCREEN_HEIGHT 32 // OLED display height, in pixels
  #define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
  #define SD_CS_PIN 5
  #define SD_FILE_NAME "hpr-log.txt"
  #define FEET_PER_METER 3.28084
  #define WAVESHARE_LED_PIN 16
  #define BLINK_RATE_FAST 100
  #define BLINK_RATE_SLOW 600

  class SensorHelper {
    public:
      void initWavesharePico();

      void initDisplay();
      void drawDisplayData(int starting_altitude, int current_altitude, int max_altitude, String flightStatus);

      void initStorage();
      void openFile();
      void closeFile();
      void writeLogHeader();
      void writeLine(String line);
      
      void initTime();
      String getCurrentTime();
      
      void initAltimeter();
      void triggerAltitudeUpdate();
      int getCurrentAltitude();

      void initWaveShareNeoPixel();
      void pixelOn(MitchColor color);
      void pixelOff();
      void blinkPixelFast(int times, MitchColor color);
      void blinkPixelSlow(int times, MitchColor color);
      uint32_t getPixelColor(MitchColor color); 
  };
}

#endif