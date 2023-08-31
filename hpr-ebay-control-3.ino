#include <Adafruit_MPL3115A2.h>
#include "MitchSDUtils.h"
#include "MitchLEDUtils.h"
#include "MitchRelayUtils.h"

#define LAUNCH_INIT_HEIGHT 2  // 2 meters
#define APOGEE_TRIGGER_DISTANCE 10 // 10 meters
#define APOGEE_COUNTER_MAX 3

using namespace HPR;

Adafruit_MPL3115A2 mpl;
HPR::MitchSD sdUtils;
HPR::MitchLED ledUtils;
HPR::MitchRelay relayUtils;

float starting_alt = 0;
float current_alt = -10;
float max_alt = 0;
bool launch_initiated = false;
bool apogee = false;
bool chute_deployed = false;
uint16_t apogee_counter = 0;

/***********************************************
 **************** SETUP *************************
 ***********************************************/
void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10);

  if (!mpl.begin()) {
    Serial.println("Could not find MPL sensor. Check wiring.");
    while (1)
      ;
  }
  mpl.setMode(MPL3115A2_ALTIMETER);
  Serial.println("MPL3115A2 OK!");

  sdUtils.initRTC();
  sdUtils.initSD();
  ledUtils.initLED();
  relayUtils.initRelay();

  getAltReading();

  starting_alt = mpl.getLastConversionResults(MPL3115A2_ALTITUDE);
  current_alt = starting_alt;

  ledUtils.blinkFast(4, GREEN);
}

/***********************************************
 **************** LOOP *************************
 ***********************************************/
void loop() {
  getAltReading();
  current_alt = mpl.getLastConversionResults(MPL3115A2_ALTITUDE);
  // if current alt is more than 2 meters above starting, then rocket has launched
  if (!launch_initiated) {
    if (current_alt > starting_alt + LAUNCH_INIT_HEIGHT) {
      launch_initiated = true;
    }
  } else {
    if (!chute_deployed && apogee) {
      // Turn on relay 2
      triggerDeployment();
      chute_deployed = true;
    }
    if (current_alt > max_alt) {
      max_alt = current_alt;
      apogee_counter = 0;
    } else if (current_alt + APOGEE_TRIGGER_DISTANCE < max_alt) {
      apogee_counter++;
      apogee = apogee_counter >= APOGEE_COUNTER_MAX;
    }
    String ts = sdUtils.getTimeStamp();
    sdUtils.openFile();
    sdUtils.writeDataLineBlocking(ts + " " + String(current_alt));
    sdUtils.closeFile();

    ledUtils.blinkFast(1, BLUE);
    // now get results
    Serial.println(current_alt);

    delay(200);
  }
}

void getAltReading() {
  // start a conversion
  // Serial.println("Starting a conversion.");
  mpl.startOneShot();

  // do something else while waiting
  // Serial.println("Counting number while waiting...");
  int count = 0;
  while (!mpl.conversionComplete()) {
    count++;
  }
  // Serial.print("Done! Counted to ");
  // Serial.println(count);
}

void triggerDeployment() {
  ledUtils.lightOn(RED);
  relayUtils.relayOn(RELAY_IN_2);
  delay(5000);
  ledUtils.lightOff();
  relayUtils.relayOff(RELAY_IN_2);
  delay(1000);
}
