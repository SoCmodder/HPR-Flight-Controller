#ifndef ORIENTATION_UTILS_H
#define ORIENTATION_UTILS_H

#define BNO055_SAMPLERATE_DELAY_MS 10 //how often to read data from the board

namespace HPR {
  class Orientation {
    public:
      void initBNO();
      String getVelocity();
      String getDirection();
      void printLinearAccelerationData();
      void printOrientationData();

      void printEvent(sensors_event_t* event);
      void demoLoop();
  };
}

#endif
