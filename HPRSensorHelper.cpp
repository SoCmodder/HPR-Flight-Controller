#include <SPI.h>
#include <Adafruit_MPL3115A2.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>
#include <arduino.h>
#include "HPRSensorHelper.h"

using namespace HPR;

RTC_DS3231 rtc;
Adafruit_MPL3115A2 altimeter;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/**
  ** DISPLAY **
 */
void SensorHelper::initDisplay() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();
}

void SensorHelper::drawDisplayData(float altitude, String flightStatus) {
  display.clearDisplay();
  // Time
  display.setCursor(0, 0);
  display.println("T:" + getCurrentTime());
  // Altitude
  display.setCursor(0, 10);
  display.println("ALT:" + String(altitude));
  // Flight Status
  display.setCursor(0, 20);
  display.println("STATUS:" + flightStatus);

  display.display();
}

/**
  ** STORAGE **
 */
void SensorHelper::initStorage() {
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

void SensorHelper::openFile() {

}

void SensorHelper::closeFile() {

}

void SensorHelper::writeLogHeader() {

}

/**
  ** TIME **
 */
void SensorHelper::initTime() {
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC!");
    //Serial.flush();
    while (1) delay(10);
  }
  if (rtc.lostPower()) {
    // this will adjust to the date and time at compilation
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  //we don't need the 32K Pin, so disable it
  rtc.disable32K();
  Serial.println("RTC OK!");  
}

String SensorHelper::getCurrentTime() {
  DateTime now = rtc.now();
  time_t t = now.unixtime();
  struct tm *lt = localtime(&t);
  char str[32];
  strftime(str, sizeof str, "%H.%M.%S", lt); 
  return str;
}

/**
  ** ALTITUDE **
 */
void SensorHelper::initAltimeter() {

}

void SensorHelper::triggerAltitudeUpdate() {
  altimeter.startOneShot();
}
