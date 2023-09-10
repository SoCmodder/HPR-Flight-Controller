#ifndef HPR_SENSOR_HELPER_H
#define HPR_SENSOR_HELPER_H

namespace HPR {
  #define SCREEN_WIDTH 128 // OLED display width, in pixels
  #define SCREEN_HEIGHT 32 // OLED display height, in pixels
  #define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
  #define SD_CS_PIN 13
  #define SD_FILE_NAME "hpr-log.txt"
  #define FEET_PER_METER 3.28084
  #define UART_ID uart0
  #define BAUD_RATE 115200
  // We are using pins 0 and 1, but see the GPIO function select table in the
  // datasheet for information on which other pins can be used.
  #define UART_TX_PIN 8
  #define UART_RX_PIN 9

  class SensorHelper {
    public:
      void initDisplay();
      void drawDisplayData(int starting_altitude, int current_altitude, int max_altitude, String flightStatus);

      void initStorage();
      void initUARTStorage();
      void openFile();
      void closeFile();
      void writeLogHeader();
      void writeLine(String line);
      void writeUARTLine(String line);
      
      void initTime();
      String getCurrentTime();
      
      void initAltimeter();
      void triggerAltitudeUpdate();
      int getCurrentAltitude();
  };
}

#endif