#include <Wire.h>
#include "MitchAltitudeUtils.h"
#include "MitchSDUtils.h"
#include "MitchLEDUtils.h"
#include "MitchRelayUtils.h"

using namespace HPR;

HPR::MitchLED led;
HPR::MitchSD sd;
HPR::MitchAlt alt;

int cycles = 0;
int CYCLE_MAX = 20;

/***********************************************
 **************** SETUP *************************
 ***********************************************/
void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10);

  led.initLED();
  sd.initRTC();
  sd.initSD();
  alt.initRelay();
  alt.initBNO();
  alt.initMPL();
  alt.initDPS();

  alt.STARTING_ALT = alt.getAltAverage();   
  delay(500);

  led.blinkFast(3, GREEN);
  delay(200);
}

/***********************************************
 **************** LOOP *************************
 ***********************************************/
void loop() {
  alt.CURRENT_ALT = alt.getAltAverage(); // average of last NUMBER_TO_AVERAGE readings
  alt.RELATIVE_ALT = alt.getRelativeAltAverage();

  if (cycles < CYCLE_MAX) {
    sd.writeDataLineBlocking("Current Alt: " + String(alt.CURRENT_ALT));
    sd.writeDataLineBlocking("Relative Alt: " + String(alt.RELATIVE_ALT));
    cycles++;
  }
  if (cycles == CYCLE_MAX) {
    sd.closeFile();
    led.lightOn(RED);
    alt.deployChute();
    //sd.writeDataLineBlocking("Chute Deployed");
    while(1);
  }

  if (alt.CURRENT_ALT > alt.MAX_ALT) {
    // update max altitude
    alt.MAX_ALT = alt.CURRENT_ALT;
  } else if (alt.shouldDeployChute()) {
    // chute deployment altitude reached
    alt.deployChute();
  }

  alt.printDPSAltitudeData();
  //printMPLAltitudeData();

  delay(200);
}
