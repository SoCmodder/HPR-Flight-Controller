#ifndef MITCH_SD_UTILS_H
#define MITCH_SD_UTILS_H

#define SD_CS_PIN 10
#define SD_FILE_NAME "log.txt"

namespace HPR {
  class MitchSD {
    public:
      void initRTC();
      void initSD();
      void closeFile();
      void openFile();
      void writeLogHeader();
      void writeDataLineBlocking(String data);
      String getTimeStamp();
  };
}

#endif
