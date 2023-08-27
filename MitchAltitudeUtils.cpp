#include <Adafruit_DPS310.h>
#include <Adafruit_BNO055.h>
#include "MitchRelayUtils.h"
#include "MitchAltitudeUtils.h"
#include <arduino.h>

using namespace HPR;

HPR::MitchRelay relay;
Adafruit_DPS310 dps;

void MitchAlt::initDPS() {
  if (!dps.begin_I2C()) {             // Can pass in I2C address here
    Serial.println("Failed to find DPS");
    while (1) yield();
  }
  dps.configurePressure(DPS310_128HZ, DPS310_128SAMPLES);
  dps.configureTemperature(DPS310_128HZ, DPS310_128SAMPLES);
  delay(1000);
  Serial.println("DPS310 OK!");
}

void MitchAlt::initRelay() {
  relay.initRelay();
}

float MitchAlt::readDPSAlt() {
  return dps.readAltitude(SEA_LEVEL_HPA);  
}

float MitchAlt::getAltAverage() {
  float sum = 0.0;
  for (int i=0; i<NUMBER_TO_AVERAGE; i++) {
    sum += readDPSAlt();
  }
  return sum / NUMBER_TO_AVERAGE;
}

float MitchAlt::getRelativeAltAverage() {
  return readDPSAlt() - STARTING_ALT;  
}

/** TODO: Check acceleration and vector as well */
bool MitchAlt::shouldDeployChute() {
  bool deploy = false;
  float altDrop = MAX_ALT - CURRENT_ALT;
  // if current alt is less than (CHUTE_DEPLOYMENT_ALT) feet above starting point and the altitude has dropped more than
  // (CHUTE_DEPLOYMENT_ALT) feet (121.92 meters), then return true.
  if ((CURRENT_ALT < (STARTING_ALT + CHUTE_DEPLOYMENT_ALT)) && (altDrop > 121.92)) {
    deploy = true;
  }
  return deploy;
}

void MitchAlt::deployChute() {
  relay.relayOn(RELAY_IN_2);
  relay.relayOff(RELAY_IN_2);
}

void MitchAlt::printDPSAltitudeData() {
  Serial.print("DPS310 => Starting Alt: ");
  Serial.print(String(STARTING_ALT));
  Serial.print(", Current Alt: ");
  Serial.print(String(CURRENT_ALT));
  Serial.print(", Relative Alt: ");
  Serial.print(String(RELATIVE_ALT));
  Serial.print(", Max Alt: ");
  Serial.print(String(MAX_ALT));
  Serial.println("");
}
