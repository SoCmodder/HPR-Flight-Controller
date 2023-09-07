#include <Adafruit_MPL3115A2.h>
#include <Adafruit_BNO055.h>
#include "MitchSDUtils.h"
#include "MitchLEDUtils.h"

#define LAUNCH_INIT_HEIGHT 6
#define APOGEE_TRIGGER_DISTANCE 6
#define APOGEE_COUNTER_MAX 2

enum FLIGHT_STATUS { IDLE, LAUNCH_INIT, APOGEE, DEPLOY };

using namespace HPR;

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
// id, address
Adafruit_BNO055 orientation = Adafruit_BNO055(-1, 0x28, &Wire);
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
FLIGHT_STATUS flight_status = IDLE;

/***********************************************
 **************** SETUP *************************
 ***********************************************/
void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10);

  initMPL();
  initOrientation();

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
  char fs = getFlightStatus();
  getAltReading();
  current_alt = mpl.getLastConversionResults(MPL3115A2_ALTITUDE);
  // if current alt is more than 2 meters above starting, then rocket has launched
  if (!launch_initiated) {
    if (current_alt > starting_alt + LAUNCH_INIT_HEIGHT || orientationLaunchTriggered()) {
      launch_initiated = true;
      flight_status = LAUNCH_INIT;
      writeString(String(fs));
    } else {
      Serial.println(String(current_alt));
      ledUtils.blinkFast(1, MAGENTA);
    }
  } else {
    if (!chute_deployed && apogee) {
      // Turn on relay 2
      // triggerDeployment();
      chute_deployed = true;
      flight_status = DEPLOY;
    }
    if (current_alt > max_alt) {
      max_alt = current_alt;
      apogee_counter = 0;
    } else if (current_alt + APOGEE_TRIGGER_DISTANCE < max_alt) {
      apogee_counter++;
      apogee = apogee_counter >= APOGEE_COUNTER_MAX;
      if (apogee) {
        flight_status = APOGEE;
      }
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

void initMPL() {
  if (!mpl.begin()) {
    Serial.println("Could not find MPL sensor. Check wiring.");
    while (1);
  }
  mpl.setMode(MPL3115A2_ALTIMETER);
  Serial.println("MPL3115A2 OK!");
}

void initOrientation() {
  /* Initialise the sensor */
  if(!orientation.begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    //while(1);
  }  

  orientation.setExtCrystalUse(true);
}

bool orientationLaunchTriggered() {
  imu::Vector<3> euler = orientation.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  uint8_t system, gyro, accel, mag = 0;
  orientation.getCalibration(&system, &gyro, &accel, &mag);

  return gyro > 0 && euler.x() > 20;
}

// enum FLIGHT_STATUS { IDLE, LAUNCH_INIT, APOGEE, DEPLOY };
char getFlightStatus() {
  switch (flight_status) {
    case IDLE: return 'I';
    case LAUNCH_INIT: return 'L';
    case APOGEE: return 'A';
    case DEPLOY: return 'D';
  }
}
