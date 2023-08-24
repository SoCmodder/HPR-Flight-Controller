#include <Adafruit_MPL3115A2.h>
#include <Adafruit_DPS310.h>
#include <Adafruit_BNO055.h>
#include "MitchRelayUtils.h"

#define SEA_LEVEL_HPA 1013.25
#define NUMBER_TO_AVERAGE 10
#define CHUTE_DEPLOYMENT_ALT 121.92 // 400 ft

Adafruit_DPS310 dps;
Adafruit_MPL3115A2 mpl;
Adafruit_BNO055 bno = Adafruit_BNO055(55);

/** BNO055 **/
//velocity = accel*dt (dt in seconds)
//position = 0.5*accel*dt^2
uint16_t BNO055_SAMPLERATE_DELAY_MS = 10; //how often to read data from the board
double ACCEL_VEL_TRANSITION =  (double)(BNO055_SAMPLERATE_DELAY_MS) / 1000.0;
double ACCEL_POS_TRANSITION = 0.5 * ACCEL_VEL_TRANSITION * ACCEL_VEL_TRANSITION;
double DEG_2_RAD = 0.01745329251; //trig functions require radians, BNO055 outputs degrees
double xPos = 0, yPos = 0, headingVel = 0;

float STARTING_ALT = 0;
float MAX_ALT = 0;
float CURRENT_ALT = 0;
bool APOGEE = false;

void initBNO() {
  if (!bno.begin()) {
    Serial.print("No BNO055 detected");
    while (1);
  }
  bno.setExtCrystalUse(true);
  Serial.println("BNO055 OK!");
  delay(500);
}

void initMPL() {
  if (!mpl.begin()) {
    Serial.println("Could not find MPL sensor. Check wiring.");
    while(1);
  }
  Serial.println("MPL3115A2 OK!");
  delay(500);
}

void initDPS() {
  if (!dps.begin_I2C()) {             // Can pass in I2C address here
    Serial.println("Failed to find DPS");
    while (1) yield();
  }
  dps.configurePressure(DPS310_128HZ, DPS310_128SAMPLES);
  dps.configureTemperature(DPS310_128HZ, DPS310_128SAMPLES);
  delay(1000);
  Serial.println("DPS310 OK!");
}

float readDPSAlt() {
  return dps.readAltitude(SEA_LEVEL_HPA);  
}

float readMPLAlt() {
  return mpl.getAltitude();
}

float getAltAverage() {
  float sum = 0.0;
  for (int i=0; i<NUMBER_TO_AVERAGE; i++) {
    sum += readDPSAlt();
  }
  return sum / NUMBER_TO_AVERAGE;
}

bool shouldDeployChute() {
  bool deploy = false;
  float altDrop = MAX_ALT - CURRENT_ALT;
  // if current alt is less than (CHUTE_DEPLOYMENT_ALT) feet above starting point and the altitude has dropped more than
  // (CHUTE_DEPLOYMENT_ALT) feet (121.92 meters), then return true.
  if ((CURRENT_ALT < (STARTING_ALT + CHUTE_DEPLOYMENT_ALT)) && (altDrop > 121.92)) {
    deploy = true;
  }
  return deploy;
}

void deployChute() {
  relayOn(RELAY_IN_1);
  relayOff(RELAY_IN_1);
}

void printDPSAltitudeData() {
  Serial.print("DPS310 => Starting Alt: ");
  Serial.print(String(STARTING_ALT));
  Serial.print(", Current Alt: ");
  Serial.print(String(CURRENT_ALT));
  Serial.print(", Max Alt: ");
  Serial.print(String(MAX_ALT));
  Serial.println("");
}

void printMPLAltitudeData() {
  Serial.print("MPL Alt: ");
  Serial.print(String(readMPLAlt()));
  Serial.println("");
}

void printLinearAccelerationData() {
  /******************** BNO055 ********************/
  imu::Vector<3> vector_accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  /* Display the floating point data */
  Serial.print("X: ");
  Serial.print(vector_accel.x());
  Serial.print(" Y: ");
  Serial.print(vector_accel.y());
  Serial.print(" Z: ");
  Serial.print(vector_accel.z());
  Serial.println("");
  delay(100);
}

void printOrientationData() {
  /******************** BNO055 ********************/
  sensors_event_t bno_event; 
  bno.getEvent(&bno_event);

  /* Display the floating point data */
  Serial.print("X: ");
  Serial.print(bno_event.orientation.x, 4);
  Serial.print("\tY: ");
  Serial.print(bno_event.orientation.y, 4);
  Serial.print("\tZ: ");
  Serial.print(bno_event.orientation.z, 4);
  Serial.println("");
}


