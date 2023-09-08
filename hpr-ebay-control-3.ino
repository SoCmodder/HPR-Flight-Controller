#include <SPI.h>
#include <Adafruit_MPL3115A2.h>
#include <RTClib.h>
#include <SD.h>
#include "HPRSensorHelper.h"

#define LAUNCH_INIT_HEIGHT 6  // 6 meters or ~20 feet
#define APOGEE_TRIGGER_DISTANCE 6 // 6 meters or ~20 feet
#define APOGEE_COUNTER_MAX 2
#define PICO_LED_PIN 25
#define SD_CS_PIN 17
#define SD_FILE_NAME "hpr-log.txt"

enum FLIGHT_STATUS { IDLE, LAUNCH_INIT, APOGEE, DEPLOY };

using namespace HPR;

Adafruit_MPL3115A2 mpl;
RTC_DS3231 rtclock;
HPR::SensorHelper sensorHelper;

float starting_alt = 0;
float current_alt = -10;
float max_alt = 0;
bool apogee = false;
bool chute_deployed = false;
uint16_t apogee_counter = 0;
File file;
FLIGHT_STATUS status = IDLE;
String fs = "IDLE";

/***********************************************
 **************** SETUP *************************
 ***********************************************/
void setup() {
  Serial.begin(115200);
  
  initLED();
  initRTC();
  initSD();
  initMPL();
  sensorHelper.initDisplay();//initOLED();

  mpl.startOneShot();
  delay(500);
  starting_alt = mpl.getLastConversionResults(MPL3115A2_ALTITUDE);
  current_alt = starting_alt;
  ledON();
}

/***********************************************
 **************** LOOP *************************
 ***********************************************/
void loop() {
  fs = getFlightStatus();

  mpl.startOneShot();
  current_alt = mpl.getLastConversionResults(MPL3115A2_ALTITUDE);

  sensorHelper.drawDisplayData(current_alt, fs);
  
  // if current alt is more than 2 meters above starting, then rocket has launched
  if (status == IDLE) {
    if (current_alt > starting_alt + LAUNCH_INIT_HEIGHT) {
      status = LAUNCH_INIT;
      writeString(fs);
    } else {
      Serial.println(String(current_alt));
    }
  } else {
    if (!chute_deployed && apogee) {
      // Turn on relay 2
      // triggerDeployment();
      chute_deployed = true;
      status = DEPLOY;
    }
    if (current_alt > max_alt) {
      max_alt = current_alt;
      apogee_counter = 0;
    } else if (current_alt + APOGEE_TRIGGER_DISTANCE < max_alt) {
      apogee_counter++;
      apogee = apogee_counter >= APOGEE_COUNTER_MAX;
      if (apogee) {
        status = APOGEE;
      }
    }

    writeString(String(starting_alt) + "," + String(current_alt) + "," + String(max_alt));

    ledON();
    delay(100);
    ledOFF();
  }
}

void initRTC() {
  if (!rtclock.begin()) {
    Serial.println("Couldn't find RTC!");
    //Serial.flush();
    while (1) delay(10);
  }
  if (rtclock.lostPower()) {
    // this will adjust to the date and time at compilation
    rtclock.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  //we don't need the 32K Pin, so disable it
  rtclock.disable32K();
  Serial.println("RTC OK!");
}

String getTimeStamp() {
  DateTime now = rtclock.now();
  time_t t = now.unixtime();
  struct tm *lt = localtime(&t);
  char str[32];
  strftime(str, sizeof str, "%H.%M.%S", lt); 
  return str;
}

void initSD() {
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  // try to open the file for writing
  openFile();
  writeLogHeader();
  closeFile();
  Serial.println("SD OK!");
}

void writeString(String s) {
  String ts = getTimeStamp();
  openFile();
  writeDataLineBlocking(ts + " " + s);
  closeFile();  
}

void writeDataLineBlocking(String data) {
  if(!file.println(data)) {
    Serial.println("Failed to write data to file...");  
  }
  delay(100);
}

void openFile() {
  file = SD.open(SD_FILE_NAME, FILE_WRITE);
  if (!file) {
    Serial.print("error opening ");
    Serial.println(SD_FILE_NAME);
  }
  delay(100);
}

void closeFile() {
  file.close();
  delay(100);
}

void writeLogHeader() {
  DateTime now = rtclock.now();
  file.println("");
  file.println("*****");
  file.print("Log Date: ");
  file.print(String(now.day()));
  file.print("/");
  file.print(String(now.month()));
  file.print("/");
  file.print(String(now.year()));
  file.print(" ");
  file.print(String(now.hour()));
  file.print(":");
  file.print(String(now.minute()));
  file.print(":");
  file.print(String(now.second()));
  file.println("starting_alt, current_alt, max_alt");
  file.println("*****");
  delay(200);
}

void initLED() {
  pinMode(PICO_LED_PIN, OUTPUT);
}

void initMPL() {
  if (!mpl.begin()) {
    Serial.println("Could not find MPL sensor. Check wiring.");
    while (1);
  }
  mpl.setMode(MPL3115A2_ALTIMETER);
  Serial.println("MPL3115A2 OK!");
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
