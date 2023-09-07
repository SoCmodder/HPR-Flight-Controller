#include <Adafruit_MPL3115A2.h>
#include "MitchSDUtils.h"
#include "MitchLEDUtils.h"

#define LAUNCH_INIT_HEIGHT 6
#define APOGEE_TRIGGER_DISTANCE 6
#define APOGEE_COUNTER_MAX 2

using namespace HPR;

Adafruit_MPL3115A2 mpl;
HPR::MitchSD sdUtils;
HPR::MitchLED ledUtils;

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
    while (1);
  }
  mpl.setMode(MPL3115A2_ALTIMETER);
  Serial.println("MPL3115A2 OK!");

  sdUtils.initRTC();
  sdUtils.initSD();
  ledUtils.initLED();

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
  Serial.println(String(current_alt));
  // if current alt is more than 2 meters above starting, then rocket has launched
  if (!launch_initiated) {
    if (current_alt > starting_alt + LAUNCH_INIT_HEIGHT) {
      launch_initiated = true;
      writeString("launch");
      delay(200);
    }
    ledUtils.blinkFast(1, MAGENTA);
  } else {
    if (!chute_deployed && apogee) {
      // Turn on relay 2
      // triggerDeployment();
      chute_deployed = true;
      writeString("deploy");
      delay(200);
    }
    if (current_alt > max_alt) {
      max_alt = current_alt;
      apogee_counter = 0;
    } else if (current_alt + APOGEE_TRIGGER_DISTANCE < max_alt) {
      apogee_counter++;
      apogee = apogee_counter >= APOGEE_COUNTER_MAX;
      writeString("apogee+1");
      delay(200);
    }

    writeString(String(current_alt));

    ledUtils.blinkFast(1, BLUE);

    delay(200);
  }
}

void writeString(String s) {
  String ts = sdUtils.getTimeStamp();
  sdUtils.openFile();
  sdUtils.writeDataLineBlocking(ts + " " + s);
  sdUtils.closeFile();  
}

void getAltReading() {
  // start a conversion
  mpl.startOneShot();

  // do something else while waiting
  int count = 0;
  while (!mpl.conversionComplete()) {
    count++;
  }
}
