#include <SPI.h>
#include <Wire.h>
#include <Adafruit_MPL3115A2.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>
#include <arduino.h>
#include <Adafruit_NeoPixel.h>
#include "HPRSensorHelper.h"

using namespace HPR;

RTC_DS3231 rtc;
Adafruit_MPL3115A2 altimeter;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
File file;
Adafruit_NeoPixel pixel(1, WAVESHARE_LED_PIN, NEO_RGB + NEO_KHZ800);

void SensorHelper::initWavesharePico() {
  SPI.setRX(4);
  SPI.setCS(5);
  SPI.setSCK(6);
  SPI.setTX(7);
  Wire.setSDA(8);
  Wire.setSCL(9);
}

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

void SensorHelper::drawDisplayData(int starting_altitude, int current_altitude, int max_altitude, String flight_status) {
  display.clearDisplay();
  // Time
  display.setCursor(72, 0);
  display.println(getCurrentTime());
  // Starting Altitude
  display.setCursor(0, 0);
  display.println("S:" + String(starting_altitude));
  // Current Altitude
  display.setCursor(0, 12);
  display.println("C:" + String(current_altitude));
  // Max Altitude
  display.setCursor(0, 24);
  display.println("M:" + String(max_altitude));
  // Flight Status
  display.setCursor(72, 24);
  display.println(flight_status);

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
  file = SD.open(SD_FILE_NAME, FILE_WRITE);
  if (!file) {
    Serial.print("error opening ");
    Serial.println(SD_FILE_NAME);
  }
  delay(100);
}

void SensorHelper::closeFile() {
  file.close();
  delay(100);
}

void SensorHelper::writeLogHeader() {
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
  file.println(String(now.second()));
  file.println("timestamp, starting_alt, current_alt, max_alt, flight_status");
  file.println("*****");
  delay(200);
}

void SensorHelper::writeLine(String line) {
  if(!file.println(line)) {
    Serial.println("Failed to write data to file...");  
  }
  delay(100);
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
  if (!altimeter.begin()) {
    Serial.println("Could not find MPL sensor. Check wiring.");
    while (1);
  }
  altimeter.setMode(MPL3115A2_ALTIMETER);
  Serial.println("Altimeter OK!");
}

void SensorHelper::triggerAltitudeUpdate() {
  altimeter.startOneShot();
}

int SensorHelper::getCurrentAltitude() {
  return (int) altimeter.getLastConversionResults(MPL3115A2_ALTITUDE) * FEET_PER_METER; // convert to feet  
}

void SensorHelper::initWaveShareNeoPixel() {
  pixel.begin();
  Serial.println("LED OK!");
}

void SensorHelper::pixelOn(MitchColor color) {
  uint32_t pColor = getPixelColor(color);
  pixel.setPixelColor(0, pColor);
  pixel.show();
}

void SensorHelper::pixelOff() {
  pixel.clear();
  pixel.show();
}

void SensorHelper::blinkPixelFast(int times, MitchColor color) {
  uint32_t pixelColor = getPixelColor(color);
  for(int i=0; i<times; i++) {
    pixel.setPixelColor(0, pixelColor);
    pixel.show();
    delay(BLINK_RATE_FAST);
    pixel.clear();
    pixel.show();
    delay(BLINK_RATE_FAST);
  }
}

void SensorHelper::blinkPixelSlow(int times, MitchColor color) {
  uint32_t pixelColor = getPixelColor(color);
  for(int i=0; i<times; i++) {
    pixel.setPixelColor(0, pixelColor);
    pixel.show();
    delay(BLINK_RATE_SLOW);
    pixel.clear();
    pixel.show();
    delay(BLINK_RATE_SLOW);
  }
}

/**
  ********************
    Color = [R, G, B]
  ********************
  Yellow = 150, 150, 0
  Cyan = 0, 150, 150
  Purple = 150, 0, 150
  White = 150, 150, 150  
  Magenta = 153, 0, 153
  Orange = 255, 128, 0
*/
uint32_t SensorHelper::getPixelColor(MitchColor color) {
  uint32_t c = pixel.Color(0, 0, 0);
  switch(color) {
    case RED: 
      c = pixel.Color(150, 0, 0);
      break;
    case GREEN:
      c = pixel.Color(0, 150, 0);
      break;
    case BLUE:
      c = pixel.Color(0, 0, 150);
      break;
    case CYAN:
      c = pixel.Color(0, 150, 150);
      break;
    case MAGENTA:
      c = pixel.Color(153, 0, 153);
      break; 
  }
  return c;
}
