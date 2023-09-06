#include <SPI.h>
#include <Adafruit_MPL3115A2.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MitchLEDUtils.h"

using namespace LED;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define LAUNCH_INIT_HEIGHT 2  // 5 meters
#define APOGEE_TRIGGER_DISTANCE 20 // 25 meters
#define APOGEE_COUNTER_MAX 2

Adafruit_MPL3115A2 mpl;
RTC_DS3231 rtclock;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
LED::MitchLED ledUtils;

float starting_alt = 0;
float current_alt = -10;
float max_alt = 0;
bool launch_initiated = true;
bool apogee = false;
bool chute_deployed = false;
uint16_t apogee_counter = 0;
static const unsigned char PROGMEM logo_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

/***********************************************
 **************** SETUP *************************
 ***********************************************/
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  initRTC();
  ledUtils.initLED();
  if (!mpl.begin()) {
    Serial.println("Could not find MPL sensor. Check wiring.");
    while (1);
  }
  mpl.setMode(MPL3115A2_ALTIMETER);
  Serial.println("MPL3115A2 OK!");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);

  getAltReading();

  starting_alt = mpl.getLastConversionResults(MPL3115A2_ALTITUDE);
  current_alt = starting_alt;

  ledUtils.blinkFast(3, BLUE);
}

/***********************************************
 **************** LOOP *************************
 ***********************************************/
void loop() {
  getAltReading();
  current_alt = mpl.getLastConversionResults(MPL3115A2_ALTITUDE);
  // if current alt is more than 2 meters above starting, then rocket has launched
  if (!launch_initiated) {
    if (current_alt > starting_alt + LAUNCH_INIT_HEIGHT) {
      launch_initiated = true;
      writeString("launch");
      delay(200);
    }
  } else {
    if (!chute_deployed && apogee) {
      // Turn on relay 2
      triggerDeployment();
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

    // now get results
    Serial.println(current_alt);

    drawAltitudeText(String(current_alt));

    delay(200);
  }
}

void drawAltitudeText(String alt) {
  display.clearDisplay();
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Alt: " + alt);
  display.display();      // Show initial text
  delay(100);  
}

void writeString(String s) {
  // String ts = sdUtils.getTimeStamp();
  // sdUtils.openFile();
  // sdUtils.writeDataLineBlocking(ts + " " + s);
  // sdUtils.closeFile();  
}

void getAltReading() {
  // start a conversion
  // Serial.println("Starting a conversion.");
  mpl.startOneShot();

  // do something else while waiting
  // Serial.println("Counting number while waiting...");
  int count = 0;
  while (!mpl.conversionComplete()) {
    count++;
  }
  // Serial.print("Done! Counted to ");
  // Serial.println(count);
}

void triggerDeployment() {
  // ledUtils.lightOn(RED);
  // relayUtils.relayOn(RELAY_IN_2);
  // delay(5000);
  // ledUtils.lightOff();
  // relayUtils.relayOff(RELAY_IN_2);
  // delay(1000);
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
  return String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());  
}
