#include "HPRSensorHelper.h"
#include <Adafruit_NeoPixel.h>

#define LAUNCH_INIT_HEIGHT 20  //20 feet
#define APOGEE_TRIGGER_DISTANCE 20 //20 feet
#define APOGEE_COUNTER_MAX 2
#define WAVESHARE_LED_PIN 16
#define BLINK_RATE_FAST 100
#define BLINK_RATE_SLOW 600

using namespace HPR;

enum FLIGHT_STATUS { IDLE, LAUNCH_INIT, APOGEE, DEPLOY };
enum MitchColor { RED, GREEN, BLUE, CYAN, MAGENTA };
int starting_alt = 0;
int current_alt = 0;
int max_alt = 0;
uint16_t apogee_counter = 0;
FLIGHT_STATUS status = IDLE;

HPR::SensorHelper sensorHelper;
Adafruit_NeoPixel pixel(1, WAVESHARE_LED_PIN, NEO_RGB + NEO_KHZ800);

/***********************************************
 **************** SETUP *************************
 ***********************************************/
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  initLED();
  sensorHelper.initTime();
  //sensorHelper.initStorage();
  sensorHelper.initUARTStorage();
  sensorHelper.initAltimeter();
  sensorHelper.initDisplay();

  sensorHelper.triggerAltitudeUpdate();
  delay(500);

  starting_alt = sensorHelper.getCurrentAltitude();
  current_alt = starting_alt;

  lightOn(MAGENTA);
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

    writeCSVLine(starting_alt, current_alt, max_alt);

    sensorHelper.drawDisplayData(starting_alt, current_alt, max_alt, getFlightStatus());

    blinkFast(1, BLUE);
  }
  
  sensorHelper.triggerAltitudeUpdate();
}

void writeCSVLine(int starting_alt, int current_alt, int max_alt) {
  String timestamp = sensorHelper.getCurrentTime();
  String csvString = timestamp + "," + String(starting_alt) + "," + String(current_alt) + "," + String(max_alt) + "," + getFlightStatus();

  // sensorHelper.openFile();
  // sensorHelper.writeLine(csvString);
  // sensorHelper.closeFile();
  sensorHelper.writeUARTLine(csvString);
}

void initLED() {
  pixel.begin();
  Serial.println("LED OK!");
}

void lightOn(MitchColor color) {
  uint32_t pColor = getPixelColor(color);
  pixel.setPixelColor(0, pColor);
  pixel.show();
}

void lightOff() {
  pixel.clear();
  pixel.show();
}

void blinkFast(int times, MitchColor color) {
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

void blinkSlow(int times, MitchColor color) {
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

// enum FLIGHT_STATUS { IDLE, LAUNCH_INIT, APOGEE, DEPLOY };
String getFlightStatus() {
  switch (status) {
    case IDLE: return "IDLE";
    case LAUNCH_INIT: return "LAUNCH";
    case APOGEE: return "APOGEE";
    case DEPLOY: return "DEPLOY";
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
uint32_t getPixelColor(MitchColor color) {
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
