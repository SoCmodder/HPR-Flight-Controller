#include <Adafruit_MPL3115A2.h>
#include <arduino.h>
#include "MPL3115A2Utils.h"

using namespace HPR;

Adafruit_MPL3115A2 mpl;

void MPLUtil::initMPL() {
  if (!mpl.begin()) {
    Serial.println("Could not find MPL sensor. Check wiring.");
    while(1);
  }
  Serial.println("MPL3115A2 OK!");
  delay(500);
}

float MPLUtil::readMPLAlt() {
  return mpl.getAltitude();
}

void MPLUtil::printMPLAltitudeData() {
  Serial.print("MPL Alt: ");
  Serial.print(String(readMPLAlt()));
  Serial.println("");
}
