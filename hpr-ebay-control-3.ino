#include <Wire.h>
#include "MitchAltitudeUtils.h"
#include "MitchSDUtils.h"
#include "MitchLEDUtils.h"

int cycles = 0;
int CYCLE_MAX = 10;

/***********************************************
 **************** SETUP *************************
 ***********************************************/
void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10);

  initLED();
  initRTC();
  initSD();
  initRelay();
  initBNO();
  initMPL();
  initDPS();

  STARTING_ALT = readDPSAlt();   
  delay(500);

  blinkFast(3, GREEN);
  delay(200);
}

/***********************************************
 **************** LOOP *************************
 ***********************************************/
void loop() {
  CURRENT_ALT = getAltAverage(); // average of last NUMBER_TO_AVERAGE readings

  if (cycles < CYCLE_MAX) {
    writeDataLineBlocking("Current Alt: " + String(CURRENT_ALT));
    cycles++;
  }
  if (cycles == CYCLE_MAX) {
    closeFile();
    lightOn(CYAN);
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
  printMPLAltitudeData();

  delay(200);
}
