#include <Adafruit_BNO055.h>
#include "OrientationUtils.h"

using namespace HPR;

/** BNO055 **/
//velocity = accel*dt (dt in seconds)
//position = 0.5*accel*dt^2
double ACCEL_VEL_TRANSITION =  (double)(BNO055_SAMPLERATE_DELAY_MS) / 1000.0;
double ACCEL_POS_TRANSITION = 0.5 * ACCEL_VEL_TRANSITION * ACCEL_VEL_TRANSITION;
double DEG_2_RAD = 0.01745329251; //trig functions require radians, BNO055 outputs degrees
double xPos = 0, yPos = 0, headingVel = 0;
uint16_t PRINT_DELAY_MS = 500; // how often to print the data
uint16_t printCount = 0; //counter to avoid printing every 10MS sample

Adafruit_BNO055 bno = Adafruit_BNO055(55);

void Orientation::initBNO() {
  if (!bno.begin()) {
    Serial.print("No BNO055 detected");
    while (1);
  }
  bno.setExtCrystalUse(true);
  Serial.println("BNO055 OK!");
  delay(500);
}

void Orientation::printLinearAccelerationData() {
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

void Orientation::printOrientationData() {
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

void Orientation::printEvent(sensors_event_t* event) {
  Serial.println();
  Serial.print(event->type);
  double x = -1000000, y = -1000000 , z = -1000000; //dumb values, easy to spot problem
  if (event->type == SENSOR_TYPE_ACCELEROMETER) {
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
  }
  else if (event->type == SENSOR_TYPE_ORIENTATION) {
    x = event->orientation.x;
    y = event->orientation.y;
    z = event->orientation.z;
  }
  else if (event->type == SENSOR_TYPE_MAGNETIC_FIELD) {
    x = event->magnetic.x;
    y = event->magnetic.y;
    z = event->magnetic.z;
  }
  else if ((event->type == SENSOR_TYPE_GYROSCOPE) || (event->type == SENSOR_TYPE_ROTATION_VECTOR)) {
    x = event->gyro.x;
    y = event->gyro.y;
    z = event->gyro.z;
  }

  Serial.print(": x= ");
  Serial.print(x);
  Serial.print(" | y= ");
  Serial.print(y);
  Serial.print(" | z= ");
  Serial.println(z);
}

void Orientation::demoLoop() {
  unsigned long tStart = micros();
  sensors_event_t orientationData , linearAccelData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  //  bno.getEvent(&angVelData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);

  xPos = xPos + ACCEL_POS_TRANSITION * linearAccelData.acceleration.x;
  yPos = yPos + ACCEL_POS_TRANSITION * linearAccelData.acceleration.y;

  // velocity of sensor in the direction it's facing
  headingVel = ACCEL_VEL_TRANSITION * linearAccelData.acceleration.x / cos(DEG_2_RAD * orientationData.orientation.x);

  if (printCount * BNO055_SAMPLERATE_DELAY_MS >= PRINT_DELAY_MS) {
    //enough iterations have passed that we can print the latest data
    Serial.print("Heading: ");
    Serial.println(orientationData.orientation.x);
    Serial.print("Position: ");
    Serial.print(xPos);
    Serial.print(" , ");
    Serial.println(yPos);
    Serial.print("Speed: ");
    Serial.println(headingVel);
    Serial.println("-------");

    printCount = 0;
  }
  else {
    printCount = printCount + 1;
  }

  while ((micros() - tStart) < (BNO055_SAMPLERATE_DELAY_MS * 1000))
  {
    //poll until the next sample is ready
  }
}
