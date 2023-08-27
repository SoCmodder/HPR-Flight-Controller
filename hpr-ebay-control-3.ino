#include <Adafruit_MPL3115A2.h>
#include "MitchSDUtils.h"
#include "MitchLEDUtils.h"
#include "MitchRelayUtils.h"

using namespace HPR;

Adafruit_MPL3115A2 mpl;
HPR::MitchSD sdUtils;
HPR::MitchLED ledUtils;
HPR::MitchRelay relayUtils;

float starting_alt = 0;
float current_alt = 0;
float max_alt = 0;
bool apogee = false;
uint16_t apogee_counter = 0;

bool debug = true;
uint16_t counter = 0;
uint16_t counter_max = 10;

/***********************************************
 **************** SETUP *************************
 ***********************************************/
void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10);

  if (!mpl.begin()) {
    Serial.println("Could not find MPL sensor. Check wiring.");
    while(1);
  }
  mpl.setMode(MPL3115A2_ALTIMETER);
  Serial.println("MPL3115A2 OK!");

  sdUtils.initRTC();
  sdUtils.initSD();
  ledUtils.initLED();
  relayUtils.initRelay();

  starting_alt = mpl.getLastConversionResults(MPL3115A2_ALTITUDE);

  ledUtils.blinkFast(3, GREEN);
}

/***********************************************
 **************** LOOP *************************
 ***********************************************/
void loop() {
  // start a conversion
  Serial.println("Starting a conversion.");
  mpl.startOneShot();

  // do something else while waiting
  Serial.println("Counting number while waiting...");
  int count = 0;
  while (!mpl.conversionComplete()) {
    count++;
  }
  Serial.print("Done! Counted to "); Serial.println(count);
  
  String ts = sdUtils.getTimeStamp();
  current_alt = mpl.getLastConversionResults(MPL3115A2_ALTITUDE);

  if (current_alt + 30 < max_alt) {
    apogee_counter++;
    apogee = apogee_counter >= 3;
  } else if (current_alt > max_alt) {
    max_alt = current_alt;
    apogee_counter = 0;
  }

  if (apogee) {
    relayUtils.relayOn(RELAY_IN_2);
    relayUtils.relayOff(RELAY_IN_2);  
  }

  sdUtils.openFile();
  sdUtils.writeDataLineBlocking(ts + " " + String(current_alt));
  sdUtils.closeFile();

  ledUtils.blinkFast(1, BLUE);

  // now get results
  Serial.println(current_alt);

  delay(1000);  

  if (debug) {
    if (counter == counter_max) {
      ledUtils.blinkFast(5, MAGENTA);

      relayUtils.relayOn(RELAY_IN_2);
      relayUtils.relayOff(RELAY_IN_2);

      delay(500);
      
      while(1) {
        delay(5000);
      }
    }
    counter++;
  }
}
