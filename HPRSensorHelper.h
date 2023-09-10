#ifndef HPR_SENSOR_HELPER_H
#define HPR_SENSOR_HELPER_H

namespace HPR {
  #define SCREEN_WIDTH 128 // OLED display width, in pixels
  #define SCREEN_HEIGHT 32 // OLED display height, in pixels
  #define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
  #define SD_CS_PIN 17
  #define SD_FILE_NAME "hpr-log.txt"
  #define FEET_PER_METER 3.28084

  class SensorHelper {
    public:
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
  };
}

#endif