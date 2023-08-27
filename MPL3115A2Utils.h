#ifndef MPL3115A2_UTILS_H
#define MPL3115A2_UTILS_H

namespace HPR {
  class MPLUtil {
    public:
      void initMPL();
      float readMPLAlt();
      void printMPLAltitudeData();
  };
}

#endif