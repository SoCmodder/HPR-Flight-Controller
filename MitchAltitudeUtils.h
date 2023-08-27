#ifndef MITCH_ALTITUDE_UTILS_H
#define MITCH_ALTITUDE_UTILS_H

#define SEA_LEVEL_HPA 1013.25
#define NUMBER_TO_AVERAGE 3
#define CHUTE_DEPLOYMENT_ALT 121.92 // 400 ft

namespace HPR {
  class MitchAlt {
    public:
      float STARTING_ALT = 0;
      float MAX_ALT = 0;
      float CURRENT_ALT = 0;
      float RELATIVE_ALT = 0;
      bool APOGEE = false;

      void initDPS();
      void initRelay();
      float readDPSAlt();
      float getAltAverage();
      float getRelativeAltAverage();
      bool shouldDeployChute();
      void deployChute();
      void printDPSAltitudeData();
  };
}

#endif
