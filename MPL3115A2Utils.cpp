#include <Adafruit_MPL3115A2.h>
#include <arduino.h>
#include "MPL3115A2Utils.h"

using namespace HPR;

Adafruit_MPL3115A2 sensor;

void MPLUtil::initMPL() {
  if (!sensor.begin()) {
    Serial.println("Could not find MPL sensor. Check wiring.");
    while(1);
  }
  sensor.setMode(MPL3115A2_ALTIMETER);
  Serial.println("MPL3115A2 OK!");
  delay(500);
}

float MPLUtil::readMPLAlt() {
  return sensor.getAltitude();
}

void MPLUtil::printMPLAltitudeData() {
  Serial.print("MPL Alt: ");
  Serial.print(String(readMPLAlt()));
  Serial.println("");
}
