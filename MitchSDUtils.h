#ifndef MITCH_SD_UTILS_H
#define MITCH_SD_UTILS_H

#define SD_CS_PIN 10
#define SD_FILE_NAME "datalog.txt"

namespace HPR {
  class MitchSD {
    public:
      void closeFile();
      void openFile();
      void initRTC();
      void writeLogHeader();
      void initSD();
      void writeDataLineBlocking(String data);
  };
}

#endif
