#include <Wire.h>
#include "MitchAltitudeUtils.h"
#include "MitchSDUtils.h"
#include "MitchLEDUtils.h"

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
  led.blinkFast(1, GREEN);
  sd.initRTC();
  sd.initSD();
  alt.initRelay();
  //alt.initBNO();
  //alt.initMPL();
  alt.initDPS();

  alt.STARTING_ALT = alt.readDPSAlt();   
  delay(500);

  led.blinkFast(3, GREEN);
  delay(200);
}

/***********************************************
 **************** LOOP *************************
 ***********************************************/
void loop() {
  //alt.CURRENT_ALT = alt.readDPSAlt(); // average of last NUMBER_TO_AVERAGE readings
  alt.CURRENT_ALT = alt.readDPSAlt();
  alt.RELATIVE_ALT = alt.getRelativeAltAverage();

  if (cycles < CYCLE_MAX) {
    //sd.writeDataLineBlocking("Current Alt: " + String(alt.CURRENT_ALT));
    sd.writeDataLineBlocking("Relative Alt: " + String(alt.RELATIVE_ALT) + " m");
    cycles++;
    delay(200);
    led.blinkFast(1, BLUE);
  } else if (cycles == CYCLE_MAX) {
    sd.writeDataLineBlocking("Chute Deployed");
    sd.closeFile();
    led.lightOn(RED);
    alt.deployChute();
    led.lightOff();
    while(1);
  }

  if (alt.CURRENT_ALT > alt.MAX_ALT) {
    // update max altitude
    alt.MAX_ALT = alt.CURRENT_ALT;
  } 
  // else if (alt.shouldDeployChute()) {
  //   // chute deployment altitude reached
  //   alt.deployChute();
  // }

  alt.printDPSAltitudeData();
  //printMPLAltitudeData();
  delay(200);
}
