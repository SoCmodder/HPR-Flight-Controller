#include <arduino.h>
#include "MitchRelayUtils.h"

using namespace N;

void MitchRelay::initRelay() {
  pinMode(RELAY_IN_1, OUTPUT);
  digitalWrite(RELAY_IN_1, HIGH);
}

/** 
  Turns on relay with given relay pin
**/  
void MitchRelay::relayOn(int relayPin) {
  digitalWrite(relayPin, LOW);
}

/** 
  Turns off relay with given relay pin
**/  
void MitchRelay::relayOff(int relayPin) {
  digitalWrite(relayPin, HIGH);
}
