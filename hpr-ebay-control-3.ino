#include "HPRSensorHelper.h"

#define LAUNCH_INIT_HEIGHT 3
#define APOGEE_TRIGGER_DISTANCE 5
#define APOGEE_COUNTER_MAX 2

using namespace HPR;

enum FLIGHT_STATUS { IDLE, LAUNCH_INIT, APOGEE, DEPLOY };
int starting_alt = 0;
int current_alt = 0;
int max_alt = 0;
uint16_t apogee_counter = 0;
FLIGHT_STATUS status = IDLE;

HPR::SensorHelper sensorHelper;

bool debug = false;

/***********************************************
 **************** SETUP *************************
 ***********************************************/
void setup() {
  Serial.begin(115200);

  sensorHelper.initWavesharePico();
  sensorHelper.initWaveShareNeoPixel();
  sensorHelper.initTime();
  sensorHelper.initStorage();
  sensorHelper.initAltimeter();
  sensorHelper.initDisplay();

  sensorHelper.triggerAltitudeUpdate();
  delay(500);

  starting_alt = sensorHelper.getCurrentAltitudeMeters();
  current_alt = starting_alt;

  sensorHelper.pixelOn(MAGENTA);

  // If debug flag is set to true. Skip IDLE and go straight to LAUNCH_INIT
  if (debug == true) {
    status = LAUNCH_INIT;
  }
}

/***********************************************
 **************** LOOP *************************
 ***********************************************/
void loop() {
  current_alt = sensorHelper.getCurrentAltitudeMeters();

  sensorHelper.drawDisplayData(starting_alt, current_alt, max_alt, getFlightStatus());
  
  // if current alt is more than specific height above starting, then rocket has launched
  if (status == IDLE) {
    if (current_alt > starting_alt + LAUNCH_INIT_HEIGHT) {
      status = LAUNCH_INIT;
    } else {
      // if rocket goes lower before launch, reset the starting altitude
      Serial.println(String(current_alt));
    }
  } else {
    if (status == APOGEE) {
      // Turn on relay 2
      // triggerDeployment();
      status = DEPLOY;
    }
    if (current_alt > max_alt) {
      max_alt = current_alt;
      apogee_counter = 0;
    } else if (current_alt + APOGEE_TRIGGER_DISTANCE < max_alt) {
      apogee_counter++;
      status = apogee_counter >= APOGEE_COUNTER_MAX ? APOGEE : LAUNCH_INIT;
    }

    writeCSVLine(starting_alt, current_alt, max_alt);

    sensorHelper.drawDisplayData(starting_alt, current_alt, max_alt, getFlightStatus());

    sensorHelper.blinkPixelFast(1, BLUE);
  }
  
  sensorHelper.triggerAltitudeUpdate();
}

void writeCSVLine(int starting_alt, int current_alt, int max_alt) {
  String timestamp = sensorHelper.getCurrentTime();
  String csvString = timestamp + "," + String(starting_alt) + "," + String(current_alt) + "," + String(max_alt) + "," + getFlightStatus();

  sensorHelper.openFile();
  sensorHelper.writeLine(csvString);
  sensorHelper.closeFile();
}

// enum FLIGHT_STATUS { IDLE, LAUNCH_INIT, APOGEE, DEPLOY };
String getFlightStatus() {
  switch (status) {
    case IDLE: return "IDLE";
    case LAUNCH_INIT: return "LAUNCH";
    case APOGEE: return "APOGEE";
    case DEPLOY: return "DEPLOY";
    default: return "IDLE";
  }
}
