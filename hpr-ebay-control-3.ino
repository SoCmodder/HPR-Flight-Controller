#include <Wire.h>
#include "MitchAltitudeUtils.h"
#include "MitchSDUtils.h"
#include "MitchLEDUtils.h"
#include "MitchRelayUtils.h"

int cycles = 0;
int CYCLE_MAX = 10;

using namespace L;

L::MitchLED led;

/***********************************************
 **************** SETUP *************************
 ***********************************************/
void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10);

  led.initLED();
  initRTC();
  initSD();
  relay.initRelay();
  initBNO();
  initMPL();
  initDPS();

  STARTING_ALT = getAltAverage();   
  delay(500);

  led.blinkFast(3, GREEN);
  delay(200);
}

/***********************************************
 **************** LOOP *************************
 ***********************************************/
void loop() {
  CURRENT_ALT = getAltAverage(); // average of last NUMBER_TO_AVERAGE readings
  RELATIVE_ALT = getRelativeAltAverage();

  if (cycles < CYCLE_MAX) {
    writeDataLineBlocking("Current Alt: " + String(CURRENT_ALT));
    writeDataLineBlocking("Relative Alt: " + String(RELATIVE_ALT));
    cycles++;
  }
  if (cycles == CYCLE_MAX) {
    closeFile();
    led.lightOn(CYAN);
    while(1);
  }

  if (CURRENT_ALT > MAX_ALT) {
    // update max altitude
    MAX_ALT = CURRENT_ALT;
  } else if (shouldDeployChute()) {
    // chute deployment altitude reached
    deployChute();
  }

  printDPSAltitudeData();
  //printMPLAltitudeData();

  delay(200);
}
