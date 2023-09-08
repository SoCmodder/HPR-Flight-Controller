#ifndef HPR_SENSOR_HELPER_H
#define HPR_SENSOR_HELPER_H

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SD_CS_PIN 17

namespace HPR {
  class SensorHelper {
    public:
      void initDisplay();
      void drawDisplayData(float altitude, String flightStatus);

      void initStorage();
      void openFile();
      void closeFile();
      void writeLogHeader();
      
      void initTime();
      String getCurrentTime();
      
      void initAltimeter();
      void triggerAltitudeUpdate();
  };
}

#endif