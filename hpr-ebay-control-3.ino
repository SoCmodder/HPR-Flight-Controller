#include "HPRSensorHelper.h"

#define LAUNCH_INIT_HEIGHT 20  //20 feet
#define APOGEE_TRIGGER_DISTANCE 20 //20 feet
#define APOGEE_COUNTER_MAX 2
#define PICO_LED_PIN 25

using namespace HPR;

enum FLIGHT_STATUS { IDLE, LAUNCH_INIT, APOGEE, DEPLOY };
int starting_alt = 0;
int current_alt = 0;
int max_alt = 0;
uint16_t apogee_counter = 0;
FLIGHT_STATUS status = IDLE;

HPR::SensorHelper sensorHelper;

/***********************************************
 **************** SETUP *************************
 ***********************************************/
void setup() {
  Serial.begin(115200);
  
  initLED();
  sensorHelper.initTime();
  sensorHelper.initStorage();
  sensorHelper.initAltimeter();
  sensorHelper.initDisplay();

  sensorHelper.triggerAltitudeUpdate();
  delay(500);

  starting_alt = sensorHelper.getCurrentAltitude();
  current_alt = starting_alt;

  ledON();
}

/***********************************************
 **************** LOOP *************************
 ***********************************************/
void loop() {
  current_alt = sensorHelper.getCurrentAltitude();

  sensorHelper.drawDisplayData(starting_alt, current_alt, max_alt, getFlightStatus());
  
  // if current alt is more than 20 feet above starting, then rocket has launched
  if (status == IDLE) {
    if (current_alt > starting_alt + LAUNCH_INIT_HEIGHT) {
      status = LAUNCH_INIT;
    } else {
      // if rocket goes lower before launch, reset the starting altitude
      starting_alt = current_alt < starting_alt ? current_alt : starting_alt;
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

    writeString(String(starting_alt) + "," + String(current_alt) + "," + String(max_alt) + "," + getFlightStatus());

    sensorHelper.drawDisplayData(starting_alt, current_alt, max_alt, getFlightStatus());

    ledON();
    delay(100);
    ledOFF();
  }
  
  sensorHelper.triggerAltitudeUpdate();
}

void writeString(String s) {
  String ts = sensorHelper.getCurrentTime();
  sensorHelper.openFile();
  sensorHelper.writeLine(ts + " " + s);
  sensorHelper.closeFile();  
}

void initLED() {
  pinMode(PICO_LED_PIN, OUTPUT);
}

void ledON() {
  digitalWrite(PICO_LED_PIN, HIGH);
}

void ledOFF() {
  digitalWrite(PICO_LED_PIN, LOW);
}

// enum FLIGHT_STATUS { IDLE, LAUNCH_INIT, APOGEE, DEPLOY };
String getFlightStatus() {
  switch (status) {
    case IDLE: return "IDLE";
    case LAUNCH_INIT: return "LAUNCH";
    case APOGEE: return "APOGEE";
    case DEPLOY: return "DEPLOY";
  }
}
