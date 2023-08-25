#ifndef MITCH_RELAY_UTILS_H
#define MITCH_RELAY_UTILS_H

#define RELAY_IN_1 5
#define RELAY_IN_2 6

namespace HPR {
  class MitchRelay {
    public:
      void initRelay();
      void relayOn(int relayPin);
      void relayOff(int replayPin);
  };
}

#endif
