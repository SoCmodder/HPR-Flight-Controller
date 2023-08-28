#include <RTClib.h>
#include <SPI.h>              // SD
#include <SD.h>               // SD
#include <arduino.h>
#include "MitchSDUtils.h"

RTC_DS3231 rtc;
// File object to represent file
File file;

using namespace HPR;

void MitchSD::initSD() {
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

void MitchSD::initRTC() {
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

void MitchSD::closeFile() {
  file.close();
  delay(200);
}

void MitchSD::openFile() {
  file = SD.open(SD_FILE_NAME, FILE_WRITE);
  if (!file) {
    Serial.print("error opening ");
    Serial.println(SD_FILE_NAME);
    //while (1);
  }
  delay(200);
}

void MitchSD::writeLogHeader() {
  DateTime now = rtc.now();
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
  file.println("");
  file.println("*****");
  delay(200);
}

String MitchSD::getTimeStamp() {
  DateTime now = rtc.now();
  return String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());  
}

void MitchSD::writeDataLineBlocking(String data) {
  /** TODO: check file size
  //       if file size greater than max write size minus some amount
  //       close file
  //       delay
  //       open file
  //       repeat 
  */
  if(!file.println(data)) {
    Serial.println("Failed to write data to file...");  
  }
  delay(500);
}
