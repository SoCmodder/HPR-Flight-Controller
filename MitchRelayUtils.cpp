#include <arduino.h>
#include "MitchRelayUtils.h"

using namespace HPR;

void MitchRelay::initRelay() {
  pinMode(RELAY_IN_1, OUTPUT);
  digitalWrite(RELAY_IN_1, HIGH);
}

/** 
  Turns on relay with given relay pin
**/  
void MitchRelay::relayOn(int relayPin) {
  digitalWrite(relayPin, LOW);
  delay(5000);
}

/** 
  Turns off relay with given relay pin
**/  
void MitchRelay::relayOff(int relayPin) {
  digitalWrite(relayPin, HIGH);
  delay(200);
}
